
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include "USART_Interrupt.h"

#define SREG   _SFR_IO8(0x3F)

void GSM_Begin();
void GSM_Calling(char *);
void GSM_HangCall();
void GSM_Response();
void GSM_Response_Display();
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg();
void GSM_Msg_Display();
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char* , char*);
void GSM_Delete_All_Msg();

char buff[160];
char status_flag = 0;
volatile int buffer_pointer;
char Mobile_no[14];
char message_received[60];
int position = 0;

int gsm_year,gsm_month,gsm_date,gsm_hours,gsm_minutes,gsm_seconds;
char GSM_TIME_DATE[128];
char gsm_buffer[100];
char pkz[4];



void GSM_Begin()
{

	while(1)
	{
		lcdcmd(0xc0);
		USART_SendString("ATE0\r");
		_delay_ms(500);
		if(strstr(buff,"OK"))
		{
			GSM_Response();
			memset(buff,0,160);
			break;
		}
		else
		{
			lcd_line_one();
			lcd_string("Error",5);
		}
	}
	_delay_ms(1000);

	lcd_clear();
	lcd_line_one();
	lcd_string("Text Mode",9);
	lcdcmd(0xc0);
	USART_SendString("AT+CMGF=1\r");/* select message format as text */
	GSM_Response();
	_delay_ms(1000);
}

void GSM_Msg_Delete(unsigned int position)
{
	buffer_pointer=0;
	char delete_cmd[20];
	sprintf(delete_cmd,"AT+CMGD=%d\r",position);
	USART_SendString(delete_cmd);
}

void GSM_Delete_All_Msg()
{
	USART_SendString("AT+CMGDA=\"DEL ALL\"\r");
}

bool GSM_Wait_for_Msg()
{
	char msg_location[4];
	int i;
	_delay_ms(500);
	buffer_pointer=0;

	while(1)
	{
		if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== '\n')
		{
			buffer_pointer++;
		}
		else
		break;
	}

	if(strstr(buff,"CMTI:"))
	{
		while(buff[buffer_pointer]!= ',')
		{
			buffer_pointer++;
		}
		buffer_pointer++;

		i=0;
		while(buff[buffer_pointer]!= '\r')
		{
			msg_location[i]=buff[buffer_pointer];
			buffer_pointer++;
			i++;
		}
		position = atoi(msg_location);

		memset(buff,0,strlen(buff));
		buffer_pointer=0;

		return true;
	}
	else
	{
		return false;
	}
}
ISR(USART_RXC_vect)
{
	buff[buffer_pointer] = UDR;
	buffer_pointer++;
	status_flag = 1;
}


void GSM_Send_Msg(char *num,char *sms)
{
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num);
	USART_SendString(sms_buffer);
	lcd_clear();
	lcd_string("Sending...",10);
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(sms);
			USART_TxChar(0x1a);
			break;
		}
		buffer_pointer++;
	}
	lcd_line_two();
	lcd_string("Sent successful",16);
	_delay_ms(2000);
	lcd_clear();
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
}

void GSM_Calling(char *Mob_no)
{
	char call[20];
	sprintf(call,"ATD%s;\r",Mob_no);
	USART_SendString(call);
}

void GSM_HangCall()
{
	lcd_clear();
	USART_SendString("ATH\r");

}

void GSM_Response()
{
	unsigned int timeout=0;
	int CRLF_Found=0;
	char CRLF_buff[2];
	int Response_Length=0;
	while(1)
	{
		if(timeout>=60000)
		return;
		Response_Length = strlen(buff);
		if(Response_Length)
		{
			_delay_ms(2);
			timeout++;
			if(Response_Length==strlen(buff))
			{
				for(int i=0;i<Response_Length;i++)
				{
					memmove(CRLF_buff,CRLF_buff+1,1);
					CRLF_buff[1]=buff[i];
					if(strncmp(CRLF_buff,"\r\n",2))
					{
						if(CRLF_Found++==2)
						{
							GSM_Response_Display();
							return;
						}
					}

				}
				CRLF_Found = 0;

			}

		}
		_delay_ms(1);
		timeout++;
	}
	status_flag=0;
}

void GSM_Response_Display()
{
	buffer_pointer = 0;
	int lcd_pointer = 0;
	while(1)
	{
		if(buff[buffer_pointer]== '\r' || buff[buffer_pointer]== '\n')
		{
			buffer_pointer++;
		}
		else
		break;
	}


	lcdcmd(0xc0);
	while(buff[buffer_pointer]!='\r')
	{
		LCD_Char(buff[buffer_pointer]);
		buffer_pointer++;
		lcd_pointer++;
		if(lcd_pointer==15)
		lcdcmd(0x80);
	}
	buffer_pointer=0;
	memset(buff,0,strlen(buff));
}

void GSM_Msg_Read(int position)
{
	char read_cmd[10];
	sprintf(read_cmd,"AT+CMGR=%d\r",position);
	USART_SendString(read_cmd);
	GSM_Msg_Display();
}

void GSM_Msg_Display()
{
	_delay_ms(500);
	if(!(strstr(buff,"+CMGR")))
	{
		lcd_line_one();
		lcd_string("No message",10);
	}
	else
	{
		buffer_pointer = 0;

		while(1)
		{
			if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== 'n')
			{
				buffer_pointer++;
			}
			else
			break;
		}
		while(buff[buffer_pointer]!=',')
		{
			buffer_pointer++;
		}
		buffer_pointer = buffer_pointer+2;
		for(int i=0;i<=12;i++)
		{
			Mobile_no[i] = buff[buffer_pointer];
			buffer_pointer++;
		}

		do
		{
			buffer_pointer++;
		}while(buff[buffer_pointer-1]!= '\n');

		lcdcmd(0xC0);
		int i=0;
		while(buff[buffer_pointer]!= '\r' && i<31)
		{
			LCD_Char(buff[buffer_pointer]);
			message_received[i]=buff[buffer_pointer];

			buffer_pointer++;
			i++;
			if(i==16)
			lcdcmd(0x80);
		}

		buffer_pointer = 0;
		memset(buff,0,strlen(buff));
	}
	status_flag = 0;
}


void gsm_time()
{
	
	gsm_cmd("AT+CCLK?\r\n", 300,50);
	gsm_cmd("AT+CLTS=1\r\n",300,31);
	gsm_cmd("AT&W\r\n",300,31);
	gsm_cmd("AT+CCLK?\r\n", 300,50);
	delay(500);
	
	if(strstr((char *)gsm_buffer,"+CCLK:"))
	{
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[19],gsm_buffer[20]);
		gsm_year=(atoi(pkz));
		
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[22],gsm_buffer[23]);
		gsm_month=(atoi(pkz));
		
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[25],gsm_buffer[26]);
		gsm_date=(atoi(pkz));
		
		
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[28],gsm_buffer[29]);
		gsm_hours=(atoi(pkz));
		
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[31],gsm_buffer[32]);
		gsm_minutes=(atoi(pkz));
		
		memset(pkz,'\0',4);
		sprintf(pkz,"%c%c",gsm_buffer[34],gsm_buffer[35]);
		gsm_seconds=(atoi(pkz));
		UART4TransmitString("GSM Date & Time :",sizeof("GSM Date & Time :"));
		sprintf(GSM_TIME_DATE,"%02d.%02d.%02d : %02d/%02d/%02d\r\n",gsm_hours,gsm_minutes,gsm_seconds,gsm_date,gsm_month,gsm_year);
		UART4TransmitString(GSM_TIME_DATE,sizeof(GSM_TIME_DATE));
	}
	
	
}