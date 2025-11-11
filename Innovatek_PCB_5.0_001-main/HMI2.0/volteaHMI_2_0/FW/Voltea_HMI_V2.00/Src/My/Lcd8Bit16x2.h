#ifndef __LCD8BIT16X2_H__
#define __LCD8BIT16X2_H__

#include<stm32f0xx_hal.h>

#define LCD_MAX_COL 17 //WITH NULL CHARACTER
extern char LcdLine1[LCD_MAX_COL];
extern char LcdLine2[LCD_MAX_COL];

extern void LCD_Initialization(void);
extern void LCD_Pulse_EN(void);
extern void LCD_Databus(unsigned char val);
extern void LcdCommand(unsigned char command);
extern void DisplayChar (unsigned char val);
extern void GotoXY(unsigned char x,unsigned char y);
extern void OutCharXY(unsigned char x,unsigned char y, unsigned char character);
extern void OutTextXY(unsigned char x,unsigned char y, char *str);
extern void Write2DataBus8bit(unsigned char val);

#endif
