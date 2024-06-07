volatile uint16_t value = 0;

void setup(){
  Serial.begin(9600);
  ADMUX |= (0 << REFS1) | (1 << REFS0);
  ADMUX |= (0 << ADLAR);
  ADMUX |= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);

  ADCSRA |= (1 << ADATE);
  ADCSRA |= (1 << ADEN);
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  ADCSRB |= (0 << ADTS0) |(0 << ADTS1) |(0 << ADTS0);

  ADCSRA |= (1 << ADSC);
}

void loop(){
  value = ADC;
  Serial.println(value);
}