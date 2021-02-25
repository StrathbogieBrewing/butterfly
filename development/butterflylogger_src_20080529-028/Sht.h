//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

//----------------------------------------------------------------------------------
//
// Sensirion SHT1x Humidity Sensor Library
//
//----------------------------------------------------------------------------------

#ifndef __sht_h
#define __sht_h

#define XTAL 				3686400		// Processor clock

#define SHT_TEMPERATURE 	0x03	
// Measure temp - for ShtMeasure
#define SHT_HUMIDITY 		0x05		// Measure humidity - for ShtMeasure

#define SHT_DDR				DDRE		// Port with clock and data pins
#define SHT_PORT			PORTE		// Port with clock and data pins
#define SHT_PIN				PINE		// Port with clock and data pins
#define SHT_CLOCK			4			// Pin used to output clock to SHT
#define SHT_DATA			5			// Pin used to read/output data from/to SHT
#define SHT_PWR 			6			// Pin used to power sensor (for powersave modes)

#define SHT_DELAY			25			// uS delay between clock rise/fall

#define STATUS_REG_W 		0x06 		// Command to read status register
#define STATUS_REG_R 		0x07 		// Command to write status register
#define RESET 				0x1e 		// Command for soft reset (not currently used)

//#define DELAY(us)			(Delay_us(((us*(XTAL/1000000))-16)/6))
#define DELAY(us)			(delay_us((us)))
#define CYCLES_PER_US 		((XTAL+500000)/1000000) 	/* cpu cycles per microsecond */
/*
// ( GNU compatability Macros )
#define _BV(bit)			(1 << (bit))
#ifndef sbi
	#define sbi(x,bit)			(x |= _BV(bit)) // set bit  ( GNU compatability )
	#define cbi(x,bit)			(x &= ~_BV(bit)) // clear bit  ( GNU compatability )
#endif
#ifndef bit_is_set
	#define bit_is_set(sfr, bit) (sfr & _BV(bit))
	#define bit_is_clear(sfr, bit) (!(sfr & _BV(bit)))
#endif
*/
void ShtInit(void);
void ShtReset(void);
int  ShtMeasure(unsigned char mode);
//void ShtCalculate(int *p_temperature, int *p_humidity);
char ShtReadStatus(unsigned char *p_value);
char ShtWriteStatus(unsigned char value);

//void Delay_us(unsigned long us);

#endif
