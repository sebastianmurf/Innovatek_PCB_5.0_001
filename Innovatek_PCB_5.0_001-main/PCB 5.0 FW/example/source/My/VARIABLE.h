#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "common.h"
#include "mmu.h"
#include<time.h>


union union_temp
{	char c[8];
	int i[2];
	long l[2];
	float f[2];
	double d;
	long long ll;
};
//------------------------------------------------------------------------------
struct device
{   //char SuperAdminEmail[51];	//rarely used variable so doesnot use ram, only use Flash memory
    unsigned int SerialNo;	
  	char Password[5];
    //char AdminEmail[51];		//rarely used variable so doesnot use ram, only use Flash memory
    //char VehicleRegNo[16];	//rarely used variable so doesnot use ram, only use Flash memory
    //char DriverName[31];		//rarely used variable so doesnot use ram, only use Flash memory
    int TimeZone;	//in seconds
	ui InputBatVoltCategory;	//6, 9, 12, 24, 36 volt into milli volt
    struct tm TimeStamp;
	//char Date[11];
    //char Time[9];
};
union val4b
{	char c[4];
	int i;
	float f;
};

struct fota
{	int PageNo;
	char Status;
};
//configurable vaiable ends ****************************************************
//DATA TYPE                 VARIABLE NAME                       PURPOSE OF USE          SUPPORTING / ADDITIONAL INFORMATION
extern boolean_t            ActivateNeedToLogFlag;//so that NeedToLogFlag does not get true multiple times in same sec
                                                                                   #define ALARM_BUFFER_MAX_LIMIT 5
extern ui                   AlarmFIFOBuff[ALARM_BUFFER_MAX_LIMIT];
extern ui                   AlarmRegister24Bit;//ACTUALLY IT IS 32 BIT BUT FLASH MEMORY SPACE ONLY 24 BIT
extern ui                   AlarmRegisterForSMS;//if this Register is not equal to AlarmRegister24Bit, Alarm status has to be sent to registered mobile numbers
                                                                                    #define LOW_FLOW_PURIFY_ALARM_FLAG  0
                                                                                    #define LOW_FLOW_WASTE_ALARM_FLAG   1
                                                                                    #define BYPASS_ALARM_FLAG			2
                                                                                    #define CABINET_TEMP_ALARM_FLAG		3
                                                                                    #define LEAKAGE_ALARM_FLAG			4
                                                                                    #define WASTE_VALUE_ALARM_FLAG		5
                                                                                    #define CIP_LOW_LEVEL_ALARM_FLAG	6
                                                                                    #define PAE_ALARMS 0x00ff80
                                                                                    #define PAE_COMM_ERROR_FLAG			7
                                                                                    #define PAE_OVP_SHUTDOWN_FLAG		8
                                                                                    #define PAE_OLP_SHUTDOWN_FLAG		9
                                                                                    #define PAE_OTP_SHUTDOWN_FLAG		10
                                                                                    #define PAE_FAN_FAIL_FLAG			11
                                                                                    #define PAE_AUX_OR_SMPS_FAIL_FLAG	12
                                                                                    #define PAE_HI_TEMP_FLAG			13
                                                                                    #define PAE_AC_POWER_DOWN_FLAG		14
                                                                                    #define PAE_AC_INPUT_FAIL_FLAG		15
                                                                                    #define LOW_PRESSURE_ALARM_FLAG		16
                                                                                    #define ATLAS_I2C_COMM_ERROR_FLAG	17
                                                                                    #define ATLAS_CONDUCTIVITY_ZERO_READ_FLAG 18
                                                                                    #define ATLAS_TEMPERATURE_ERROR_FLAG    19
                                                                                    #define FLOWMETER_COMM_ALARM_FLAG   20
                                                                                    #define MAX_NO_OF_ALARMS 			21 //22
                                                                                    ////1111 1111 1111 1111 0001 0111 1111 0111 1111
                                                                                    #define ALARM_MASK_FOR_SMS 0xFFF17F7F
extern ui                   AnalogPressureStartLevel;   //pressure below which operation should start
extern ui                   AnalogPressureStopLevel;    //pressure above which operation should stop
extern ui                   AnalogPressureBypassLevel;  //pressure above which bypass should start
extern ui                   AnalogPressureCipLevel;     //pressure above which CIP should start
extern ui                   AnaOutProportionalFactor;
extern float                AverageFlow;
extern float                AverageFlowAccumulator;
extern uc                   AverageFlowCounter;
extern ui                   AverageVoltagePure;
extern ui                   AverageVoltageWaste;
extern boolean_t            AverageVoltagePureFlag;
extern boolean_t            AverageVoltageWasteFlag;
//extern boolean_t          AverageFlowFlag;
//extern boolean_t          AverageConductivityFlag;
extern ui                   AverageConductivity;
extern boolean_t            BuzzerFlag;
extern boolean_t            bTemp;
#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
extern ui                   Bypass2AlarmSecCounter;
extern boolean_t            BypassOperation2AlarmSecCounterFlag;
extern uc                   BypassOperationStartCounter;
extern uc                   BypassOperationStopCounter;
extern uc                   BypassOption;
#endif
//extern boolean_t          CabinetAlarmFlag;
extern ui                   CabinetTemperature;
extern ui                   CabinetHighTemperatureSetVal;
//extern ui                 CallPtr;
extern char                 cFunRetVal;
extern ui                   CIP_DosingOffTimeInSec;
extern ui                   CIP_DosingOnTimeInSec;
extern ui                   CIP_DosingRunTimeInSec;
extern boolean_t            CIP_DosingTotalFlowTimerFlag;
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
extern ui                   CIP_DosingTotalTimeInSec;
extern ui                   CIP_PulseOffTimeInSec;
extern ui                   CIP_PulseOnTimeInSec;
extern ui                   CIP_PulseRunTimeInSec;
extern ui                   CIP_PulseTotalTimeInSec;
#endif
#if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_NO))
extern ui                   CIP_LowerLimitCycle;    //cycle counter from where CIP attempt should start 
extern ui                   CIP_FavorableTime;      //Most favorable time for CIP (MID)
extern ui                   CIP_FavorableTimeGraceMin;
#endif
extern uc                   CommunicationMediaPrefered;
extern uc                   CommunicationMediaCurrent;
extern uc                   CommunicationMediaNext;
extern boolean_t            CIP_PulsedTotalFlowTimerFlag;
extern ui                   CIP_WaitTimeInSec;
extern ui                   CIP_FlushTimeInSec;
extern ui                   CIP_PulsedTotalFlowTimerCountInSec;
extern ui                   CIP_PumpRunTimeInSec;
extern boolean_t            CIP_RuntimeSecCounterFlag;
extern char                 CopyBuff[256];//256 so that if char variable used in assignment it doesn't overflow the copybuff size limit
extern char                 cTemp;
extern float                CurrentCyclePrepurifyAvgFlow;
extern float                CurrentCyclePurifyAvgFlow;
extern char                 CurrentProcessStep;
extern ui                   CurrentProportionalFactor;
extern ui                   CycleCounter;
extern boolean_t            DailySummaryReportMailFlag;
extern boolean_t            DateValidFlag;
extern struct device        Device;
extern double               dTemp;
extern ui                   DynaProportionalFactor;
extern boolean_t            EmailInitializationCompleteFlag;
extern float                EndVolume;//total volume at the end of step
extern boolean_t            FirstWasteCycle;//The initial waste time should be set to 50% of the normal waste time to optimize the process.
extern boolean_t            FlushShuntPendingFlag;    //To indicate step should goto SHUNT after completion of WASTE
//extern uli                FlowPulseCounter;
extern float                fTemp;
extern ui                   HF_WasteTimeInSec;
extern uc                   HF_WasteShuntTimeInSec;
extern int                  iTemp;
extern uc                   LastCycleOptimizedOutputVoltageCode;
extern ui                   LastCyclePureAverageConductivity;
extern ui                   LastFiveCyclePureAverageConductivityAverage;
extern ui                   LastFiveCyclePureAverageConductivitySamples[5];
//extern boolean_t          LeakageAlarmFlag;
extern long long            llTemp;
extern boolean_t            LoadSettingFlag;
//extern boolean_t          LowFlowPurifyAlarmFlag;
extern ui                   LowFlowPurifyAlarmSetpoint;
//extern boolean_t          LowFlowWasteAlarmFlag;
extern ui                   LowFlowWasteAlarmSetpoint;
extern ui                   MAX_CIP_PumpRunTimeInSec;
extern ui                   MaxCycleCountForCIP;
extern uc                   Mode;
//extern boolean_t          MonthlySummaryReportMailFlag;
extern uc                   NabeenTestValue;
extern int                  OptimizedPAE_CurrentForPrepureStep;
extern int                  OptimizedPAE_CurrentForPureStep;//To be retained in NVRAM
extern int                  OptimizedPAE_CurrentForWasteStep;//To be retained in NVRAM
extern ui                   PAE_PrepurifyStepVoltageSetpoint;
extern ui                   PAE_PureStepVoltageSetpoint;
extern ui                   PAE_WasteStepVoltageSetpoint;
extern ui                   PAE_PureCurrentSetpoint;

extern uc                   PumpManualMode;
//extern ui                 PAE_VoltageAtProcessStart;
//extern ui                 PAE_VoltageAtProcessEnd;
#if(FLASH_PAGE_SIZE ==528)
	extern char             PasteBuff[529];
#else
	extern char             PasteBuff[FLASH_PAGE_SIZE];
#endif
extern uc                   PercentageRecovery[5];
extern boolean_t            PowerOnResetActionFlag;
extern ui                   PrepurifyTimeInSec;
extern float                PressureOffset;
extern ui                   ProcessStepRunSec;
extern ui                   PureStepRunSec;  //to monitor how much sec pure including prepure has run so that waste can be decided how long should it run if premature pure cycle has run.
extern float                PureVolumeOfBatchJustCompleted;
extern ui                   PurifyTimeInSec;
extern struct tm            RecordTimeStamp;
extern struct tm            ReportingTime;
//extern ui                 ServerTimeoutSec; //within this sec+120sec if server connection is not successful then ResetDeviceFlag = TRUE;
extern uc                   ShuntSec;//temp variable
//extern boolean_t          SpecificDateTimeReportMailFlag;
//extern long long          SpecificReportDateTime;
extern float                StartVolume;//volume at start of step
extern boolean_t            SerialNoValidFlag;
extern uc                   ServerType;
extern uc                   ServerNoResponseCounter;
extern boolean_t            StopFlag;
extern uc                   StopTimer;
extern boolean_t            TankFullFlag;
extern ui                   TargetOutletConductivity;
extern ui                   TargetPureFlowLperM;
extern boolean_t            TimeValidFlag;
extern float                TotalPureVolume;
extern ui                   TemperatureAtWhichConductivityWasCalibrated;
extern float                TotalPureVolumeB4CycleStart;
extern uc                   ucTemp;
extern ui                   uiTemp;
extern uli                  uliTemp;
extern union union_temp     u,ut,urc;
extern uc                   VolumeUnit;
extern ui                   WaitHT_TimeInSec;
extern ui                   WaitTimeB4BypassAlarmInSec;
extern ui                   WaitTimeB4BypassAlarmInSecCounter;
//extern boolean_t          WasteBypassLowFlag;	//flag to indicate Bypass sig remained low for whole waste stage
//extern boolean_t          WasteBypassHighFlag;	//flag to indicate Bypass sig remained high for whole waste stage
extern ui                   WasteTimeInSec;
extern uc                   ShuntPostPureStepTimeInSec;//WastePreShuntTimeInSec;
extern uc                   ShuntPostWasteStepTimeInSec;
//extern boolean_t          WasteValveAlarmFlag;
extern float                WasteVolumeOfBatchJustCompleted;
extern ui                   WV_CheckTimeInSec;
//extern boolean_t          YearlySummaryReportMailFlag;
extern uc                   ZeroEcSecondCounter;
///////////////////////////////////////////////////////////
extern uc                   DynamicCapDIMode;
extern uc                   PumpManualMode;
//////////////////////////////////////////////////////////
extern ui                   PrePureCurrentSetpoint;
extern ui                   PureCurrentSetpoint;
extern ui                   WasteCurrentSetpoint;

extern ui                   PumpManualSpeed;

//project parameter
//extern char PidMCR[(MAX_PROJECT_PARAMETER/8)+1];	//Pid master control register
//extern char PidTPR[(MAX_PROJECT_PARAMETER/8)+1];	//pid trigger pending register
//extern ui LastTriggerExtBattery;
//extern ui LastTriggerAI1;
//extern boolean_t ExtBatteryTriggerFlag;
//extern boolean_t AI1TriggerFlag;
//extern char ResponseCode2Server;
//extern union val4b LogVal,LogMval,LogSval,TrigPara1Val,TrigPara2Val,TrigPara3Val;

//extern ui CarBatteryLastLogVal;

//extern char LogUnit;
//extern boolean_t TrigResponseFlag;
//extern char TrigPriority;
//extern char TrigCondition;
//extern char TrigPara1Unit,TrigPara2Unit,TrigPara3Unit;
extern char TrigMsg[101];
extern struct fota Fota;
extern ui LogPtr;//memory pointer pointing vacent location where next packet is to be saved
extern ui UploadPtr;//memory pointer pointing memory location from where packet is to be uploaded
extern ui EmailPtr,TempEmailPtr;
extern ui DAC_FB_Dval,DAC_FB_Aval;
extern float AnaPressureAval;
//extern ui Ana1AV, Ana1AVLastLogVal, Ana1_MaxMilliVolt, Ana1_MinMilliVolt;
//extern ui DO1Allocation;
//extern ui DO2Allocation;4

//extern uc OnTime,OffTime;

//extern char LogBuff[1056];
extern ui UploadPtr2Increment;
extern boolean_t ResetDeviceFlag;
extern boolean_t NeedToLogFlag;
//extern uc TriggerPid;//stores the pid which just triggered
//extern boolean_t TriggerValidFlag;
//extern uc TriggerPriorityVal;

extern char MinRecCounter4PeriodicSave;
#endif