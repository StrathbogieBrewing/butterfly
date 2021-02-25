//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//***************************************************************************
//
//  File........: usart.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 2.28a
//
//  Description.: AVR Butterfly USART routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//
//***************************************************************************

//mtA
// only changed the includes here
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
//mtE
#include "main.h"
#include "usart.h"
#include "ADC.h"
#include "BCD.h"
#include "RTC.h"
#include "alarm.h"
#include "speed.h"
#include "pgmspacehlp.h"
#include "ds18x20.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
//#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define SETCLOCK	 0
#define SETALARM     1

char gEOL;			//global EOL characters
int errorCount;		//global error counter used to keep track of user attempts to enter data correctly.
					//after 3 incorrect attempts most functions will terminate.

#ifdef EXTENDED_USART
const char  UART_ERROR_MSG[] PROGMEM =	"ERR:";
const char	UART_ERROR_MSG2[] PROGMEM =	"ERRS";
const char	COMPILEDATE[] PROGMEM= __DATE__ " " __TIME__ " ";
extern const char MT_AVRBF[] PROGMEM  ; // for use in status command
extern unsigned int glastSpeed;					// for use in status command
typedef void (*CMD_CALLBACK_FUNC) (char);
extern char gPowerSaveTimer;

#endif


/*****************************************************************************
*
*   Function name : USART_Init
*
*   Returns :       None
*
*   Parameters :    unsigned int baudrate
*
*   Purpose :       Initialize the USART
*
*****************************************************************************/
void USART_Init(unsigned int baudrate)
{
    // Set baud rate
    UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char)baudrate;
	
    // Enable 2x speed
    UCSRA = (1<<U2X);
	
    // Enable receiver and transmitter
#ifdef EXTENDED_USART
    UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(0<<UDRIE);
	gUART=TRUE;// stay out of sleep mode by default
#else
		UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);
		gUART=FALSE;// only sending so go to sleep
#endif
			// Async. mode, 8N1
			UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
			
			gEOL=DEFAULT_EOL;
}


/*****************************************************************************
*
*   Function name : USART_Tx
*
*   Returns :       None
*
*   Parameters :    char data: byte to send
*
*   Purpose :       Send one byte through the USART
*
*****************************************************************************/
void USART_Tx(char data)
{
    while (!(UCSRA & (1<<UDRE)));
    UDR = data;
}


/*****************************************************************************
*
*   Function name : USART_Rx
*
*   Returns :       char: byte received
*
*   Parameters :    None
*
*   Purpose :       Receives one byte from the USART
*
*****************************************************************************/
char USART_Rx(void)
{
    while (!(UCSRA & (1<<RXC)));
    return UDR;
}


/*****************************************************************************
*
*   Function name : USART_Tx_Hex
*
*   Returns :       None
*
*   Parameters :    char data: byte to send
*
*   Purpose :       Sends one byte through the USART as a two char Hex number
*
*****************************************************************************/
void USART_Tx_Hex(char data)
{
    char low, high;
	
    high = data >> 4;
    high += '0';
    if (high > '9' ) high += 7;
	
    low = data & 0x0f;
    low += '0';
    if (low >'9') low += 7;
	
    USART_Tx(high);
    USART_Tx(low);
	
}


/*****************************************************************************
*
*   Function name : USART_Tx_Byte
*
*   Returns :       None
*
*   Parameters :    char data: byte to send
*
*   Purpose :       Sends one byte through the USART as a 2 digit decimal number
*
*****************************************************************************/
void USART_Tx_Byte(char data)
{
    char temp;
	
	temp = CHAR2BCD2(data);
	USART_Tx_Hex(temp);	
}

/*
void USART_Tx_Int(unsigned int data)
{
	unsigned long temp;
	char i;
	
	temp = int2BCD5(data);
	i = 16;
	while (i){
		i -= 2;
		USART_Tx_Hex((temp>>i)&0xff);	
	}
	
}
*/

/*****************************************************************************
*
*   Function name : USART_Tx_Bool
*
*   Returns :       None
*
*   Parameters :    char data: byte to send
*
*   Purpose :       prints a boolean value to the usart
*
*****************************************************************************/
void USART_Tx_Bool(char data)
{
#ifdef EXTENDED_USART
	if(data){
		USART_sendmsg(PSTR("ON"));
	}else{
		USART_sendmsg(PSTR("OFF"));
	}
#else
	if(data){
		USART_Tx('1');
	}else{
		USART_Tx('0');
	}
#endif
}

/*****************************************************************************
*
*   Function name : USART_EOL
*
*   Returns :       None
*
*   Parameters :    char EOl tyoe to send. 
*
*   Purpose :       Sends EOL char(s) as specified by parameters
*
*****************************************************************************/
void USART_EOL(char eol){
	//Send apropriate EOL char
	//NOTE: EOL_WIN =EOL_MAC | EOL_UNIX so both Chars are sent.
	if (eol & EOL_MAC)  USART_Tx(0x0d);
	if (eol & EOL_UNIX) USART_Tx(0x0a);
	if (eol & EOL_TERM) USART_Tx(0x04);
	
}

//////////////////////////////////////////////////////////////////////////////
#ifdef EXTENDED_USART

const char unitlabels[]={'H','M','S','T'};	//Hour, Minute, Second, Tick
const char eolLabels[] ={'N','U','M','W','T'};	//None, Unix, Mac, Windows, EOT

//Default
void cmd_default(char shift){
	USART_Tx('?');
}

//A
void cmd_alarm(char shift){
#ifdef USART_CMD_ALARM
	if (shift){ //Set alarm
		USART_setTheClock(SETALARM);
	}
	
	//display alarm status
	ALARM_print_status();
	USART_Tx(' ');
	ALARM_print();
#endif
}

//B
void cmd_dumpB(char shift){
#ifdef USART_CMD_BDUMP
	if(shift)
		DumpFlash(DUMP_BIN);
#endif
}

//C
void cmd_clock(char shift){
#ifdef USART_CMD_CLOCK
	if(shift) // set time and date 
		USART_setTheClock(SETCLOCK);
	
	//query time and date
	USART_Tx_Byte( gHOUR);USART_Tx(':');
	USART_Tx_Byte( gMINUTE);USART_Tx(':');
	USART_Tx_Byte( gSECOND);USART_Tx(' ');
	USART_Tx_Byte( gDAY);USART_Tx('/');
	USART_Tx_Byte( gMONTH);USART_Tx('/');
	USART_Tx_Byte( gYEAR);
#endif
}

//D
void cmd_dump(char shift){
#ifdef USART_CMD_DUMP
	if(shift)// download ALL 
		DumpFlash(DUMP_ALL);
	
	else// download 
		DumpFlash(DUMP_NORMAL);
#endif
}

//E
void cmd_eol(char shift){
#ifdef USART_CMD_EOL
	if(shift){//toggle EOL chars
		gEOL++;
		if (gEOL>EOL_TERM) gEOL=EOL_NONE;		
	}
	
	//query EOL char
	USART_Tx(eolLabels[(int)gEOL]);
#endif
}

//F
void cmd_flash(char shift){
#ifdef USART_CMD_ROLLOVER
	if(shift)// toggle flash rollover
		gEnableRollover=!gEnableRollover;
	
	// query flash rollover 
	USART_Tx_Bool(gEnableRollover);
#endif
}

//G
void cmd_intervalS(char shift){
#ifdef USART_CMD_WINDINTERVAL
	char tmp;
	if(shift){//set the number of seconds to log
		if(USART_getValue(&tmp	,00,99))
			gSpeedSecondsToLog=(int) tmp*10; // enter in multiples of 10s
	}
	
	// print number actual value of gSpeedSecondsToLog
	ADC2RAW(TRUE,gSpeedSecondsToLog);
#endif
}


// H
void cmd_diplayText(char shift){
#ifdef USART_CMD_DISPLAY_TEXT
	char count;
#if !NO_LCD
	char ch;
	char buffer[TEXTBUFFER_SIZE+1]; // bytes buffer for display
#endif
	if(shift){
#if !NO_LCD
		//read char untill end of line 
		count = 0;
		ch = USART_Rx();	
		while(ch != 13 && ch != 10 && count<TEXTBUFFER_SIZE ){
			USART_Tx(ch); //ECHO it back.
			buffer[(int)count++] = ch;
			ch = USART_Rx();	
		}
		buffer[(int)count]=0; //terminate the string
		LCD_puts(buffer,1);
#endif
	}else{
		
		//Print out data names
#if EN_LOG_DATE
		USART_sendmsg(PSTR("DATE "));
#endif
		
#if EN_LOG_CLOCK
		USART_sendmsg(PSTR("TIME "));
#endif
		
#if EN_LOG_LIGHT
		USART_sendmsg(PSTR("LIGHT "));
#endif
		
#if EN_LOG_DIR
		for (count=0;count<DIRECTION_ADCS;count++){     
			USART_sendmsg(PSTR("ADC-"));
			USART_Tx_Byte(count);
			USART_Tx(' ');
		} 
#endif
	
#if EN_LOG_BATTERY
		USART_sendmsg(PSTR("BATTERY "));
#endif
		
#if EN_LOG_SHT
		USART_sendmsg(PSTR("SHT11 "));
#endif
		
#if EN_LOG_SPEED
		USART_sendmsg(PSTR("SPEED "));
#endif
		
#if EN_LOG_TEMP
		USART_sendmsg(PSTR("TEMP "));
#endif
		
#if EN_LOG_DS1820
#if DS1820_COUNT>1 
		for (count=0;count<DS1820_COUNT;count++){
			USART_sendmsg(PSTR("DS1820-"));
			USART_Tx_Byte(count);
			USART_Tx(' ');
		}
#else
		USART_sendmsg(PSTR("DS1820 "));
#endif		
#endif
		USART_EOL(gEOL);
	}
#endif
}		

//I
void cmd_interval(char shift){
#ifdef USART_CMD_INTERVAL
	if(shift)//Set the logging interval
		USART_changeInterval();
	
	//display the interval
	USART_Tx(unitlabels[(int)gLogTimeUnit]);
	USART_Tx_Byte(gLogTime);
#endif
}
//j
//k


//L
void cmd_light(char shift){
#ifdef USART_CMD_LIGHT
	// query light level command character
	USART_sendADC(LIGHT_SENSOR,TRUE);
#endif
}
//m


//N
void cmd_lognow(char shift){
#ifdef USART_CMD_LOGNOW
	if(shift)//Log Now
		doLogging();
	
	// show current buffer (next write)
	DF_Print_page(0,gDataPosition,DUMP_NORMAL);
#endif
}

//O
void cmd_logstatus(char shift){
#ifdef USART_CMD_LOGSTATUS
	if(shift) // toggle logging
		gLogging = !gLogging;
	// query loggin status
	USART_Tx_Bool(gLogging|glogSpeed);
#endif
}

//P
void cmd_pwrsave(char shift){
#ifdef USART_CMD_PWRSAVE
	if(shift) // toggle auto poweer save
		gAutoPowerSave= !gAutoPowerSave;
	// query loggin status
	USART_Tx_Bool(gAutoPowerSave);
#endif
}
//q
void cmd_ds18x20(char shift){
#if EN_LOG_DS1820
#ifdef USART_CMD_DS18X20
#if DS1820_COUNT>1 
	uint8_t i;
	for (i=0;i<gnSensors;i++){
		ds18x20_print(i);	
		USART_Tx(' ');
	}
#else
	ds18x20_print(0);		
#endif
#endif
#endif 
}


//R
void cmd_reset(char shift){
#ifdef USART_CMD_RESET
	if(shift){// reset command character
		gRolloverFlash = 0;
		gDataPage = 0;
		gDataPosition=0;
	}
#endif
}

//S
void cmd_sleep(char shift){
#ifdef USART_CMD_SLEEP
	if(shift){// go back to sleep mode 
		gUART=FALSE;
		gPowerSave = TRUE;
	}
#endif
}

//T
void cmd_temp(char shift){
#ifdef USART_CMD_TEMP
	int t;
	//display current temperature
	ADC_init(TEMPERATURE_SENSOR);// set up adc
		t = ADC_read();		// read adc   
		ADC2Temp(TRUE,t);	// print ADC
#endif
}

//U
void cmd_intervalP(char shift){
#ifdef USART_CMD_INTERVAL
	char interval;
	if(shift){//Set the power save time out
		if (USART_getValue(&interval,0,90)) 
			gPowerSaveTimeout = interval;
	}
	
	//display the interval
	
	USART_Tx_Byte(gPowerSaveTimeout);
#endif
}

//V
void cmd_battery(char shift){
#ifdef USART_CMD_BATTERY
	int t;
	//case 'v':
	ADC_init(VOLTAGE_SENSOR);// set up adc
		t = ADC_read();		// read adc   
		ADC2Volt(TRUE,t);	// print ADC		
#endif
}

//W
void cmd_windspeed(char shift){
#ifdef USART_CMD_WINDSPEED
	if (shift){
		// total wind speed for current logging interval
		PrintSpeed(gSpeedTotal); 
	}else{	
		// last wind speed for previous second
		USART_Tx_Byte(glastSpeed);
	}
#endif
}
//x

//y

//Z
void cmd_status(char shift){
#ifdef USART_CMD_STATUS
	char d[5],i;						// digits for displaying space avail in flash
	unsigned int spaceLeft;
	unsigned long Temp;
	
	// status command 
	// this command sends software version number back to user.
	// Primary use is to let user know system is functioning.
	USART_sendmsg(MT_AVRBF);	//MT_AVRBF is defined in "menu.h"
	USART_Tx(' ');
	USART_sendmsg(COMPILEDATE);
	USART_Tx('v');
	USART_Tx(SWHIGH + '0');     //SWHIGH/LOW are defined in "main.h"
	USART_Tx(SWLOW  + '0');
	USART_Tx(SWREV  + 'a' - 1 );
	USART_Tx(' ');
	
	if (gRolloverFlash)
		USART_sendmsg(PSTR("Full"));
	else{
		//calculate space left in flash
		spaceLeft = TOTALPAGESINFLASH - gDataPage-1;
		spaceLeft *= RECORDSPERPAGE;
		spaceLeft += RECORDSPERPAGE- gDataPosition;
		
		// convert to BCD and print
		Temp = int2BCD5(spaceLeft);             
		d[0] = ((char)Temp & 0x0F) + '0';	
		for (i=1;i<5;i++){
			d[(int)i] = ((char)(Temp >>= 4)& 0x0F) + '0';
		} 
		for (i=4;i<5;i--){
			USART_Tx(d[(int)i]);
		} 
		USART_sendmsg(PSTR(" avail"));
	}	
#endif
}

// command jump table for usart commands
const CMD_CALLBACK_FUNC USART_cmd[] PROGMEM ={
	cmd_alarm,	cmd_dumpB, cmd_clock,cmd_dump, cmd_eol, cmd_flash, cmd_intervalS, cmd_diplayText, 
	cmd_interval, cmd_default, cmd_default, cmd_light, cmd_default, cmd_lognow, cmd_logstatus, 
	cmd_pwrsave, cmd_ds18x20, cmd_reset, cmd_sleep, cmd_temp, cmd_intervalP, cmd_battery, cmd_windspeed,
	cmd_default, cmd_default, cmd_status
};


/****************************************************************************
*
*	Function Name :	SIG_USART_RECV
*
*	Returns : 		None
*
*	Parameters :	None
*
*	Purpose :		Interrupt service routine for Data received from UART
*					Receives one byte of data from the USART. Determines
*					command char received and execute code as required.
*
*****************************************************************************/

SIGNAL(SIG_USART_RECV)
{
	char userCommand;		 
	char cmd_index;
	char shift;
	CMD_CALLBACK_FUNC cmd;
	
	gPowerSaveTimer = 0;				// reset the auto sleep timer
	gUART = TRUE;						//stay out of power save mode
	userCommand = USART_Rx();			// get command character from UART receive register
	shift=FALSE;
	
	//is it a number for ADC channel
	if (userCommand>='0' && userCommand<='7'){
		USART_sendADC(userCommand-'0',FALSE);
	}else{
		
		// is it an ucase letter?
		if (userCommand < 'a'){
			shift=TRUE;
			userCommand += 32; //convert to lcase
		}
		
		//is it a command letter a-z
		if (userCommand >= 'a' && userCommand <= 'z'){
			cmd_index = userCommand-'a';
			cmd = (CMD_CALLBACK_FUNC) pgm_read_word(USART_cmd+cmd_index);
			cmd(shift);
		}
		
	}
	
	USART_EOL(gEOL);
	
}


/*****************************************************************************
*
*   Function name : USART_sendmsg
*
*   Returns :       nothing
*
*   Parameters :    pointer to program memory string
*
*   Purpose :       Sends a string out on the UART
*
*****************************************************************************/
void USART_sendmsg (const char *s)
{
	char c;
	while ((c=pgm_read_byte(s++)) ){	// get next char
		USART_Tx(c);	//send char
	}
} 


/*****************************************************************************
*
*   Function name : USART_sendADC
*
*   Returns :       nothing
*
*   Parameters :    char channel to read, char invert reading
*
*   Purpose :       Read from an ADC and send it out via the UART
*
*****************************************************************************/
void USART_sendADC(char channel,char invert)
{
	int logdata;
	
	ADC_init(channel);					// set up adc
	logdata=ADC_read();					// read adc   
	if (invert){
		logdata = 1023-logdata;
	}
	
	ADC2RAW(TRUE,logdata);
}


/*****************************************************************************
*
*   Function name : USART_changeInterval
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       change the logging interval via USART
*
*****************************************************************************/
void USART_changeInterval(void)
{
	char temp;
	char interval;
	USART_sendmsg(PSTR("h/m/s/t:"));
	temp = USART_Rx();
	if (temp <= 'a') temp += 32;		// convert to lcase
	
	switch(temp){
		case 's':
			gLogTimeUnit=SECOND;
			break;
		case 'm':
			gLogTimeUnit=MINUTE;
			break;
		case 't':
			gLogTimeUnit=TICK;
			break;
		default:
			gLogTimeUnit=HOUR;
	}
	
	USART_sendmsg(PSTR("Interval:"));
	if (!USART_getValue(&interval,0,90)) interval = 90;
	gLogTime = interval;
}


/*****************************************************************************
*
*   Function name : USART_setTheClock
*
*   Returns :       nothing
*
*   Parameters :    nothing
*
*   Purpose :       Takes input from user via Uart and sets the system time.
*
*****************************************************************************/
void USART_setTheClock(char target)
{
	char theDay;
	char theMonth;
	char theYear;
	char theHour;
	char theMinutes;
	char theSeconds;
	
	errorCount = 0;		//set error counter to zero
	USART_Tx('Y');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theYear,0,99));		//Go get the Year value
	USART_Tx('M');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theMonth,1,12));		//Go get the month value
	USART_Tx('D');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theDay,1,31));		//Go get the day value 
	USART_Tx('H');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theHour,0,23));		//Go get the hour value	
	USART_Tx('M');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theMinutes,0,59));	//Go get the minute value
	USART_Tx('S');
	while ((errorCount < USART_ERRORMAX) && !USART_getValue(&theSeconds,0,59));	//Go get the second value 
	
	if (errorCount >= USART_ERRORMAX){	//if the error counter is USART_ERRORMAX then stop trying to set the clock. 
		errorCount = 0;					//set error counter to zero
		USART_sendmsg(UART_ERROR_MSG2);
		return;
	}
	
	if (target==SETCLOCK){		
		gDAY = (uint8_t)theDay;				//set the global day variable
		gMONTH = (uint8_t)theMonth;			//set the global month variable
		gYEAR = (uint8_t)theYear;			//set the global year variable
		gSECOND = (uint8_t)theSeconds;		//set the global seconds variable
		gMINUTE = (uint8_t)theMinutes;		//set the global minutes variable
		gHOUR = (uint8_t)theHour;			//set the global hour variable.
		gTICK = 0;							//reset thg global tick variable
	}else if(target==SETALARM){
		ALARM_set(theYear,theMonth,theDay,theHour,theMinutes,theSeconds);
	}
}

/*****************************************************************************
*
*   Function name : USART_getValue
*
*   Returns :       int - 0 for unsuccessful / 1 for successful
*
*   Parameters :    int 
*
*   Purpose :       Takes an input from user via Uart and does basic error check
*
*****************************************************************************/
char USART_getValue(char *value, char lowerbound, char upperbound)
{
	char retval;						//function return value
	char temp;
	retval = 1;							//set retval to 1 as default (successful)
	*value = 0;							//initialise the parameter
	
	//	print out the limits for the input
	USART_Tx_Byte(lowerbound);
	USART_Tx('-');
	USART_Tx_Byte(upperbound);
	USART_Tx(' ');
	
	temp = USART_Rx();					//get first character sent
	*value = ((char)(temp - '0')) * 10;	//convert char from Ascii to Hex and multiply by 10
	
	temp = USART_Rx();					//get second character sent
	*value += ((char)(temp - '0'));		//convert char from Ascii to Hex and add to yearValue
	
	if ((*value < lowerbound) | (*value > upperbound)){  
		//check the value provided by user is valid
		//if not then increment error counter return 0  
		errorCount++;					//increment error counter	
		retval = 0;						//set retval to 0 signifying unsuccessful operation
		USART_sendmsg(UART_ERROR_MSG);
		USART_EOL(gEOL);
		return retval;
	}
	
	
	return retval;
}


//  EXTENDED_USART /////////////////////////////////////////////////////////////
#endif



