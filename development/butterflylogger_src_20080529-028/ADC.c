//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//***************************************************************************
//
//  File........: ADC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.2
//
//  Description.: AVR Butterfly ADC routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20050610	      modified for VUW datalogger use				- N.Lott
//
//***************************************************************************

//mtA
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pgmspacehlp.h"
//mtE

#include "main.h"
#include "ADC.h"
#include "BCD.h"
#include "LCD_functions.h"
#include "timer0.h"
#include "usart.h"
#include "speed.h"
#include "dataflash.h"

// mt __flash int TEMP_Celcius_pos[] =    // Positive Celcius temperatures (ADC-value)
const int TEMP_Celcius_pos[] PROGMEM =    // Positive Celcius temperatures (ADC-value)
{                           // from 0 to 60 degrees
	806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
	636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
	452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
	297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
	187,
};

// mt __flash int TEMP_Celcius_neg[] =    // Negative Celcius temperatures (ADC-value)
const int TEMP_Celcius_neg[] PROGMEM =    // Negative Celcius temperatures (ADC-value)
{                           // from -1 to -15 degrees
	815,825,834,843,851,860,868,876,883,891,898,904,911,917,923,
};

char 	degree = CELCIUS; // char degree = CELCIUS;

extern char gLogging;
extern char gLogNow;
int 	gMaxTemp=1023;
int 	gMinTemp=0;


/*****************************************************************************
*
*   Function name : ADC_init
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Initialize the ADC with the selected ADC-channel
*
*****************************************************************************/
void ADC_init(char input)
{
	char i;
	
	ADCSRB = (0<<ADTS2) |(0<<ADTS1) |(0<<ADTS0);  
    ADCSRA = (0<<ADEN) |(0<<ADIE) |(1<<ADPS2)| (0<<ADPS1) | (1<<ADPS0);    // set ADC prescaler to , 4MHz / 32 = 125kHz
	
	if (input == VOLTAGE_SENSOR){
		if (!(ADMUX & ((1<<REFS0)|(1<<REFS1)))){
			ADMUX = (0<<REFS0)|(0<<REFS1);	//select external AREF
			PORTF &= ~(1<<PF3);			// discharge the external capacitor
			DDRF |= (1<<DDF3);
			
			// wait ~100uS for 0.2uF to discharge from VCC
			for (i=0; i < 134; i++) {};	// one loop takes 3 cpu cycles (740nS @ 4Mhz) 
			DDRF &= ~(1<<DDF3);
		}
		ADMUX = VOLTAGE_SENSOR|(1<<REFS0)|(1<<REFS1); // select internal bandgap reference see VOLTAGE_SENSOR definition
		
	}else{
		 // Enable the VCP (VC-peripheral)
		PORTF |= (1<<PF3); // mt sbi(PORTF, PORTF3);    
		DDRF|=  (1<<DDF3); // sbi(DDRF, PORTF3);    
		PORTF &= ~((1<<PF4)|(1<<PF5)|(1<<PF6)); 
		DDRF &= ~((1<<PF4)|(1<<PF5)|(1<<PF6));
		ADMUX = input;    // external AREF and ADCx
    }
	
	// do a dummy read to allow references to stabalise
    ADCSRA = (1<<ADEN)| (1<<ADSC) | (0<<ADATE) | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);    // set ADC prescaler to , 4MHz / 32 = 125kHz
    while(ADCSRA & (1<<ADSC));			// wait for conversion done, ADSC flag to zero
}


/*****************************************************************************
*
*   Function name : ADC_read
*
*   Returns :       int ADC
*
*   Parameters :    None
*
*   Purpose :       Do a Analog to Digital Conversion
*
*****************************************************************************/
int ADC_read(void)
{
    char i;
    int ADC_temp;
	// mt int ADC = 0 ;
	int ADCr = 0;
    
    // To save power, the voltage over the LDR and the NTC is turned off when not used
    // This is done by controlling the voltage from a I/O-pin (PORTF3)
	// The VCP should be disabled if the internal reference is being used
	if (ADMUX & ((1<<REFS0)|(1<<REFS1))){
		// disable the VCP (VC-peripheral)
		PORTF &= ~(1<<PF3);
		DDRF &= ~(1<<DDF3);
	}else{
		// Enable the VCP (VC-peripheral)
		PORTF |=(1<<PF3);
		DDRF |= (1<<DDF3);
	}
	
    //do a dummy readout first
    ADCSRA |=(1<<ADEN)| (1<<ADSC);      // do single conversion
    while(ADCSRA & (1<<ADSC));			// wait for conversion done, ADSC flag to zero
	
	// do the ADC conversion 8 times for better accuracy 
    for(i=0;i<8;i++){
        ADCSRA |= (1<<ADSC);			// do single conversion
        while(ADCSRA & (1<<ADSC));		// wait for conversion done, ADSC flag to zero
        
        ADC_temp = ADCL;				// read out ADCL register
        ADC_temp += (ADCH << 8);		// read out ADCH register        
		
        ADCr += ADC_temp;				// accumulate result (8 samples) for later averaging
    }
	
	// average the 8 samples
    ADCr = ADCr >> 3;					

	// disable the VCP (VC-peripheral)
    PORTF &= ~(1<<PF3);
	DDRF &= ~(1<<DDF3);
	
	// disable the ADC
	ADCSRA &= ~(1<<ADEN);
    return ADCr;
}


/*****************************************************************************
*
*   Function name : ADC_periphery
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calculates the Temperature/Voltage/Ligth from the ADC_read
*                   and puts it out on the LCD.
*
*****************************************************************************/
void ADC_periphery(void)
{
    int ADCresult = 0;
    
    if (!gLogNow) {						// don't read while lig in process
		ADCresult=ADC_read();			// read ADC Value
		
		//process ADC value accordingly
		switch ((ADMUX & 7)){
			case TEMPERATURE_SENSOR:
				ADC2Temp(FALSE,ADCresult);
				break;
			case LIGHT_SENSOR:
				//invert for light readings more light = higher reading
				ADCresult= 1023-ADCresult;
				ADC2RAW(FALSE,ADCresult);
				break;
			case VOLTAGE_SENSOR:
				ADC2Volt(FALSE,ADCresult);
				break;
			default:
				ADC2Direction(FALSE,ADCresult,(ADMUX & 0x07));
		}
    }
    
}


/*****************************************************************************
*
*   Function name : ADC2Temp
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to rs232 else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a temp
*
*****************************************************************************/
void ADC2Temp(char log,int ADCresult)
{
    //   int ADCresult = 0;
    //   int ADCresult_temp = 0;
    int Temp_int=0;
    int dif;
    int top;
    
    char Temp;
    unsigned char i = 0;
    char TL;
    char TH; 
    char VH;	
    char VL;
    
    if (ADCresult<gMaxTemp)   // these appear backwards (the gt and lt) due to NTC of thermister
		gMaxTemp=ADCresult;
    if (ADCresult>gMinTemp)
		gMinTemp=ADCresult;
    
	if(ADCresult > 810){         // If it's a negtive temperature
			for (i=0; i<=25; i++){   // Find the temperature
				if (ADCresult <= pgm_read_word(&TEMP_Celcius_neg[i])){
					// add a decimal reading through liner interpolation
					if (ADCresult < pgm_read_word(&TEMP_Celcius_neg[i])){	
						dif= pgm_read_word(&TEMP_Celcius_neg[i])-pgm_read_word(&TEMP_Celcius_neg[i-1]);
						top =(ADCresult-pgm_read_word(TEMP_Celcius_neg[i-1]))*100;
						
						Temp_int = top/(dif+1);
						i++;
					}
					break;
				}
			}
#if !NO_LCD
			if (!log) LCD_putc(0, '-');       // Put a minus sign in front of the temperature
#endif
	}else if (ADCresult < 800){   // If it's a positive temperature
		for (i=0; i<60; i++)  {
			if (ADCresult >= pgm_read_word(&TEMP_Celcius_pos[i])){
				// add a decimal reading through liner interpolation
				if (ADCresult > pgm_read_word(&TEMP_Celcius_pos[i])){	
					dif= pgm_read_word(&TEMP_Celcius_pos[i-1])-pgm_read_word(&TEMP_Celcius_pos[i]);
					top =(pgm_read_word(&TEMP_Celcius_pos[i-1])-ADCresult)*100;
					
					Temp_int = top/(dif+1);
					i--;
				}
					break;
			}
		}        

	} else{                        //If the temperature is zero degrees
		i = 0;
#if !NO_LCD
		LCD_putc(0, ' ');
#endif
	}
		
		Temp = CHAR2BCD2(i);        // Convert from char to bin
		TL = (Temp & 0x0F) + '0';   // Find the low-byte
		TH = (Temp >> 4) + '0';     // Find the high-byte
		
		Temp= CHAR2BCD2(Temp_int);
		VH = (Temp >> 4) + '0';
		VL = (Temp & 0x0F) + '0';
		
		if(log)	{
			USART_Tx(TH);
			USART_Tx(TL);
			USART_Tx('.');
			USART_Tx(VH);
			USART_Tx(VL);
		} else {
#if !NO_LCD
			LCD_putc(0, TH);
			LCD_putc(1, TL);
			LCD_putc(2, VH);
			LCD_putc(3, VL);
			LCD_putc(4, '*');
			LCD_putc(5, 'C');
			LCD_Colon(TRUE);
			LCD_putc(6, '\0');
#endif
		}

}


/*****************************************************************************
*
*   Function name : ADC2Direction
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Direction Reading
*
*****************************************************************************/
void ADC2Direction(char log, int ADCresult, char sensor)
{
    //  int Temp_int;
    int Temp;
    char TL;
    char TH; 
    char THH;
    
    Temp = CHAR2BCD3(ADCresult);                
    
    TL = (Temp & 0x0F) + '0';       
    if(TL > '9')        // if the hex-value is over 9, add 7 in order to go 
		TL += 7;        // jump to the character in the ASCII-table
    
    TH = ((Temp >>= 4)& 0x0F) + '0';
    if(TH > '9')        // if the hex-value is over 9, add 7 in order to go 
		TH += 7;        // jump to the character in the ASCII-table
    
    THH = ((Temp >>= 4)& 0x0F) + '0';
    if(THH > '9')        // if the hex-value is over 9, add 7 in order to go 
		THH += 7;        // jump to the character in the ASCII-table
    
    if (log)
    {
		USART_Tx(THH);
		USART_Tx(TH);
		USART_Tx(TL);
		USART_Tx(' ');    
    }else
    {
#if !NO_LCD
		LCD_putc(0, sensor+'T');		// print x,y or z 
		LCD_putc(1, ' ');
		LCD_putc(2, ' ');
		LCD_putc(3, THH);
		LCD_putc(4, TH);
		LCD_putc(5, TL);
		LCD_putc(6, '\0');
#endif
    }
}

/*****************************************************************************
*
*   Function name : ADC2Volt
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Voltage
*
*****************************************************************************/
void ADC2Volt(char log, int ADCresult)
{
    //	int ADCresult_temp = 0;
    char Temp;
    
    char TL;
    char VH;	
    char VL;
	
    float V_ADC;
    char VoltageHB;
    char VoltageLB;
	
    V_ADC = ( ADCresult * VREF ) / 1024; // Calculate the voltage
	   
    V_ADC = ( V_ADC * 6 );      // Multiply by 6 because of the voltage division
    
    VoltageHB = V_ADC;              // Store the high-byte
    V_ADC = ( V_ADC - VoltageHB );
    VoltageLB = ( V_ADC * 100 );    // Store the low-byte
    
    Temp = CHAR2BCD2(VoltageHB);    // Convert from char to bin
    
    TL = (Temp & 0x0F) + '0';
    
    Temp = CHAR2BCD2(VoltageLB);    // Convert from char to bin
    
    VH = (Temp >> 4) + '0';
    VL = (Temp & 0x0F) + '0';
    if (log)
    {
		USART_Tx(TL);
		USART_Tx('.');
		USART_Tx(VH);
		USART_Tx(VL);  
    }else
    {
#if !NO_LCD
		LCD_putc(0, ' ');
		LCD_putc(1, ' ');
		LCD_putc(2, TL);
		LCD_putc(3, 'v');
		LCD_putc(4, VH);
		LCD_putc(5, VL);
		LCD_putc(6, '\0');
#endif
    }   
    
}


/*****************************************************************************
*
*   Function name : ADC2RAW
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Light Reading
*
*****************************************************************************/
void ADC2RAW(char log, unsigned int ADCresult)
{
    //  int Temp_int;
    int Temp;
    //    char i = 0;
    char TL;
    char TH; 
    char THH;

    
    Temp = CHAR2BCD3(ADCresult);                
    
    TL = (Temp & 0x0F) + '0';       
    if(TL > '9')        // if the hex-value is over 9, add 7 in order to go 
		TL += 7;        // jump to the character in the ASCII-table
    
    TH = ((Temp >>= 4)& 0x0F) + '0';
    if(TH > '9')        // if the hex-value is over 9, add 7 in order to go 
		TH += 7;        // jump to the character in the ASCII-table
    
    THH = ((Temp >>= 4)& 0x0F) + '0';
    if(THH > '9')        // if the hex-value is over 9, add 7 in order to go 
		THH += 7;        // jump to the character in the ASCII-table
    
    if (log)
    {
		if (THH>'9'){ // adjust to print 10 rather than A
			USART_Tx('1');
			THH = '0';
		}
		USART_Tx(THH);
		USART_Tx(TH);
		USART_Tx(TL);        
    }else
    {
#if !NO_LCD
		LCD_putc(0, 'A');
		LCD_putc(1, 'D');
		LCD_putc(2, 'C');
		LCD_putc(3, THH);
		LCD_putc(4, TH);
		LCD_putc(5, TL);
		LCD_putc(6, '\0');
#endif
    }
}

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : TemperatureFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable temperature measurements
*
*****************************************************************************/
char TemperatureFunc(char input)
{
    static char enter = 1;
    
    if (enter)
    {
        enter = 0;
        
        ADC_init(TEMPERATURE_SENSOR);       // Init the ADC
		
        // Enable auto-run of the ADC_perphery every 10ms 
        // (it will actually be more than 10ms cause of the SLEEP)
        Timer0_RegisterCallbackFunction(ADC_periphery); 
    }
    else
        LCD_UpdateRequired(TRUE, 0);        // New data to be presented
    
    if (input == KEY_PREV)
    {
        // Disable the auto-run of the ADC_periphery
        Timer0_RemoveCallbackFunction(ADC_periphery);
        
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
        
        return ST_TEMPERATURE;
    }
    
    return ST_TEMPERATURE_FUNC;        
}
#endif

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : VoltageFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable voltage measurements
*
*****************************************************************************/
char VoltageFunc(char input)
{
    static char enter = 1;
    
    if (enter)
    {
        enter = 0;
		
        ADC_init(VOLTAGE_SENSOR);       // Init the ADC
        
        // Enable auto-run of the ADC_perphery every 10ms 
        // (it will actually be more than 10ms cause of the SLEEP)        
        Timer0_RegisterCallbackFunction(ADC_periphery);        
    }
    else
        LCD_UpdateRequired(TRUE, 0); 
	
    if (input == KEY_PREV)
    {
        // Disable the auto-run of the ADC_periphery        
        Timer0_RemoveCallbackFunction(ADC_periphery);
        
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
        
        return ST_VOLTAGE;
    }
    else
        return ST_VOLTAGE_FUNC;    
}    
#endif

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : DirectionFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable voltage measurements
*
*****************************************************************************/
char DirectionFunc(char input)
{
    static char enter = 1;
    static unsigned char sensor=0;
    if (enter)
    {
        enter = 0;
		
        ADC_init(DIR_SENSOR);       // Init the ADC
        
        // Enable auto-run of the ADC_perphery every 10ms 
        // (it will actually be more than 10ms cause of the SLEEP)        
        Timer0_RegisterCallbackFunction(ADC_periphery);        
    }
    else
        LCD_UpdateRequired(TRUE, 0); 
	if (input == KEY_PLUS){
		if (sensor<(DIRECTION_ADCS-1)){
			sensor++;
			ADC_init(DIR_SENSOR+sensor);       // Init the ADC
		}
	}
	
	if (input == KEY_MINUS){
		if (sensor){
			sensor--;
			ADC_init(DIR_SENSOR+sensor);       // Init the ADC
		}
	}
    
    if (input == KEY_PREV)
    {
        // Disable the auto-run of the ADC_periphery        
        Timer0_RemoveCallbackFunction(ADC_periphery);
        
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
        
        return ST_DIR;
    }
    else
        return ST_DIR_FUNC;    
}    
#endif

#if !NO_MENUS
/*****************************************************************************
*
*   Function name : LightFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable light measurements
*
*****************************************************************************/
char LightFunc(char input)
{
    static char enter = 1;
    
    if (enter)
    {
		
        enter = 0;
        
        ADC_init(LIGHT_SENSOR);     // Init the ADC
        
        // Enable auto-run of the ADC_perphery every 10ms 
        // (it will actually be more than 10ms cause of the SLEEP)  
        Timer0_RegisterCallbackFunction(ADC_periphery);        
    }
    else
        LCD_UpdateRequired(TRUE, 0); 
	
    if (input == KEY_PREV)
    {
        // Disable the auto-run of the ADC_periphery      
        Timer0_RemoveCallbackFunction(ADC_periphery);
        
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
		
        return ST_LIGHT;
    }
    else
        return ST_LIGHT_FUNC;    
}    
#endif
