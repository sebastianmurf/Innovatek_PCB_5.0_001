#ifndef __1WIRE_H__
#define __1WIRE_H__

#define IBUTTON_FAMILY_CODE 0x01
#define TICO_FAMILY_CODE 	0x02	//actually it is 0x01 which matches with IBUTTON_FAMILY_CODE
#define DS18B20_FAMILY_CODE 0x28

#define ONE_WIRE_TEMPERATURE_ID 1
#define ONE_WIRE_TICO_MIFARE_ID 2
#define ONE_WIRE_IBUTTON_ID 3

#define RFID_READER_1W_H5_05_M12

#include "delay.h"

extern boolean_t FindAllOneWireDevicesFlag;
extern void FindAllOneWireDevices();

extern void OneWireReset(void);
extern char OneWirePresence(void);
extern void OneWireWriteByte(unsigned char dat);
extern unsigned char OneWireReadByte(void);

//DQ at pin 53 P04
#define DQ	(1<<4)

#define DQ_INIT_AS_OUTPUT	FM3_GPIO->PFR0 &=~DQ; FM3_GPIO->PDOR0 |= DQ; FM3_GPIO->DDR0 |= DQ;
#define DQ_LOW FM3_GPIO->PDOR0 &= ~DQ
#define DQ_HIGH FM3_GPIO->PDOR0 |= DQ
#define DQ_TOG FM3_GPIO->PDOR0 ^= DQ

#define DQ_INIT_AS_INPUT  FM3_GPIO->PFR0 &= ~DQ;FM3_GPIO->PCR0 |= DQ; FM3_GPIO->DDR0 &= ~DQ;

extern uc OneWireID;
extern union union_temp OneWireData;


#endif