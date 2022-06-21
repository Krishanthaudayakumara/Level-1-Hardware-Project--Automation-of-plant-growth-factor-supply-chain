 

#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>

#define SREG   _SFR_IO8(0x3F)

#include<avr/io.h>

#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "include/lcd.c"
#include "include/keypad.c"
#include "include/ultrasonic.c"
#include "include/ultrasonic_2.c"
#include "include/dht11.c"
#include "include/ldr.c"
#include "include/spi_master.c"
#include "include/functions.c"
#include "include/I2C_Master_H_file.h"
#include "include/I2C_Master_C_file.c"

int ldrconfig();
int get_plantcount();
void water_level();
void nt_level();
void dht11_output();
void displayTime();
void saveEEP();
void initEEP();
void my_delay_ms(int ms);


int savedDate, savedHour, startHour, memoryLocation,count = 4, avgTemp = 0, avgHum = 0, LDRthres;
float WmotorTime = 6000, FmotorTime = 6000;



int main(void)

{

	DDRA=0xff;
	DDRB=0x07;
	DDRD=0xf0;
	DDRC=0xFF;

	char buffer[5];
	char key;
	int wtrperPlant, fertilizerPlant, humMin = 0, humMax, temprtMin, temprtMax, plantCount;
	

	lcdint();
	initEEP();
	
	SPImstr_init();
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER 1 TO",10);
	lcd_line_two();
	lcd_string("Configure system",16);
	_delay_ms(1200);
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER 2 TO",10);
	lcd_line_two();
	lcd_string("Activate system",16);
	_delay_ms(1200);

	
	while(1)
	{
		
		lcd_clear();
		lcd_string("ENTER COMMAND:",15);
		key = scankey();

		
		switch(key)
		{	
			case '1':
				lcd_clear();
				lcd_string("Configure SYS",12);
				plantCount = get_plantcount();
				
				wtrperPlant = waterPerPlant();
				fertilizerPlant = fertilizePerPlant();
				humMin = humidityMin();
				humMax = humidityMax();
				temprtMin = tempMin();
				temprtMax = tempMax();
				WmotorTime = (float)wtrperPlant/27.0*plantCount;
				FmotorTime = (float)fertilizerPlant/27.0 * plantCount;
				/* SPI_write(80);

				LDRthres = getLDR();
				SPI_write(LDRthres);
				*/

				break;
			case '2':
				
			while(1){
				if(!humMin == 0) {
					dht11_output(humMin,humMax,temprtMin,temprtMax);

				} else dht11_output(75,85,23,29);
				water_level();
				_delay_ms(500);
				nt_level();
				_delay_ms(1000);
				displayTime();
				_delay_ms(200);
				saveEEP();
				_delay_ms(300);

			}
				break;
				
			case '3':
				if(!humMin == 0) {
					dht11_output(humMin,humMax,temprtMin,temprtMax);

				} else dht11_output(75,85,23,29);
			break;		
			
			case '4':
				water_level();
				_delay_ms(500);
			break;
				
			case '5':
				nt_level();
				_delay_ms(1000);
			break;
			
			case '6':
				displayTime();
				_delay_ms(200);
			case '7':
				SPI_write(1);
			
			default:
			break;
			
			}				
		}
		
}
	
	




int displayKey()
{
	int tmpNum;
	int data[10];
	int i = 0;
	int num = 0;
	while(1)
	{
		key=scankey();
		lcddata(key);
		if(key == '#') break;
		tmpNum = charToInt(key);
		data[i] = tmpNum;
		i++;
		_delay_ms(300);
	}
	int k = i-1;
	
	for(int j=0;i>j;j++){
		num += data[j]*pow(10,k);
		k--;
	}
	
	lcdcmd(0x01);
	
	return num;
}


int get_plantcount(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER PLANT",11);
	lcd_line_two();
	lcd_string("COUNT: ",7);
	key = scankey();
	int plantCount = displayKey();
	
	char text[10];
	sprintf(text, "%d", plantCount);
	
	lcdcmd(0x01);
	lcd_string("PLANT COUNT IS",15);
	lcdcmd(0xC0);
	lcd_string(text,10);
	_delay_ms(1000);
	return plantCount;
}

void water_level(){
	char numberString[4];
	uint8_t spiSignal = 2;

	while(1) {
		
		uint16_t r;
		
		_delay_ms(100);

		lcd_clear();
		
		
		HCSR04Init();

		while(1)
		{
			HCSR04Trigger();
			r=GetPulseWidth();
			if(r==US_ERROR)
			{
				lcd_clear();
				lcd_string("E!",2);
				_delay_ms(1000);
				
			}
			else
			{
				
				distance=50-(r*0.034/2.0);
				
				
				
				if (distance != previous_distance)
				{
					lcd_clear();
				}
				
				
				
				lcd_line_one();
				lcd_string("WATER LV =",11);
				
				itoa(distance, numberString, 10);
				lcd_string(numberString,3);

				lcd_string("cm",2);
				
				
				previous_distance = distance;
				_delay_ms(30);
				
				if(distance<10)
				{
					lcd_line_two();
					lcd_string("WATER LOW!!!",13);
					_delay_ms(500);
					SPI_write(spiSignal);
					break;
					
				}
				if(distance>=10)
				{
					lcd_line_two();
					lcd_string("ENOUGH WATER",12);
					break;
					
				}
				key = scankey();
				break;
				_delay_ms(1200);
				
			}
			break;
		}
		
		break;
	}
}


void nt_level(){
	char numberString[4];
	uint8_t spiSignal = 3;

	while(1) {
		
		uint16_t r;
		
		_delay_ms(100);

		lcd_clear();
		
		
		HCSR04Init_2();

		while(1)
		{
			HCSR04Trigger_2();
			r=GetPulseWidth_2();
			if(r==US_ERROR)
			{
				lcd_clear();
				lcd_string("E!",2);
				_delay_ms(1000);
				
			}
			else
			{
				
				distance=50-(r*0.034/2.0);
				
				
				
				if (distance != previous_distance)
				{
					lcd_clear();
				}
				
				
				
				lcd_line_one();
				lcd_string("NT LV =",8);
				
				itoa(distance, numberString, 10);
				lcd_string(numberString,3);

				lcd_string("cm",2);
				
				
				previous_distance = distance;
				_delay_ms(30);
				
				if(distance<10)
				{
					lcd_line_two();
					lcd_string("NT LEVEL LOW!!!",15);
					_delay_ms(500);
					SPI_write(spiSignal);
					break;
					
				}
				if(distance>=10)
				{
					lcd_line_two();
					lcd_string("ENOUGH NT LEVEL",16);
					break;
					
				}
				key = scankey();
				break;
				_delay_ms(1200);
				
			}
			break;
		}
		
		break;
	}
}

void dht11_output(int humMin, int humMax, int temprtMin, int temprtMax){
	
	char data[5];
	DDRC=0xff;
	while(1)
	{
		lcd_clear();
		lcd_line_one();
		Request();
		Response();
		I_RH=Receive_data();
		D_RH=Receive_data();
		I_Temp=Receive_data();
		D_Temp=Receive_data();
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			lcd_string("Error",5);
		}
		
		else
		{
			itoa(I_RH,data,10);
			lcd_string("H=",2);
			lcd_string(data,2);
			lcd_string(".",1);
			
			itoa(D_RH,data,10);
						
			lcd_string(data,2);
			lcd_string("%",1);

						
			itoa(I_Temp,data,10);
			lcd_string("T=",2);
			lcd_string(data,2);
			lcd_string(".",1);
			
			itoa(D_Temp,data,10);
			lcd_string(data,2);
			lcddata(0xDF);
			lcd_string("C ",2);
						
			lcd_line_two();
		}
		
		_delay_ms(500);
		
		if ((I_Temp + D_Temp) <= temprtMin)
		{
			lcd_string("H ON  ",6);
			SPI_write((uint8_t)71);
			
			
		} else {
			SPI_write((uint8_t)70);
		}
		
		
		
		if ((I_Temp + D_Temp) >= temprtMax)
		{
			lcd_string("FA ON  ",7);
			SPI_write((uint8_t)4);
			
	}else{
			SPI_write((uint8_t)5);

	}
		if ((I_RH + D_RH ) <= humMin)
		{
			lcd_string("FO ON ",7);
			_delay_ms(1000);
			SPI_write((uint8_t)61);
		}
		
		else 
		{
			
			lcd_string("FO OFF",7);
			_delay_ms(1000);
			SPI_write((uint8_t)60);
			
		}
		_delay_ms(2000);
		break;
		
	}
}

void displayTime(){
	int motor=0;
	char buffer[20];
	char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	

	I2C_Init();
	lcdint();

	DDRD |= 1 << PIND0;
	
	
	while(1)
	{
		RTC_Read_Clock(0);
		if (hour & TimeFormat12)
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			if(IsItPM(hour))
			strcat(buffer, "PM");
			else
			strcat(buffer, "AM");
			lcd_string(buffer,2);
		}
		
		else
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour), minute, second);
			

			lcd_line_one();
			lcd_string("Time:",5);
			lcd_string(buffer,9);
			
			int time;
			if (hour == 0b00010001) {
				PORTC= PORTC | (1<<6);
				lcd_line_two();
				lcd_string("Water Motor ON",16);
				time = (int) WmotorTime;
				my_delay_ms(time);
				PORTC= PORTC &(~(1<<6)) & (~(1<<7));
				PORTC= PORTC | (1<<7);
				lcd_line_two();
				lcd_string("NT Motor ON",12);
				time = (int) FmotorTime;
				my_delay_ms(time);
				PORTC= PORTC &(~(1<<6)) & (~(1<<7));

				
			}
			
			
			else {
				lcd_line_two();
				//lcd_string("Motor Off",12);
				
			}
			
		}
		break;
	}
}

void initEEP(){
	eeprom_busy_wait();
	savedDate = eeprom_read_byte((uint8_t *)0);
	startHour = eeprom_read_byte((uint8_t *)1);
	if (savedDate == 255)
		eeprom_write_byte((uint8_t *)0,(uint8_t)0);	
	if (startHour == 255)
		eeprom_write_byte((uint8_t *)1,(uint8_t)0);

	
}

void saveEEP(){
	RTC_Read_Clock(0);
	RTC_Read_Calendar(3);
	savedHour = eeprom_read_byte((uint8_t *)2);
	eeprom_write_byte((uint8_t *)1,(uint8_t)hour);

	if(savedHour == !hour){
	eeprom_write_byte((uint8_t *)2,(uint8_t)hour);
	eeprom_write_byte((uint8_t *)count,(uint8_t)I_RH);
	eeprom_write_byte((uint8_t *)count+25,(uint8_t)I_Temp);
	eeprom_write_byte((uint8_t *)2,(uint8_t)hour);
	eeprom_write_byte((uint8_t *)3,(uint8_t)count);
	count++;
	
}
	memoryLocation = eeprom_read_byte((uint8_t *)3);
	if (memoryLocation == 255)
	{
		avgHum = I_RH;
		avgTemp = I_Temp;
	}
	else{
		int totHum = 0, totTemp = 0;
		for (int i = 4; i>= memoryLocation; i++)
		{
			int svdHum = eeprom_read_byte((uint8_t *)i);
			int svdTemp = eeprom_read_byte((uint8_t *)i+25);
			totHum+=svdHum;
			totTemp+=svdTemp;
			avgHum = totHum/(i-3);
			avgTemp = totTemp/(i-3);
		}
	}
	char buff[5];
	lcd_clear();
	lcd_line_one();
	lcd_string("Avg Tem:",8);
	sprintf(buff,"%d",avgTemp);
	lcd_string(buff,3);
	lcd_line_two();
	lcd_string("Avg Hum:",8);
	sprintf(buff,"%d",avgHum);
	lcd_string(buff,3);
	
}

void my_delay_ms(int ms)
{
	while (0 < ms)
	{
		_delay_ms(1);
		--ms;
	}
}