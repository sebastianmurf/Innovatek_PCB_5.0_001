#include "exint.h"
#include "gpio.h"
#include "MyUsb.h"
#include "variable.h"

#define INTERRUPT_PANIC 0x01

extern boolean_t DigitalInput1InterruptFlag;
extern unsigned int DigitalInput1InterruptCounter;
extern boolean_t DigitalInput2InterruptFlag;
extern unsigned int DigitalInput2InterruptCounter;
extern boolean_t DigitalInput3InterruptFlag;
extern unsigned int DigitalInput3InterruptCounter;
extern boolean_t DigitalInput4InterruptFlag;
extern unsigned int DigitalInput4InterruptCounter;

extern unsigned char ExternalInterruptRegister;

extern void ExternalInterruptInitialization();
