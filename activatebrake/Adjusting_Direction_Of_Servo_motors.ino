void motor_set() {
 
  DDRB |= (1 << PB5) | (1 << PB6);


  TCCR1A = 0;
  TCCR1B = 0;
  

  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  
  TCCR1B |= (1 << CS11);

  ICR1 = 39999;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
}

int setServoPosition(int angleA, int angleB) {
  int pulseWidthA = 1500 + 10 * angleA;
  int ocrValueA = 40000 * pulseWidthA / 20000;

  int pulseWidthB = 1500 + 10 * angleB;
  int ocrValueB = 40000 * pulseWidthB / 20000;

  OCR1A = ocrValueA;
  OCR1B = ocrValueB;

  return ocrValueA;
  return ocrValueB;  
}

void setup() {
  Serial.begin(9600); 
  motor_set();
}

void loop() {
  int angles[] = {-90, -70, -40, 0, 40, 70, 90};
  for (int i = 0; i < 7; i++) {
    int angleA = angles[i];
    int angleB = -angles[i];  // Setting opposite direction since two motors are not facing the same direction
    int tick = setServoPosition(angleA, angleB);  
    Serial.print("angleA: ");
    Serial.print(angleA);
    Serial.print(" degrees, angleB: ");
    Serial.print(angleB);
    Serial.print(" degrees, tick: ");
    Serial.println(tick);

    delay(1500);
  }
} 

