#include <stdio.h>
#include <math.h>

#include<avr/io.h>

//scankey

char keycheck()

{
	PORTD=0b11101111;

	_delay_ms(10);

	if((PIND&0b00000001)==0)
	{
		return '1';
	}
	if((PIND&0b00000010)==0)
	{
		return '2';
	}
	if((PIND&0b00000100)==0)
	{
		return '3';
	}

	PORTD=0b11011111;

	_delay_ms(10);

	if((PIND&0b00000001)==0)
	{
		return '4';
	}

	if((PIND&0b00000010)==0)
	{
		return '5';
	}

	if((PIND&0b00000100)==0)
	{
		return '6';
	}

	PORTD=0b10111111;

	_delay_ms(10);

	if((PIND&0b00000001)==0)
	{
		return '7';
	}

	if((PIND&0b00000010)==0)
	{
		return '8';
	}

	if((PIND&0b00000100)==0)
	{
		return '9';
	}

	PORTD=0b01111111;

	_delay_ms(10);

	if((PIND&0b00000001)==0)
	{
		return '*';
	}

	if((PIND&0b00000010)==0)
	{
		return '0';
	}

	if((PIND&0b00000100)==0)
	{
		return '#';
	}

	return 'a';

}


char scankey()
{
	char key='a';

	while(key=='a')
	{
		key=keycheck();
	}
	return key;
}
