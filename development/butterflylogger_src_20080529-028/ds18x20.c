
/*********************************************************************************
Title:    DS18X20-Functions via One-Wire-Bus
Author:   Martin Thomas <eversmith@heizung-thomas.de>   
          http://www.siwawi.arubi.uni-kl.de/avr-projects
Software: avr-gcc 3.4.1 / avr-libc 1.0.4 
Hardware: any AVR - tested with ATmega16/ATmega32 and 3 DS18B20

Partly based on code from Peter Dannegger and others

changelog:
20041124 - Extended measurements for DS18(S)20 contributed by Carsten Foss (CFO)
200502xx - function DS18X20_read_meas_single
20050310 - DS18x20 EEPROM functions (can be disabled to save flash-memory)
           (DS18X20_EEPROMSUPPORT in ds18x20.h)
20070919 - modified for ButterflyLogger project by Nick Lott 
           http://butterflylogger.sf.net/
**********************************************************************************/

#include <avr/io.h>

#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"
#include "usart.h"

//#ifdef DS18X20_EEPROMSUPPORT
// for 10ms delay in copy scratchpad
#include "delay.h"
//#endif

uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];
uint8_t search_sensors(void);
#if MAXSENSORS>1
uint8_t gnSensors;
#endif


void ds18x20_init(void)
{

#if MAXSENSORS>1
	gnSensors = search_sensors();
#endif
	ds18x20_print(0);
	
}


void DS18x20_uart_put_temp(const uint8_t subzero, uint8_t cel, 
		   uint8_t cel_frac_bits)
{

	USART_Tx((subzero)?'-':'+');
	USART_Tx_Byte(cel );
	USART_Tx('.');
	USART_Tx_Byte( cel_frac_bits*DS18X20_FRACCONV/100);
	
	//cel_frac_bits*DS18X20_FRACCONV      = 4 decimal place
	//cel_frac_bits*DS18X20_FRACCONV/10   = 3dp
	//cel_frac_bits*DS18X20_FRACCONV/100  = 2dp
	//cel_frac_bits*DS18X20_FRACCONV/1000 = 1dp
}



//do a reading return 1 if ok 0 if error
uint8_t ds18x20_read(uint8_t nSensor, uint8_t *subzero, uint8_t *cel, uint8_t *cel_frac_bits)
{

#if MAXSENSORS>1		
	if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, 
				 &gSensorIDs[nSensor][0] ) == DS18X20_OK ) {
		delay_ms(DS18B20_TCONV_12BIT);
		if (DS18X20_read_meas( &gSensorIDs[nSensor][0], subzero,
					cel, cel_frac_bits) == DS18X20_OK ) {
				return 1;
			
		}
	}
	return 0;
	//	else uart_puts_P("Start meas. failed (short circuit?)");
#else	
	if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, 0 ) == DS18X20_OK ) {
		delay_ms(DS18B20_TCONV_12BIT);
		if(DS18X20_read_meas_single(SENSOR_ID,subzero,
					 cel, cel_frac_bits) == DS18X20_OK ) {
			return 1;	
		}
	}
	return 0;
	//	else uart_puts_P("Start meas. failed (short circuit?)");

#endif
		//		else uart_puts_P("CRC Error (lost connection?)");
		//		uart_puts_P("\r");

}


void ds18x20_print(uint8_t nSensor)
{
	uint8_t subzero, cel, cel_frac_bits;
	
	if(ds18x20_read(nSensor,&subzero, &cel, &cel_frac_bits)){
		DS18x20_uart_put_temp(subzero, cel, cel_frac_bits);
	}

}





/* 
convert raw value from DS18x20 to Celsius
   input is: 
   - familycode fc (0x10/0x28 see header)
   - scratchpad-buffer
   output is:
   - cel full celsius
   - fractions of celsius in millicelsius*(10^-1)/625 (the 4 LS-Bits)
   - subzero =0 positiv / 1 negativ
   always returns  DS18X20_OK
   TODO invalid-values detection (but should be covered by CRC)
*/
uint8_t DS18X20_meas_to_cel( uint8_t fc, uint8_t *sp, 
	uint8_t* subzero, uint8_t* cel, uint8_t* cel_frac_bits)
{
	uint16_t meas;
	uint8_t  i;
	
	meas = sp[0];  // LSB
	meas |= ((uint16_t)sp[1])<<8; // MSB
	//meas = 0xff5e; meas = 0xfe6f;
	
	
#if SENSOR_ID == DS18S20_ID
	//  only work on 12bit-base
//	if( fc == DS18S20_ID ) { // 9 -> 12 bit if 18S20
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
		meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
		meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
//	}
#endif
	
	// check for negative 
	if ( meas & 0x8000 )  {
		*subzero=1;      // mark negative
		meas ^= 0xffff;  // convert to positive => (twos complement)++
		meas++;
	}
	else *subzero=0;
	
#if SENSOR_ID == DS18B20_ID
	// clear undefined bits for B != 12bit
//	if ( fc == DS18B20_ID ) { // check resolution 18B20
		i = sp[DS18B20_CONF_REG];
		if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;
		else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT ) 
			meas &= ~(DS18B20_11_BIT_UNDF);
		else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT ) 
			meas &= ~(DS18B20_10_BIT_UNDF);
		else { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) { 
			meas &= ~(DS18B20_9_BIT_UNDF);
		}
//	}			
#endif	
	
	*cel  = (uint8_t)(meas >> 4); 
	*cel_frac_bits = (uint8_t)(meas & 0x000F);
	
	return DS18X20_OK;
}


/* start measurement (CONVERT_T) for all sensors if input id==NULL 
   or for single sensor. then id is the rom-code */
uint8_t DS18X20_start_meas( uint8_t with_power_extern, uint8_t id[])
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_CONVERT_T, id );
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		return DS18X20_OK;
	} 
	else { 
		return DS18X20_START_FAIL;
	}
}

#if MAXSENSORS>1

uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
	
	nSensors = 0;
	
	for( diff = OW_SEARCH_FIRST; 
	     diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );
		
		for (i=0;i<OW_ROMCODE_SIZE;i++)
			gSensorIDs[nSensors][i]=id[i];
		
		nSensors++;
	}
	
	return nSensors;
}


/* find DS18X20 Sensors on 1-Wire-Bus
input/ouput: diff is the result of the last rom-search
output: id is the rom-code of the sensor found */
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
	for (;;) {
		*diff = ow_rom_search( *diff, &id[0] );
		if ( *diff==OW_PRESENCE_ERR || *diff==OW_DATA_ERR ||
		     *diff == OW_LAST_DEVICE ) return;
		if ( id[0] == DS18B20_ID || id[0] == DS18S20_ID ) return;
	}
}


/* reads temperature (scratchpad) of sensor with rom-code id
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac 
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas(uint8_t id[], uint8_t *subzero, 
	uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];
	
	ow_reset(); //**
	ow_command(DS18X20_READ, id);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}
#else
/* reads temperature (scratchpad) of a single sensor (uses skip-rom)
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac 
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas_single(uint8_t familycode, uint8_t *subzero, 
	uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];
	
	ow_command(DS18X20_READ, NULL);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(familycode, sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}
#endif
