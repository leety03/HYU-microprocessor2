int ocrValue;
int pressure_flag;
int pressure_value;
int balancing_value;

void setup_servo_motor(){
  // 디지털 9번 핀을 출력으로 설정
  DDRB |= (1 << PB5);
  DDRB |= (1 << PB6);

  // 타이머 초기화
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  
  // 고속 PWM 모드, TOP: ICR1
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  
  // 분주율 8 설정 (클럭: 2MHz)
  TCCR1B |= (1 << CS11);
  
  // TOP 값 설정 (20ms 주기, 50Hz)
  ICR1 = 39999;

  // 비반전 모드 설정
  TCCR1A |= (1 << COM1A1);
  TCCR1A |= (1 << COM1B1);
  TCCR1A |= (1 << COM1C1);
}

int setServoPosition(int angle){
  int pulseWidth = 1500 + 10 * angle;
  int ocrValue = 40000 * pulseWidth / 20000 - 1 ;
  return ocrValue;
}

void activateBrake_1(){
  ocrValue = setServoPosition(40);
  OCR1A = ocrValue;
}

void activateBrake_2(){
  ocrValue = setServoPosition(-40);
  OCR1B = ocrValue;
}

void return_brake(){
  ocrValue = setServoPosition(0);
  OCR1A = ocrValue;
  OCR1B = ocrValue;
}

void balancing(int angle){
  balancing_value = setServoPosition(angle);
  OCR1C = balancing_value;
}

void setup() {
  setup_servo_motor();
}

void loop() {
  if (pressure_flag == 0){   // 잡지 않았을 때 = 0
    activateBrake_1();
    activateBrake_2();
    delay(4000);
    pressure_flag = 1;
  }
  else{
    return_brake();
  }
}