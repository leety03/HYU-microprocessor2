#define FOSC 16000000 
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr) {
    UBRR1H = (unsigned char)(ubrr >> 8); 
    UBRR1L = (unsigned char)ubrr;        
    UCSR1B = (1 << RXEN1) | (1 << TXEN1); 
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); 

int16_t USART_Receive_int16() {
    while (!(UCSR1A & (1 << RXC1))); 
    
    while (!(UCSR1A & (1 << RXC1))); 
    unsigned char low_byte = UDR1;

    return (int16_t)((high_byte << 8) | low_byte); 
}

void setup() {
    Serial.begin(9600); 
    USART_Init(MYUBRR); 
}

void loop() {
    int16_t receivedValue = USART_Receive_int16(); 
    Serial.print("Received Value: ");
    Serial.println(receivedValue); 
    delay(50); // 송신기랑 delay값을 맞춰줘야 잘 출력되었음!
}
