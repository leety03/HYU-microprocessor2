volatile uint16_t value = 0;


//ADC interrupt
ISR (ADC_vect){
  value = ADC;
}

void brake(){
  Serial.println("brake activated");
  //테스트 용도 angle
  int angle = random(0,91);

  if (angle > 10){
    Serial.println("brake activated");
  }
  else{
    Serial.println("not activated");
  }
}

void setup(){
  Serial.begin(9600);
  ADMUX |= (0 << REFS1) | (1 << REFS0);
  ADMUX |= (0 << ADLAR);
  ADMUX |= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
  ADCSRA |= (1 << ADATE);
  ADCSRA |= (1 << ADEN);
  ADCSRA |= (1 << ADIE);
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  ADCSRB |= (0 << ADTS0) |(0 << ADTS1) |(0 << ADTS0);
  SREG |= 0x01 << SREG_I;
  ADCSRA |= (1 << ADSC);
}
 
void loop(){
  Serial.println(value);
  if (value == 0){
    brake();