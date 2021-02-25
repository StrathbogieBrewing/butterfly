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

// global vars
extern unsigned int glastSpeed; // last 1 second count
extern char glogSpeed; // tell RTC to add counts to total each second
extern unsigned int gSpeedSeconds;// no of seconds counted
extern unsigned int gSpeedTotal; // used for counting the wind speed events
extern unsigned int gSpeedSecondsToLog; // number of seconds to log timer counts

// Function declarations
void Speed_init(void);
char SpeedFunc(char);
void PrintSpeed(unsigned int Speed);
void ShowSpeed(void);

//enable internal pullup 20-50K½
#define SPEED_EN_INTERNALPULLUP 0
