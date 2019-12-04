#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define entrada PORTF0 // Arduino - A0


#define set_bit(reg,bit) (reg |= (1<<bit))
#define reset_bit(reg, bit) (reg &= ~(1<<bit))
#define myDigitalWrite(reg, bit, level) ((level == 1) ? set_bit(reg,bit) : reset_bit(reg,bit))
#define myDigitalRead(pino, bit) (pino & (1<<bit) ? 1 : 0)


void mySerialBegin(int baudRate) {

  UBRR0H = ((((F_CPU / 16 / baudRate)) - 1) >> 8);
  UBRR0L = (((F_CPU / 16 / baudRate)) - 1);


  UCSR0B = (1 << TXEN0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); // habilita R e T

  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // dados de 8 bits
}

void mySerialPrint(char* data)
{
  while (!(UCSR0A & (1 << UDRE0)));
  for (int i = 0; i < strlen(data); i++)
  {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data[i];
  }
  while (!(UCSR0A & (1 << UDRE0)));
}

void adc_init()
{
  // AREF = AVcc
  ADMUX = (1 << REFS0);

  // ADC Enable and prescaler of 128
  // 16000000/128 = 125000
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint32_t myAnalogRead(uint8_t ch)
{
  ch &= 0b00000111;
  ADMUX = (ADMUX & 0xF8) | ch;

  // start single convertion
  // write ’1′ to ADSC
  ADCSRA |= (1 << ADSC);

  // wait for conversion to complete
  // ADSC becomes ’0′ again
  // till then, run loop continuously
  while (ADCSRA & (1 << ADSC));

  return (ADC);
}

int main() {

  mySerialBegin(9600);

  sei();
  adc_init();

  //Entradas Analogicas
  reset_bit(DDRF, entrada);

  uint32_t potenciometro;
  char result[10];
  while (1) {

    //Leitura do Valor de Ignicao
    potenciometro = (3* myAnalogRead(0)) / 512; // Varia de 0 a 5
    sprintf(result, "%d \n", potenciometro);
    mySerialPrint(result);
  }
}
