/*
 Copyright (c) 2005,2006, Nick Lott
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of 
 this software and associated documentation files (the "Software"), to deal in 
 the Software without restriction, including without limitation the rights to 
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all 
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/* set alarm fuinctions
*/
#include "main.h"
#include "alarm.h"
#include "usart.h"

char ALARM_enabled;

char ALARM_year;
char ALARM_month;
char ALARM_day;
char ALARM_hour;
char ALARM_min;
char ALARM_second;

void ALARM_set(char year, char month, char day, char hour, char min, char second){
	 ALARM_year=year;
	 ALARM_month=month;
	 ALARM_day=day;
	 ALARM_hour=hour;
	 ALARM_min=min;
	 ALARM_second=second;
	 ALARM_enabled=TRUE;
}

char ALARM_check(char year, char month, char day, char hour, char min, char second){
	if(ALARM_enabled){	
		if(	ALARM_year==year && 
			ALARM_month==month &&
			ALARM_day==day &&
			ALARM_hour==hour &&
			ALARM_min==min &&
			ALARM_second==second){
			return TRUE;
		}
	}
	return FALSE;
}

void ALARM_print_status(void){
	USART_Tx_Bool(ALARM_enabled);
	return;
}

void ALARM_print(void){
	USART_Tx_Byte(ALARM_hour);	USART_Tx(':');
	USART_Tx_Byte(ALARM_min);	USART_Tx(':');
	USART_Tx_Byte(ALARM_second);USART_Tx(' ');
	USART_Tx_Byte(ALARM_day);	USART_Tx('/');
	USART_Tx_Byte(ALARM_month);	USART_Tx('/');
	USART_Tx_Byte(ALARM_year);	
}

/* Disable the alarm */
void ALARM_clear(void){
	ALARM_enabled=FALSE;
}
