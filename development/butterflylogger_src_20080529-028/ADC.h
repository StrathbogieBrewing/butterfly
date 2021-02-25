//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

#define TEMPERATURE_SENSOR  0
#define VOLTAGE_SENSOR      1
#define LIGHT_SENSOR        2
#define DIR_SENSOR     	    4

#define CELCIUS             3
#define FARENHEIT           4


// this is effeted by the light and temp a little
// analog reading can vary by +/- 50mV
// this is okay as we are only using it to monitor the battery voltage
#define VREF		1.1f		
							
void ADC_init(char );
int ADC_read(void);
void ADC_periphery(void);
void ADC2Temp(char log, int ADCResult);
void ADC2Volt(char log, int ADCResult);
void ADC2RAW(char log, unsigned int ADCResult);
void ADC2Direction(char log, int ADCresult,char sensor);

// Temperature sensor function
char TemperatureFunc(char);

// Voltage reader function
char VoltageFunc(char);

// Light sensor function
char LightFunc(char);

//Direction reader function
char DirectionFunc(char);


