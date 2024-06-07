// USART 통신 구현 
#define FOSC 16000000 
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define WHO_AM_I_MPU9250  0x75
#define ACCEL_CONFIG      0x1C
#define USER_CTRL         0x6A

#define ACCEL_XOUT_H      0x3B
#define ACCEL_XOUT_L      0x3C
#define ACCEL_YOUT_H      0x3D
#define ACCEL_YOUT_L      0x3E
#define ACCEL_ZOUT_H      0x3F
#define ACCEL_ZOUT_L      0x40
#define window_size 20

volatile uint16_t value = 0;
uint8_t acc_full_scale = 0x01;

float   acc_scale;
float   ax, ay, az;
char    ax_str[10], ay_str[10], az_str[10];
uint8_t rawData[6];
int16_t accelCount[3];
float pitch_degrees_values[window_size] = {0};
int idx = 0;
int count = 0;
float filtered_values = 0;

void USART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8); 
    UBRR0L = (unsigned char)ubrr;        
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
}

void USART_Transmit_uint16(uint16_t data) {
    unsigned char high_byte = (data >> 8); 
    unsigned char low_byte = (data & 0xFF); 

    while (!(UCSR0A & (1 << UDRE0))); 
    UDR0 = high_byte;
    while (!(UCSR0A & (1 << UDRE0))); 
    UDR0 = low_byte; 



uint8_t transfer_SPI(uint8_t registerAddress, uint8_t data, bool isRead) {
    uint8_t response = 0x00;
    registerAddress |= (isRead ? 0x80 : 0x00);

    PORTB &= ~(1 << PB2);

    SPDR = registerAddress;
    while (!(SPSR & (1 << SPIF)));
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    response = SPDR;

    PORTB |= (1 << PB2);
    return response;
}

void check_IMU(uint8_t registerAddress) {
  byte c = transfer_SPI(registerAddress, 0x00, true);
  Serial.println("<< Check signature of MPU9250 >>");
  Serial.print("Value of WHO_AM_I register: 0x");
  Serial.print(c, HEX);
  Serial.println("\t(should be 0x71 or 0x73)");

  if (c == 0x71 | c == 0x73) {
    Serial.println("\t-> signature matched.");
    delay(2000);
  }
  else {
    Serial.print("\t-> wrong signature. written value: 0x");
    Serial.println(c, HEX);
    while(1);
  }
}

void disable_i2c_comm() {
  uint8_t current_setting = transfer_SPI(USER_CTRL, 0x00, true);
  current_setting |= 0x10;
  transfer_SPI(USER_CTRL, current_setting, false);
}

void setup_accel_scale(uint8_t scale) {
  uint8_t current_config = transfer_SPI(ACCEL_CONFIG, 0x00, true);
  current_config &= ~0x18;
  current_config |= (scale << 3);
  transfer_SPI(ACCEL_CONFIG, current_config, false);

  switch (scale) {
    case 0x00: 
      acc_scale = 2.0f / 32768.0f;
      break;
    case 0x01: 
      acc_scale = 4.0f / 32768.0f;
      break;
    case 0x02: 
      acc_scale = 8.0f / 32768.0f;
      break;
      acc_scale = 16.0f / 32768.0f;
      break;
  }
}

void setup() {
 
  DDRB |=  (1 << PB2) | (1 << PB3) | (1 << PB5);
  DDRB &= ~(1 << PB4);

  
  SPCR |=   (1 << SPE)  | (1 << MSTR) | (1 << SPR0);
  SPCR &= ~((1 << DORD) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1));

  
  PORTB |= (1 << PB2);

  check_IMU(WHO_AM_I_MPU9250);
  setup_accel_scale(acc_full_scale);
  disable_i2c_comm();
  USART_Init(MYUBRR); 

  delay(1000);
}

void loop() {
  
  rawData[0] = transfer_SPI(ACCEL_XOUT_H, 0x00, true);
  rawData[1] = transfer_SPI(ACCEL_XOUT_L, 0x00, true);
  rawData[2] = transfer_SPI(ACCEL_YOUT_H, 0x00, true);
  rawData[3] = transfer_SPI(ACCEL_YOUT_L, 0x00, true);
  rawData[4] = transfer_SPI(ACCEL_ZOUT_H, 0x00, true);
  rawData[5] = transfer_SPI(ACCEL_ZOUT_L, 0x00, true);

  accelCount[0] = (rawData[0] << 8) | rawData[1];
  accelCount[1] = (rawData[2] << 8) | rawData[3];
  accelCount[2] = (rawData[4] << 8) | rawData[5];

  ax = accelCount[0] * acc_scale;
  ay = accelCount[1] * acc_scale;
  az = accelCount[2] * acc_scale;

  dtostrf(ax, 4, 2, ax_str);
  dtostrf(ay, 4, 2, ay_str);
  dtostrf(az, 4, 2, az_str);

  sprintf(message, "ax = %s, ay = %s, az = %s", ax_str, ay_str, az_str);
  
  float pitch = atan2(ax, sqrt(ay*ay + az*az));
  float pitch_degrees = pitch * 180.0 / PI;
  filtered_values = moving_average_filter(pitch_degrees);
  USART_Transmit_uint16(filtered_values); // 값 전송
  delay(50); // 수신기랑 맞춰야지 잘 출력이되었음
}

float moving_average_filter(float current_value)
{
  pitch_degrees_values[idx] = current_value;
  idx = (idx + 1) % window_size;

  if(count < window_size)
  {
    count++;
  } 
  
  float sum  = 0.0;
  for(int i = 0 ; i < count ; i++)
  {
    sum += pitch_degrees_values[i];
  }
  return sum/count;
}
