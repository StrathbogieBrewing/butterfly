//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//*****************************************************************************
//
//  File........: RTC.h
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Description.: Functions for RTC.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - File created                                  - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//*****************************************************************************


//mtA
//extern char gSECOND;
//extern char gMINUTE;
//extern char gHOUR;
//extern char gDAY;
//extern char gMONTH;
extern uint8_t gSECOND;
extern uint8_t gMINUTE;
extern uint8_t gHOUR;
extern uint8_t gDAY;
extern uint8_t gMONTH;
//mtE
extern uint8_t gYEAR;


//  Function declarations
void RTC_init(void);            //initialize the Timer Counter 2 in asynchron operation
void Time_update(void);        //updates the time and date
char ShowClock(char input);
char SetClock(char input);

char ShowDate(char input);
char SetDate(char input);

#ifndef MINIMAL_MENU
#define CLOCK_12_ENABLE		//enable 12hr vlock display
#define DATE_MULTIFORMAT    //enable multiple date formats on screen. 
char SetClockFormat(char input);
char SetDateFormat(char input);
#endif

void LogDate(void);
void LogClock(void);
void PrintDate(char D0,char D1);
void PrintClock(char t0,char t1, char t2);

#define CLOCK_24    1
#define CLOCK_12    0
#define HOUR		0
#define MINUTE		1
#define SECOND		2
#define	TICK		3

// run ISR at 16Hz rather than 1Hz
#define	FASTCLOCK 
#define TICKSPERSECOND	16	

#include "dataflash.h"

extern uint8_t	gTICK;
extern uint8_t gSECOND;
extern uint8_t gMINUTE;
extern uint8_t gHOUR;
extern uint8_t gDAY;
extern uint8_t gMONTH;
extern uint8_t gYEAR;

extern char gLogTimeCounter;
extern char gLogTimeUnit;
extern char gLogTime;	    //default to logging every 10 minutes
