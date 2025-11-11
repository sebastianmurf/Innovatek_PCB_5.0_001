#include"ProjectVariable.h"
#include "ProjectConstants.h"

/*##################################### CONDITIONAL DATA ############################################*/
/*#######################################################################################*/
char CopyBuff[100];		//stores data from the memory temporarily
char cTemp;
int iTemp;
char MasterBuff[MASTER_BUFF_SIZE];
char PasteBuff[100];		//stores data to be stored into memory temporarily
sli sliTemp;
union temp u;
uc ucTemp;
ui uiTemp;
float fTemp;
uli uliTemp;

uint16_t FlowKvalue;
uint64_t FlowIntrCounterCurrent, FlowIntrCounterLastSent;
uint16_t CurrentFlowIntrCounter;
float TotalVolume;
ui CurrentFlow;

uint32_t TemperatureDVal;
/*#######################################################################################*/
