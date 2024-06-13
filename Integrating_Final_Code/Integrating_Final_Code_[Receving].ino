#define FOSC 16000000 
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define UPPER_THRESHOLD 7
#define LOWER_THRESHOLD -7
#define Adjusment 1
static bool brakeFlag = false; 
int ocrValue;
int value;
int16_t balancing_value;

void USART_Init(unsigned int ubrr) {
    UBRR1H = (unsigned char)(ubrr >> 8); 
    UBRR1L = (unsigned char)ubrr;       
    UCSR1B = (1 << RXEN1) | (1 << TXEN1); 
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); 
}

void init_ADC() {
  ADMUX |= (0 << REFS1) | (1 << REFS0);
  ADMUX |= (0 << ADLAR);
  ADMUX |= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);

  ADCSRA |= (1 << ADATE);
  ADCSRA |= (1 << ADEN);
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  ADCSRB |= (0 << ADTS0) |(0 << ADTS1) |(0 << ADTS0);

  ADCSRA |= (1 << ADSC);
}

void setup_servo_motor(){

  DDRH |= (1 << PH3); 
  DDRH |= (1 << PH4); 
  DDRH |= (1 << PH5); 

  TCCR4A = 0;
  TCCR4B = 0;
  TCCR4C = 0;
  
  TCCR4A |= (1 << WGM41);
  TCCR4B |= (1 << WGM42) | (1 << WGM43);
  
  TCCR4B |= (1 << CS41);
  
  ICR4 = 39999;

  TCCR4A |= (1 << COM4A1);
  TCCR4A |= (1 << COM4B1);
  TCCR4A |= (1 << COM4C1);
}

int16_t USART_Receive_int16() {
  while (!(UCSR1A & (1 << RXC1))); 
  unsigned char high_byte = UDR1;
    
  while (!(UCSR1A & (1 << RXC1))); 
  unsigned char low_byte = UDR1;

  return (int16_t)((high_byte << 8) | low_byte); 
}

int setServoPosition(int angle){
  int pulseWidth = 1500 + 10 * angle;
  int ocrValue = 40000 * pulseWidth / 20000 - 1 ;
  return ocrValue;
}

void activateBrake_1(){
  ocrValue = setServoPosition(90);
  OCR4A = ocrValue;
}

void activateBrake_2(){
  ocrValue = setServoPosition(-90);
  OCR4B = ocrValue;
}

void return_brake(){
  ocrValue = setServoPosition(40);
  OCR4A = ocrValue;
  ocrValue = setServoPosition(-40);
  OCR4B = ocrValue;
}

void balancing(int16_t angle){
  balancing_value = setServoPosition(angle * 1);
  OCR4C = balancing_value;

}

void setup() {
  Serial.begin(9600); 
  USART_Init(MYUBRR); 
  setup_servo_motor();
  init_ADC();
}

void loop() {
  int16_t receivedValue = USART_Receive_int16(); 
  value = ADC;
  balancing(receivedValue);
  Serial.print(value);
  Serial.print('\t');
  Serial.println(receivedValue);
  if(value == 0){
    brakingSystem(receivedValue);
  }
  else {
    return_brake();
  }
  delay(30);
}


void brakingSystem(int16_t slopeValue) {
    
    if (slopeValue > UPPER_THRESHOLD + Adjusment) {
        if (!brakeFlag) {
            activateBrake_1();
            activateBrake_2();
            
        }
    } else if (slopeValue < LOWER_THRESHOLD - Adjusment) {
        if (!brakeFlag) {
            activateBrake_1();
            activateBrake_2();
            
        }
    } else if (slopeValue < UPPER_THRESHOLD - Adjusment && slopeValue > LOWER_THRESHOLD + Adjusment) {
        if (brakeFlag) {
            return_brake();
            
    }
  }
}
