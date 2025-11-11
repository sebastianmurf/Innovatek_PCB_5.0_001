#include "Lcd8bit16x2.h"
#include "MyDelay.h"
#include "main.h"

char LcdLine1[LCD_MAX_COL];
char LcdLine2[LCD_MAX_COL];

/*###########################################################################################################*/
void OutCharXY(unsigned char x,unsigned char y, unsigned char character){
	GotoXY(x,y);
	DisplayChar(character);
}
/*###########################################################################################################*/
void OutTextXY(unsigned char x,unsigned char y, char *str){
	GotoXY(x,y);
	for(;*str!='\0';str++)
		DisplayChar(*str);
}
/*###########################################################################################################*/
void GotoXY(unsigned char x,unsigned char y){
	switch(y)
	{	case 1:	LcdCommand(0x80+x);break;
		case 2: LcdCommand(0xc0+x);break;
		//case 3: LcdCommand(0x94+x);break;
		//case 4: LcdCommand(0xD4+x);break;
	}
}
/*###########################################################################################################*/
void LCD_Initialization(void){
	LcdCommand(0x38);
	MyDelayMS(4);
	LcdCommand(0x38);
	MyDelayMS(8);
	LcdCommand(0x38);
	MyDelayMS(4);
	LcdCommand(0x01);
	MyDelayMS(2);
	LcdCommand(0x0c);
	MyDelayMS(2);
}
/*###########################################################################################################*/
void LcdCommand(unsigned char command){
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_RESET);//RS = 0;
	Write2DataBus8bit(command);
	LCD_Pulse_EN();	
	DelayMachineCycle(200);	//200
}
/*###########################################################################################################*/
void DisplayChar (unsigned char val){
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_SET);//RS = 1;
 	Write2DataBus8bit(val);
	LCD_Pulse_EN();	
	DelayMachineCycle(100);//100
}
/*###########################################################################################################*/
void LCD_Pulse_EN(void){
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port,LCD_EN_Pin,GPIO_PIN_SET);//EN = 1;
	DelayMachineCycle(100);//100	
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port,LCD_EN_Pin,GPIO_PIN_RESET);//EN = 0;
}
/*###########################################################################################################*/
void Write2DataBus8bit(unsigned char val){
	if((val&0x01)==0)
		HAL_GPIO_WritePin(DATA_D0_GPIO_Port,DATA_D0_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D0_GPIO_Port,DATA_D0_Pin,GPIO_PIN_SET);
	
	if((val&0x02)==0)
		HAL_GPIO_WritePin(DATA_D1_GPIO_Port,DATA_D1_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D1_GPIO_Port,DATA_D1_Pin,GPIO_PIN_SET);
	
	if((val&0x04)==0)
		HAL_GPIO_WritePin(DATA_D2_GPIO_Port,DATA_D2_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D2_GPIO_Port,DATA_D2_Pin,GPIO_PIN_SET);
		
	if((val&0x08)==0)
		HAL_GPIO_WritePin(DATA_D3_GPIO_Port,DATA_D3_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D3_GPIO_Port,DATA_D3_Pin,GPIO_PIN_SET);
	
	if((val&0x10)==0)
		HAL_GPIO_WritePin(DATA_D4_GPIO_Port,DATA_D4_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D4_GPIO_Port,DATA_D4_Pin,GPIO_PIN_SET);
		
	if((val&0x20)==0)
		HAL_GPIO_WritePin(DATA_D5_GPIO_Port,DATA_D5_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D5_GPIO_Port,DATA_D5_Pin,GPIO_PIN_SET);
		
	if((val&0x40)==0)
		HAL_GPIO_WritePin(DATA_D6_GPIO_Port,DATA_D6_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D6_GPIO_Port,DATA_D6_Pin,GPIO_PIN_SET);
		
	if((val&0x80)==0)
		HAL_GPIO_WritePin(DATA_D7_GPIO_Port,DATA_D7_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DATA_D7_GPIO_Port,DATA_D7_Pin,GPIO_PIN_SET);
}
/*###########################################################################################################*/

