#ifndef __GPIO_H__
#define __GPIO_H__

#include"common.h"

//port 0
#define P00	(1<<0)	//M95 DTR
#define P01 (1<<1)	//DO7
#define P02 (1<<2)	//DO8
#define P03 (1<<3)	//POWER_RESET
#define P04 (1<<4)	//ONEWIRE
#define P0A (1<<10)	//LFS SDA
#define P0B (1<<11)	//LFS SCL
#define P0C (1<<12)	//M95 POWER KEY
//#define P0F (1<<15)	//ATLAS CONDUCTIVITY I2C_SCL
#define P0_MASK (P00|P01|P02|P03|P04|P0A|P0B|P0C/*|P0F*/)
#define P0_INIT	FM3_GPIO->PFR0 &=~P0_MASK; FM3_GPIO->PDOR0 &= ~P0_MASK; FM3_GPIO->DDR0 |= P0_MASK;
#define P0_TOG FM3_GPIO->PDOR0 ^= P0_MASK

#define RESET_DEVICE FM3_GPIO->PDOR0 |= P03

//port 1
#define P10	(1<<0)	//DO6
#define P11 (1<<1)	//Rx Pin for GPRS Tx
#define P12 (1<<2)	//Tx Pin for GPRS Rx
#define P13 (1<<3)	//DO9
#define P14 (1<<4)	//ANALOG INPUT OF DAC FB
#define P15 (1<<5)	//FREE 2
#define P17 (1<<7)	//Rx Pin for MAX232 Ch-2 Tx
#define P18 (1<<8)	//Tx Pin for MAX232 Ch-2 Rx
#define P19 (1<<9)	//analog input of pressure sensor
#define P1_AS_OUTPUT (P10|P13)
#define P1_MASK (P10|P11|P12|P13|P14|P17|P18|P19)
#define P1_INIT	FM3_GPIO->PFR1 &=~P1_MASK; FM3_GPIO->PDOR1 &= ~P1_MASK; FM3_GPIO->DDR1 |= P1_MASK;
#define P1_TOG FM3_GPIO->PDOR1 ^= P1_MASK

//port 2
#define P21 (1<<1)	//Rx Pin for MAX232 Ch-1 Tx
#define P22 (1<<2)	//Tx Pin for MAX232 Ch-1 Rx
#define P23 (1<<3)	//I2C SCL
#define P2_MASK (P21|P22|P23)
#define P2_INIT	FM3_GPIO->PFR2 &=~P2_MASK; FM3_GPIO->PDOR2 &= ~P2_MASK; FM3_GPIO->DDR2 |= P2_MASK;
#define P2_TOG FM3_GPIO->PDOR2 ^= P2_MASK
//#define DISABLE_DO2 FM3_GPIO->PDOR2 &= ~DO2_RELAY
//#define ENABLE_DO2 FM3_GPIO->PDOR2 |= DO2_RELAY
//#define TOGGLE_DO2 FM3_GPIO->PDOR2 ^= DO2_RELAY

//port 3
//#define P30 (1<<0)	//DI2
//#define P31 (1<<1)	//DI3
//#define P32 (1<<2)	//DI4
#define P33 (1<<3)		//USB
#define P39 (1<<9)		//DO1
#define P3A (1<<10)		//DO2
#define P3B (1<<11)		//DO3
#define P3C (1<<12)		//DO4
#define P3D (1<<13)		//DO5
//#define P3E (1<<14)	//DI5
#define P3F (1<<15)		//PWM2
#define P3_MASK (/*P30|P31|P32|*/P33|P39|P3A|P3B|P3C|P3D|/*P3E|*/P3F)
#define P3_INIT	FM3_GPIO->PFR3 &=~P3_MASK; FM3_GPIO->PDOR3 &= ~P3_MASK; FM3_GPIO->DDR3 |= P3_MASK;
#define P3_TOG FM3_GPIO->PDOR3 ^= P3_MASK

#define DUMMY_PWM_OFF	FM3_GPIO->PDOR3 &= ~P3E
#define DUMMY_PWM_ON 	FM3_GPIO->PDOR3 |= P3E
#define DUMMY_PWM_TOGGLE 		FM3_GPIO->PDOR3 ^= P3E

#define DO1_RELAY_OFF	        FM3_GPIO->PDOR3 &= ~P39
#define DO1_RELAY_ON 	        FM3_GPIO->PDOR3 |= P39
#define DO1_TOGGLE 		FM3_GPIO->PDOR3 ^= P39
#define DO2_RELAY_OFF 	        FM3_GPIO->PDOR3 &= ~P3A
#define DO2_RELAY_ON 	        FM3_GPIO->PDOR3 |= P3A
#define DO2_TOGGLE 		FM3_GPIO->PDOR3 ^= P3A
#define DO3_RELAY_OFF 	        FM3_GPIO->PDOR3 &= ~P3B
#define DO3_RELAY_ON 	        FM3_GPIO->PDOR3 |= P3B
#define DO3_TOGGLE 		FM3_GPIO->PDOR3 ^= P3B
#define DO4_RELAY_OFF 	        FM3_GPIO->PDOR3 &= ~P3C
#define DO4_RELAY_ON 	        FM3_GPIO->PDOR3 |= P3C
#define DO4_TOGGLE 		FM3_GPIO->PDOR3 ^= P3C
#define DO5_RELAY_OFF 	        FM3_GPIO->PDOR3 &= ~P3D
#define DO5_RELAY_ON 	        FM3_GPIO->PDOR3 |= P3D
#define DO5_TOGGLE 		FM3_GPIO->PDOR3 ^= P3D
#define DO6_RELAY_OFF 	        FM3_GPIO->PDOR1 &= ~P10
#define DO6_RELAY_ON 	        FM3_GPIO->PDOR1 |= P10
#define DO6_TOGGLE 		FM3_GPIO->PDOR1 ^= P10
#define DO7_RELAY_OFF 	        FM3_GPIO->PDOR0 &= ~P01
#define DO7_RELAY_ON 	        FM3_GPIO->PDOR0 |= P01
#define DO7_TOGGLE 		FM3_GPIO->PDOR0 ^= P01
#define DO8_RELAY_OFF 	        FM3_GPIO->PDOR0 &= ~P02
#define DO8_RELAY_ON 	        FM3_GPIO->PDOR0 |= P02
#define DO8_TOGGLE 		FM3_GPIO->PDOR0 ^= P02
#define DO9_RELAY_OFF 	        FM3_GPIO->PDOR1 &= ~P13
#define DO9_RELAY_ON 	        FM3_GPIO->PDOR1 |= P13
#define DO9_TOGGLE 		FM3_GPIO->PDOR1 ^= P13

//port 4
#define P46 (1<<6)	//BOOTLOADER LED
#define P47 (1<<7)	//MAIN CODE LED
#define P49 (1<<9)	//FLASH MEM_WP
#define P4A (1<<10)	//MEM_CS
#define P4B (1<<11)	//MEM_RST
#define P4C (1<<12)	//SPI_SCK
#define P4D (1<<13)	//SPI_SI
#define P4E (1<<14)	//SPI_SO
#define P4_MASK (P46|P47|P49|P4A|P4B|P4C|P4D|P4E)
#define P4_INIT	FM3_GPIO->PFR4 &=~P4_MASK; FM3_GPIO->PDOR4 &= ~P4_MASK; FM3_GPIO->DDR4 |= P4_MASK;
#define P4_TOG FM3_GPIO->PDOR4 ^= P4_MASK
#define BOOTLOADER_LED_ON 		FM3_GPIO->PDOR4 &= ~P46
#define BOOTLOADER_LED_OFF 		FM3_GPIO->PDOR4 |= P46
#define BOOTLOADER_LED_TOGGLE	FM3_GPIO->PDOR4 ^= P46
#define MAIN_PROG_LED_OFF 		FM3_GPIO->PDOR4 |= P47
#define MAIN_PROG_LED_ON 		FM3_GPIO->PDOR4 &= ~P47
#define MAIN_PROG_LED_TOGGLE	FM3_GPIO->PDOR4 ^= P47

//port 5
#define P50	(1<<0)	//FREE
#define P51 (1<<1)	//FREE
//#define P52 (1<<2)//DI1
#define P5_MASK (P50|P51/*|P52*/)
#define P5_INIT	FM3_GPIO->PFR5 &=~P5_MASK; FM3_GPIO->PDOR5 &= ~P5_MASK; FM3_GPIO->DDR5 |= P5_MASK;
#define P5_TOG FM3_GPIO->PDOR5 ^= P5_MASK
//port 6
#define P60	(1<<0)	//USB
#define P61 (1<<1)	//USB
//#define P62 (1<<2)	//ATLAS I2C CONDUCTIVITY SDA
#define P6_MASK (P60|P61/*|P62*/)
#define P6_INIT	FM3_GPIO->PFR6 &=~P6_MASK; FM3_GPIO->PDOR6 &= ~P6_MASK; FM3_GPIO->DDR6 |= P6_MASK;
#define P6_TOG FM3_GPIO->PDOR6 ^= P6_MASK
//port 8
#define P80	(1<<0)	//USB
#define P81 (1<<1)	//USB
#define P8_MASK (P80|P81)
#define P8_INIT	FM3_GPIO->PFR8 &=~P8_MASK; FM3_GPIO->PDOR8 &= ~P8_MASK; FM3_GPIO->DDR8 |= P8_MASK;
#define P8_TOG FM3_GPIO->PDOR8 ^= P8_MASK

//OUTPUT LOCATION IN REGISTER
#define OUTPUT_RLY1	0
#define OUTPUT_RLY2     1
#define OUTPUT_RLY3	2
#define OUTPUT_RLY4	3
#define OUTPUT_RLY5	4
#define OUTPUT_RLY6	5
#define OUTPUT_RLY7     6
#define OUTPUT_RLY8     7
#define OUTPUT_RLY9     8

#define MAX_NO_OF_OUTPUT	9

//name to output
#define OUTPUT_MIV	        OUTPUT_RLY1
#define OUTPUT_BYPASS           OUTPUT_RLY2
#define OUTPUT_POV	        OUTPUT_RLY3
#define OUTPUT_WOV	        OUTPUT_RLY4
#define OUTPUT_CIP	        OUTPUT_RLY5
#define OUTPUT_SHUNT	        OUTPUT_RLY6
#define OUTPUT_POLARITY         OUTPUT_RLY7
#define OUTPUT_PAE              OUTPUT_RLY8
#define OUTPUT_FREE             OUTPUT_RLY9
//ALTERNATIVE name W.R.T CFO
#define OUTPUT_MIV	        OUTPUT_RLY1
#define OUTPUT_WOV2             OUTPUT_RLY2
#define OUTPUT_POV1	        OUTPUT_RLY3
#define OUTPUT_WOV1	        OUTPUT_RLY4
#define OUTPUT_CIP	        OUTPUT_RLY5
#define OUTPUT_SHUNT	        OUTPUT_RLY6
#define OUTPUT_POLARITY         OUTPUT_RLY7
#define OUTPUT_POV2             OUTPUT_RLY8
#define OUTPUT_FREE             OUTPUT_RLY9


//INPUT-------------------------------------------------------------------------
#define DIGITAL_INPUT_2 	(1UL<<0) //0th bit of Port3 register
#define DIGITAL_INPUT_3         (1UL<<1) //1th bit of Port3 register
#define DIGITAL_INPUT_4    	(1UL<<2) //2th bit of Port3 register
//#define MOV_SENSE		(1UL<<9) //9th bit of port3 register
#define DIGITAL_INPUT_5         (1UL<<14)//14th bit of Port3 register
#define DIN3_MASK		(DIGITAL_INPUT_2 | DIGITAL_INPUT_3 | DIGITAL_INPUT_4 | DIGITAL_INPUT_5)

#define DIN3_PFR		FM3_GPIO->PFR3
#define DIN3_PDOR		FM3_GPIO->PDOR3
#define DIN3_PCR		FM3_GPIO->PCR3
#define DIN3_PDIR		FM3_GPIO->PDIR3
#define DIN3_DDR		FM3_GPIO->DDR3

#define DIGITAL_INPUT_1     	(1UL<<2) //2th bit of Port5 register
#define DIN5_MASK		(DIGITAL_INPUT_1)

#define DIN5_PFR		FM3_GPIO->PFR5
#define DIN5_PDOR		FM3_GPIO->PDOR5
#define DIN5_PCR		FM3_GPIO->PCR5
#define DIN5_PDIR		FM3_GPIO->PDIR5
#define DIN5_DDR		FM3_GPIO->DDR5

//INPUT LOCATION IN REGISTER for server identification
#define IP_CIP_LVL		0x01
#define IP_BYPASS_SIG	0X02
#define IP_LEAKAGE_SIG	0X04
#define IP_START_SWT	0X08
#define IP_SPARE_SWT    0X10
//REST 3 BIT INPUT ARE NOT DEFINED YET IN THE SERVER

//#define DIGITAL_INPUT5 0X10
//#define DIGITAL_INPUT6 0X20
//#define DIGITAL_INPUT7 0X40
//#define DIGITAL_INPUT8 0X80
struct input
{	//char Name[10];
	char RegisterBitId;
};
#define MAX_NO_OF_INPUT		8

//4 to 20mA analog input conversion to physical value
#define PHYSICAL_VALUE_HIGH     10  //bar
#define PHYSICAL_VALUE_LOW      0  //bar

extern struct input Input[MAX_NO_OF_INPUT];

extern boolean_t DisableIgnitionFlag;
extern uc CurDigitalInput,PrevDigitalInput;

extern uint16_t DigitalOutput, PrevDigitalOutput;
extern boolean_t DigitalOutputSaveFlag;

extern void GpioInit(void);
extern void ScanInput();
extern boolean_t CheckOutput(char outputid);
extern char OutputDependency2Constant(char *str);
extern char* strcatOutputDependency(uc output_alloc_id);
extern char CallAction2Constant(char *str);
extern char* strcatCallAction(uc output_alloc_id);
extern void UpdateOutput();
extern boolean_t UpdateOutputAsPerDependency(char dependid,boolean_t onoff);

#endif