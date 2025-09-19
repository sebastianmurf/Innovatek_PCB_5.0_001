#include"gprs.h"
#include"GprsUart.h"
#include"myfun.h"
#include"MyUsb.h"
#include"constants.c"
#include"VARIABLE.H"
#include"command.h"
#include<string.h>
#include<ctype.h>
#include"math.h"
#include"AT45DB161.h"
#include"gpio.h"
#include"DualTimer.h"
#include"GpsUart.h"
#include"delay.h"
#include"led.h"
#include"SerialHmi.h"
#include "AtlasReplacement.h"

//boolean_t             BalanceCheckFlag = FALSE;
boolean_t               CallResponsePendingFlag = FALSE;
boolean_t               CountryCodeFlag = TRUE;
long long               CallResponsePendingCaller;
ui                      CallTimeOutSec;
//ui                    GprsPowerWaitSec = 0;//it counts the second elapsed after it was set zero
uc                      EmailAttemptCounter=0;
//boolean_t             FtpFlag = FALSE;
//boolean_t             GetMobileRtcTimeFlag = TRUE;//Flag to indicate GPS time sync not possible from last 5 min so sync with mobile tower
uc                      GprsCR;
boolean_t               GprsDataModeFlag = FALSE;
boolean_t               GprsCommandResponseFlag = FALSE;
ui                      GprsEmailSendingResponseTimeOutTime = 0;
struct gprspacket       GprsPacket;
ui                      LastServerConnectionTime = 0;
uc                      MinRecPerConnection;
boolean_t               MobileCommandFlag;//indicates at commmand has been issued or waiting for at command response
boolean_t               MobileDeepSleepFlag = FALSE;
uc                      MobileResponseWaitSec;
unsigned char           MobileStage;
ui                      MobileStageWaitSec;
char                    MsgBuff[529];
ui                      NormalPowerDownDelay;
unsigned char           PortOpenErrorCounter = 0;
uc                      ReturnToMobileStage;
boolean_t               SendMailFlag = FALSE;
struct server_profile   Server;//,Server4Check[MAX_NO_OF_SERVER];
union SIM_IP            sip;
//boolean_t             SmsCheckFlag = TRUE;		
uc                      SmsLoc;
uc                      TotalSmsSpace;
uc                      UnreadSmsCounter;
uc                      UserCounter;//variable to count the number of email id added as for sending mail.
uc                      WifiServerDisconnectCounter = 0;
uc                      WifiErrorCounter = 0;
////////////////////////////////////////////////////////////////////////////////
#define MOBILE_SLEEP_SEC 840
#define COMMAND 0
#define RESPONSE 1

void CheckAlertSMS2Send()
{	ui alarm_out_from_alarm_fifo;
    uc lp;
	
    for(lp=0; lp<ALARM_BUFFER_MAX_LIMIT; lp++){
        alarm_out_from_alarm_fifo = AlarmFIFO(0xFFFFFFFF);
        if(alarm_out_from_alarm_fifo == 0xFFFFFFFF)
          continue;
        if((alarm_out_from_alarm_fifo & ALARM_MASK_FOR_SMS)  != AlarmRegisterForSMS)//there is alert SMS to be sent
        {	PutStringUsb("\r\nALARM TRIGGER FOUND");
            sprintf(MsgBuff,"ALARM: 0x%xh",AlarmRegister24Bit);
            /*
            uiTemp = alarm_out_from_alarm_fifo ^ AlarmRegisterForSMS;//get difference alert
            strcpy(MsgBuff,"ALARM:");
            if((uiTemp & (1<<LOW_FLOW_PURIFY_ALARM_FLAG)) > 0)//LOW_FLOW_PURIFY_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<LOW_FLOW_PURIFY_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nLOW FLOW PURIFY ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nLOW FLOW PURIFY ALARM RESOLVED");
            }
            if((uiTemp & (1<<LOW_FLOW_WASTE_ALARM_FLAG)) > 0)//LOW_FLOW_WASTE_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<LOW_FLOW_WASTE_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nLOW FLOW WASTE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nLOW FLOW WASTE ALARM RESOLVED");
            }
            if((uiTemp & (1<<BYPASS_ALARM_FLAG)) > 0)//BYPASS_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<BYPASS_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nBYPASS ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nBYPASS ALARM RESOLVED");
            }
            if((uiTemp & (1<<CABINET_TEMP_ALARM_FLAG)) > 0)//CABINET_TEMP_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<CABINET_TEMP_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nCABINET TEMPERATURE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nCABINET TEMPERATURE ALARM RESOLVED");
            }
            if((uiTemp & (1<<LEAKAGE_ALARM_FLAG)) > 0)//LEAKAGE_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<LEAKAGE_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nLEAKAGE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nLEAKAGE ALARM RESOLVED");
            }
            if((uiTemp & (1<<WASTE_VALUE_ALARM_FLAG)) > 0)//WASTE_VALUE_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<WASTE_VALUE_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nWASTE VALUE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nWASTE VALUE ALARM RESOLVED");
            }
            if((uiTemp & (1<<CIP_LOW_LEVEL_ALARM_FLAG)) > 0)//CIP_LOW_LEVEL_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<CIP_LOW_LEVEL_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nCIP LOW LEVEL ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nCIP LOW LEVEL ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_COMM_ERROR_FLAG)) > 0)//PAE_COMM_ERROR_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_COMM_ERROR_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nCIP LOW LEVEL ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nCIP LOW LEVEL ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_OVP_SHUTDOWN_FLAG)) > 0)//PAE_OVP_SHUTDOWN_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_OVP_SHUTDOWN_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE OVP SHUTDOWN ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE OVP SHUTDOWN ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_OLP_SHUTDOWN_FLAG)) > 0)//PAE_OLP_SHUTDOWN_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_OLP_SHUTDOWN_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE OLP SHUTDOWN ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE OLP SHUTDOWN ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_OTP_SHUTDOWN_FLAG)) > 0)//PAE_OTP_SHUTDOWN_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_OTP_SHUTDOWN_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE OTP SHUTDOWN ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE OTP SHUTDOWN ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_FAN_FAIL_FLAG)) > 0)//PAE_FAN_FAIL_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_FAN_FAIL_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE FAN FAIL ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE FAN FAIL ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_AUX_OR_SMPS_FAIL_FLAG)) > 0)//PAE_AUX_OR_SMPS_FAIL_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_AUX_OR_SMPS_FAIL_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE AUX OR SMPS FAIL ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE AUX OR SMPS FAIL ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_HI_TEMP_FLAG)) > 0)//PAE_HI_TEMP_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_HI_TEMP_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE HIGH TEMPERATURE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE HIGH TEMPERATURE ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_AC_POWER_DOWN_FLAG)) > 0)//PAE_AC_POWER_DOWN_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_AC_POWER_DOWN_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE AC POWER DOWN ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE AC POWER DOWN ALARM RESOLVED");
            }
            if((uiTemp & (1<<PAE_AC_INPUT_FAIL_FLAG)) > 0)//PAE_AC_INPUT_FAIL_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<PAE_AC_INPUT_FAIL_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nPAE AC INPUT FAIL ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nPAE AC INPUT FAIL ALARM RESOLVED");
            }
            if((uiTemp & (1<<LOW_PRESSURE_ALARM_FLAG)) > 0)//LOW_PRESSURE_ALARM_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<LOW_PRESSURE_ALARM_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nLOW PRESSURE ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nLOW PRESSURE ALARM RESOLVED");
            }
            if((uiTemp & (1<<ATLAS_I2C_COMM_ERROR_FLAG)) > 0)//ATLAS_I2C_COMM_ERROR_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<ATLAS_I2C_COMM_ERROR_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nATLAS I2C COMM ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nATLAS I2C COMM ALARM RESOLVED");
            }
            if((uiTemp & (1<<ATLAS_CONDUCTIVITY_ZERO_READ_FLAG)) > 0)//ATLAS_CONDUCTIVITY_ZERO_READ_FLAG alert occured or resolved
            {	if((AlarmRegister24Bit & (1<<ATLAS_CONDUCTIVITY_ZERO_READ_FLAG)) > 0)//alarm occured
                    strcat(MsgBuff,"\r\nZERO EC ALARM OCCURED");
                else
                    strcat(MsgBuff,"\r\nZERO EC ALARM RESOLVED");
            }
            */
            for(ucTemp=0;ucTemp<10;ucTemp++)
            {	u.ll = 0;
                DirectMemRead(USER_OTHERS_MOBa+ucTemp*USER_TABLE_RECORD_SIZE,u.c,UT_MOBn);
                if(u.c[0] != 0xFF)
                {	CommandFormat.Sender = u.ll; 
                    SendCommandReplyToSender(MsgBuff);
                }
            }
            AlarmRegisterForSMS = (alarm_out_from_alarm_fifo & ALARM_MASK_FOR_SMS);
        }
    }
}
void MobileRoutine()
{	//static uc MobileRestartCounter = 0;
    if( GprsByteRecvFlag == TRUE)
	{	while(GprsByteRecvFlag == TRUE)
		{	GprsByteRecvFlag = FALSE;
            DelayInMilliSec(100);
		}
        /*
        if(CheckGPRSResponse("SERVER,CLOSED",1)==TRUE)
        {   if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)  
            {   MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING;
                MobileStageWaitSec = 0;
				MobileCommandFlag = FALSE;
            }
        }*/
		if(MobileDebugFlag == TRUE)
		{	PutStringUsb("\r\nCOMM -> ");
			PutStringUsb(GprsBuff);
            if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)
            {   sprintf(PasteBuff,"Len=%d, GprsCommandFlag=%d",strlen(GprsBuff),GprsCommandFlag);
                PutStringUsb(PasteBuff);
                if(strlen(GprsBuff) == 0)
                  GprsCommandFlag = FALSE;
                for(uiTemp=0;GprsBuff[uiTemp] != '\0';uiTemp++)
                {   if(GprsBuff[uiTemp] < 10 || GprsBuff[uiTemp] > 126)
                    {   PutStringUsb("Illegal Char found in response");
                        MobileStage = MOBILE_STAGE_POWER_UP_WAIT;
                        GPRS_PWR_KEY_HIGH;
                   }
                }
            }
		}
		GprsCommandResponseFlag = TRUE;
		
	}
	
	bTemp = FALSE;
	switch(MobileStage)
	{	case MOBILE_STAGE_POWER_UP_WAIT:
            if(MobileStageWaitSec > 10)
			{	CommunicationMediaCurrent = COMM_MEDIA_NONE;
                MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;	
			}
	    break;
        case MOBILE_STAGE_SWITCH_TO_ETHERNET:
            GPRS_UART->SCR &= ~0x10;//DISABLE SERIAL receive interrupt in order to prevent controller being hang due to serial Rx pin gets low and controller gets stuck in serial interrupt
            GPRS_PWR_KEY_LOW;
            GPRS_DTR_LOW;
            MobileStage = MOBILE_STAGE_SWITCHING_WAIT;
            PutStringUsb("\r\nCOMM->SWITCH_TO_ETHERNET");
            CommunicationMediaCurrent = COMM_MEDIA_ETHERNET;
            MobileStageWaitSec = 0;
        break;
        case MOBILE_STAGE_SWITCHING_WAIT:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_SWITCHING_WAIT");
            if(MobileStageWaitSec > 10)
            {   InitUartGprs();
                if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)
                {    MobileStage = MOBILE_STAGE_SEND_DATA;
                }
                if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)
                {   MobileStage = MOBILE_STAGE_WIFI_AT_COMMAND;
                    MobileStageWaitSec = 0;
                    MobileCommandFlag = FALSE;
                }
                GprsReceiveCounter = 0;
                GprsBuff[GprsReceiveCounter] = '\0';
            }
        break;
        
        case MOBILE_STAGE_SEND_DATA:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_SEND_DATA");
            //check if fota file request going on
            //sprintf(CopyBuff,"\r\nFota.Status=%d",Fota.Status);
            //PutStringUsb(CopyBuff);
            if(Fota.Status == FIRMWARE_UPGRADE_COMMAND_RECEIVED || Fota.Status == FIRMWARE_DOWNLOADING)
            {   SendFotaCommandToServer();
                MobileStage = MOBILE_STAGE_RESPONSE_WAIT;
                MobileStageWaitSec = 0;
            }
            //second priority check if date time sync is required
            else if(DateValidFlag == FALSE || TimeValidFlag == FALSE)
            {   PutCharGprs('[');
                GprsCrcCodeOut = 0;
                #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                    sprintf(MsgBuff,"%d,0000,%d,307.0=?,308.0=?,410.0=%d,410.1=%d,410.2=%d,410.3=%d,410.4=%d,410.5=%d,410.6=%d,410.7=%d,410.8=%d,410.9=%d,410.10=%d,410.11=%d,410.12=%d,410.13=%d,410.14=%.1f,410.15=%.1f,410.16=%.1f,410.17=%d,410.18=%d,410.19=%d,410.20=%.2f,410.21=%.2f,410.22=%.1f,410.23=%.1f,410.24=%.2f,410.25=%d,410.26=%d,410.27=%d,410.28=%.2f,410.29=%.2f,410.30=%.2f,410.31=%.2f,410.32=%d,410.33=%d",
                            SERVER_READ_WRITE_PACKET,
                            Device.SerialNo,
                            TargetOutletConductivity,   //0
                            PrepurifyTimeInSec,         //1
                            PurifyTimeInSec,            //2
                            WasteTimeInSec,             //3
                            HF_WasteTimeInSec,          //4
                            CIP_DosingOnTimeInSec,      //5
                            CIP_DosingOffTimeInSec,     //6
                            CIP_DosingTotalTimeInSec,   //7
                            CIP_PulseTotalTimeInSec,    //8
                            CIP_PulseOnTimeInSec,       //9
                            CIP_PulseOffTimeInSec,      //10
                            CIP_FlushTimeInSec,         //11
                            WV_CheckTimeInSec,          //12
                            WaitHT_TimeInSec,           //13
                            (float)TargetPureFlowLperM/10,     //14
                            (float)LowFlowPurifyAlarmSetpoint/10,  //15
                            (float)LowFlowWasteAlarmSetpoint/10,   //16
                            MaxCycleCountForCIP,            //17
                            CabinetHighTemperatureSetVal,   //18
                            MAX_CIP_PumpRunTimeInSec,       //19
                            (float)AnaOutProportionalFactor/100,   //20
                            (float)DynaProportionalFactor/100,     //21
                            (float)PAE_PureStepVoltageSetpoint/10, //22
                            (float)PAE_WasteStepVoltageSetpoint/10,    //23
                            (float)CurrentProportionalFactor/100,  //24
                            FlowMeter.K_Factor,             //25
                            VolumeUnit,                 //26
                            0,                          //27 bypass option
                            (float)AnalogPressureStartLevel/100,   //28
                            (float)AnalogPressureStopLevel/100,    //29
                            (float)AnalogPressureBypassLevel/100,  //30
                            (float)AnalogPressureCipLevel/100,     //31
                            CIP_WaitTimeInSec,                      //32
                            0);             //33 = WaitTimebB4BypassAlarmInSec
                #endif
                #if (PRODUCT_TYPE == PRODUCT_DIENTRY)
                    sprintf(MsgBuff,"%d,0000,%d,307.0=?,308.0=?,410.0=%d,410.1=%d,410.2=%d,410.3=%d,410.4=%d,410.5=%d,410.6=%d,410.7=%d,410.8=%d,410.9=%d,410.10=%d,410.11=%d,410.12=%d,410.13=%d,410.14=%.1f,410.15=%.1f,410.16=%.1f,410.17=%d,410.18=%d,410.19=%d,410.20=%.2f,410.21=%.2f,410.22=%.1f,410.23=%.1f,410.24=%.2f,410.25=%d,410.26=%d,410.27=%d,410.28=%.2f,410.29=%.2f,410.30=%.2f,410.31=%.2f,410.32=%d,410.33=%d",
                            SERVER_READ_WRITE_PACKET,
                            Device.SerialNo,
                            TargetOutletConductivity,   //0
                            PrepurifyTimeInSec,         //1
                            PurifyTimeInSec,            //2
                            WasteTimeInSec,             //3
                            HF_WasteTimeInSec,          //4
                            CIP_DosingOnTimeInSec,      //5
                            CIP_DosingOffTimeInSec,     //6
                            0,
                            0,
                            0,
                            0,
                            CIP_FlushTimeInSec,         //11
                            WV_CheckTimeInSec,          //12
                            WaitHT_TimeInSec,           //13
                            (float)TargetPureFlowLperM/10,     //14
                            (float)LowFlowPurifyAlarmSetpoint/10,  //15
                            (float)LowFlowWasteAlarmSetpoint/10,   //16
                            MaxCycleCountForCIP,            //17
                            CabinetHighTemperatureSetVal,   //18
                            MAX_CIP_PumpRunTimeInSec,       //19
                            (float)AnaOutProportionalFactor/100,   //20
                            (float)DynaProportionalFactor/100,     //21
                            (float)PAE_PureStepVoltageSetpoint/10, //22
                            (float)PAE_WasteStepVoltageSetpoint/10,    //23
                            (float)CurrentProportionalFactor/100,  //24
                            FlowMeter.K_Factor,             //25
                            VolumeUnit,                 //26
                            BypassOption,               //27
                            (float)AnalogPressureStartLevel/100,   //28
                            (float)AnalogPressureStopLevel/100,    //29
                            (float)AnalogPressureBypassLevel/100,  //30
                            (float)AnalogPressureCipLevel/100,     //31
                            CIP_WaitTimeInSec,             //32
                            WaitTimeB4BypassAlarmInSec);   //33
                #endif
                #if (PRODUCT_TYPE == PRODUCT_DISERIES)
                    sprintf(MsgBuff,"%d,0000,%d,307.0=?,308.0=?,410.0=%d,410.1=%d,410.2=%d,410.3=%d,410.4=%d,410.5=%d,410.6=%d,410.7=%d,410.8=%d,410.9=%d,410.10=%d,410.11=%d,410.12=%d,410.13=%d,410.14=%.1f,410.15=%.1f,410.16=%.1f,410.17=%d,410.18=%d,410.19=%d,410.20=%.2f,410.21=%.2f,410.22=%.1f,410.23=%.1f,410.24=%.2f,410.25=%d,410.26=%d,410.27=%d,410.28=%.2f,410.29=%.2f,410.30=%.2f,410.31=%.2f,410.32=%d,410.33=%d",
                            SERVER_READ_WRITE_PACKET,
                            Device.SerialNo,
                            TargetOutletConductivity,   //0
                            PrepurifyTimeInSec,         //1
                            PurifyTimeInSec,            //2
                            WasteTimeInSec,             //3
                            HF_WasteTimeInSec,          //4
                            CIP_DosingOnTimeInSec,      //5
                            CIP_DosingOffTimeInSec,     //6
                            0,
                            0,
                            0,
                            0,
                            CIP_FlushTimeInSec,         //11
                            WV_CheckTimeInSec,          //12
                            WaitHT_TimeInSec,           //13
                            (float)TargetPureFlowLperM/10,     //14
                            (float)LowFlowPurifyAlarmSetpoint/10,  //15
                            (float)LowFlowWasteAlarmSetpoint/10,   //16
                            MaxCycleCountForCIP,            //17
                            CabinetHighTemperatureSetVal,   //18
                            MAX_CIP_PumpRunTimeInSec,       //19
                            (float)AnaOutProportionalFactor/100,   //20
                            (float)DynaProportionalFactor/100,     //21
                            (float)PAE_PureStepVoltageSetpoint/10, //22
                            (float)PAE_WasteStepVoltageSetpoint/10,    //23
                            (float)CurrentProportionalFactor/100,  //24
                            FlowMeter.K_Factor,             //25
                            VolumeUnit,                 //26
                            BypassOption,               //27
                            (float)AnalogPressureStartLevel/100,   //28
                            (float)AnalogPressureStopLevel/100,    //29
                            (float)AnalogPressureBypassLevel/100,  //30
                            (float)AnalogPressureCipLevel/100,     //31
                            CIP_WaitTimeInSec,             //32
                            WaitTimeB4BypassAlarmInSec);   //33
                #endif
                strcpy(PasteBuff,MsgBuff);
                PutStringGprs(PasteBuff);
                if(MobileDebugFlag == TRUE)
                {	PutStringUsb("\r\nCOMM <- [");
                    PutStringUsb(PasteBuff);
                }	
                PutCharGprs('*');
                if(MobileDebugFlag == TRUE)
                {	PutStringUsb("*");
                }
                sprintf(CopyBuff,"%d]",GprsCrcCodeOut);
                PutStringGprs(CopyBuff);
                if(MobileDebugFlag == TRUE)
                {	PutStringUsb(CopyBuff);
                    PutStringUsb("\r\nDATE TIME REQUEST SENT. WAITING FOR SERVER RESPONSE\r\n");
                }
                MobileStage = MOBILE_STAGE_RESPONSE_WAIT;
                MobileStageWaitSec = 0;
            }
            //check if any packet to send
            else if(LogPtr != UploadPtr)
            {   if(LogPtr >= UploadPtr)
                {	if(((LogPtr-UploadPtr)/SECOND_LOG_PACKET_SIZE) >= MinRecPerConnection)
                    {	Upload1Pkt2Server();
                        if(MobileDebugFlag == TRUE)
                            PutStringUsb("\r\nPACKET SENT. WAITING FOR SERVER RESPONSE\r\n");
                    }
                }
                else
                {   if(((SECOND_LOG_END_ADD-UploadPtr+LogPtr-SECOND_LOG_START_ADD)/SECOND_LOG_PACKET_SIZE) >= MinRecPerConnection)
                    {   Upload1Pkt2Server();
                        if(MobileDebugFlag == TRUE)
                            PutStringUsb("\r\nPACKET SENT. WAITING FOR SERVER RESPONSE\r\n");
                    }
                }
                MobileStage = MOBILE_STAGE_RESPONSE_WAIT;
                MobileStageWaitSec = 0;
            }
            
            else//now there is no packet to send
            {   
                /*//SEND HEART BEAT EVERY 1 MIN
                if(MobileStageWaitSec / 60 > 0)
                {	MobileStageWaitSec = 0;
                    //send heartbeat packet
                    PutCharGprs('[');
                    GprsCrcCodeOut = 0;
                    sprintf(MsgBuff,"%d,%s,%d,%s\n*",
                                SERVER_HEART_BEAT_PACKET,
                                SERVER_COMM_PROTOCOL_VERSION,
                                Device.SerialNo,
                                DEVICE_VERSION);
                    strcpy(PasteBuff,MsgBuff);
                    PutStringGprs(PasteBuff);
                    if(MobileDebugFlag == TRUE)
                    {	PutStringUsb("\r\nCOMM <- [");
                        PutStringUsb(PasteBuff);
                    }
                    sprintf(CopyBuff,"%d]",GprsCrcCodeOut);
                    PutStringGprs(CopyBuff);
                    if(MobileDebugFlag == TRUE)
                    {	PutStringUsb(CopyBuff);
                    }
                    
                    MobileStage = MOBILE_STAGE_RESPONSE_WAIT;
                }
                */
                /*
                //disconnect connection
                if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)//IS MEDIA WIFI
                {   MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_PACKET_GENERATION;
                }*/
            }
        break;
        case MOBILE_STAGE_RESPONSE_WAIT:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_RESPONSE_WAIT");
            if(MobileStageWaitSec > 10)
			{	ServerNoResponseCounter++;
                if(ServerNoResponseCounter > 2)
                {   if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)
                        MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;	
                    else
                        MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_SWITCHING;
                }
                else
                {   MobileStage = MOBILE_STAGE_SEND_DATA;
                }
            }
			else//wait more
			{   if(GprsCommandResponseFlag == TRUE)//IS any response?
                {   MobileStage = MOBILE_STAGE_ANALYSE_RESPOND;
                    ServerNoResponseCounter = 0;
                }
            }
        break;
        case MOBILE_STAGE_ANALYSE_RESPOND:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_ANALYSE_RESPOND");
            /*
            if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)
            {   if(CheckGPRSResponse("SERVER,DISCONNECTED",1)==TRUE)
                {   if(CommunicationMediaPrefered == COMM_MEDIA_WIFI)
                    {   GprsCommandResponseFlag = FALSE; 
                        ServerNoResponseCounter = 0;
                        GprsReceiveCounter = 0; 
                        GprsBuff[0]='\0';
                        MobileStage = MOBILE_STAGE_RESPONSE_WAIT;//wait for connetion to happen
                        break;
                    }
                }
                else if(CheckGPRSResponse("SERVER,CONNECTED",1)==TRUE)
                {   MobileStage = MOBILE_STAGE_SEND_DATA;
                    break;
                }
            }
            */
            //if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)//IS MEDIA ETHERNET
            {   AnalyzeResponse();
                GprsCommandResponseFlag = FALSE; 
                ServerNoResponseCounter = 0;
                GprsReceiveCounter = 0; 
                GprsBuff[0]='\0';
            }
            /*else//media = wifi
            {   
            }*/
            if(MobileStage != MOBILE_STAGE_WIFI_NETWORK_QUERY)//if analyzeResponse did not sent to Network query, then wait for response
            {   if(MobileStage != MOBILE_STAGE_SWITCH_TO_WIFI)
                {   if(MobileStage != MOBILE_STAGE_ROUTING_DECISION_MAKING)
                    {   if(MobileStage != MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING)
                        {   MobileStage = MOBILE_STAGE_RESPONSE_WAIT;//wait more 5 sec to receive command or more command
                            MobileStageWaitSec = 0;
                            //PutStringUsb("\r\nWaiting For Server Command");
                        }
                    }
                }
            }
        break;
        case MOBILE_STAGE_ROUTING_DECISION_MAKING:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_ROUTING_DECISION_MAKING");
            //check if there is anything to send
            //if(LogPtr != UploadPtr)
            {
                if(CommunicationMediaPrefered == COMM_MEDIA_ETHERNET)
                {   if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)//IS MEDIA ETHERNET 
                       MobileStage = MOBILE_STAGE_SEND_DATA;
                    else if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)//IS MEDIA WIFI 
                       MobileStage = MOBILE_STAGE_SWITCH_TO_ETHERNET;
                    else //if(CommunicationMediaCurrent == COMM_MEDIA_NONE)
                      MobileStage = MOBILE_STAGE_SWITCH_TO_ETHERNET;
                }
                else if(CommunicationMediaPrefered == COMM_MEDIA_WIFI)
                {   if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)//IS MEDIA ETHERNET
                       MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                    else if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)//media = wifi
                    {   if(GprsDataModeFlag == TRUE)//is already in data mode
                        {   if(ServerNoResponseCounter < 3)
                               MobileStage = MOBILE_STAGE_SEND_DATA;  
                            else
                            {   MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_SWITCHING; 
                                //MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;//restart from begining on wifi connection
                            }
                        }
                        else
                            MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                    }
                    else
                      MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                }
                else if(CommunicationMediaPrefered == COMM_MEDIA_AUTO_SWITCH)
                {   if(CommunicationMediaCurrent == COMM_MEDIA_ETHERNET)//IS MEDIA ETHERNET
                    {   if(ServerNoResponseCounter < 3)
                            MobileStage = MOBILE_STAGE_SEND_DATA; 
                        else
                        {    MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                             ServerNoResponseCounter = 0;
                        }
                    }
                    else if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)//CURRENT MEDIA IS WIFI
                    {   if(ServerNoResponseCounter < 3)
                            MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI; 
                        else
                        {   MobileStage = MOBILE_STAGE_SWITCH_TO_ETHERNET;
                            ServerNoResponseCounter = 0;
                        }
                    }
                    else
                      MobileStage = MOBILE_STAGE_SWITCH_TO_ETHERNET;
                }
            }
            /*else
            {   //switch off both module
                CommunicationMediaCurrent = COMM_MEDIA_NONE;
                GPRS_PWR_KEY_HIGH;
            }*/
        break;
        
        case MOBILE_STAGE_SWITCH_TO_WIFI:
            PutStringUsb("\r\nCOMM->SWITCH_TO_WIFI");
            GPRS_UART->SCR &= ~0x10;//DISABLE SERIAL receive interrupt in order to prevent controller being hang due to serial Rx pin gets low and controller gets stuck in serial interrupt
            GPRS_PWR_KEY_LOW;
            GPRS_DTR_HIGH;
            
            CommunicationMediaCurrent = COMM_MEDIA_WIFI;
            MobileStage = MOBILE_STAGE_SWITCHING_WAIT;
            GprsDataModeFlag = TRUE;    //SO THAT IT ISSUE COMMAND MODE COMMAND +++ BEFORE ANY COMMAND, COMPULSORY SEND SendGPRScommand("+++")
            MobileStageWaitSec = 0;
            WifiErrorCounter = 0;
        break;
        case MOBILE_STAGE_WIFI_AT_COMMAND:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_AT_COMMAND");
            if(MobileStageWaitSec > 15)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                ServerNoResponseCounter++;	
                PutStringUsb("\r\nWIFI RTO");    //NTC: 22.06.08 00:03
                //SoftwareResetProcessor();                       //NTC: 22.06.08 00:03
			}
			else
			{	if(MobileCommandFlag == FALSE)
				{	strcpy(PasteBuff,"AT");
                    SendGPRScommand(PasteBuff);
                }
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("OK",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_WIFI_ROUTER_SSID_AND_PASSWORD;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   strcpy(PasteBuff,"+++");
                        SendGPRScommand(PasteBuff);
                        ServerNoResponseCounter++;
                        MobileCommandFlag = FALSE;	//REISSUE COMMAND
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//REISSUE COMMAND	
					}
				}
			}
        break;
        case MOBILE_STAGE_WIFI_ROUTER_SSID_AND_PASSWORD:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_ROUTER_SSID_AND_PASSWORD");
            if(MobileStageWaitSec > 5)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                ServerNoResponseCounter++;	
			}
			else
			{	if(MobileCommandFlag == FALSE)
				{	strcpy(PasteBuff,"AT+WJAP=");
                    DirectMemRead(WIFI_SSID_NAMEa,CopyBuff,WIFI_SSID_NAMEn);
                    CopyBuff[WIFI_SSID_NAMEn] = '\0';
                    strcat(PasteBuff,CopyBuff);
                    strcat(PasteBuff,",");
                    DirectMemRead(WIFI_SSID_PASSWORDa,CopyBuff,WIFI_SSID_PASSWORDn);
                    CopyBuff[WIFI_SSID_PASSWORDn] = '\0';
                    strcat(PasteBuff,CopyBuff);
                    SendGPRScommand(PasteBuff);
                    DelayInMilliSec(100);//1000
						
				}
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("OK",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_WIFI_NETWORK_QUERY;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   strcpy(PasteBuff,"+++");
                        SendGPRScommand(PasteBuff);
                        ServerNoResponseCounter++;
                        MobileCommandFlag = FALSE;	//REISSUE COMMAND
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//REISSUE COMMAND	
					}
				}
			}
        break;
        case MOBILE_STAGE_WIFI_NETWORK_QUERY:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_NETWORK_QUERY");
            if(MobileStageWaitSec > 30)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                ServerNoResponseCounter++;	
			}
			else
			{	if(MobileCommandFlag == FALSE)
				{	SendGPRScommand("AT+WJAPS");
				}
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("STATION_UP",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(CheckGPRSResponse("OK",1)==TRUE)
					{   //MobileCommandFlag = FALSE;//REISSUE THE SAME COMMAND TO CHECK AGAIN IF STATION IS UP
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                        GprsReceiveCounter = 0;
                        GprsBuff[GprsReceiveCounter] = '\0';
                        ServerNoResponseCounter++;
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//NO RESPONSE = REISSUE COMMAND	
					}
				}
			}
        break;
        case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_PACKET_GENERATION:
        case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_SWITCHING:
        case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING");
            if(MobileStageWaitSec > 30)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                ServerNoResponseCounter++;	
			}
			else
			{	if(MobileCommandFlag == FALSE)
				{	SendGPRScommand("AT+CIPSTOP=1");
				}
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("OK",1)==TRUE)
					{   switch(MobileStage)
                        {   case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_SWITCHING:
                                GprsDataModeFlag = FALSE;
                                MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                            break;
                            case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING:
                                MobileStage = MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT;
                            break;
                            case MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_PACKET_GENERATION:
                                MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                            break;
                        }
                        MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                        GprsReceiveCounter = 0;
                        GprsBuff[GprsReceiveCounter] = '\0';
                        ServerNoResponseCounter++;
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//NO RESPONSE = REISSUE COMMAND	
					}
				}
			}
        break;
        case MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT");
            if(MobileStageWaitSec > 30)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;	
			}
			else
			{	if(MobileCommandFlag == FALSE)
				{	strcpy(PasteBuff,"AT+CIPSTART=1,tcp_client,");
                    DirectMemRead(SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE+ST_IPorDOMAINp,CopyBuff,ST_IPorDOMAINn);
                    CopyBuff[ST_IPorDOMAINn] = '\0';
                    strcat(PasteBuff,CopyBuff);
                    strcat(PasteBuff,",");
                    DirectMemRead(SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE+ST_PORTp,CopyBuff,ST_PORTn);
                    u.i[0] = 0;
                    u.c[0] = CopyBuff[0];
                    u.c[1] = CopyBuff[1];
                    sprintf(CopyBuff,"%d",u.i[0]);
                    strcat(PasteBuff,CopyBuff);
                    SendGPRScommand(PasteBuff);
				}
				else if(GprsCommandResponseFlag == TRUE)
				{   if(CheckGPRSResponse("SERVER,CLOSED",1)==TRUE)
					{  MobileCommandFlag = FALSE;//RESSIUE THE COMMAND 
                    }
                    else if(CheckGPRSResponse("SERVER,CONNECTED",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_WIFI_ENABLE_AUTO_RECEIVE;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
                        WifiErrorCounter = 0;
					}
					else if(CheckGPRSResponse("OK",1)==TRUE)
					{   //MobileCommandFlag = FALSE;//REISSUE THE SAME COMMAND TO CHECK AGAIN IF SERVER CONNECTED
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   WifiErrorCounter++;
                        if(WifiErrorCounter > 50)
                        {   GPRS_PWR_KEY_HIGH;//DEACTIVATE BOTH WIFI & ETHERNET
                            GPRS_DTR_LOW;//NOT WIFI
                            MobileStage = MOBILE_STAGE_POWER_UP_WAIT;
                            MobileStageWaitSec = 0;
                        }
                        else
                        {   MobileCommandFlag = FALSE;
                        }
					}
				}
                else if(MobileResponseWaitSec>2)
                {	MobileCommandFlag = FALSE;	//NO RESPONSE = REISSUE COMMAND	
                }
			}
        break;
        case MOBILE_STAGE_WIFI_ENABLE_AUTO_RECEIVE:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_ENABLE_AUTO_RECEIVE");
            /*if(MobileStageWaitSec > 30)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;	
			}
			else*/
			{	if(MobileCommandFlag == FALSE)
				{	SendGPRScommand("AT+CIPRECVCFG=1");
				}
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("OK",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_WIFI_COMMAND_TO_DATA_MODE;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   //MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
						GprsReceiveCounter = 0;
                        GprsBuff[GprsReceiveCounter] = '\0';
                        //MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//NO RESPONSE = REISSUE COMMAND	
					}
				}
			}
        break;
        case MOBILE_STAGE_WIFI_COMMAND_TO_DATA_MODE:
            //PutStringUsb("\r\nSTAGE->MOBILE_STAGE_WIFI_COMMAND_TO_DATA_MODE");
            /*if(MobileStageWaitSec > 30)
			{	MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;	
			}
			else*/
			{	if(MobileCommandFlag == FALSE)
				{	SendGPRScommand("AT+CIPSENDRAW");
				}
				else if(GprsCommandResponseFlag == TRUE)
				{  	if(CheckGPRSResponse("OK",1)==TRUE)
					{   MobileStage = MOBILE_STAGE_SEND_DATA;
						MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
                        GprsDataModeFlag = TRUE;
                        GprsBuff[0] = '\0';//flush buffer
                        GprsReceiveCounter = 0;
					}
					else if(CheckGPRSResponse("ERROR",1)==TRUE)
					{   //MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
						GprsReceiveCounter = 0;
                        GprsBuff[GprsReceiveCounter] = '\0';
                        //MobileStageWaitSec = 0;
						MobileCommandFlag = FALSE;
					}
					else if(MobileResponseWaitSec>2)
					{	MobileCommandFlag = FALSE;	//NO RESPONSE = REISSUE COMMAND	
					}
				}
			}
        break;
        
        case MOBILE_STAGE_WAIT:		
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////
void AnalyzeResponse()
{   if(strlen(GprsBuff)>0)
    {   /*
        if(MobileDebugFlag == TRUE)
        {   PutStringUsb("\r\nSERVER-> ");
            PutStringUsb(GprsBuff);
        }
        */
        //if received via wifi, packet looks like this -> +CIPEVENT:SOCKET,1,9,[0,1*183]<CR><LF>
        //If the packet is received via wifi, only msg inbetween [] will be present
        if(CommunicationMediaCurrent == COMM_MEDIA_WIFI)
        {
            if(CheckGPRSResponse("STATION_DOWN",1)==TRUE)
            {   PutStringUsb("\r\nROUTER GOT DISCONNECTED. CHECK ROUTER ");
                MobileStage = MOBILE_STAGE_WIFI_NETWORK_QUERY;
                GprsReceiveCounter = 0;
                GprsBuff[GprsReceiveCounter] = '\0';
                MobileStageWaitSec = 0;
                MobileCommandFlag = FALSE;
            }
            else if(CheckGPRSResponse("SERVER,DISCONNECTED",1)==TRUE)
            {   WifiServerDisconnectCounter++;
                if(WifiServerDisconnectCounter>6)
                {   GPRS_PWR_KEY_HIGH;//DEACTIVATE BOTH WIFI & ETHERNET
                    GPRS_DTR_LOW;//NOT WIFI
                    MobileStage = MOBILE_STAGE_POWER_UP_WAIT;
                    MobileStageWaitSec = 0;
                }
                else if(WifiServerDisconnectCounter>4)
                {   GPRS_PWR_KEY_HIGH;//DEACTIVATE BOTH WIFI & ETHERNET
                    GPRS_DTR_LOW;//NOT WIFI
                    MobileStage = MOBILE_STAGE_SWITCH_TO_WIFI;
                }
                else if(WifiServerDisconnectCounter > 2)
                {   MobileStage = MOBILE_STAGE_ROUTING_DECISION_MAKING;
                    GprsReceiveCounter = 0;
                    GprsBuff[GprsReceiveCounter] = '\0';
                    MobileStageWaitSec = 0;
                    MobileCommandFlag = FALSE;
                }
                else
                {   MobileStage = MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING;
                    GprsReceiveCounter = 0;
                    GprsBuff[GprsReceiveCounter] = '\0';
                    MobileStageWaitSec = 0;
                    MobileCommandFlag = FALSE;
                }
            }
        }
        //CALCULATE CRC
        for(u.i[0]=0,uiTemp=0;GprsBuff[u.i[0]]!= '*';u.i[0]++)
        {	uiTemp += GprsBuff[u.i[0]];
        }
        uiTemp += GprsBuff[u.i[0]];
        GprsBuff[u.i[0]] = '\0'; //TO INDICATE END OF COMMAND AT PLACE OF *
        //get message crc, msg ends with either NULL or '}'
        for(u.i[0]++,u.i[1]=0;GprsBuff[u.i[0]]!= '\0' && GprsBuff[u.i[0]]!= '}';u.i[0]++)
        {	u.i[1] *= 10;
            u.i[1] += GprsBuff[u.i[0]]-48;
        }
        
        if(MobileDebugFlag == TRUE)
        {   sprintf(PasteBuff,"\r\nMyCrc=%d RecvCrc=%d",uiTemp,u.i[1]);	//TESTING CODE
            PutStringUsb(PasteBuff);	//TESTING CODE
        }
        if(uiTemp == u.i[1] || u.i[1] == 0)
        {	WifiServerDisconnectCounter = 0;
            //decode received msg
            for(u.i[0]=0,u.i[1]=0;GprsBuff[u.i[0]]!= ',';u.i[0]++)
            {	u.i[1] *= 10;
                u.i[1] += GprsBuff[u.i[0]]-48;
            }
            CommandFormat.Cmd[0] = u.i[1];//command/response type
            
            switch(CommandFormat.Cmd[0])
            {	case SERVER_READ_WRITE_PACKET:{
                    CommandFormat.Sender = SENDER_SERVER;
                    if(MobileDebugFlag == TRUE)
                    {   PutStringUsb("\r\n\r\nSERVER TO DEVICE COMMAND = ");
                        PutStringUsb(&GprsBuff[u.i[0]+1]);
                        PutStringUsb("\r\n\r\n");
                    }
                    Command(&GprsBuff[u.i[0]+1]);
                }
                break;
                case SERVER_FOTA_PACKET:
                    /*
                    if(MobileDebugFlag == TRUE){
                        PutStringUsb("\r\n\r\nSERVER TO DEVICE FOTA RESPONSE");
                        sprintf(CopyBuff,"----%s----",&GprsBuff[u.i[0]+1]);
                        PutStringUsb(CopyBuff);
                    }*/
                    for(u.i[0]++,u.i[1]=0;GprsBuff[u.i[0]]!= ',';u.i[0]++)
                    {	u.i[1] *= 10;
                        u.i[1] += GprsBuff[u.i[0]]-48;
                    }
                    CommandFormat.Parameter[0][0]= u.i[1];
                    //sprintf(CopyBuff,"---CommandFormat.Parameter[0][0]=%d",CommandFormat.Parameter[0][0]);
                    //PutStringUsb(CopyBuff);
                    switch(CommandFormat.Parameter[0][0])
                    {	case SERVER_FOTA_REQ_REJECTED: 
                            Server.Sr &= ~SERVER_PROFILE_CONTROL_ENABLE;	//DISABLE FURTHER CONNECTION UNTILL RESET
                            //LISTENER ACTIVE STATUS
                            Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                            Fota.Status = FIRMWARE_NOT_AVAILABLE;
                            CopyBuff[0] = Fota.Status;
                            RandomMemWrite(FOTA_STATUSa,CopyBuff,FOTA_STATUSn);
                            if(MobileDebugFlag == TRUE)
                            {	PutStringUsb("\r\nS.R=REJECTED");
                            }
                        break;
                        case SERVER_FOTA_RESPONSE_ACK:
                            if(MobileDebugFlag == TRUE)
                            {	PutStringUsb("\r\nS.R=FOTA ACK");
                            }
                            //Extract page no
                            for(u.i[0]++,u.i[1]=0;GprsBuff[u.i[0]]!= ',';u.i[0]++)
                            {	u.i[1] *= 10;
                                u.i[1] += GprsBuff[u.i[0]]-48;
                            }
                            u.i[0]++;//move cursor ahead of comma
                            
                            RandomMemWrite(SECOND_LOG_START_ADD+u.i[1]*FLASH_PAGE_SIZE,&GprsBuff[u.i[0]],FLASH_PAGE_SIZE);
                            //verify write
                            DirectMemRead(SECOND_LOG_START_ADD+u.i[1]*FLASH_PAGE_SIZE,PasteBuff,FLASH_PAGE_SIZE);
                            for(uiTemp=0;uiTemp<FLASH_PAGE_SIZE;uiTemp++)
                            {	if(GprsBuff[u.i[0]+uiTemp] == PasteBuff[uiTemp])
                                    continue;
                                else
                                    break;
                            }
                            if(uiTemp == FLASH_PAGE_SIZE)
                            {    Fota.PageNo++;
                                /*/do page erase
                                for(uiTemp=0;uiTemp<FLASH_PAGE_SIZE;uiTemp++)
                                    PasteBuff[uiTemp]=0xFF;
                                RandomMemWrite(SECOND_LOG_START_ADD+Fota.PageNo*FLASH_PAGE_SIZE,PasteBuff,FLASH_PAGE_SIZE);
                                DelayInMilliSec(100);
                                //page erase done*/
                            }
                            else
                                PutStringUsb("\r\nMEMORY WRC ERROR");
                            Fota.Status = FIRMWARE_DOWNLOADING;
                            CopyBuff[0] = Fota.Status;
                            RandomMemWrite(FOTA_STATUSa,CopyBuff,FOTA_STATUSn);
                            Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                            SendFotaCommandToServer();
                            //now wait for response
                            GprsReceiveCounter = 0;
                            GprsByteRecvFlag = FALSE;
                            MobileStageWaitSec = 0;
                        break;		 
                        case SERVER_FOTA_FILE_TRANSFER_COMPLETE:
                            //Extract page no
                            for(u.i[0]++,u.i[1]=0;GprsBuff[u.i[0]]!= ',';u.i[0]++)
                            {	u.i[1] *= 10;
                                u.i[1] += GprsBuff[u.i[0]]-48;
                            }
                            //sprintf(PasteBuff,"\r\nReq^ Page No=%d, Recv^ Page No=%d",Fota.PageNo,u.i[1]);
                            //PutStringUsb(PasteBuff);
                            if(Fota.PageNo == u.i[1])
                            {	CopyBuff[0] = 0xff;
                                CopyBuff[1] = 0xff;
                                RandomMemWrite(SECOND_LOG_START_ADD+u.i[1]*FLASH_PAGE_SIZE,CopyBuff,2);
                                PutStringUsb("\r\nFile Received Completed");//send full stop
                                //CHECK FIRMWARE VALIDITY HERE
                                //IF VALID
                                Fota.Status = FIRMWARE_AVAILABLE;
                                CopyBuff[0] = Fota.Status;
                                RandomMemWrite(FOTA_STATUSa,CopyBuff,FOTA_STATUSn);
                                //DISABLE FOTA SERVER
                                Server.Sr = SERVER_PROFILE_CONTROL_DISABLE;
                                //writing to memory
                                PasteBuff[ST_SRp] = Server.Sr;
                                strcpy(&PasteBuff[ST_IPorDOMAINp],Server.IpOrDomainName);
                                u.i[0] = Server.Port;
                                PasteBuff[ST_PORTp] = u.c[0];
                                PasteBuff[ST_PORTp+1] = u.c[1];
                                RandomMemWrite(SERVER_START_ADDRESS+5*SERVER_RECORD_BYTE_SIZE,PasteBuff,SERVER_RECORD_BYTE_SIZE);
                            /*
                                PutStringUsb("\r\n-------------------------------------------------------------\r\nREADING SAVED FILE\r\n--------------------------------------------\r\n");
                                
                                for(uiTemp=0;uiTemp<10;uiTemp++)
                                {	#ifdef WATCH_DOG_TIMER_ENABLED
                                        RESET_WATCH_DOG_TIMER;
                                    #endif
                                    DelayInSec(1);
                                }
                                for(u.i[1]=0;u.i[1]<=Fota.PageNo;u.i[1]++)
                                {	
                                    #ifdef WATCH_DOG_TIMER_ENABLED
                                        RESET_WATCH_DOG_TIMER;
                                    #endif
                                    DirectMemRead(SECOND_LOG_START_ADD+u.i[1]*FLASH_PAGE_SIZE,PasteBuff,FLASH_PAGE_SIZE);
                                    PasteBuff[FLASH_PAGE_SIZE]= '\0';
                                    PutStringUsb(PasteBuff);
                                    DelayInMilliSec(200);
                                }
                                */
                                //reset processor
                                //PutStringUsb("\r\nRESTARTING TO UPDATE FIRMWARE");
                                strcpy(LcdLine1,"FIRMWARE UPDATIN");
                                strcpy(LcdLine2," MAINTAIN POWER ") ;
                                SendSerialHmiPacket();
                                PutStringUsb("\r\nRESTART DUE TO FIRMWARE UPDATE");//RESET TEST CODE
                                SoftwareResetProcessor();
                            }
                            else
                            {	PutStringUsb("\r\nEND OF FILE PAGE DID NOT MATCHED");
                                Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                                SendFotaCommandToServer();
                                //now wait for response
                                GprsReceiveCounter = 0;
                                GprsByteRecvFlag = FALSE;
                                MobileStageWaitSec = 0;
                            }
                        break;
                    }
                    
                break;
                case SERVER_LOG_PACKET: //RESPONSE
                    for(u.i[0]++,u.i[1]=0;GprsBuff[u.i[0]]!= '\0' && GprsBuff[u.i[0]]!= ',';u.i[0]++)//astric has been replaced by NULL while calculating CRC
                    {	u.i[1] *= 10;
                        u.i[1] += GprsBuff[u.i[0]]-48;
                    }
                    CommandFormat.Parameter[0][0]= u.i[1];
                    
                    switch(CommandFormat.Parameter[0][0])
                    {	case SERVER_RESPONSE_REQ_REJECTED: 
                            Server.Sr &= ~SERVER_PROFILE_CONTROL_ENABLE;	//DISABLE FURTHER CONNECTION UNTILL RESET
                            GPRS_PWR_KEY_HIGH;//DEACTIVATE BOTH WIFI & ETHERNET
                            GPRS_DTR_LOW;//NOT WIFI
                            MobileStage = MOBILE_STAGE_POWER_UP_WAIT;
                            MobileStageWaitSec = 0;
                            CommunicationMediaCurrent = COMM_MEDIA_NONE;
                            CommunicationMediaPrefered = COMM_MEDIA_NONE;
                            //LISTENER ACTIVE STATUS
                            Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                            if(MobileDebugFlag == TRUE)
                            {	PutStringUsb("\r\nS.R=REJECTED");
                            }
                        break;
                        case SERVER_RESPONSE_ACK: 	
                            //ServerAckTimeStamp = FreeRunSec;
                            if(MobileDebugFlag == TRUE)
                            {	PutStringUsb("\r\nS.R=ACK");
                            }
                            UploadPtr += UploadPtr2Increment;
                            if(UploadPtr >= SECOND_LOG_END_ADD)
                                UploadPtr = SECOND_LOG_START_ADD + (UploadPtr - SECOND_LOG_END_ADD);
                            //update upload pointer
                            UploadPtr2Increment = 0;//by mistake if server sends ack again
                            //LISTENER ACTIVE STATUS
                            Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                            //check periodic save
                            u.i[0] = 0;
                            DirectMemRead(UPLOAD_PTR_PROFILE1a,u.c,UPLOAD_PTRn);
                            if(UploadPtr > u.i[0])
                            {   if(((UploadPtr - u.i[0])/SECOND_LOG_PACKET_SIZE) > 100)
                                {    PeriodicSave();
                                     //PutStringUsb("\r\nPERIODIC SAVE UP>MEM");
                                }
                            }
                            else
                            {   if(((UploadPtr-SECOND_LOG_START_ADD+SECOND_LOG_END_ADD-u.i[0])/SECOND_LOG_PACKET_SIZE) > 100)
                                {    //sprintf(PasteBuff,"\r\nUploadPtr=%d\r\nMemUpPtr=%d",UploadPtr,u.i[0]);
                                     //PutStringUsb(PasteBuff);
                                     //PutStringUsb("\r\nPERIODIC SAVE MEM>UP");
                                     PeriodicSave();
                                }
                            }
                        break;
                        case SERVER_RESPONSE_CRC_ERROR: 
                            if(MobileDebugFlag == TRUE)
                                PutStringUsb("\r\nS.R=NACK");
                            MobileStage = ReturnToMobileStage;
                            MobileStageWaitSec = 0;
                            MobileCommandFlag = FALSE;
                            //LISTENER ACTIVE STATUS
                            Server.Sr &= ~(SERVER_PROFILE_DONT_ATTEMPT_CONNECTION | SERVER_PROFILE_NOT_RESPONDING);
                        break;
                        default:if(MobileDebugFlag == TRUE)
                                {    sprintf(CopyBuff,"\r\nS.R=%d UNKNOWN",CommandFormat.Parameter[0][0]);
                                     PutStringUsb(CopyBuff);
                                }
                        break;
                    }
                    MobileStageWaitSec = 3;
                    GprsReceiveCounter = 0; GprsBuff[0]='\0';
                break;
                default: //for testing only
                    sprintf(CopyBuff,"\r\nUNKNOWN SERVER RESPONSE COMMAND = %d",CommandFormat.Cmd[0]);
                    PutStringUsb(CopyBuff);
                break;
            }
        }
        else								//TESTING CODE
        {	PutStringUsb("\r\nCRC ERROR");	//TESTING CODE
        }									//TESTING CODE
        GprsCommandFlag = FALSE;
    }
}
///////////////////////////////////////////////////////////////////////////////
/*
void CheckCall()
{	uiTemp = SearchString(GprsBuff,"+CLIP:");
	if(uiTemp != -1)
	{   PutStringUsb(GprsBuff);//testing code
		//EXTRACT NUMBER
		//if(MobileDebugFlag == TRUE)
		//	PutStringUsb("\r\nCaller ID format code = ");//testing code
		for(;GprsBuff[uiTemp]!=',';uiTemp++);//this is mobile no end BYTE"
		for(u.i[0] = uiTemp+1,ucTemp=0;GprsBuff[u.c[0]] != ',';u.i[0]++)
		{	ucTemp *= 10;
			ucTemp += GprsBuff[u.i[0]]-48;
			//if(MobileDebugFlag == TRUE)
			//	PutCharUsb(GprsBuff[u.i[0]]);//testing code
		}
		switch(ucTemp)
		{	case 161:
			case 129:	uiTemp -= 11;CountryCodeFlag = FALSE;break;
			case 145: 	for(;GprsBuff[uiTemp] != '+';uiTemp--);uiTemp++;CountryCodeFlag = TRUE;break;
		}
		//if(MobileDebugFlag == TRUE)
		//	PutStringUsb(" Caller = ");
		for(CommandFormat.Sender = 0;GprsBuff[uiTemp]!='"';uiTemp++)
		{	CommandFormat.Sender *= 10;
			CommandFormat.Sender += GprsBuff[uiTemp]-48;
			//if(MobileDebugFlag == TRUE)
			//	PutCharUsb(GprsBuff[uiTemp]);//TESTING CODE
		}
		if((GprsCR & CALL_CONTROL) > 0)
		{	if(MobileDebugFlag == TRUE)
			{	PutStringUsb("\r\nCall Received From ");//TESTING CODE
				//D2A(CommandFormat.Sender,CopyBuff);//TESTING CODE
				sprintf(CopyBuff,"%lld",CommandFormat.Sender);
				PutStringUsb(CopyBuff);//TESTING CODE
			}
			iTemp = SearchUserMobileNumber(CommandFormat.Sender,0); 
			if(iTemp>=0)
			{	DirectMemRead(USER_TABLE_START_ADDRESS+(iTemp*USER_TABLE_RECORD_SIZE),u.c,UT_MOBn);
				CommandFormat.Sender = u.ll;
				if(MobileDebugFlag == TRUE)
				{	sprintf(CopyBuff,"\r\nUser mem Loc : %d mob = %lld",iTemp,u.ll);//TESTING CODE
					PutStringUsb(CopyBuff);//TESTING CODE
				}
				CheckCallResponse(iTemp);
			}
			else if(MobileDebugFlag == TRUE)
				PutStringUsb("\r\nUnknown Caller");
		}
		CountryCodeFlag = TRUE;
		//ENTER INTO SLEEP MODE
		GPRS_MODULE_SLEEP;
	}
}
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
char CheckGPRSResponse(char* response,unsigned int gstate)
{	//char i;
    GprsByteRecvFlag = FALSE;
    for(;gstate>0;gstate--)
    {   FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
		FM3_SWWDT->WDOGINTCLR = 0x11111111;
		FM3_SWWDT->WDOGLOCK = 0;
		
		DelayInMilliSec(1);
        if(GprsByteRecvFlag == TRUE || GprsReceiveCounter>0)
        {   while(GprsByteRecvFlag == TRUE)
            {   GprsByteRecvFlag = FALSE;
                Delay(65530);
            }
            if(SearchString(GprsBuff,response) != -1)
			  	return TRUE;//FOUND
			else if(SearchString(GprsBuff,"ERROR") != -1)
                return ERROR;   //error
        }
    }
    return FALSE; //NOT FOUND			
}
///////////////////////////////////////////////////////////////////////////////
void SendGPRScommand(char *command)
{	//ui c;
  	//if(GprsCR > 0)
  	{	if(MobileDebugFlag == TRUE)
		{	PutStringUsb("\r\n---\r\nComm <- ");
			PutStringUsb(command);
        }
        else
          DelayInMilliSec(100);
		if(GprsDataModeFlag == TRUE || strcmp(command,"+++")==0)
		{	DelayInMilliSec(100);
			PutStringGprs("+++");
			DelayInMilliSec(100);
			GprsDataModeFlag = FALSE;
		}
		PutStringGprs(command);
		GprsReceiveCounter = 0;
		GprsBuff[GprsReceiveCounter] = '\0';
		PutCharGprs('\r');
		MobileCommandFlag = TRUE;
		MobileResponseWaitSec = 0;
		GprsCommandResponseFlag = FALSE;
	}
}	
///////////////////////////////////////////////////////////////////////////////
unsigned char SendSms(long long mobileno,boolean_t countrycodeflag,char* buff)
{   static char reattemptcounter = 0;
  	//PutStringUsb("\r\nInside SendSms");
	FM3_SWWDT->WDOGLOCK = 0x1ACCE551;
	FM3_SWWDT->WDOGINTCLR = 0x11111111;
	FM3_SWWDT->WDOGLOCK = 0;
	
	if(mobileno == 0)
	  	return SUCCESS;
	
	if(reattemptcounter == 3)
	{  	reattemptcounter = 0;
	  	return ERROR;
	}
	reattemptcounter++;
	
	if(countrycodeflag==TRUE)
		sprintf(CopyBuff,"AT+CMGS=\"+%lld\"",mobileno);
	else
	  	sprintf(CopyBuff,"AT+CMGS=\"%lld\"",mobileno);
    SendGPRScommand(CopyBuff);
    if(CheckGPRSResponse(">",30000)==TRUE)
    {   PutStringGprs(buff);
        PutCharGprs(26);
        if(CheckGPRSResponse("OK",10000)==TRUE)
        {   reattemptcounter = 0;
			MobileCommandFlag = FALSE;
			MobileStageWaitSec = 0;
		  	return SUCCESS;
        }
        else
        {   if(MobileDebugFlag == TRUE)
				PutStringUsb(GprsBuff);
			SendSms(mobileno,countrycodeflag,buff);
		}
    }
    /*else
    {   PutStringUsb("\r\nGPRS > ERROR: Message Prompt '>' Not received");
		SendSms(mobileno,countrycodeflag,buff);
	}*/
    reattemptcounter = 0;
	MobileCommandFlag = FALSE;
	MobileStageWaitSec = 0;
	return ERROR;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Upload1Pkt2Server()
{	if(ServerType == SERVER_DATA)
	{	if(((UploadPtr-SECOND_LOG_START_ADD)%SECOND_LOG_PACKET_SIZE)!=0)
		{	//PutStringUsb("\r\nERROR ON UPLOADPOINTER");
            if(LogPtr == SECOND_LOG_START_ADD)
				UploadPtr = SECOND_LOG_END_ADD-SECOND_LOG_PACKET_SIZE;
			else
				UploadPtr = LogPtr-SECOND_LOG_PACKET_SIZE;
			//Record this 
			//TriggerPid = MY_PID_INVALID_UPLOAD_PTR;
			//LogRecord();
		}
        //sprintf(CopyBuff,"\r\nLog=%d,Up=%d",LogPtr,UploadPtr);
        //PutStringUsb(CopyBuff);
		if(LogPtr > UploadPtr)
		{	//PutStringUsb(" L>U");
            if(((LogPtr-UploadPtr)/SECOND_LOG_PACKET_SIZE) >= MAX_REC_PER_CONNECTION)
			{	ucTemp = MAX_REC_PER_CONNECTION;
                //PutStringUsb(" MAX");
            }
            else
			{	ucTemp = (LogPtr-UploadPtr)/SECOND_LOG_PACKET_SIZE;
            }
		}
		else
		{	if(((SECOND_LOG_END_ADD-UploadPtr+LogPtr-SECOND_LOG_START_ADD)/SECOND_LOG_PACKET_SIZE) >= MAX_REC_PER_CONNECTION)
				ucTemp = MAX_REC_PER_CONNECTION;
			else
				ucTemp = ((SECOND_LOG_END_ADD-UploadPtr+LogPtr-SECOND_LOG_START_ADD)/SECOND_LOG_PACKET_SIZE);
		}
		
		#if (PROTOCOL_VERSION == ASCII)
		  	PutCharGprs('[');
            GprsCrcCodeOut = 0;
            sprintf(MsgBuff,"%d,%s,%d,%s\n",
						SERVER_LOG_PACKET,
                        SERVER_COMM_PROTOCOL_VERSION,
                        Device.SerialNo,
                        DEVICE_VERSION);
            strcpy(PasteBuff,MsgBuff);
			PutStringGprs(PasteBuff);
            if(MobileDebugFlag == TRUE)
            {	PutStringUsb("\r\nCOMM <- [");
                PutStringUsb(PasteBuff);
            }	
                        
			for(cTemp=0;cTemp<ucTemp;cTemp++)
			{	if(ucTemp==cTemp+1)
				{	uiTemp = LogPtr-SECOND_LOG_PACKET_SIZE;
					if(uiTemp < SECOND_LOG_START_ADD)
					  	uiTemp = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
				  	DirectMemRead(uiTemp,CopyBuff,SECOND_LOG_PACKET_SIZE);//latest
				}
				else
				{  	uiTemp = UploadPtr+cTemp*SECOND_LOG_PACKET_SIZE;
					if(uiTemp >= SECOND_LOG_END_ADD)
					  uiTemp = SECOND_LOG_START_ADD + (uiTemp - SECOND_LOG_END_ADD);
					DirectMemRead(uiTemp,CopyBuff,SECOND_LOG_PACKET_SIZE);
				}
				//GprsCrcCodeOut = 0;
				//log in fifo method starts
				//start of record
                sprintf(MsgBuff,"\r%2d%2d%2d,%2d%2d%2d",
                        CopyBuff[LT_SECOND_DATEp],
						CopyBuff[LT_SECOND_DATEp+1],
						CopyBuff[LT_SECOND_DATEp+2],
						CopyBuff[LT_SECOND_TIMEp],
						CopyBuff[LT_SECOND_TIMEp+1],
						CopyBuff[LT_SECOND_TIMEp+2]);
                for(u.c[0]=0;MsgBuff[u.c[0]]!='\0';u.c[0]++)
                {   if(MsgBuff[u.c[0]] == ' ')
                        MsgBuff[u.c[0]] = '0';
                }
                strcpy(PasteBuff,MsgBuff);
                
				//cycle and step
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_CYCLEp];
				u.c[1] = CopyBuff[LT_SECOND_CYCLEp+1];
				sprintf(MsgBuff,",%d,%d",u.i[0],CopyBuff[LT_SECOND_STEPp]);
				strcat(PasteBuff,MsgBuff);
				
				//Run Sec
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_RUN_SECp];
				u.c[1] = CopyBuff[LT_SECOND_RUN_SECp+1];
				sprintf(MsgBuff,",%d",u.i[0]);
				strcat(PasteBuff,MsgBuff);
				
				//PAE volt
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_PAE_VOLTp];
				u.c[1] = CopyBuff[LT_SECOND_PAE_VOLTp+1];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/100);
				strcat(PasteBuff,MsgBuff);
				
                //total pure volume
				u.c[0] = CopyBuff[LT_SECOND_T_FLOWp];
				u.c[1] = CopyBuff[LT_SECOND_T_FLOWp+1];
				u.c[2] = CopyBuff[LT_SECOND_T_FLOWp+2];
				u.c[3] = CopyBuff[LT_SECOND_T_FLOWp+3];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/100);
				strcat(PasteBuff,MsgBuff);
				
				//Current Flow
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_CURRENT_FLOWp];
				u.c[1] = CopyBuff[LT_SECOND_CURRENT_FLOWp+1];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/100);
				strcat(PasteBuff,MsgBuff);
				
				//Avg EC
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_AVERAGE_ECp];
				u.c[1] = CopyBuff[LT_SECOND_AVERAGE_ECp+1];
				u.c[2] = CopyBuff[LT_SECOND_AVERAGE_ECp+2];
				sprintf(MsgBuff,",%d",u.i[0]/100);//LastFiveCyclePureAverageConductivityAverage
				strcat(PasteBuff,MsgBuff);
				
				//alarm
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_ALARMp];
				u.c[1] = CopyBuff[LT_SECOND_ALARMp+1];
				u.c[2] = CopyBuff[LT_SECOND_ALARMp+2];
				sprintf(MsgBuff,",%d",u.i[0]);
				strcat(PasteBuff,MsgBuff);
                
                //WATER TEMP
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_WATER_TEMPERATUREp];
				u.c[1] = CopyBuff[LT_SECOND_WATER_TEMPERATUREp+1];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/1000);
				strcat(PasteBuff,MsgBuff);
				
				//CABINET TEMP
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_CABINET_TEMPERATUREp];
				u.c[1] = CopyBuff[LT_SECOND_CABINET_TEMPERATUREp+1];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/10);
				strcat(PasteBuff,MsgBuff);
				
                //PRESSURE
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_PRESSUREp];
				u.c[1] = CopyBuff[LT_SECOND_PRESSUREp+1];
                if(u.i[0] < 32767)
                    sprintf(MsgBuff,",%.2f",(float)u.i[0]/100);
				else
                    sprintf(MsgBuff,",-%.2f",(float)(65536-u.i[0])/100);
                strcat(PasteBuff,MsgBuff);
				
                //analog output voltage
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp];
				u.c[1] = CopyBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp+1];
				sprintf(MsgBuff,",%.2f",(float)u.i[0]/1000);
				strcat(PasteBuff,MsgBuff);
				
                //input status
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_DIGITAL_INPUTp];
				sprintf(MsgBuff,",%d",u.i[0]);
				strcat(PasteBuff,MsgBuff);
				
                //output status, % recovery, mode
                u.i[0] = 0;
				u.c[0] = CopyBuff[LT_DIGITAL_OUTPUTp];
				u.c[1] = CopyBuff[LT_DIGITAL_OUTPUTp+1];
				sprintf(MsgBuff,",%d,%d,%d",u.i[0],CopyBuff[LT_PERCENTAGE_RECOVERYp],CopyBuff[LT_MODEp]);
				strcat(PasteBuff,MsgBuff);
				
                //LIVE EC
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_LIVE_ECp];
				u.c[1] = CopyBuff[LT_SECOND_LIVE_ECp+1];
				u.c[2] = CopyBuff[LT_SECOND_LIVE_ECp+2];
				sprintf(MsgBuff,",%d",u.i[0]/100);//LastFiveCyclePureAverageConductivityAverage
				strcat(PasteBuff,MsgBuff);
				
                //PAE current
				u.i[0] = 0;
				u.c[0] = CopyBuff[LT_SECOND_PAE_CURRENTp];
				u.c[1] = CopyBuff[LT_SECOND_PAE_CURRENTp+1];
				sprintf(MsgBuff,",%.2f\n",(float)u.i[0]/100);
				strcat(PasteBuff,MsgBuff);
				
                PutStringGprs(PasteBuff);
                if(MobileDebugFlag == TRUE)
                {	PutStringUsb(PasteBuff);
                }
                
			}
            PutCharGprs('*');
            if(MobileDebugFlag == TRUE)
            {	PutStringUsb("*");
            }
            sprintf(CopyBuff,"%d]",GprsCrcCodeOut);
            PutStringGprs(CopyBuff);
            if(MobileDebugFlag == TRUE)
            {	PutStringUsb(CopyBuff);
            }
			//PutCharGprs(']');//end of packet  
		#elif (PROTOCOL_VERSION == HEX)
				//send packet header
				PutCharGprs('[');
				GprsCrcCodeOut = 0;
				u.ll = 	GprsPacket.Imei;
				PutCharGprs(u.c[0]);//LSB
				PutCharGprs(u.c[1]);
				PutCharGprs(u.c[2]);
				PutCharGprs(u.c[3]);
				PutCharGprs(u.c[4]);
				PutCharGprs(u.c[5]);
				PutCharGprs(u.c[6]);//MSB
				//PutCharGprs(u.c[7]);
						
				for(cTemp=0;cTemp<ucTemp;cTemp++)
				{	if(ucTemp==cTemp+1)
					{	uiTemp = LogPtr-SECOND_LOG_PACKET_SIZE;
						if(uiTemp < SECOND_LOG_START_ADD)
							uiTemp = SECOND_LOG_END_ADD - SECOND_LOG_PACKET_SIZE;
						DirectMemRead(uiTemp,CopyBuff,SECOND_LOG_PACKET_SIZE);//latest
					}
					else
					{  	uiTemp = UploadPtr+cTemp*SECOND_LOG_PACKET_SIZE;
						if(uiTemp >= SECOND_LOG_END_ADD)
						  uiTemp = SECOND_LOG_START_ADD + (uiTemp - SECOND_LOG_END_ADD);
						DirectMemRead(uiTemp,CopyBuff,SECOND_LOG_PACKET_SIZE);
					}
				
					for(iTemp=0;iTemp<SECOND_LOG_PACKET_SIZE;iTemp++)
						PutCharGprs(CopyBuff[iTemp]);		
				}
				//whole pkt has been sent. now send crc_16
				u.i[0] = GprsCrcCodeOut;
				PutCharGprs(u.c[0]);
				PutCharGprs(u.c[1]);
				PutCharGprs(']');
		#endif
		//UPDATE UPLOAD POINTER HERE
		//if(ucTemp < MAX_REC_PER_CONNECTION)
		if(uiTemp<LogPtr-SECOND_LOG_PACKET_SIZE)
			UploadPtr2Increment = (ucTemp-1)*SECOND_LOG_PACKET_SIZE;
		else
			UploadPtr2Increment = (ucTemp)*SECOND_LOG_PACKET_SIZE;
		
	}
	else	//ServerType == SERVER_FOTA_TYPE
	{	SendFotaCommandToServer();
	  	
	}
	//now wait for response
	GprsReceiveCounter = 0;
	GprsByteRecvFlag = FALSE;
    GprsCommandResponseFlag = FALSE; 
}

/*
void GetMobileInfo()
{	sprintf(PasteBuff,"IMEI:%lld",GprsPacket.Imei);
	strcat(PasteBuff,Gmt2LocalTimeStamp(GprsPacket.TimeStamp,LAST));
	sprintf(CopyBuff,"\r\nhttp://maps.google.com/?q=%.6f,%.6f",
			GprsPacket.Lat,
			GprsPacket.Lng);
	strcat(PasteBuff,CopyBuff);
	
	switch(GprsPacket.BatteryChargingStatus)
	{	case NOT_CHARGING: 	
		case CHARGING: 		if(CarBatteryAV>5000)
								strcat(PasteBuff,"\r\nCharging: YES");
							else
								strcat(PasteBuff,"\r\nCharging: NO");
							break;
		case CHARGING_COMPLETE: strcat(PasteBuff,"\r\nCharging: FULL");break;
	}
	sprintf(CopyBuff," %d%%=%dmV",GprsPacket.BatteryPercentage,GprsPacket.BatteryVoltage);
	strcat(PasteBuff,CopyBuff);
	sprintf(CopyBuff,"\r\nSignal: %d LEVEL",GprsPacket.SignalStrength);
	strcat(PasteBuff,CopyBuff);
	switch(GprsPacket.Network)
	{	case NETWORK_REGISTERED_HOME: 	strcat(PasteBuff,"\r\nNtk: HOME");break;
		case NETWORK_UNKNOWN:			strcat(PasteBuff,"\r\nNtk: UNKNOWN");break;
		case NETWORK_REGISTERED_ROAMING:strcat(PasteBuff,"\r\nNtk: ROAMING");break;
	}
}
*/
void SendFotaCommandToServer()
{	/*
    ReadSerialInfoInCopyBuff();
	u.c[0] = CopyBuff[1];
	u.c[1] = CopyBuff[2];
	u.c[2] = CopyBuff[3];
	u.c[3] = CopyBuff[4];
	*/
	//retrive the file name
	DirectMemRead(UPDATE_FIRMWARE_FILENAMEa,CopyBuff,UPDATE_FIRMWARE_FILENAMEn);
	CopyBuff[UPDATE_FIRMWARE_FILENAMEn]='\0';//so that it never exceeds its length
				
	PutCharGprs('[');	//packet header
    if(MobileDebugFlag == TRUE)
    {	PutCharUsb('[');
    }
	GprsCrcCodeOut = 0;	//initialize CRC
		
	sprintf(PasteBuff,"%d,%d,%d,%s*",
			SERVER_FOTA_PACKET,//MOVED AS FIRST FIELD INSTEAD OF SECOND FIELD, for PCB ver2.0
            Device.SerialNo,//u.i[0], //GprsPacket.Imei,
			Fota.PageNo,    //file page no
			CopyBuff);      //file name
	PutStringGprs(PasteBuff);
    if(MobileDebugFlag == TRUE)
    {	PutStringUsb(PasteBuff);
    }
    sprintf(PasteBuff,"%d]",GprsCrcCodeOut);
    PutStringGprs(PasteBuff);
    if(MobileDebugFlag == TRUE)
    {	PutStringUsb(PasteBuff);
    }
	//PutStringUsb("\r\nDevice to Server -> ");	//testing Code
	//PutStringUsb(PasteBuff);					//testing Code
}	
