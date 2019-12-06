#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define valvula1 PORTF1 // Arduino - A0
#define valvula2 PORTF2 // Arduino - A1
#define led PORTD7 // Arduino - 38
#define buzzer PORTB4 // Arduino - 10
#define chave PORTB3 // Arduino - 50


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

uint16_t myAnalogRead(uint8_t ch)
{
  // select the corresponding channel 0~7
  // ANDing with ’7′ will always keep the value
  // of ‘ch’ between 0 and 7
  ch &= 0b00000111;  // AND operation with 7
  ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing

  // start single convertion
  // write ’1′ to ADSC
  ADCSRA |= (1 << ADSC);

  // wait for conversion to complete
  // ADSC becomes ’0′ again
  // till then, run loop continuously
  while (ADCSRA & (1 << ADSC));

  return (ADC);
}

bool Beep = false;
int timeSleep = 0;

int main() {
  mySerialBegin(9600);

  sei();

  reset_bit(DDRF, valvula1);
  reset_bit(DDRF, valvula2);
  reset_bit(DDRB, chave);
  set_bit(PORTB, chave);
  set_bit(DDRD, led);
  set_bit(DDRB, buzzer);

  adc_init();

  //Beep
  TCCR1A = 0;
  TCNT1 = 0;
  OCR1A = 4992;// Compara com 4992 = 100 ms
  TCCR1B = _BV(WGM12) | _BV(CS10) | _BV(CS12); // modo CTC, prescaler 1024
  TIMSK1 = _BV(OCIE1A);

  //SET PWM
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  float va1, va2, valorVin, valorVout, nivel = 50.0, soma, decNivel, decValorVin, decValorVout;
  char result[100];

  while (1) {
    va1 = myAnalogRead(0);
    valorVin = va1/1023;
    va2 = myAnalogRead(1);
    valorVout = va2/1023;
    if (nivel < 100) {
      
      soma = nivel + valorVin - valorVout;
      
      nivel = (soma>100) ? 100 : ((soma<0) ? 0 : soma);

    }
    else {
      if (valorVout > valorVin) {
        nivel = nivel + valorVin - valorVout;
      }
    }

    myDigitalWrite(PORTD, led, ((nivel==100) ? 1 : ((Beep & (nivel>80) & (nivel<100)) ? 1 : 0)));
    OCR2A = (nivel==100) ? 200 : ((Beep & (nivel>80)) ? 200 : 0);
    
    decNivel = nivel*100;
    decNivel = int(decNivel) - (int(nivel)*100);
    
    decValorVin = valorVin*100;
    decValorVin = int(decValorVin) - (int(valorVin)*100);
    
    decValorVout = valorVout*100;
    decValorVout = int(decValorVout) - (int(valorVout)*100);
    
    sprintf(result, "/%d.%02d/%d.%02d/%d.%02d/\n", (int)nivel, (int)decNivel, (int)valorVin, (int)decValorVin, (int)valorVout, (int)decValorVout);
    mySerialPrint(result);
    //delay(1000);
  }
  /*timeSleep = 0;
  while(timeSleep < 5);*/

}

ISR(TIMER1_COMPA_vect)
{
  Beep = !Beep;
  timeSleep++;
}
