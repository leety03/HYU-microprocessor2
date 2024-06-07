int ocrValue;
int pressure_flag = 0;

void setup_servo_motor(){
  DDRB |= (1 << PB5);
  DDRB |= (1 << PB6);

  TCCR1A = 0;
  TCCR1B = 0;
  
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  
  TCCR1B |= (1 << CS11);
  
  ICR1 = 39999;

  TCCR1A |= (1 << COM1A1);
  TCCR1A |= (1 << COM1B1);
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