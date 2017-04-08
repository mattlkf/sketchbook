
/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUD 115200
#include <util/setbaud.h>

void uart_init(void) {
   UBRR0H = UBRRH_VALUE;
   UBRR0L = UBRRL_VALUE;

#if USE_2X
   UCSR0A |= _BV(U2X0);
#else
   UCSR0A &= ~(_BV(U2X0));
#endif

   UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
   UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c) {
   loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
   UDR0 = c;
}

char uart_getchar(void) {
   loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
   return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


int main(void)
{

   /* Setup serial port */
   uart_init();
   stdout = &uart_output;
   stdin  = &uart_input;

//   char input;
   char buf[4];

   // Setup ports
//   DDRB |= (1<<1) | (1<<0);
   DDRB |= (1 << DDB1) | (1 << DDB2); // Output PB1, PB2
   PORTB |= (1<<0);
   PORTB &= ~(1<<1);

//   TCCR1A |= _BV(COM1A1) | _BV(WGM10);
//   TCCR1B |= _BV(CS10) | _BV(WGM12);

   ICR1 = 0xFFFF;

   TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
   TCCR1A |= (1 << WGM11);
   TCCR1B |= (1 << WGM12) | (1 << WGM13);

   TCCR1B |= (1 << CS10);

   uint8_t pwm = 0xBF;
   OCR1A = (pwm << 8) | 0xFF;

   //OCR1A = 0xBFFF;

   /* Print hello and then echo serial
   ** port data while blinking LED */
   printf("Hello world!\r\n");
   while(1) {
      //input = getchar();
	  fgets(buf, 4, stdin);
	  pwm = (uint8_t) atoi(buf);
	  OCR1A = (pwm << 8) | 0xFF;
	  printf("You wrote %d\r\n", (int)pwm);
      //printf("You wrote %u\r\n", (unsigned int) input);
      //PORTB ^= 0x01;
   }

}
