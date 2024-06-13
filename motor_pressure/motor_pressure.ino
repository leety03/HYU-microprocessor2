int ocrValue;
int value;
int16_t balancing_value;
int angle;


void init_ADC() { 
  // 압력센서 ADC 활성화
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
  // 디지털 핀 출력으로 설정
  DDRH |= (1 << PH3); // OC4A
  DDRH |= (1 << PH4); // OC4B
  DDRH |= (1 << PH5); // OC4C

  // 타이머 초기화
  TCCR4A = 0;
  TCCR4B = 0;
  TCCR4C = 0;
  
  // 고속 PWM 모드, TOP: ICR4
  TCCR4A |= (1 << WGM41);
  TCCR4B |= (1 << WGM42) | (1 << WGM43);
  
  // 분주율 8 설정 (클럭: 2MHz)
  TCCR4B |= (1 << CS41);
  
  // TOP 값 설정 (20ms 주기, 50Hz)
  ICR4 = 39999;

  // 비반전 모드 설정
  TCCR4A |= (1 << COM4A1);
  TCCR4A |= (1 << COM4B1);
  TCCR4A |= (1 << COM4C1);
}

int setServoPosition(int angle){ 
  // 모터 입력값 설정
  int pulseWidth = 1500 + 10 * angle;
  int ocrValue = 40000 * pulseWidth / 20000 - 1 ;
  return ocrValue;
}

void activateBrake_1(){ 
  //브레이크 작동 함수 1
  ocrValue = setServoPosition(40);
  OCR4A = ocrValue;
}

void activateBrake_2(){ 
  //브레이크 작동 함수 2
  ocrValue = setServoPosition(-40);
  OCR4B = ocrValue;
}

void return_brake(){ 
  //브레이크 모터 원위치 함수
  ocrValue = setServoPosition(0);
  OCR4A = ocrValue;
  OCR4B = ocrValue;
}

void balancing(int16_t angle){ 
  //밸런싱 모터 작동 함수
  balancing_value = setServoPosition(angle * -1);
  OCR4C = balancing_value;
}

void setup() { 
  // 시리얼 통신 시작
  Serial.begin(9600); 
  setup_servo_motor(); 
  init_ADC();
}

void loop() {
  value = ADC;
  if(value == 0){
    brakingSystem(angle);
  }
  else {
    return_brake();
  }
  delay(30);
}

void brakingSystem(int16_t slopeValue) { 
  //브레이크 작동
  activateBrake_1();
  activateBrake_2();
}
