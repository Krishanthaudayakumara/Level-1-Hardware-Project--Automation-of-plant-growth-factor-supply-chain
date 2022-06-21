

#include "USART_Interrupt.h"

void USART_Init(unsigned long BAUDRATE)
{
	UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL)| (1 << UCSZ0) | (1 << UCSZ1);
	UBRRL = BAUD_PRESCALE;
	UBRRH = (BAUD_PRESCALE >> 8);
}

char USART_RxChar()
{
	while (!(UCSRA & (1 << RXC)));
	return(UDR);
}

void USART_TxChar(char data)
{
	UDR = data;
	while (!(UCSRA & (1<<UDRE)));
}

void USART_SendString(char *str)
{
	int i=0;
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);
		i++;
	}
}