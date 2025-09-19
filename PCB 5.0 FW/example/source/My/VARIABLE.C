#include "VARIABLE.h"
#include "constants.c"

boolean_t DateValidFlag = FALSE;
boolean_t TimeValidFlag = FALSE;
boolean_t ActivateNeedToLogFlag = TRUE;//so that NeedToLogFlag does not get true multiple times in same sec
                
boolean_t bTemp;
char CopyBuff[256];
struct device Device;
//char LogBuff[1056];
#if(FLASH_PAGE_SIZE ==528)
	char PasteBuff[529];
#else
	char PasteBuff[FLASH_PAGE_SIZE];
#endif
char cTemp;
long long llTemp;
int iTemp;
float fTemp;
double dTemp;
char cFunRetVal;
uc ucTemp;
ui uiTemp;
uli uliTemp;
union union_temp u,ut,urc;
//uli FlowPulseCounter;
//project parameter
//boolean_t AI1TriggerFlag;
//uc Ana1LastLogValInPercent;
//ui CarBatteryLastLogVal;
//char PidMCR[(MAX_PROJECT_PARAMETER/8)+1];	//Pid master control register
//char PidTPR[(MAX_PROJECT_PARAMETER/8)+1];
//ui LastTriggerExtBattery;
//ui LastTriggerAI1;
uc NabeenTestValue;
boolean_t ExtBatteryTriggerFlag;
char GeofenceLastTrigVal[7];//max 8*7=56 geofence in this variable
union val4b LogVal,TrigPara1Val,TrigPara2Val,TrigPara3Val;
boolean_t SerialNoValidFlag = FALSE;
//char ResponseCode2Server;
char VibrationSensitivity;

// VOLTEA VARIABLE //////////////
char CurrentProcessStep=IDLE;
uc Mode = MODE_LOGOUT;
ui ProcessStepRunSec = 0;
ui PureStepRunSec = 0;  //to monitor how much sec pure including prepure has run so that waste can be decided how long should it run if premature pure cycle has run.

//boolean_t AverageFlowFlag = FALSE;
//boolean_t AverageConductivityFlag = FALSE;
boolean_t AverageVoltagePureFlag = FALSE;
boolean_t AverageVoltageWasteFlag = FALSE;
boolean_t StopFlag = FALSE;
uc StopTimer = 0;
boolean_t FirstWasteCycle = FALSE;//The initial waste time should be set to 50% of the normal waste time to optimize the process.
boolean_t FlushShuntPendingFlag = FALSE;    //To indicate step should goto SHUNT after completion of WASTE
//boolean_t WasteBypassLowFlag;	//flag to indicate Bypass sig remained low for whole waste stage
//boolean_t WasteBypassHighFlag;	//flag to indicate Bypass sig remained high for whole waste stage
boolean_t CIP_RuntimeSecCounterFlag = FALSE;
boolean_t CIP_DosingTotalFlowTimerFlag,CIP_PulsedTotalFlowTimerFlag;

ui CIP_PulsedTotalFlowTimerCountInSec;

ui AlarmFIFOBuff[ALARM_BUFFER_MAX_LIMIT];
float AverageFlow=0,AverageFlowAccumulator=0;
uc AverageFlowCounter = 0;
float StartVolume=0, EndVolume=0, PureVolumeOfBatchJustCompleted, WasteVolumeOfBatchJustCompleted, TotalPureVolume;
float TotalPureVolumeB4CycleStart;
//ui CurrentCyclePureAverageConductivity=0;
uc ZeroEcSecondCounter = 0;
ui AverageConductivity = 1;
float CurrentCyclePrepurifyAvgFlow, CurrentCyclePurifyAvgFlow;
uc PercentageRecovery[5];
ui LastFiveCyclePureAverageConductivitySamples[5];
ui LastCyclePureAverageConductivity;
ui LastFiveCyclePureAverageConductivityAverage;
uc ShuntPostPureStepTimeInSec;//WastePreShuntTimeInSec;
uc ShuntPostWasteStepTimeInSec;//WastePostShuntTimeInSec;
/********************* Setpoints *******************************/
ui LowFlowPurifyAlarmSetpoint=10, LowFlowWasteAlarmSetpoint=3;//divide it by 10 to get actual value
ui CabinetTemperature=25, CabinetHighTemperatureSetVal=40;
ui PrepurifyTimeInSec=5;
ui PurifyTimeInSec=115;
ui WasteTimeInSec=80;
ui HF_WasteTimeInSec=0;
uc HF_WasteShuntTimeInSec;
ui CycleCounter,MaxCycleCountForCIP=500;
ui CIP_DosingOnTimeInSec=2; 
ui CIP_DosingOffTimeInSec = 9;
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
ui CIP_DosingTotalTimeInSec = 99;
ui CIP_PulseTotalTimeInSec = 1800;
ui CIP_PulseOnTimeInSec = 6;
ui CIP_PulseOffTimeInSec = 20;
#endif
ui CIP_WaitTimeInSec = 3600;
ui WaitTimeB4BypassAlarmInSec = 900;
ui WaitTimeB4BypassAlarmInSecCounter;
ui TargetPureFlowLperM = 29;
ui TargetOutletConductivity = 100;
ui MAX_CIP_PumpRunTimeInSec = 450;
ui CIP_PumpRunTimeInSec;
ui CIP_FlushTimeInSec=1800;
ui WV_CheckTimeInSec=5;
ui WaitHT_TimeInSec=1800;
ui AnaOutProportionalFactor = 5;//for actual divide it by 100
ui DynaProportionalFactor = 50;//for actual divide it by 100
uc VolumeUnit = VOLUME_UNIT_GALLON;
uc DynamicCapDIMode = DYNAMIC_CAPDI_MODE_ON;

ui PrePureCurrentSetpoint = 30;
ui PureCurrentSetpoint = 35;
ui WasteCurrentSetpoint = 60;

float PressureOffset;
boolean_t TankFullFlag;
#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
ui                   Bypass2AlarmSecCounter = 0;
boolean_t            BypassOperation2AlarmSecCounterFlag = FALSE;
uc                   BypassOperationStartCounter = 0;
uc                   BypassOperationStopCounter = 0;
uc BypassOption = BYPASS_INTERNAL;
#endif
ui                   AnalogPressureStartLevel;   //pressure below which operation should start
ui                   AnalogPressureStopLevel;    //pressure above which operation should stop
ui                   AnalogPressureBypassLevel;  //pressure above which bypass should start
ui                   AnalogPressureCipLevel;     //pressure above which CIP should start
#if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
ui CIP_LowerLimitCycle;//CIP Counter within this percentage, one condition is fulfilled for CIP 
ui CIP_FavorableTime;   //Most favorable time for CIP (MID)
ui CIP_FavorableTimeGraceMin;
#endif
/*********************** Added by Arun @ NOV 14 **************************************/

ui PAE_PrepurifyStepVoltageSetpoint;
ui PAE_PureStepVoltageSetpoint;
ui PAE_WasteStepVoltageSetpoint;
ui CurrentProportionalFactor;	//client name as w_value
ui PAE_PureCurrentSetpoint;
int OptimizedPAE_CurrentForPrepureStep, OptimizedPAE_CurrentForPureStep, OptimizedPAE_CurrentForWasteStep;//To be retained in NVRAM
//////////////////// Variables for calculations /////////////////////////////////////
//ui PAE_VoltageAtProcessStart, PAE_VoltageAtProcessEnd;
ui AverageVoltagePure, AverageVoltageWaste;
/*************************************************************************************/
uc ShuntSec = SHUNT_TIME_IN_SEC_DEFAULT_VAL;

ui CIP_DosingRunTimeInSec = 0;
ui CIP_PulseRunTimeInSec = 0;

uc CommunicationMediaPrefered = COMM_MEDIA_AUTO_SWITCH;//auto switch
uc CommunicationMediaNext = 0;   //0=autoswitch which is default, 1=ethernet 2=wifi
uc CommunicationMediaCurrent = 0;//0=none, 1=autoswitch, 2=ethernet 3=wifi
uc                   ServerNoResponseCounter = 0;

struct tm ReportingTime;
struct tm RecordTimeStamp;
//alarm variable flags
ui AlarmRegister24Bit;
ui AlarmRegisterForSMS;//if this Register is not equal to AlarmRegister24Bit, Alarm status has to be sent to registered mobile numbers
boolean_t BuzzerFlag = FALSE;
/*boolean_t LowFlowPurifyAlarmFlag, LowFlowWasteAlarmFlag;
boolean_t BypassAlarmFlag;
boolean_t CabinetAlarmFlag;
boolean_t LeakageAlarmFlag;
boolean_t WasteValveAlarmFlag;*/
////// non volatile variables
uc LastCycleOptimizedOutputVoltageCode;
ui TemperatureAtWhichConductivityWasCalibrated;

/////////////////////////////////
boolean_t PowerOnResetActionFlag;
boolean_t DailySummaryReportMailFlag = FALSE;
boolean_t EmailInitializationCompleteFlag = FALSE;
//boolean_t MonthlySummaryReportMailFlag = FALSE;
//boolean_t YearlySummaryReportMailFlag = FALSE;
//boolean_t SpecificDateTimeReportMailFlag = FALSE;
//long long SpecificReportDateTime;

//char LogUnit;
//boolean_t TrigResponseFlag;
//char TrigPriority;
//char TrigCondition;
//char TrigPara1Unit,TrigPara2Unit,TrigPara3Unit;
char TrigMsg[101];
//need to arrange variables
char MinRecCounter4PeriodicSave = 0;
//uc OnTime=40,OffTime=40;

uc EngineOnTimer;//variable which will initialize to zero once engine is on. This variable will be used to take fuel same after 1 min of engine on.
ui DAC_FB_Dval,DAC_FB_Aval;
float AnaPressureAval;

//ui Ana1AV,Ana1AVLastLogVal,Ana1_MaxMilliVolt,Ana1_MinMilliVolt;
ui LogPtr;//memory pointer pointing vacent location where next packet is to be saved
ui UploadPtr;//memory pointer pointing memory location from where packet is to be uploaded
ui UploadPtr2Increment=0;
ui EmailPtr,TempEmailPtr;
uc                   ServerType = SERVER_DATA;

boolean_t ResetDeviceFlag = FALSE;
boolean_t NeedToLogFlag;
//uc TriggerPid;//stores the pid which just triggered
//boolean_t TriggerValidFlag;
//uc TriggerPriorityVal;
//ui CallPtr;
//ui ServerTimeoutSec;//within this sec+120 if server connection is not successful then ResetDeviceFlag = TRUE;
//ui BalanceChkHour;
boolean_t LoadSettingFlag=FALSE;

struct fota Fota;




/*
typedef struct my_control_register
{
  uc MasterEmailControl		: 1;
  uc LogEmailControl      	: 1;
  uc ReceivedSmsEmailControl: 1;
  uc BalanceEmailControl    : 1;
  uc GprsControl			: 1;
  uc MasterSmsControl		: 1;
  uc TriggerSmsControl		: 1;
  uc SmsReplyControl		: 1;
  uc CallSmsReplyControl	: 1;
  uc MasterCallControl		: 1;
 
} MyControlRegister;
*/