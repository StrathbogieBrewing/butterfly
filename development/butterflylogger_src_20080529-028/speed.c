//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//***************************************************************************
//
//  File........: speed.c
//
//  Author(s)...: Nick Lott
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
//
//  Description.: Wind Speed monitoring routines for AVR Butterfly 
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20040419 - 1.0  - Created                                       - NAL
//  20050610 -        Ported to GCC									- NAL
//
//***************************************************************************


//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>

#include "main.h"
#include "speed.h"
#include "ADC.h"
#include "BCD.h"
#include "LCD_functions.h"
#include "timer0.h"
#include "usart.h"                       
#include "dataflash.h"

unsigned int glastSpeed; // last 1 second count
BOOL glogSpeed; // tell RTC to add to 60 sec log
unsigned int gSpeedSeconds; // no of seconds counted
unsigned int gSpeedTotal; // used for counting the wind events
unsigned int gSpeedSecondsToLog;
extern BOOL gLogNow;

/******************************************************************************
*
*   Function name:  Speed_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Start Timer/Counter1 using a
*                   T1 pin with falling edge
*
*******************************************************************************/
void Speed_init(void)
{
    cli(); // mt __disable_interrupt();  // disable global interrupt

	// Mode: Normal top=FFFFh
	// OC1A output: Discon.
	// OC1B output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge

	TCCR1A=0x00;		
	TCCR1B=(1<<CS12)|(1<<CS11); 	// Clock source: T1 pin Falling Edge
	TCNT1H=0x00;
	TCNT1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x00;
	OCR1BH=0x00;
	OCR1BL=0x00;
	
	gSpeedSecondsToLog= DEFAULT_SECONDSTOLOG;
	DDRG	&= ~(1<<PG3);				//Set to input
	
	//enable pull up on PG3 ( remember to remove R200 )
#if SPEED_EN_INTERNALPULLUP
	PORTG	|= (1<<PG3);				//Enable Internal Pullup
#endif
    sei(); // __enable_interrupt();  // enable global interrupt
}


#if !NO_MENUS
/*****************************************************************************
*
*   Function name : SpeedFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Veiw current windspeed (per second)
*
*****************************************************************************/
char SpeedFunc(char input)
{
    static char enter = 1;
    
    if (enter)
    {
        enter = 0;
        // Enable auto-run of the ShowSpeed function every 10ms 
        // (it will actually be more than 10ms because of the SLEEP)      

	// init Windspeed counter and disable the t1 segments 
	// on LCD if not already dont at startup	    
#if	!EN_LOG_SPEED
	    gSpeedTotal =0;
	    glastSpeed =0;
	    Speed_init();				

	    //	apart from seg24 which is used for timer/counter 1
	    LCDCRB = (1<<LCDCS) | (3<<LCDMUX0) | (6<<LCDPM0);

#endif    
	    Timer0_RegisterCallbackFunction(ShowSpeed);        
    }
    else
        LCD_UpdateRequired(TRUE, 0); 

    if (input == KEY_PREV)
    {
        // Disable the auto-run 
        Timer0_RemoveCallbackFunction(ShowSpeed);
	//re enable the t1 segments
#if	!EN_LOG_SPEED    
		LCDCRB = (1<<LCDCS) | (3<<LCDMUX0) | (7<<LCDPM0);
#endif
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
        
        return ST_SPEED;
    }
    else
        return ST_SPEED_FUNC;    
}    
#endif

/*****************************************************************************
*
*   Function name : PrintSpeed
*
*   Returns :       nothing
*
*   Parameters :    int speed
*
*   Purpose :       log the total to flash after 60 seconds
*
*****************************************************************************/
void PrintSpeed(unsigned int Speed)
{
	unsigned long Temp;
	char D[6],i;
	
	Temp = int2BCD5(Speed);                
	D[0] = '0';
	
	for (i=5;i;i--){
		D[(int)i] = (Temp & 0x0F) + '0';       
	//	Not needed when using BCD5
	//	if(D[(int)i]  > '9')        // if the hex-value is over 9, add 7 in order to go 
	//		D[(int)i]  += 7;        // jump to the character in the ASCII-table
		(Temp >>= 4);
	}

	if(D[0]  > '9')		// if the hex-value is over 9, add 7 in order to go 
		D[0]  += 7;        // jump to the character in the ASCII-table
	
	for (i=0;i<6;i++){
		USART_Tx(D[(int)i]);
	}
	
	USART_Tx(' ');    

}

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : ShowSpeed
*
*   Returns :       nothing
*
*   Parameters :    nothing
*
*   Purpose :       Show speed (last 1 sec) as rpm on LCD
*
*****************************************************************************/
void ShowSpeed(void)
{
	unsigned long Temp;
	char D[3],i;//,unit;	
	
	 if (!gLogNow) // don't read while log in progress
	{
		Temp = int2BCD5(glastSpeed);                
		
		if(glastSpeed>255){// overflow
			LCD_putc(0, 'O');
			LCD_putc(1, 'V');
			LCD_putc(2, 'R');
			
			LCD_putc(3, 'F');
			
			LCD_putc(4, 'L');
			LCD_putc(5, 'W');
			LCD_putc(6, '\0');
		 }else{
			 for (i=3;i;i--){				
				 D[(int)i-1] = (Temp & 0x0F) + '0';       
				 (Temp >>= 4);
			 }
			 
			 LCD_putc(0, D[0]);
			 LCD_putc(1, D[1]);
			 LCD_putc(2, D[2]);
			 
			 LCD_putc(3, ' ');
			 
			 LCD_putc(4, 'H');
			 LCD_putc(5, 'Z');
			 LCD_putc(6, '\0');
		 }
		
		 

		
		 
	}
}
#endif
