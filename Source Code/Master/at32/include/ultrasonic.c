
#define US_PORT PORTC
#define	US_PIN	PINC
#define US_DDR 	DDRC

#define US_TRIG_POS	PC2
#define US_ECHO_POS	PC3


#define US_ERROR		-1
#define	US_NO_OBSTACLE	-2

int distance, previous_distance;




void HCSR04Init();
void HCSR04Trigger();

void lcd_command( unsigned char );

void HCSR04Init()
{
	
	
	US_DDR|=(1<<US_TRIG_POS);
}

void HCSR04Trigger()
{
	
	US_PORT|=(1<<US_TRIG_POS);	//high
	
	_delay_us(15);				//wait 15uS
	
	US_PORT&=~(1<<US_TRIG_POS);	//low
}

uint32_t GetPulseWidth()
{
	
	uint64_t i,result;

	for(i=0;i<600000;i++)
	{
		if(!(US_PIN & (1<<US_ECHO_POS)))
		continue;
		else
		break;
	}

	if(i==600000)
	return US_ERROR;
	
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;

	
	for(i=0;i<600000;i++)
	{
		if(US_PIN & (1<<US_ECHO_POS))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
		break;
	}

	if(i==600000)
	return US_NO_OBSTACLE;

	result=TCNT1;
	TCCR1B=0x00;

	if(result > 60000)
	return US_NO_OBSTACLE;
	else
	return (result>>1);
}
