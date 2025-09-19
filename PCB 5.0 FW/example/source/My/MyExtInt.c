#include "MyExtInt.h"

boolean_t DigitalInput1InterruptFlag = FALSE;
unsigned int DigitalInput1InterruptCounter=0;
boolean_t DigitalInput2InterruptFlag = FALSE;
unsigned int DigitalInput2InterruptCounter=0;
boolean_t DigitalInput3InterruptFlag = FALSE;
unsigned int DigitalInput3InterruptCounter=0;
boolean_t DigitalInput4InterruptFlag = FALSE;
unsigned int DigitalInput4InterruptCounter=0;
unsigned char ExternalInterruptRegister = 0;

//CHECK INTERRUPT DEBOUNCING
void INT0_7_Handler(void)
{	if ((FM3_EXTI->EIRR & 0X08)>0)
	{  	FM3_EXTI->EICL &= ~(0X08);
		/*if((1<<Input[0].RegisterBitId) == 0x08)
			ExternalInterruptRegister |= INTERRUPT_PANIC;
		*/
		/*if((1<<Input[0].RegisterBitId) == 0x02)
			FlowPulseCounter++;
		*/
		/*if(DigitalInput1InterruptFlag == FALSE)
		{	DigitalInput1InterruptFlag = TRUE;
			DigitalInput1InterruptCounter++;
		}*/
	}
	if ((FM3_EXTI->EIRR & 0X04)>0)
	{  	FM3_EXTI->EICL &= ~(0X04);
		/*if((1<<Input[1].RegisterBitId) == 0x08)
			ExternalInterruptRegister |= INTERRUPT_PANIC;
		*/
		/*if((1<<Input[1].RegisterBitId) == 0x02)
			FlowPulseCounter++;*/
		/*if(DigitalInput2InterruptFlag == FALSE)
		{	DigitalInput2InterruptFlag = TRUE;
			DigitalInput2InterruptCounter++;
		}*/
	}
	if ((FM3_EXTI->EIRR & 0X10)>0)
	{  	FM3_EXTI->EICL &= ~(0X10);
		/*if((1<<Input[2].RegisterBitId) == 0x08)
			ExternalInterruptRegister |= INTERRUPT_PANIC;
		*/
		/*if((1<<Input[2].RegisterBitId) == 0x02)
			FlowPulseCounter++;*/
		/*if(DigitalInput3InterruptFlag == FALSE)
		{	DigitalInput3InterruptFlag = TRUE;
			DigitalInput3InterruptCounter++;
		}*/
	}
	if ((FM3_EXTI->EIRR & 0X20)>0)
	{  	FM3_EXTI->EICL &= ~(0X20);
		if((1<<Input[3].RegisterBitId) == 0x08)
			ExternalInterruptRegister |= INTERRUPT_PANIC;
		/*if((1<<Input[3].RegisterBitId) == 0x02)
			FlowPulseCounter++;
		*/
		/*if(DigitalInput4InterruptFlag == FALSE)
		{	DigitalInput4InterruptFlag = TRUE;
			DigitalInput4InterruptCounter++;
		}*/
	}
}
void ExternalInterruptInitialization()
{	
	//NVIC_DisableIRQ(EXINT0_7_IRQn);	//disable all external interrupt from 0 to 7
	FM3_EXTI->ENIR = 0;			//disable all external interrupt from 0 to 31
	FM3_EXTI->ELVR = 0XFFFFFFFF;//all 0-15 interrupt on FALLING EDGE trigger mode. change below by AND(COMP)-ing if other mode required
	FM3_EXTI->EICL = 0X00000000;//clear all 0-31 external interrupt cause if any
	
	NVIC_ClearPendingIRQ(EXINT0_7_IRQn);
  	NVIC_SetPriority(EXINT0_7_IRQn, 2);
	//FM3_GPIO->EPFR06 |= (0 << 4);	//select channel 0 of external interrupt 2
	//FM3_GPIO->EPFR06 |= (3 << 6);	//select channel 2 of external interrupt 3
	//FM3_GPIO->EPFR06 |= (3 << 8);	//select channel 2 of external interrupt 4
	//FM3_GPIO->EPFR06 |= (3 << 10);//select channel 2 of external interrupt 5
	FM3_GPIO->EPFR06 |= 0x00000FC0;//select interrupt channel 2_0, 3_2, 4_2 & 5_2
	
	//FM3_EXTI->ENIR |= 0X04;	//Enables external interrupt 2_0 request.
	//FM3_EXTI->ENIR |= 0X08;	//Enables external interrupt 3_2 request.
	//FM3_EXTI->ENIR |= 0X10;	//Enables external interrupt 4_2 request.
	//FM3_EXTI->ENIR |= 0X20;	//Enables external interrupt 5_2 request.
	FM3_EXTI->ENIR |= 0X3C;	//Enables external interrupt 2_0, 3_2, 4_2 & 5_2 request.
  	NVIC_EnableIRQ(EXINT0_7_IRQn);
}
