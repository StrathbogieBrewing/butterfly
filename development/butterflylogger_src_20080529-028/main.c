//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//
//$Id: main.c,v 1.7 2008/05/13 21:56:03 brokentoaster Exp $
//***************************************************************************
//
//  File........: main.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
//
//  Description.: AVR Butterfly main module
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - KS
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas (*)
//  20040308 - 0.14 - Modified for VUW TempBug Use					- N. Lott
//	20060815 - 0.25 - Modified for Butterfly Logger					- N. Lott
//***************************************************************************

// (*) Martin Thomas, Kaiserslautern, Germany, e-mail: mthomas(at)rhrk.uni-kl.de 
// or eversmith(at)heizung-thomas.de
//
// I'm not working for ATMEL.
// The port is based on REV_06 of the ATMEL-Code (for IAR-C)
// Initialy I marked my changes with "// mt" or enclosed them with "// mtA" and 
// "// mtE" but forgot this for some changes esp. during debugging. 'diff' 
// against the original code to see everything that has been changed.

//mtA
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>
//mtE

#include "main.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"
#include "RTC.h"
#include "timer0.h"
#include "BCD.h"
#include "usart.h"
#include "ADC.h"
#include "dataflash.h"
#include "speed.h"

#include "menu.h"
#include "Sht.h"
#include "alarm.h"
#include "onewire.h"
#include "ds18x20.h"



#define pLCDREG_test (*(char *)(0xEC))

extern unsigned int LCD_character_table[] PROGMEM;
char gAutoPressJoystick = FALSE;		// global variable used in "LCD_driver.c"
unsigned char state;					// helds the current state, according to 
										// "menu.h"

//  gPowerSave globals
//
char gPowerSaveTimeout;					// Autopowersave period in minutes
BOOL gAutoPowerSave;						// Variable to enable/disable the Auto Power Save func
char gPowerSave;							// current state of Power save mode
extern BOOL glogSpeed;					// from speed.c to prevent sleep while timming speed.
char gUART;								// prevent entering power save, when using the UART

// timing globals from RTC.c
// 
extern char gLogTimeCounter;			// current elapsed logging interval
extern char gLogTimeUnit;				// units for gLogTime
extern char gLogTime;					// interval between logging
extern char gPowerSaveTimer;			// external Counter from "RTC.c"

// Global variables for the Flash
//
BOOL gEnableRollover;					// allow flash to reset after filling completly
char gRolloverFlash;					// number of times flash has filled
unsigned char gDataPosition;			// position in page of next write to df   
unsigned int gDataPage;					// current page to be written to the flash
BOOL gUploading;						// Let other functions know we are uploading from the df
BOOL gLogging;							// Logging is enabled
BOOL gLogNow;							// write to log next iteration of the main loop

// Private functions
//
void DF_write_int(int data);
int DF_read_int(void);

#ifdef EN_LOG_DS1820

#endif

/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/
// mt __C_task void main(void)
int main(void)
{    
	
#if !NO_MENUS
	//    unsigned char state, nextstate;
    unsigned char nextstate;
    // mt static char __flash *statetext;
	PGM_P statetext;
    char (*pStateFunc)(char);
    char input;
    char buttons;
#endif
	uint8_t i; // char i;

#if !NO_JOYSTICK
    char last_buttons;
#endif
	
	// set default values for the global variables
	gEnableRollover = DEFAULT_EN_ROLLOVER;
	gRolloverFlash = 0;			
	gDataPosition=0;		
	gDataPage=0;			
	gUploading= FALSE;		
	gLogging =DEFAULT_EN_LOGGING;
	gLogNow = FALSE;
	gUART = TRUE;
	gPowerSaveTimeout = DEFAULT_POWERSAVETIMEOUT;	
	gAutoPowerSave = DEFAULT_EN_POWERSAVE;	
	gPowerSave = FALSE;		
	
#if !NO_JOYSTICK
	last_buttons='\0';	// mt
#endif
	
#if !NO_MENUS
    // Initial state variables
    state = nextstate = ST_AVRBF;
    statetext = MT_AVRBF;
    pStateFunc = 0;
#endif	
	
    // Program initalization
    Initialization();
    sei(); // mt __enable_interrupt();
	
    for (;;)            // Main loop
    {
#if !NO_MENUS		
        if(!gPowerSave)          // Do not enter main loop in power-save
        {

			// Plain menu text
            if (statetext)
            {
                LCD_puts_f(statetext, 1);
                LCD_Colon(0);
                statetext = NULL;
            }

#if !NO_JOYSTICK		
            input = getkey();           // Read buttons
#endif
			
            if (pStateFunc)
            {
                // When in this state, we must call the state function
                nextstate = pStateFunc(input);
            }
            else if (input != KEY_NULL)
            {
                // Plain menu, clock the state machine
                nextstate = StateMachine(state, input);
            }
			
            if (nextstate != state)
            {
                state = nextstate;
                // mt: for (i=0; menu_state[i].state; i++)
				for (i=0; pgm_read_byte(&menu_state[i].state); i++)
                {
                    //mt: if (menu_state[i].state == state)
					if (pgm_read_byte(&menu_state[i].state) == state)
                    {
						// mtA
                        // mt - original: statetext =  menu_state[i].pText;
                        // mt - original: pStateFunc = menu_state[i].pFunc;
						/// mt this is like the expample from an avr-gcc guru (mailing-list):
						statetext =  (PGM_P) pgm_read_word(&menu_state[i].pText);	// mt adresses?!
																					// mt - store pointer to function from menu_state[i].pFunc in pStateFunc
																					//// pStateFunc = pmttemp;	// oh je - wie soll ich das jemals debuggen - ?
						pStateFunc = (PGM_VOID_P) pgm_read_word(&menu_state[i].pFunc);
						// mtE
                        break;
                    }
                }
            }
        }
#endif
        
        //Put the ATmega169 enable power save mode if autopowersave
        if(gAutoPowerSave)
        {
            if(gPowerSaveTimer >= gPowerSaveTimeout)
            {
                state = ST_AVRBF;
                gPowerSaveTimer = 0;
                gPowerSave = TRUE;
				gUART=FALSE;			
            }
        }
        
#if !NO_JOYSTICK
        // Check if the joystick has been in the same position for some time, 
        // then activate auto press of the joystick
        buttons = (~PINB) & PINB_MASK;
        buttons |= (~PINE) & PINE_MASK;
        
        if( buttons != last_buttons ) 
        {
            last_buttons = buttons;
            gAutoPressJoystick = FALSE;
        }
        else if( buttons )
        {
            if( gAutoPressJoystick == TRUE)
            {
                PinChangeInterrupt();
                gAutoPressJoystick = AUTO;
            }
            else    
                gAutoPressJoystick = AUTO;
        }
		
#endif
        
        if(!gUART && !glogSpeed)              // Do not enter Power save if using UART or playing tunes
        {
            if(gPowerSave)
                cbi(LCDCRA, 7);             // disable LCD
			
			// mtA
            SMCR = (3<<SM0) | (1<<SE);      // Enable Power-save mode
			asm volatile ("sleep"::);
            // __sleep();                      // Go to sleep
			// mt 20031204 - avr-libc 1.0 sleep.h seems to be incompatible with mega169 
			/// no! // set_sleep_mode(SLEEP_MODE_PWR_SAVE);
			/// no! // sleep_mode();
			// mtE
            
            if(gPowerSave)
            {
                if(!(PINB & 0x40))              // press UP to wake from SLEEP
                {
                    gPowerSave = FALSE;
                    gUART = TRUE;			// wake up Uart as well
                    for(i = 0; i < 20; i++) // set all LCD segment register to the variable ucSegments
                    {
                        *(&pLCDREG_test + i) = 0x00;
                    }
#if !NO_LCD    
                    sbi(LCDCRA, 7);             // enable LCD
#endif
					
#if !NO_JOYSTICK
                    input = getkey();           // Read buttons
#endif
                }
            }
        }
        else
        {		
			// mtA
			SMCR = (1<<SE);                 // Enable idle mode
			asm volatile ("sleep"::);
			//__sleep();                      // Go to sleep        
			// mt 20031204 - avr-libc 1.0 sleep.h seems to be incompatible with mega169 
			/// no! // set_sleep_mode(SLEEP_MODE_IDLE);
			/// no! // sleep_mode();
			// mtE
            
        }   
		
        SMCR = 0;                       // Just woke, disable sleep
										// do logging if needed
		if (gLogNow) 
		{	
			doLogging();
		}
    } //End Main loop
	
	return 0; // mt 
}



#if !NO_MENUS
/*****************************************************************************
*
*   Function name : StateMachine
*
*   Returns :       nextstate
*
*   Parameters :    state, stimuli
*
*   Purpose :       Shifts between the different states
*
*****************************************************************************/
unsigned char StateMachine(char state, unsigned char stimuli)
{
    unsigned char nextstate = state;    // Default stay in same state
    unsigned char i;
	
    // mt: for (i=0; menu_nextstate[i].state; i++)
	for (i=0; pgm_read_byte(&menu_nextstate[i].state); i++)
    {
        // mt: if (menu_nextstate[i].state == state && menu_nextstate[i].input == stimuli)
		if (pgm_read_byte(&menu_nextstate[i].state) == state && 
			pgm_read_byte(&menu_nextstate[i].input) == stimuli)
        {
            // This is the one!
            // mt: nextstate = menu_nextstate[i].nextstate;
			nextstate = pgm_read_byte(&menu_nextstate[i].nextstate);
            break;
        }
    }
	
    return nextstate;
}
#endif



/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
    unsigned char tst;           // dummy
	
    OSCCAL_calibration();       // calibrate the OSCCAL byte
	
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
	
    // set prescaler = 2, Inter RC 8Mhz / 2 = 4Mhz
	// WARNING NOW RUNNING AT 4MHZ
    CLKPR = (0<<CLKPS1) | (1<<CLKPS0);
	
    // Disable Analog Comparator (power save)
    ACSR = (1<<ACD);
	
    // Disable Digital input on PF0-2 (power save)
    DIDR1 = (7<<ADC0D);
	
    // mt PORTB = (15<<PORTB0);       // Enable pullup on 
	PORTB = (15<<PB0);       // Enable pullup on 
							 // mt PORTE = (15<<PORTE4);
	PORTE = (15<<PE4);
	PORTF &= ~((1<<PORTF4)|(1<<PORTF6));
	DDRF &=	~(1<<PORTF4);
	

    Button_Init();              // Initialize pin change interrupt on joystick

    RTC_init();                 // Start timer2 asynchronous, used for RTC clock
	
#if !NO_MENUS
    Timer0_Init();              // Used for call backs
#endif
	
#ifdef OSC_ALT
	 USART_Init(3);             // Baud rate = 28800bps @ 1Mhz, 115200 @ 4Mhz
#else
    USART_Init(12);             // Baud rate = 9600bps @ 1Mhz, 38400 @ 4Mhz
#endif
	
    DF_SPI_init();              // init the SPI interface to communicate with the DataFlash
    tst = Read_DF_status();
    DF_CS_inactive;             // disable DataFlash
	USART_Tx_Hex(tst);			// dumpflash status on powerup as a safety check.
	
#if NO_LCD == 0
    LCD_Init();                 // initialize the LCD
#endif
	
#if EN_LOG_SPEED
	Speed_init();				// init Windspeed counter
#else
	gSpeedSecondsToLog =0;
#endif
#if EN_LOG_SHT
	ShtInit();					//  Initialize Temp / Humidity sensor;
	ShtReadStatus(&tst);
	USART_Tx_Hex(tst);	
#endif

#if EN_LOG_DS1820
	ds18x20_init();
#endif
	
  	
	ALARM_clear();
}




#if NO_LCD == 0 
/*****************************************************************************
*
*   Function name : BootFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Reset the ATmega169 which will cause it to start up in the 
*                   Bootloader-section. (the BOOTRST-fuse must be programmed)
*
*****************************************************************************/
// mt __flash char TEXT_BOOT[]                     
// mt - as in jw-patch: const char TEXT_BOOT[] PROGMEM	= "Jump to bootloader";

char BootFunc(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        // mt jw LCD_puts_f(TEXT_BOOT, 1);
		LCD_puts_f(PSTR("Jump to bootloader"), 1);
    }
    else if(input == KEY_ENTER)
    {
        // mt WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
		WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
        while(1);   // wait for watchdog-reset, since the BOOTRST-fuse is 
                    // programmed, the Boot-section will be entered upon reset.
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_BOOT;
    }
    
    return ST_OPTIONS_BOOT_FUNC;
}
#endif




#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : PowerSaveFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable power save
*
*****************************************************************************/
// mt __flash char TEXT_POWER[]                     = "Press enter to sleep";
// mt jw const char TEXT_POWER[]  PROGMEM  = "Press enter to sleep";

char PowerSaveFunc(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        //mt jw LCD_puts_f(TEXT_POWER, 1);
		LCD_puts_f(PSTR("Press enter to sleep"), 1);
    }
    else if(input == KEY_ENTER)
    {
        gPowerSave = TRUE;
        enter = 1;
        return ST_AVRBF;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_POWER_SAVE;
    }
	
    return ST_OPTIONS_POWER_SAVE_FUNC;
	
}
#endif

#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : AutoPower
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable auto power save
*
*****************************************************************************/
char AutoPower(char input)
{
    static char enter = 1;    
    
    char PH;
    char PL;
    
    if(enter)
    {
        enter = 0;
        
        if(gAutoPowerSave)  
        {     
            PH = CHAR2BCD2(gPowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
			
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
			
            LCD_UpdateRequired(TRUE, 0);    
        }
        else
            LCD_puts_f(PSTR("Off"),1);	// mt LCD_puts("Off", 1);        
		
    }
    else if(input == KEY_ENTER)
    {
		enter = 1;
		
		return ST_OPTIONS_AUTO_POWER_SAVE;
    }
    else if (input == KEY_PLUS)
    {
		
        gPowerSaveTimeout += 5;
		
        if(gPowerSaveTimeout > 90)
        {
            gPowerSaveTimeout = 90;
        }
        else
        {    
            gAutoPowerSave = TRUE;
			
            PH = CHAR2BCD2(gPowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
			
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
			
            LCD_UpdateRequired(TRUE, 0);        
        }
    }
    else if (input == KEY_MINUS)
    {
        if(gPowerSaveTimeout)
            gPowerSaveTimeout -= 5;
		
        if(gPowerSaveTimeout < 5)
        {
            gAutoPowerSave = FALSE;
            gPowerSaveTimeout = 0;
            LCD_puts_f(PSTR("Off"),1);	// mt LCD_puts("Off", 1);
        }
        else
        {   
            gAutoPowerSave = TRUE;
			
            PH = CHAR2BCD2(gPowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
            
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
			
            LCD_UpdateRequired(TRUE, 0);                     
        }
    }
	
    return ST_OPTIONS_AUTO_POWER_SAVE_FUNC;    
	
}
#endif
#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : LogTiming
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable RS232 Logging
*
*****************************************************************************/
char LogTiming(char input)
{
    static char enter = 1;    
	static char logging,logtime,logunit;
    
    if(enter)
    {
        enter = 0;
        
		// get local copy of global variables.
		logging=gLogging;
		logtime=gLogTime;
		logunit=gLogTimeUnit;
		
        if(logging)  
        {   
			PrintLogTime( logtime,  logunit); 
		}
        else
            LCD_puts("Off", 1);        
        
    }
    else if(input == KEY_ENTER)
    {
		enter = 1;
		
		//save changes and exit
		gLogging = logging;		
		gLogTime = logtime;
		gLogTimeUnit= logunit ;
		
		return ST_OPTIONS_LOGTIME;
    }
    else if (input == KEY_PLUS)
    {
		
        logtime += 1;
		
        if(logtime > 90)
        {
            logtime = 90;
        }
        else
        {    
            logging = TRUE;
			
			PrintLogTime( logtime,  logunit);         
        }
    }
    else if (input == KEY_MINUS)
    {
        if(logtime)
            logtime -= 1;
		
        if(logtime <1 )
        {
            logging = FALSE;
            logtime = 0;
            LCD_puts("Off", 1);
        }
        else
        {   
            logging = TRUE;
			
			PrintLogTime( logtime,  logunit);                     
        }
    }
	else if (input == KEY_NEXT)
	{
		logunit--;
		if (logunit > TICK)
			logunit = HOUR;
		if(logging)
        {
			PrintLogTime( logtime,  logunit);         }
        else
        {   
			LCD_puts("Off", 1);
        }
    }
	else if (input == KEY_PREV)
	{
		logunit++;
		if (logunit > TICK)
			logunit = TICK;
		if(logging)
        {
			PrintLogTime( logtime,  logunit); 
        }
        else
        {   
			LCD_puts("Off", 1);
        }
	}
    return ST_OPTIONS_LOGTIME_FUNC;    
}
#endif
#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : PrintLogTime
*
*   Returns :       None
*
*   Parameters :    char logtime, char logunit: the unit and time between logs.
*
*   Purpose :       Print a time to the LCD
*
*****************************************************************************/
void PrintLogTime(char logtime, char logunit)
{
    char PH;
    char PL;
	
	PH = CHAR2BCD2(logtime);
	PL = (PH & 0x0F) + '0';
	PH = (PH >> 4) + '0';
	
	if (logunit==HOUR)
	{
		LCD_putc(0, 'H');
		LCD_putc(1, 'R');
		LCD_putc(2, 'S');
	}else if (logunit==MINUTE)
	{
		LCD_putc(0, 'M');
		LCD_putc(1, 'I');
		LCD_putc(2, 'N');
	}else if (logunit==SECOND)
	{
		LCD_putc(0, 'S');
		LCD_putc(1, 'E');
		LCD_putc(2, 'C');
	}else if (logunit==TICK)
	{
		LCD_putc(0, 'T');
		LCD_putc(1, 'C');
		LCD_putc(2, 'K');
	}	
	
	LCD_putc(3, ' ');
	LCD_putc(4, PH);
	LCD_putc(5, PL);
	LCD_putc(6, '\0');
	
	LCD_UpdateRequired(TRUE, 0); 
}
#endif
/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop (based on a 1Mhz Clock)
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
    // mt, int i did not work in the simulator:  int i; 
	uint8_t i;
    
    while (millisec--)
		//mt: for (i=0; i<125; i++);
		for (i=0; i<125; i++)  
			asm volatile ("nop"::);
}


#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : Revision
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Display the software revision
*
*****************************************************************************/
char Revision(char input)
{
	static char enter = 1;
	//char TH;
	//char TL;     
	//char HB;
	//int Temp;
	
    if(enter == 1)
    {
        enter = 0;
        
		// mtA 
        LCD_putc(0, 'R'); // LCD_putc(0, 'R');
        LCD_putc(1, 'E'); // LCD_putc(1, 'E');
        LCD_putc(2, 'V'); // LCD_putc(2, 'V');
						  // LCD_putc(3, ' ');
        LCD_putc(3, (SWHIGH + 0x30)); // LCD_putc(4, (SWHIGH + 0x30));       //SWHIGH/LOW are defined in "main.h"
        LCD_putc(4, (SWLOW + 0x30)); // LCD_putc(5, (SWLOW + 0x30));
		LCD_putc(5, (SWREV + 0x40)); // LCD_putc(5, (SWLOW + 0x30));
        LCD_putc(6, '\0');
		// mtE
        
        LCD_UpdateRequired(TRUE, 0);          
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_AVRBF;
    }
	
    return ST_AVRBF_REV;
}
#endif
#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : UploadFunc
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Send data to RS232
*
*****************************************************************************/         
//__flash char TEXT_UPLOAD[]                     = "Press enter to send data"  ;
char UploadFunc(char input)
{
	static char enter = 1;
	
	if(enter == 1)
	{
		enter = 0;
		LCD_puts_f(PSTR("Press enter to send data"),1);
		//	LCD_UpdateRequired(TRUE, 0);       
	}
	else if (input == KEY_PREV)
	{
		enter = 1;
		return ST_OPTIONS_UPLOAD;
	}
	else if (input == KEY_ENTER)    
	{
		// send current data
		DumpFlash(DUMP_NORMAL);
		enter=1;
		
	}
	else if (input == KEY_PLUS)    
	{
		// send ALL the data
		DumpFlash(DUMP_ALL);
		enter=1;
		
	}
	else if (input == KEY_MINUS)    
	{
		// send ALL the data
		// in binary format (lots Faster)
		DumpFlash(DUMP_BIN);
		enter=1;
		
	}
	return ST_OPTIONS_UPLOAD_FUNC;
}
#endif

#if NO_LCD == 0
/*****************************************************************************
*
*   Function name : ResetFlash
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Reset current flash page to zero.
*
*****************************************************************************/         
//__flash char TEXT_RESET[]                     = "Press enter to Reset"  ;
char ResetFlash(char input)
{
	static char enter = 1;
	
	if(enter == 1)
	{
		enter = 0;
		LCD_puts_f(PSTR("Press enter to Reset"),1);
		
	}
	else if (input == KEY_PREV)
	{
		enter = 1;
		return ST_OPTIONS_RESET;
	}
	else if (input == KEY_ENTER)    
	{
		gRolloverFlash = 0;
		gDataPage = 0;
		gDataPosition=0;
		return ST_AVRBF;
	}
	
	return ST_OPTIONS_RESET_FUNC;
}
#endif


/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;
	
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
								// set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2
	
    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 
	
    TIMSK0 = 0;             // delete any interrupt sources
	
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling
	
    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared
	
    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
		
        while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag
		
        TCCR1B = 0; // stop timer1
		
        sei(); // __enable_interrupt();  // enable global interrupt
		
        if ( (TIFR1 && (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
		

        if (temp > OSC_MAX)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < OSC_MIN)
        {
            OSCCAL++;   // the internRC oscillator runs too slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct
		
        TCCR1B = (1<<CS10); // start timer1
    }
}

/*****************************************************************************
*
*   Function name : DumpFlash
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Dump the flash contents to the usart :)
*
*****************************************************************************/
void DumpFlash(char dumpall)     
{ 
	unsigned int page=0;
	int totalPages=0;
	
	gUploading=TRUE;
	
	
	if (dumpall || gRolloverFlash)
		totalPages=TOTALPAGESINFLASH;
	else
		totalPages=gDataPage;
	
	for (page=0;page<totalPages;page++){ // 0 to last logged page            	
		DF_Print_page(page,RECORDSPERPAGE,dumpall);
	}    
	
	if (!dumpall){// we need to Print the last partial page 
		DF_Print_page(page,gDataPosition,0);
	}
	
	USART_Tx(4); // end with EOT
	USART_Tx(4); // end with EOT
	USART_Tx(4); // end with EOT
				 //USART_Tx(26); // and SUB signals 
	
	DF_CS_inactive;
	
	while (!(UCSRA & (1<<TXC))); // wait for last frame to empty 
	
	gUploading = FALSE;
	
	
}


/*****************************************************************************
*
*   Function name : DF_write_int
*
*   Returns :       None
*
*   Parameters :    int data
*
*   Purpose :       write a 16 bit value to the dataflash
*
*****************************************************************************/
void DF_write_int(int data)
{
	unsigned char tmpH,tmpL;
	
	tmpH=data >> 8;
	tmpL=data & 0x00ff;
	DF_SPI_RW(tmpH);
	DF_SPI_RW(tmpL);
}


/*****************************************************************************
*
*   Function name : DF_read_int
*
*   Returns :       int data
*
*   Parameters :    None
*
*   Purpose :       read a 16 bit int from the dataflash
*
*****************************************************************************/
int DF_read_int(void)
{
	unsigned char tmpH,tmpL;
	unsigned int tmpInt;
	
	tmpH = DF_SPI_RW(0x00);
	tmpL = DF_SPI_RW(0x00);
	tmpInt = ((tmpH<<8)+tmpL);
	
	return tmpInt;
}


/*****************************************************************************
*
*   Function name : DF_Print_page
*
*   Returns :       None
*
*   Parameters :    char page no of flash to read, char number of records to print
*					char mode to print (ASCII or Binary)
*
*   Purpose :       output 1 page of flash to the usart or buffer if partial page
*
*****************************************************************************/
void DF_Print_page(unsigned int page, char recordstoprint, char mode)
{
	unsigned char pos=0;
	unsigned char record; 
	unsigned char tmp[3];
	unsigned int  tmpInt;
#if DS1820_COUNT>1 
	unsigned char idx;
#endif
	DF_CS_inactive;
	DF_CS_active;  
	
	if (recordstoprint == RECORDSPERPAGE){//read from flash
		Cont_Flash_Read_Enable (page,0);// 0 == start of page  
	}else{//assume we are reading the last page from the buffer
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		for (pos=0;pos<4;pos++)
			 DF_SPI_RW(0x00);			//don't cares and zero address

	}
	
	for (record=0;record<recordstoprint;record++){
		if (mode==DUMP_BIN){
			for (pos=0;pos<RECORDSIZE;pos++) 
				USART_Tx(DF_SPI_RW(0x00));
		}else{
			
			// START OF NEW RECORD
			
#if EN_LOG_DATE
			// read and print the date
			tmp[1] = DF_SPI_RW(0x00);
			tmp[0] = DF_SPI_RW(0x00);
			PrintDate(tmp[0],tmp[1]);
			USART_Tx(' ');
#endif
			
#if EN_LOG_CLOCK
			// read and print the time
			for (pos=0;pos<3;pos++)
				tmp[pos] = DF_SPI_RW(0x00);
			PrintClock(tmp[2],tmp[1],tmp[0]);
			USART_Tx(' ');
#endif
			//Dump params as RAW ADC counts 10Bit #s
			for (pos=0;pos<(NOOFPARAMS-NONSTDPARAMS);pos++){     
				tmpInt=DF_read_int();
				ADC2RAW(TRUE,tmpInt);	//convert and log
				USART_Tx(' ');
			} 
			
			// Non-standard parmas with custom output formatting
			// go here
			//
#if EN_LOG_BATTERY
			tmpInt=DF_read_int();
			ADC2Volt(TRUE,tmpInt);	//convert and log
			USART_Tx(' ');
#endif
			
#if EN_LOG_SHT
			// Dump temp and humidity as raw HEX bytes
			for (tmp[0]=0;tmp[0]<2;tmp[0]++){
				for (pos=0;pos<2;pos++)
					USART_Tx_Hex(DF_SPI_RW(0x00));
				USART_Tx(' ');
			}
#endif
			
#if EN_LOG_SPEED
			// speed can have value greater than 1023 so we use
			// special printing function for a 5 digit number
			tmpInt=DF_read_int();
			PrintSpeed(tmpInt);
#endif
			
#if EN_LOG_TEMP
			//last entry is a temperature
			tmpInt=DF_read_int();
			ADC2Temp(TRUE,tmpInt);
			USART_Tx(' ');
#endif
			
#if EN_LOG_DS1820
#if DS1820_COUNT>1 
			
			for (idx=0;idx<DS1820_COUNT;idx++){
				for (pos=0;pos<3;pos++){
					tmp[pos] = DF_SPI_RW(0x00);
				}
				DS18x20_uart_put_temp(tmp[0], tmp[1], tmp[2]);	
				USART_Tx(' ');
			}
#else
			for (pos=0;pos<3;pos++){
				tmp[pos] = DF_SPI_RW(0x00);
			}
			DS18x20_uart_put_temp(tmp[0], tmp[1], tmp[2]);	
#endif		
#endif
			USART_EOL(gEOL);
		}
	} 
}

/*****************************************************************************
*
*   Function name : doLogging
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       read all the ADC Channels and save to flash
*					this takes ~10ms for adc samples.
*****************************************************************************/
void doLogging(void)
{
	unsigned int	log_data=0;
#if !NO_MENUS
	char	cbdisable;
#endif
	char	adcv;
#if EN_LOG_DIR | EN_LOG_DS1820
	char	i; 
#endif
#if EN_LOG_DS1820
	uint8_t subzero, cel, cel_frac_bits;
#endif
	// exit if flash is full
	if (!gEnableRollover && gRolloverFlash) return;
	
#if !NO_MENUS
	// we should never do this if we are in view mode, so....
	cbdisable = Timer0_RemoveCallbackFunction(ADC_periphery); //disable lcd updates for a while
#endif
	adcv = ADMUX; 
	
	//log to buffer
	DF_CS_inactive;
	DF_CS_active;  
	
	Buffer_Write_Enable (1,gDataPosition*RECORDSIZE);
	
#if EN_LOG_DATE
	LogDate(); 				// send the Date to flash	
#endif
	
#if EN_LOG_CLOCK
	LogClock();				// send the Clock to flash	
#endif
	
	// log Light 
#if EN_LOG_LIGHT
	ADC_init(LIGHT_SENSOR);				// set up adc
	log_data=1023-ADC_read();			// read adc
	DF_write_int(log_data);
#endif
	
	// Log direction XYZ Accerlerometer
	// (ADC4..6)
#if EN_LOG_DIR
	for (i=0;i<DIRECTION_ADCS;i++){
		ADC_init(DIR_SENSOR+i);			// set up adc
		log_data=ADC_read();			// read adc
		DF_write_int(log_data);
	}
#endif
	
	// Log Battery Voltage
#if EN_LOG_BATTERY
	ADC_init(VOLTAGE_SENSOR);			// set up adc
	log_data=ADC_read();				// read adc
	DF_write_int(log_data);
#endif
	
	// Log Humidity and external Temp
#if EN_LOG_SHT
	ShtReset();
	log_data=ShtMeasure(SHT_TEMPERATURE);
	DF_write_int(log_data);
	log_data=ShtMeasure(SHT_HUMIDITY);
	DF_write_int(log_data);
#endif
	
	//log wind speed (rpm)
#if EN_LOG_SPEED
	DF_write_int(gSpeedTotal);
	gSpeedTotal=0;						// clear this if manual logging is being used
#endif
	
	//log temp
#if	EN_LOG_TEMP
	ADC_init(TEMPERATURE_SENSOR);		// set up adc
	log_data=ADC_read();				// read adc
	DF_write_int(log_data);
#endif

	// log ds18x20
#if EN_LOG_DS1820
	#if DS1820_COUNT>1 
		for (i=0;i<gnSensors;i++){
			if(ds18x20_read(i,&subzero, &cel, &cel_frac_bits)){
				DF_SPI_RW(subzero);
				DF_SPI_RW(cel);
				DF_SPI_RW(cel_frac_bits);
			}else{DF_SPI_RW(0x01);DF_SPI_RW(0);DF_SPI_RW(0);}
		}
		for (i=gnSensors;i<DS1820_COUNT;i++){ // signal error sensor not found, -0.0
			DF_SPI_RW(0x01);DF_SPI_RW(0);DF_SPI_RW(0);
			
		}
	#else
		if(ds18x20_read(0,&subzero, &cel, &cel_frac_bits)){
			DF_SPI_RW(subzero);
			DF_SPI_RW(cel);
			DF_SPI_RW(cel_frac_bits);
		}else{DF_SPI_RW(0x01);DF_SPI_RW(0);DF_SPI_RW(0);}		
	#endif
	
#endif	
	gDataPosition++;
	
	//transfer to DataFlash if the page is full
	if (gDataPosition==RECORDSPERPAGE){
		Buffer_To_Page (1,gDataPage);             
		gDataPosition=0;
		gDataPage++;
		
		//Check for Flash Rollover
		if(gDataPage==TOTALPAGESINFLASH){
			gRolloverFlash++; // dump entire flash from now on.
			gDataPage=0; // reset to page 0;
		}
		
		// read the next page into buffer 1
		//Page_To_Buffer (gDataPage, 1); 
		
	}
	
	DF_CS_inactive;
	
#if !NO_MENUS
	if (cbdisable){
		ADMUX=adcv;
		Timer0_RegisterCallbackFunction(ADC_periphery); 			
	}
#endif
	
	gLogNow=FALSE;				// clear flag
	
}	


