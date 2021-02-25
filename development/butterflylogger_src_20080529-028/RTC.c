//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//*****************************************************************************
//
//  File........: RTC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
//
//  Description.: Real Time Clock (RTC)
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//*****************************************************************************

//  Include files
//mtA
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
//mtE
#include <inttypes.h>
#include "ADC.h"
#include "usart.h"  
#include "speed.h"
#include "main.h"
#include "RTC.h"
#include "LCD_functions.h"
#include "BCD.h"
#include "alarm.h"

/*
#define HOUR       0
#define MINUTE     1
#define SECOND     2
#define	TICK	   3
 */

#define YEAR        0
#define MONTH       1
#define DAY         2

extern unsigned int glastSpeed;      // last 1 second count
extern char glogSpeed;		     // tell RTC to add to 60 sec log
extern unsigned int gSpeedSeconds;   // no of seconds counted
extern unsigned int gSpeedTotal;     // used for counting the wind speed events
extern BOOL gLogging;		// Logging is enabled
extern BOOL gLogNow;		// Log as soon as we return to main loop

char gLogTimeCounter=0;
char gLogTimeUnit=MINUTE;
char gLogTime=10;			//default to logging every 10 minutes

uint8_t	gTICK;
uint8_t gSECOND;
uint8_t gMINUTE;
uint8_t gHOUR;
uint8_t gDAY;
uint8_t gMONTH;
uint8_t gYEAR;

// mtA
//char gPowerSaveTimer = 0;
//char dateformat = 0;
volatile uint8_t gPowerSaveTimer = 0;
uint8_t dateformat = 0;
// mtE

// Lookup table holding the length of each mont. The first element is a dummy.
// mt this could be placed in progmem too, but the arrays are accessed quite
//    often - so leaving them in RAM is better...
char MonthLength[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#ifdef CLOCK_12_ENABLE
char TBL_CLOCK_12[] =   // table used when displaying 12H clock  
{12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
char clockformat = CLOCK_24;    // set initial clock format to 24H
#endif


#ifndef MINIMAL_MENU
	// different date formates (text only)
	// mtA
	//__flash char EUROPEAN_DATE_TEXT[] =   "DDMMYY";
	//__flash char AMERICAN_DATE_TEXT[] =   "MMDDYY";
	//__flash char CANADIAN_DATE_TEXT[] =   "YYMMDD"; 
	const char EUROPEAN_DATE_TEXT[] PROGMEM =   "DDMMYY";
	#ifdef DATE_MULTIFORMAT
	const char AMERICAN_DATE_TEXT[] PROGMEM =   "MMDDYY";
	const char CANADIAN_DATE_TEXT[] PROGMEM =   "YYMMDD"; 
	#endif
	// mtE


	// different date formates, table for putting DD, MM and YY at the right place
	// on the LCD
	//mtA
	//__flash char EUROPEAN_DATE_NR[] =   { 4, 5, 2, 3, 0, 1 };
	//__flash char AMERICAN_DATE_NR[] =   { 4, 5, 0, 1, 2, 3 };
	//__flash char CANADIAN_DATE_NR[] =   { 0, 1, 2, 3, 4, 5 }; 
	const char EUROPEAN_DATE_NR[] PROGMEM =   { 4, 5, 2, 3, 0, 1 };
	#ifdef DATE_MULTIFORMAT
	const char AMERICAN_DATE_NR[] PROGMEM =   { 4, 5, 0, 1, 2, 3 };
	const char CANADIAN_DATE_NR[] PROGMEM =   { 0, 1, 2, 3, 4, 5 }; 
	#endif
	//mtE

	//mtA
	//__flash char __flash *DATEFORMAT_TEXT[] = {EUROPEAN_DATE_TEXT, AMERICAN_DATE_TEXT, CANADIAN_DATE_TEXT};
	//__flash char __flash *DATE_FORMAT_NR[] = {EUROPEAN_DATE_NR, AMERICAN_DATE_NR, CANADIAN_DATE_NR};
	#ifdef DATE_MULTIFORMAT
	PGM_P DATEFORMAT_TEXT[] = {EUROPEAN_DATE_TEXT, AMERICAN_DATE_TEXT, CANADIAN_DATE_TEXT};
	#else
	PGM_P DATEFORMAT_TEXT[] = {EUROPEAN_DATE_TEXT};
	#endif
	// mt this should be: const uint8_t *DATE_FORMAT_NR[] PROGMEM = {EUROPEAN_DATE_NR, AMERICAN_DATE_NR, CANADIAN_DATE_NR};
	// but I keep the array in ram for now TODO
	#ifdef DATE_MULTIFORMAT
	const char *DATE_FORMAT_NR[]  = {EUROPEAN_DATE_NR, AMERICAN_DATE_NR, CANADIAN_DATE_NR};
	#else
	const char *DATE_FORMAT_NR[]  = {EUROPEAN_DATE_NR};
	#endif
	//mtE

#endif //minimal_menu

void check_log(void); //increment log timer and check to start logging


/******************************************************************************
*
*   Function name:  RTC_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Start Timer/Counter2 in asynchronous operation using a
*                   32.768kHz crystal.
*
*******************************************************************************/
void RTC_init(void)
{
    Delay(1000);            // wait for 1 sec to let the Xtal stabilize after a power-on,
    
    cli(); // mt __disable_interrupt();  // disabel global interrupt
    
    cbi(TIMSK2, TOIE2);             // disable OCIE2A and TOIE2
    
    ASSR = (1<<AS2);        // select asynchronous operation of Timer2
    
    TCNT2 = 0;              // clear TCNT2A
	
#ifdef FASTCLOCK
	//WARNING!!! CLOCK IS NOW 16X faster :)
	TCCR2A = (0<<CS22) | (1<<CS21) | (0<<CS20);             // select precaler: 32.768 kHz / 8 = 1/16 sec between each overflow
	
	// uncomment next line for 128Hz Tick.
	//	TCCR2A = (0<<CS22) | (0<<CS21) | (1<<CS20);             // select precaler: 32.768 kHz / 1 = 1/128 sec between each overflow
	
#else
	TCCR2A = (1<<CS22) | (0<<CS21) | (1<<CS20);             // select precaler: 32.768 kHz / 128 = 1 sec between each overflow
#endif
	
	
    while((ASSR & 0x01) | (ASSR & 0x04));       // wait for TCN2UB and TCR2UB to be cleared
    
    TIFR2 = 0xFF;           // clear interrupt-flags
    sbi(TIMSK2, TOIE2);     // enable Timer2 overflow interrupt
    
    sei(); // mt __enable_interrupt();                 // enable global interrupt
    
    // initial time and date setting
	gTICK	 = 0;
    gSECOND  = 0;
    gMINUTE  = 0;
    gHOUR    = 12;
    // mt release timestamp
    gDAY     = 05;
    gMONTH   = 12;
    gYEAR    = 3;
}

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : ShowClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the clock on the LCD
*
*****************************************************************************/
char ShowClock(char input)
{
    //char HH, HL, MH, ML, SH, SL;
    uint8_t HH, HL, MH, ML, SH, SL;

#ifdef CLOCK_12_ENABLE
    if (clockformat == CLOCK_12)    // if 12H clock
        HH = CHAR2BCD2(TBL_CLOCK_12[gHOUR]);   
    else
#endif
        HH = CHAR2BCD2(gHOUR);
    
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';
    
    MH = CHAR2BCD2(gMINUTE);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';
    
    SH = CHAR2BCD2(gSECOND);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';
    
    LCD_putc(0, HH);
    LCD_putc(1, HL);
    LCD_putc(2, MH);
    LCD_putc(3, ML);
    LCD_putc(4, SH);
    LCD_putc(5, SL);
    LCD_putc(6, '\0');
    
    LCD_Colon(1);
    
    LCD_UpdateRequired(TRUE, 0);
    
    if (input == KEY_PREV)
        return ST_TIME_CLOCK;
    else if (input == KEY_NEXT)
        return ST_TIME_CLOCK_ADJUST;
    
    return ST_TIME_CLOCK_FUNC;
}
#endif

/*****************************************************************************
*
*   Function name : PrintClock
*
*   Returns :       None
*
*   Parameters :    time in 3 byte binary format
*
*   Purpose :       Prints a time to the uart
*
*****************************************************************************/
void PrintClock(char t0,char t1, char t2)
{
    
	// Clock format from dataflash storage
	/*
	 t0[0..6] = ticks
	 t0[7],t1[0..4] = Seconds
	 t1[5..7],t2[0..2]= Minutes
	 t2[3..7] =Hours
	 */
	
	// Print Hours
	USART_Tx_Byte(t2>>3);
	USART_Tx(':');
	
    // Print Minutes
	USART_Tx_Byte(( (t2 & 0x07) << 3) + (t1>>5) );
	USART_Tx(':');
    
	// Print Seconds
	USART_Tx_Byte( ( (t1 & 0x1F) << 1) + (t0 >>7) );
	USART_Tx('.');
	
	// Print Ticks
	USART_Tx_Byte(t0 & 0x7f);

	
}


/*****************************************************************************
*
*   Function name : LogClock
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Logs the time to dataflash in a 3 byte binary format
*
*****************************************************************************/
void LogClock(void)
{
	char t0,t1,t2;
	
	// Clock format for dataflash storage
	/*
	 t0[0..6] = ticks
	 t0[7],t1[0..4] = Seconds
	 t1[5..7],t2[0..2]= Minutes
	 t2[3..7] =Hours
	 */
    t0 = gTICK;
	t0 += (gSECOND & 0x01) << 7;
	
	t1 =  (gSECOND & 0x3f) >> 1;
	t1 += (gMINUTE & 0x3f) << 5;
	
	t2 =  (gMINUTE & 0x3f) >>3;
	t2 += (gHOUR & 0x7F) << 3;
	
    DF_SPI_RW(t2);
    DF_SPI_RW(t1);
    DF_SPI_RW(t0);      
}

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : SetClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the clock
*
*****************************************************************************/
char SetClock(char input)
{
    static char enter_function = 1;
    // mtA
    // static char time[3];    // table holding the temporary clock setting
    // static char mode = HOUR;
    // char HH, HL, MH, ML, SH, SL;
    static uint8_t time[3];
    static uint8_t mode = HOUR;
    uint8_t HH, HL, MH, ML, SH, SL;
    // mtE
    
    if (enter_function)
    {
        time[HOUR] = gHOUR;
        time[MINUTE] = gMINUTE;
        time[SECOND] = gSECOND;
    }
    
#ifdef CLOCK_12_ENABLE
    if (clockformat == CLOCK_12)    // if 12H clock
        HH = CHAR2BCD2(TBL_CLOCK_12[time[HOUR]]);
    else
#endif
	HH = CHAR2BCD2(time[HOUR]);
    
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';
    
    MH = CHAR2BCD2(time[MINUTE]);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';
    
    SH = CHAR2BCD2(time[SECOND]);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';
    
    LCD_putc(0, HH | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(1, HL | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(2, MH | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(3, ML | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(4, SH | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(5, SL | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(6, '\0');
    
    LCD_Colon(1);
    
    if (input != KEY_NULL)
        LCD_FlashReset();
    
    LCD_UpdateRequired(TRUE, 0);
    
    enter_function = 1;
    
    // Increment/decrement hours, minutes or seconds
    if (input == KEY_PLUS)
        time[mode]++;
    else if (input == KEY_MINUS)
        time[mode]--;
    else if (input == KEY_PREV)
    {
        if (mode == HOUR)
            mode = SECOND;
        else
            mode--;
    }
    else if (input == KEY_NEXT)
    {
        if (mode == SECOND)
            mode = HOUR;
        else
            mode++;
    }
    else if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gHOUR = time[HOUR];
        gMINUTE = time[MINUTE];
        gSECOND = time[SECOND];
        sei(); // mt __enable_interrupt();
        mode = HOUR;
        return ST_TIME_CLOCK_FUNC;
    }
    
    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (time[HOUR] == 255)
        time[HOUR] = 23;
    if (time[HOUR] > 23)
        time[HOUR] = 0;
    
    if (time[MINUTE] == 255)
        time[MINUTE] = 59;
    if (time[MINUTE] > 59)
        time[MINUTE] = 0;
    
    if (time[SECOND] == 255)
        time[SECOND] = 59;
    if (time[SECOND] > 59)
        time[SECOND] = 0;
    
    enter_function = 0;
    return ST_TIME_CLOCK_ADJUST_FUNC;
}
#endif

#ifndef MINIMAL_MENU
/*****************************************************************************
*
*   Function name : SetClockFormat
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the Clockformat
*
*****************************************************************************/
char SetClockFormat(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        
        if(clockformat == CLOCK_24)
            LCD_puts_f(PSTR("24H"), 1);	 // mt LCD_puts("24H", 1);            
        else
            LCD_puts_f(PSTR("12H"), 1);	// mt LCD_puts("12H", 1);		
		
    }
    if (input == KEY_PLUS)
    {
        if(clockformat == CLOCK_24)
        {
            clockformat = CLOCK_12;
            LCD_puts_f(PSTR("12H"), 1); // mt LCD_puts("12H", 1);
        }
        else
        {
            clockformat = CLOCK_24;
            LCD_puts_f(PSTR("24H"), 1); // mt LCD_puts("24H", 1);            
        }
    }
    else if (input == KEY_MINUS)
    {
        if(clockformat == CLOCK_12)
        {
            clockformat = CLOCK_24;
            LCD_puts_f(PSTR("24H"), 1);	// mt LCD_puts("24H", 1);
        }
        else
        {
            clockformat = CLOCK_12;
            LCD_puts_f(PSTR("12H"), 1);   // mt LCD_puts("12H", 1);            
        }
    }
    else if (input == KEY_ENTER)    
    {
        enter = 1;
        return ST_TIME_CLOCK_FUNC;
    }        
    return ST_TIME_CLOCKFORMAT_ADJUST_FUNC;
}
#endif

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : ShowDate
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the date on the LCD
*
*****************************************************************************/
char ShowDate(char input)
{
    char YH, YL, MH, ML, DH, DL;
    
    YH = CHAR2BCD2(gYEAR);
    YL = (YH & 0x0F) + '0';
    YH = (YH >> 4) + '0';
    
    MH = CHAR2BCD2(gMONTH);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';
    
    DH = CHAR2BCD2(gDAY);
    DL = (DH & 0x0F) + '0';
    DH = (DH >> 4) + '0';
    
	
	
	#ifndef MINIMAL_MENU
    // mtA - based on jw
    // TODO: check poss. opt. with pgm_read_word
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 0), YH);
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 1), YL);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 0), YH);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 1), YL);
    
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 2), MH);
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 3), ML);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 2), MH);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 3), ML);
    
    
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 4), DH);
    // LCD_putc( *(DATE_FORMAT_NR[dateformat] + 5), DL);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 4), DH);
    LCD_putc( pgm_read_byte(DATE_FORMAT_NR[dateformat] + 5), DL);
    // mtE
    #else //fixed at european format
	LCD_putc( 4, YH);
    LCD_putc( 5, YL);
    LCD_putc( 2, MH);
    LCD_putc( 3, ML);
    LCD_putc( 0, DH);
    LCD_putc( 1, DL);
	#endif
	
    LCD_putc(6, '\0');
    
    LCD_Colon(1);
    
    LCD_UpdateRequired(TRUE, 0);
    
    
    if (input == KEY_PREV)
        return ST_TIME_DATE;
    else if (input == KEY_NEXT)
        return ST_TIME_DATE_ADJUST;
    else   
        return ST_TIME_DATE_FUNC;
}
#endif


/*****************************************************************************
*
*   Function name : PrintDate
*
*   Returns :       none
*
*   Parameters :    The date in compressed 2 byte binary format
*
*   Purpose :       Prints a date to the Uart YY/MM/DD
*
*****************************************************************************/
void PrintDate(char D0,char D1)
{
	/*
	 D0[0..4] = Day
	 D0[5..7], D1[0] = Month
	 D1[1..7] = year
	 */
	USART_Tx_Byte((D1>>1));
    USART_Tx('/');

    USART_Tx_Byte((D0>>5) + ((D1 & 1)<<3) );
    USART_Tx('/');

    USART_Tx_Byte((D0 & 0x1F));
	
}


/*****************************************************************************
*
*   Function name : LogDate
*
*   Returns :       none
*
*   Parameters :    none
*
*   Purpose :       Saves date to the Flash in a comprressed 2 byte format
*
*****************************************************************************/
void LogDate(void)
{
    char D0,D1;
	/*
	 D0[0..4] = Day
	 D0[5..7], D1[0] = Month
	 D1[1..7] = year
	 */
	D1 = (gYEAR<<1);
	D1 += (gMONTH & 0x0F)>>3;
	
	D0 = (gDAY & 0x1F);
	D0 += (gMONTH & 0x07)<<5;
	
	DF_SPI_RW(D1);
	DF_SPI_RW(D0);  

}

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : SetDate
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the date
*
*****************************************************************************/
char SetDate(char input)
{
    static char enter_function = 1;
    // mtA
    // static char date[3];    // table holding the temporary date setting
    // static char mode = DAY;
    // char YH, YL, MH, ML, DH, DL;
    // char MonthLength_temp;
    // char LeapMonth;
    static uint8_t date[3];    // table holding the temporary date setting
    static uint8_t mode = DAY;
    uint8_t YH, YL, MH, ML, DH, DL;
    uint8_t MonthLength_temp;
    uint8_t LeapMonth;
    // mtE
    
    if (enter_function)
    {
        date[YEAR] = gYEAR;
        date[MONTH] = gMONTH;
        date[DAY] = gDAY;
    }
    
    if (mode == YEAR)
    {
        YH = CHAR2BCD2(date[YEAR]);
        YL = (YH & 0x0F) + '0';
        YH = (YH >> 4) + '0';
        
        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'Y');
        LCD_putc( 3, 'Y');        
        LCD_putc( 4, YH);
        LCD_putc( 5, YL);
    }
    else if (mode == MONTH)
    {
        MH = CHAR2BCD2(date[MONTH]);
        ML = (MH & 0x0F) + '0';
        MH = (MH >> 4) + '0';
		
        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'M');
        LCD_putc( 3, 'M');        
        LCD_putc( 4, MH);
        LCD_putc( 5, ML);
    }
    else if (mode == DAY)
    {
        DH = CHAR2BCD2(date[DAY]);
        DL = (DH & 0x0F) + '0';
        DH = (DH >> 4) + '0';
		
        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'D');
        LCD_putc( 3, 'D');        
        LCD_putc( 4, DH);
        LCD_putc( 5, DL);
    }
    
    LCD_putc(6, '\0');
    
    LCD_Colon(0);
    
    if (input != KEY_NULL)
        LCD_FlashReset();
    
    LCD_UpdateRequired(TRUE, 0);
    
    
    enter_function = 1;
    
    // Increment/decrement years, months or days
    if (input == KEY_PLUS)
        date[mode]++;
    else if (input == KEY_MINUS)
        date[mode]--;
    else if (input == KEY_PREV)
    {
        if (mode == YEAR)
            mode = DAY;
        else
            mode--;
    }
    else if (input == KEY_NEXT)
    {
        if (mode == DAY)
            mode = YEAR;
        else
            mode++;
    }
    else if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gYEAR = date[YEAR];
        gMONTH = date[MONTH];
        gDAY = date[DAY];
        sei(); // mt __enable_interrupt();
        mode = YEAR;
        return ST_TIME_DATE_FUNC;
    }
    
    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (date[YEAR] == 255)
        date[YEAR] = 99;
    if (date[YEAR] > 99)
        date[YEAR] = 0;
    
    if (date[MONTH] == 0)
        date[MONTH] = 12;
    if (date[MONTH] > 12)
        date[MONTH] = 1;
    
    // Check for leap year, if month == February
    if (gMONTH == 2)
        if (!(gYEAR & 0x0003))              // if (gYEAR%4 == 0)
            if (gYEAR%100 == 0)
                if (gYEAR%400 == 0)
                    LeapMonth = 1;
                else
                    LeapMonth = 0;
            else
                LeapMonth = 1;
        else
            LeapMonth = 0;
    else
        LeapMonth = 0;
    
    if (LeapMonth)
        MonthLength_temp = 29;
    else
        MonthLength_temp = MonthLength[date[MONTH]];
    
    if (date[DAY] == 0)
        date[DAY] = MonthLength_temp;
    if (date[DAY] > MonthLength_temp)
        date[DAY] = 1;
    
    enter_function = 0;
    
    return ST_TIME_DATE_ADJUST_FUNC;
}
#endif




#ifndef MINIMAL_MENU
/*****************************************************************************
*
*   Function name : SetDateFormat
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the Dateformat
*
*****************************************************************************/
char SetDateFormat(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        
		LCD_puts_f(DATEFORMAT_TEXT[dateformat], 1);
    }
    if (input == KEY_PLUS)
    {
        if(dateformat >= 2)
            dateformat = 0;
        else
            dateformat++;
		
        LCD_puts_f(DATEFORMAT_TEXT[dateformat], 1);        
    }
    else if (input == KEY_MINUS)
    {
        if(dateformat == 0)
            dateformat = 2;
        else
            dateformat--;
		
        LCD_puts_f(DATEFORMAT_TEXT[dateformat], 1);            
    }
    else if (input == KEY_ENTER)    
    {
        enter = 1;
        return ST_TIME_DATE_FUNC;
    }        
    return ST_TIME_DATEFORMAT_ADJUST_FUNC;
}
#endif


/******************************************************************************
*
*   Timer/Counter2 Overflow Interrupt Routine
*
*   Purpose: Increment the real-time clock
*            The interrupt occurs once a second (running from the 32kHz crystal)
*			 (or 16 times a second if FASTCLOCK is defined)
*
*******************************************************************************/
// mtA
// #pragma vector = TIMER2_OVF_vect
// __interrupt void TIMER2_OVF_interrupt(void)
SIGNAL(SIG_OVERFLOW2)
// mtE
{
    static char LeapMonth;
#ifdef FASTCLOCK
	gTICK++;
	
	if (gTICK==TICKSPERSECOND)
	{
		gTICK=0;
#endif
		gSECOND++;               // increment second
		
		glastSpeed = TCNT1L + (TCNT1H <<8); // Read windspeeed for last sercond
		TCNT1L=0;				// Reset Counter for next second
		TCNT1H=0;
		
		if (gSECOND == 60)
		{
			gSECOND = 0;
			gMINUTE++;
			
			gPowerSaveTimer++;
			
			if (gMINUTE > 59)
			{
				gMINUTE = 0;
				gHOUR++;
				
				if (gHOUR > 23)
				{
					
					gHOUR = 0;
					gDAY++;
					
					// Check for leap year if month == February
					if (gMONTH == 2)
						if (!(gYEAR & 0x0003))              // if (gYEAR%4 == 0)
							if (gYEAR%100 == 0)
								if (gYEAR%400 == 0)
									LeapMonth = 1;
								else
									LeapMonth = 0;
							else
								LeapMonth = 1;
						else
							LeapMonth = 0;
					else
						LeapMonth = 0;
					
					// Now, we can check for month length
					if (gDAY > (MonthLength[gMONTH] + LeapMonth))
					{
						gDAY = 1;
						gMONTH++;
						
						if (gMONTH > 12)
						{
							gMONTH = 1;
							gYEAR++;
						}
					}
				}
		
				// start of every hour do this....
				if (gLogTimeUnit == HOUR) 
					check_log();
				//
			}
				// start of every minute do this....
				if (gLogTimeUnit == MINUTE) 
					check_log();
				//		
		}
				// start of every second do this....

				// check alarm every second clock to start logging
				if (ALARM_check(gYEAR,gMONTH,gDAY,gHOUR,gMINUTE,gSECOND)){
					gLogging=TRUE;
					ALARM_clear();
				}
				
				if (gLogTimeUnit == SECOND)
					check_log();
				
				gSpeedTotal +=glastSpeed;
				
				if (glogSpeed){ // we are logging wind speed for SECONDSTOLOG seconds
					if(gSpeedSeconds++ >=gSpeedSecondsToLog){//log all the data
						gLogging = TRUE;
						glogSpeed = FALSE;
						gLogNow=TRUE;
					}
				}
				//
#ifdef FASTCLOCK
				}
			// every tick do this....
			if (gLogTimeUnit == TICK){ 
				check_log();
				if (glogSpeed){
					// we don't log wind speed on this setting,
					// skip straight to saving to flash
					gLogging = TRUE;
					glogSpeed = FALSE;
					gLogNow=TRUE;
				}
			}
			
#endif
}


/*****************************************************************************
*
*   Function name : check_log
*
*   Returns :       none
*
*   Parameters :    none
*
*   Purpose :       check log timer and and increment if needed, Sets flags 
*					if it it time to log to the data to the flash
*
*****************************************************************************/
void check_log(void)
{
	if (gLogging && gLogTime>0) {
		gLogTimeCounter++;
		if (gLogTimeCounter>=gLogTime){
			glogSpeed = TRUE; // start logging wind speed for SECONDSTOLOG seconds
			gSpeedTotal=0;
			gLogTimeCounter=0;
			gSpeedSeconds = 0;
			gLogging = FALSE;
		}
	}
}
