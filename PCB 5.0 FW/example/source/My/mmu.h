#ifndef __MMU_H__
#define __MMU_H__

#include "common.h"

//TESTING CODE ACTIVATION setting starts

//TESTING CODE ACTIVATION setting ends

#define ALARM_OCCURED	1
#define ALARM_RESOLVED	0

//-------- DEFINATION       		ADDRESS	MAX LENGTH 				NO_OF_BYTES	DESCRIPTION
#define FLASH_PAGE_SIZE	528
//FIRMWARE RELATED
//#define FIRMWARE_START_PAGE 0
//#define FIRMWARE_START_ADDRESS 0
//#define FIRMWARE_MAX_SIZE_IN_KB 128
//#define FIRMWARE_END_ADDRESS 131071	//128*1024
//#define FIRMWARE_END_PAGE 246
/*******************************************************************************
Memory gap between firmware and system setting due to page size can be used for
other purpose i.e memory from 131072 to 131471 = 400 byte
*******************************************************************************/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//SYSTEM RELATED
#define SYSTEM_SETTING_START_PAGE 		0
#define SYSTEM_SETTING_START_ADDRESS 	0
//-------- DEFINATION       			ADDRESS	MAX LENGTH 				NO_OF_BYTES	DESCRIPTION
#define FOTA_STATUSa					0                                           //this location should not change as it is also used by fota
	#define	FOTA_STATUSn							1
#define SETTING_STATUS_REGa				1	                                        //this location should not change as it is also used by fota
	#define SETTING_STATUS_REGn						1
#define DEVICE_PASSWORDa 				19
	#define DEVICE_PASSWORDn						4	//4digit password

#define SUPER_ADMIN_EMAILa				23													//Super admin mobile no
	#define SUPER_ADMIN_EMAILn						50
//#define ADMIN_EMAILa					73
//	#define ADMIN_EMAILn							50	//50byte email

#define PRODUCT_REG_NOa					123
	#define PRODUCT_REG_NOn							15	//HR1234ABC1234
#define OPERATOR_NAMEa					138
	#define OPERATOR_NAMEn							30	//30 byte name
#define TIME_ZONEa						168
	#define TIME_ZONEn								3	//in second
//#define REPORTING_TIMEa					171
//	#define REPORTING_TIMEn			   				2	//in milli volt
#define PROJ_PARA_MCRa					173				//project parameter master control register
	#define PROJ_PARA_MCRn							32
#define LAST_OUTPUT_STATUSa				205
	#define LAST_OUTPUT_STATUSn						2
#define ADMIN_CALL_ACTIONa				207
	#define ADMIN_CALL_ACTIONn						1
#define OPERATOR_CALL_ACTIONa			208
	#define OPERATOR_CALL_ACTIONn					1
#define USER_CALL_ACTIONa				209
	#define USER_CALL_ACTIONn						1
#define BALANCE_CHECK_NOa				210
	#define BALANCE_CHECK_NOn						10
#define MOBILE_SUBSCRIBER_IDENTITYa		220		
	#define MOBILE_SUBSCRIBER_IDENTITYn				8
#define UPDATE_FIRMWARE_FILENAMEa		228
	#define UPDATE_FIRMWARE_FILENAMEn				30

//project parameter SETPOINTS starts
#define PREPURIFY_TIME_IN_SECa			258
	#define PREPURIFY_TIME_IN_SECn					1
#define PURIFY_TIME_IN_SECa				259
	#define PURIFY_TIME_IN_SECn						2
#define WASTE_TIME_IN_SECa				261
	#define WASTE_TIME_IN_SECn						2
#define HF_WASTE_TIME_IN_SECa			263
	#define HF_WASTE_TIME_IN_SECn					2
#define CIP_DOSING_ON_TIME_IN_SECa		265
	#define CIP_DOSING_ON_TIME_IN_SECn				2
#define CIP_DOSING_OFF_TIME_IN_SECa		267
	#define CIP_DOSING_OFF_TIME_IN_SECn				2
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
#define CIP_DOSING_TOTAL_TIME_IN_SECa	269
	#define CIP_DOSING_TOTAL_TIME_IN_SECn				2
#define CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa		271
	#define CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn			2
#define CIP_FLOW_PULSE_ON_TIME_IN_SECa			273
	#define CIP_FLOW_PULSE_ON_TIME_IN_SECn				1
#define CIP_FLOW_PULSE_OFF_TIME_IN_SECa			274
	#define CIP_FLOW_PULSE_OFF_TIME_IN_SECn				1
#endif
#define CIP_FLUSH_TIME_IN_SECa					275
	#define CIP_FLUSH_TIME_IN_SECn						2
#define WV_CHECK_TIME_IN_SECa					277
	#define WV_CHECK_TIME_IN_SECn						1
#define WAIT_HT_TIME_IN_SECa					278
	#define WAIT_HT_TIME_IN_SECn						2
#define TARGET_PURE_FLOW_LITER_PER_MINa			280//for actual value divide it by 10
	#define TARGET_PURE_FLOW_LITER_PER_MINn				1
#define TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa	281
	#define TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn		2
#define LOW_FLOW_PURIFY_ALARM_SET_POINTa		283	//for actual value divide it by 10
	#define LOW_FLOW_PURIFY_ALARM_SET_POINTn			1
#define LOW_FLOW_WASTE_ALARM_SET_POINTa			284	//for actual value divide it by 10
	#define LOW_FLOW_WASTE_ALARM_SET_POINTn				1
#define MAX_CYCLE_RUNCOUNT_FOR_CIPa				285
	#define MAX_CYCLE_RUNCOUNT_FOR_CIPn					2
#define CABINET_HIGH_TEMPERATURE_SET_VALa		287
	#define CABINET_HIGH_TEMPERATURE_SET_VALn			1
#define MAX_CIP_PUMP_RUNTIMEa					288
	#define MAX_CIP_PUMP_RUNTIMEn						2
//1 byte blank here
#define PUMP_P_FACTORa							291		//for actual value divide it by 100
	#define PUMP_P_FACTORn								2
#define DYNAMIC_P_FACTORa						293		//for actual value divide it by 100
	#define DYNAMIC_P_FACTORn							2
#define AVERAGE_VOLTAGE_PUREa					295	
	#define AVERAGE_VOLTAGE_PUREn						2
#define AVERAGE_VOLTAGE_WASTEa					297	
	#define AVERAGE_VOLTAGE_WASTEn						2
#define PAE_PURE_CYCLE_VOLTAGEa					299		
	#define PAE_PURE_CYCLE_VOLTAGEn						2
#define PAE_WASTE_CYCLE_VOLTAGEa				301
	#define PAE_WASTE_CYCLE_VOLTAGEn					2
#define WASTE_VALUE_CURRENTa					305
	#define WASTE_VALUE_CURRENTn						2
#define FLOWMETER_K_FACTORa						311
	#define FLOWMETER_K_FACTORn							2
#define VOLUME_UNITa                            313
    #define VOLUME_UNITn                                1
#define TEMPERATURE_OFFSET_REGISTERa            314
    #define TEMPERATURE_OFFSET_REGISTERn                4
#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
    #define BYPASS_OPTIONa                      318
        #define BYPASS_OPTIONn                          1
#endif
#define TANK_FULLa                              319
    #define TANK_FULLn                                  1
#define ANALOG_PRESSURE_START_LEVELa            320
    #define ANALOG_PRESSURE_START_LEVELn                2
#define ANALOG_PRESSURE_STOP_LEVELa             322
    #define ANALOG_PRESSURE_STOP_LEVELn                 2
#define ANALOG_PRESSURE_BYPASS_LEVELa           324
    #define ANALOG_PRESSURE_BYPASS_LEVELn               2
#define ANALOG_PRESSURE_CIP_LEVELa              326
    #define ANALOG_PRESSURE_CIP_LEVELn                  2
#define CIP_WAIT_TIME_IN_SECa                   328
    #define CIP_WAIT_TIME_IN_SECn                       2
#define WAIT_TIME_B4_BYPASS_ALARM_IN_SECa       330
    #define WAIT_TIME_B4_BYPASS_ALARM_IN_SECn           2
#define SP_EC_ZERO_CYCLEa                       332
    #define SP_EC_ZERO_CYCLEn                           1
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)
#define CIP_LOWER_LIMIT_CYCLEa                  333
    #define CIP_LOWER_LIMIT_CYCLEn                      2
#define CIP_FAVORABLE_TIMEa                     335
    #define CIP_FAVORABLE_TIMEn                         2
#define CIP_FAVORABLE_TIME_GRACE_MINa           337
    #define CIP_FAVORABLE_TIME_GRACE_MINn               1
#endif

//project non-volatile variable starts
#define LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEa	400
	#define LAST_CYCLE_OPTIMIZED_OUTPUT_VOLTAGEn		1
#define TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDa 401
	#define TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDn	4
#define SHUNT_TIME_IN_SECa                          405
    #define SHUNT_TIME_IN_SECn                          1
#define COMMUNICATION_MEDIA_PREFEREDa           406
    #define COMMUNICATION_MEDIA_PREFEREDn               1
#define PRESSURE_OFFSETa                        407             //float
    #define PRESSURE_OFFSETn                            4    
#define EC_ZERO_CYCLE_COUNTa                    411
    #define EC_ZERO_CYCLE_COUNTn                        1
#define WASTE_PRE_SHUNT_TIME_IN_SECa            412
    #define WASTE_PRE_SHUNT_TIME_IN_SECn                1
#define WASTE_POST_SHUNT_TIME_IN_SECa           413
    #define WASTE_POST_SHUNT_TIME_IN_SECn               1
#define HF_WASTE_SHUNT_TIME_IN_SECa             414
    #define HF_WASTE_SHUNT_TIME_IN_SECn                 1
#define SWITCH_PAE_VOLT_WASTEa                  415
    #define SWITCH_PAE_VOLT_WASTEn                      1
#define PAE_PREPURIFY_CYCLE_VOLTAGEa            417
    #define PAE_PREPURIFY_CYCLE_VOLTAGEn                2
#define SWITCH_PAE_VOLT_PUREa                   419
    #define SWITCH_PAE_VOLT_PUREn                       1
#define DYNAMIC_CAPDI_MODE_a                    420
    #define DYNAMIC_CAPDI_MODE_n                        1
#define PRE_PURE_CURRENT_a                      421
    #define PRE_PURE_CURRENT_n                          2
#define PURE_CURRENT_a                          423
    #define PURE_CURRENT_n                              2
#define WASTE_CURRENT_a                         425
    #define WASTE_CURRENT_n                             2
#define PRE_CHARGE_TIME_IN_SEC_a                427
    #define PRE_CHARGE_TIME_IN_SEC_n                    2
#define POST_CHARGE_TIME_IN_SEC_a               429
    #define POST_CHARGE_TIME_IN_SEC_n                   2
//vacent space from 431 to 527
/*******************************************************************************
SERVER TABLE
FIELD	DESCRIPTION		BYTE	TYPE
FIELD 1: IP ADDRESS		4 		U.C[4]
FIELD 2: PORT NO 		2		INT
FIELD 3: APN			15 		CHAR
FIELD 4: APN ACCESS NO	8 		DOUBLE
FIELD 5: APN USERNAME	10 		CHAR
FIELD 6: APN PASSWORD	10 		CHAR
1st record will be our server details and rest 5 will be of customer
*******************************************************************************/
#define SERVER_PAGE					1
#define SERVER_CLIENTa				528		
	#define SERVER_CLIENTn						1	//DEVICE WILL BE SERVER OR A CLIENT
		#define AS_A_CLIENT 0
		#define AS_A_SERVER 1
#define MIN_REC_PER_CONNECTIONa		529
	#define MIN_REC_PER_CONNECTIONn				1
//#if (PROTOCOL_VERSION == HEX)
//						#define MAX_REC_PER_CONNECTION		20
//#else
						#define MAX_REC_PER_CONNECTION		10
//#endif
#define APNa									530
	#define APNn									30
#define GPRS_CRa								560		//1b=call control, 2b=gsm control, 3b=GPSR CONTROL, 4b=EMAIL CONTROL
	#define GPRS_CRn								1
#define EMAIL_USERNAMEa							561
	#define EMAIL_USERNAMEn							51
#define EMAIL_PASSWORDa							612
	#define EMAIL_PASSWORDn							40
#define EMAIL_SENDER_EMAIL_ADDRESSa				652
	#define EMAIL_SENDER_EMAIL_ADDRESSn				51
#define EMAIL_OUTGOING_EMAIL_SERVERa			703
	#define EMAIL_OUTGOING_EMAIL_SERVERn			51
#define EMAIL_OUTGOING_EMAIL_SERVER_PORTa		754
	#define EMAIL_OUTGOING_EMAIL_SERVER_PORTn		2
#define WIFI_SSID_NAMEa                         765
    #define WIFI_SSID_NAMEn                         32
#define WIFI_SSID_PASSWORDa                     797
    #define WIFI_SSID_PASSWORDn                     32
// 829 to 859 vacant space available here
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SERVER_START_ADDRESS		860
#define SERVER_RECORD_BYTE_SIZE		28
//#define MAX_NO_OF_SERVER			1
//------------------------------------------------------------------------------
//PARAMETER VARIABLE		POSITION	NO OF BYTE
#define ST_SRp					0		
	#define ST_SRn							1
#define ST_IPorDOMAINp			1
	#define ST_IPorDOMAINn					25
#define ST_PORTp				26
	#define ST_PORTn						2
/*
#define	ST_APNp					28
	#define ST_APNn							15
#define ST_APN_ACCESS_NOp		43
	#define ST_APN_ACCESS_NOn				8
#define	ST_APN_USERNAMEp		51
	#define ST_APN_USERNAMEn				10
#define ST_APN_PASSWORDp		61
	#define ST_APN_PASSWORDn				10
*/
//------------------------------------------------------------------------------
//vacant space available here
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
PERIODIC SAVE REQUIRED SYSTEM PARAMETERS
*******************************************************************************/
#define PERIODIC_SAVE_PAGE 2
#define PERIODIC_SAVE_PAGE_START_ADDa	1056

//#define PERIODIC_SAVE_MINa				1056
//	#define PERIODIC_SAVE_MINn					1
#define POWER_OFF_DATEa					1057
	#define POWER_OFF_DATEn						3
#define POWER_OFF_TIMEa					1060
	#define POWER_OFF_TIMEn						3
#define CIP_PUMP_RUNTIMEa				1063
	#define CIP_PUMP_RUNTIMEn					2
#define LAST_CYCLE_COUNTER_FOR_CIPa		1065
	#define LAST_CYCLE_COUNTER_FOR_CIPn			2

#define OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa	1067
	#define OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPn		2
#define OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa	1069
	#define OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPn		2

#define MODEa							1071
	#define MODEn								1
#define LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYa	1072
	#define LAST_CYCLE_PURE_AVERAGE_CONDUCTIVITYn	3
#define LAST_CYCLE_PURE_VOLUMEa			1075
	#define LAST CYCLE_PURE_VOLUMEn				2
#define LAST_CYCLE_WASTE_VOLUMEa			1077
	#define LAST_CYCLE_WASTE_VOLUMEn				2
#define CIP_DOSING_RUNTIME_IN_SECa			1079//so that if system restart during cip, system should remember how much has been dosed before restart
	#define CIP_DOSING_RUNTIME_IN_SECn			2
#define CIP_PULSE_RUNTIME_IN_SECa			1081//so that if system restart during cip PULSE, system should remember how much PASSED before restart
	#define CIP_PULSE_RUNTIME_IN_SECn			2
#define PERCENT_RECOVERYa                   1083
    #define PERCENT_RECOVERYn                   5
#define LAST_FIVE_CYCLE_AVG_ECa         1088
    #define LAST_FIVE_CYCLE_AVG_ECn             20  //3byte each 4*5=20
//#define BALANCE_CHECK_FLAGa				1108
//	#define BALANCE_CHECK_FLAGn					1
#define EMAIL_PTRa						1109
	#define EMAIL_PTRn							3
#define LOG_PTRa						1112
	#define LOG_PTRn							3
#define UPLOAD_PTRa						1115
	#define UPLOAD_PTRn							3
#define UPLOAD_PTR_PROFILE0a			UPLOAD_PTRa                 //1115
#define UPLOAD_PTR_PROFILE1a			UPLOAD_PTRa+UPLOAD_PTRn     //1118
#define UPLOAD_PTR_PROFILE2a			UPLOAD_PTRa+UPLOAD_PTRn*2   //1121
#define UPLOAD_PTR_PROFILE3a			UPLOAD_PTRa+UPLOAD_PTRn*3   //1124
#define UPLOAD_PTR_PROFILE4a			UPLOAD_PTRa+UPLOAD_PTRn*4   //1127
#define UPLOAD_PTR_PROFILE5a			UPLOAD_PTRa+UPLOAD_PTRn*5   //1130

#define LAST_STEP_B4_POWER_OFFa         1133
    #define LAST_STEP_B4_POWER_OFFn             1
#define LAST_RUN_SEC_B4_POWER_OFFa      1134
    #define LAST_RUN_SEC_B4_POWER_OFFn          4
#define T_FLOWa                         1138
    #define T_FLOWn                             4

#define OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa 1142
    #define OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPn 2

////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
project parameter starts
*******************************************************************************/
#define PP_START_PAGE						3
#define PP_MAX_BYTE							132	//PAGE SIZE/4 = 132 i.e 4 PARAMETER PER PAGE
#define PP_START_LOC						1584
#define MAX_PROJECT_PARAMETER				68	//Remember to change all below page if increased
#define PROJECT_PARAMETER_NO_OF_PAGE		17	//MAX_PROJECT_PARAMETER*PP_MAX_BYTE/FLASH_PAGE_SIZE
//PROJECT PARAMETER VARIABLE	POSITION	NO OF BYTE
#define PP_LOG_VALp					0
	#define PP_LOG_VALn							4
#define PP_LOG_UNITp				4
	#define PP_LOG_UNITn						1
#define PP_TRIG_PRIORITY_CONDITIONp	5			
	#define PP_TRIG_PRIORITY_CONDITIONn			1	//bit 0-4 condition, 5-6 priority, 7 master control			
#define PP_TRIG_PARA1_VALp			6			
	#define PP_TRIG_PARA1_VALn					4				
#define PP_TRIG_PARA1_UNITp			10			
	#define PP_TRIG_PARA1_UNITn					1				
#define PP_TRIG_PARA2_VALp			11			
	#define PP_TRIG_PARA2_VALn					4				
#define PP_TRIG_PARA2_UNITp			15			
	#define PP_TRIG_PARA2_UNITn					1
#define PP_TRIG_PARA3_VALp			16			
	#define PP_TRIG_PARA3_VALn					4				
#define PP_TRIG_PARA3_UNITp			20			
	#define PP_TRIG_PARA3_UNITn					1
#define PP_RESERVEDp				21
	#define PP_RESERVEDn						11
#define PP_MSGp						32				
	#define PP_MSGn								100				
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
INPUT TABLE no of page 1
FIELD 1: INPUT NAME 10 BYTE
FIELD 2: SERVER INPUT ID - STATUS REGISTER BIT LOCATION 1 BYTE (0 TO 255)
*******************************************************************************/
#define INPUT_TABLE_START_PAGE		20
#define INPUT_TABLE_START_ADDRESS	10560
#define INPUT_TABLE_RECORD_SIZE		11 //FIELD1 + FIELD 2
//PROJECT USER TABEL VARIABLE	POSITION	NO OF BYTE
#define IT_NAMEp					0
	#define IT_NAMEn						10
#define IT_REG_BIT_ADDRESSp			10
	#define IT_REG_BIT_ADDRESSn				1
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
Alarm TABLE
FIELD 1: Date at which alarm generated
FIELD 2: Time at which alarm generated
*******************************************************************************/
#define ALARM_TABLE_START_PAGE		21
#define ALARM_TABLE_START_ADDRESS	11088
#define ALARM_TABLE_RECORD_SIZE		6
//PROJECT USER TABEL VARIABLE	POSITION	NO OF BYTE
#define AT_DATEp					0
	#define AT_DATEn						3
#define AT_TIMEp					3
	#define AT_TIMEn						3	//hour and min and sec
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
USER TABLE no of page 3
FIELD 1: PROJECT PARAMETER TRIGGER ID	1 BYTE
FIELD 2: MOBILE NO	6 BYTE INCLUDING COUNTRY CODE EXCEPT +
*******************************************************************************/
#define USER_TABLE_START_PAGE		22
#define USER_TABLE_NO_OF_PAGE		3
#define USER_TABLE_START_ADDRESS	11616
#define MAX_NO_OF_USER				198//226
#define USER_TABLE_RECORD_SIZE		59
//fixed record in user table
#define USER_SUPER_ADMIN_MOBa		11616//USER_TABLE_START_ADDRESS
#define USER_ADMIN_MOBa				11675//USER_SUPER_ADMIN_MOBa + USER_TABLE_RECORD_SIZE
#define USER_OTHERS_MOBa			11734//USER_OPERATOR_MOBa + USER_TABLE_RECORD_SIZE
//PROJECT USER TABEL VARIABLE	POSITION	NO OF BYTE
//bit 0 to bit 48 is mobile no with country code
//bit 49 to bit 57 process id associated with the number
#define UT_MOBp						0	
	#define UT_MOBn								6
#define UT_EMAILp					6
	#define UT_EMAILn							51
#define UT_PP_T_IDp					57	
	#define UT_PP_T_IDn							1
#define CALL_ATTEMPTp				58
	#define CALL_ATTEMPTn						1
///////////////////////////////////////////////////////////////////////////////
//one page vaccent here
///////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
YEARLY LOG TABLE
*******************************************************************************/
#define YEARLY_LOG_TABLE_START_PAGE		32
#define YEARLY_LOG_START_ADD 				16896	
#define YEARLY_LOG_END_ADD					18696	 
#define YEARLY_LOG_PACKET_SIZE				16
////////////////////////////////////////////////////////////////////////////////
//DEFINATION						LOCATION	NO.OF.BYTE		//FIELD.NAME	NO.OF.BYTE	DATA.TYPE	UNIT

#define LT_YEARLYp						0						//DATE			3			int			val
	#define LT_YEARLYn								1
#define LT_YEARLY_T_CYCLEp				1						//TOT.FLOW		4			BITS		0-255
	#define LT_YEARLY_T_CYCLEn						3
#define LT_YEARLY_AVG_P_VOLTp			4						//TIME			3			int			val
	#define LT_YEARLY_AVG_P_VOLTn					1										
#define LT_YEARLY_AVG_W_VOLTp			5						//CYCLE			2			INT			YYMMDD
	#define LT_YEARLY_AVG_W_VOLTn					1										
#define LT_YEARLY_T_FLOWp				6						//STEP			1			INT			HHMMSS
	#define LT_YEARLY_T_FLOWn						4									
#define LT_YEARLY_AVG_ECp				10						//VOLTAGE		1			BITS		0-255
	#define LT_YEARLY_AVG_ECn						3									
#define LT_YEARLY_ALARMp				13						//TOT.FLOW		4			BITS		0-255
	#define LT_YEARLY_ALARMn						3
/*******************************************************************************
MONTHLY LOG TABLE
*******************************************************************************/
#define MONTHLY_LOG_TABLE_START_PAGE		36
#define MONTHLY_LOG_START_ADD 				19008	
#define MONTHLY_LOG_END_ADD					19164	 
#define MONTHLY_LOG_PACKET_SIZE				16
////////////////////////////////////////////////////////////////////////////////
//DEFINATION						LOCATION	NO.OF.BYTE		//FIELD.NAME	NO.OF.BYTE	DATA.TYPE	UNIT

#define LT_MONTHLY_DATEp				0						//DATE			3			int			val
	#define LT_MONTHLY_DATEn						2
#define LT_MONTHLY_T_CYCLEp				2						//TOT.FLOW		4			BITS		0-255
	#define LT_MONTHLY_T_CYCLEn						2
#define LT_MONTHLY_AVG_P_VOLTp			4						//TIME			3			int			val
	#define LT_MONTHLY_AVG_P_VOLTn					1										
#define LT_MONTHLY_AVG_W_VOLTp			5						//CYCLE			2			INT			YYMMDD
	#define LT_MONTHLY_AVG_W_VOLTn					1										
#define LT_MONTHLY_T_FLOWp				6						//STEP			1			INT			HHMMSS
	#define LT_MONTHLY_T_FLOWn						4									
#define LT_MONTHLY_AVG_ECp				10						//VOLTAGE		1			BITS		0-255
	#define LT_MONTHLY_AVG_ECn						3									
#define LT_MONTHLY_ALARMp				13						//TOT.FLOW		4			BITS		0-255
	#define LT_MONTHLY_ALARMn						3
/*******************************************************************************
DAILY LOG TABLE
*******************************************************************************/
#define DAILY_LOG_TABLE_START_PAGE		37
#define DAILY_LOG_START_ADD 				19536	
#define DAILY_LOG_END_ADD					20001	 
#define DAILY_LOG_PACKET_SIZE				17
////////////////////////////////////////////////////////////////////////////////
//DEFINATION						LOCATION	NO.OF.BYTE		//FIELD.NAME	NO.OF.BYTE	DATA.TYPE	UNIT

#define LT_DAILY_DATEp					0						//DATE			3			int			val
	#define LT_DAILY_DATEn							3
#define LT_DAILY_T_CYCLEp				3						//TOT.FLOW		4			BITS		0-255
	#define LT_DAILY_T_CYCLEn						2
#define LT_DAILY_AVG_P_VOLTp			5						//TIME			3			int			val
	#define LT_DAILY_AVG_P_VOLTn					1										
#define LT_DAILY_AVG_W_VOLTp			6						//CYCLE			2			INT			YYMMDD
	#define LT_DAILY_AVG_W_VOLTn					1										
#define LT_DAILY_T_FLOWp				7						//STEP			1			INT			HHMMSS
	#define LT_DAILY_T_FLOWn						4									
#define LT_DAILY_AVG_ECp				11						//VOLTAGE		1			BITS		0-255
	#define LT_DAILY_AVG_ECn						3									
#define LT_DAILY_ALARMp					14						//TOT.FLOW		4			BITS		0-255
	#define LT_DAILY_ALARMn							3
/*******************************************************************************
SECOND LOG TABLE
*******************************************************************************/
#define SECOND_LOG_TABLE_START_PAGE			38
#define SECOND_LOG_START_ADD 				20064	
#define SECOND_LOG_END_ADD					2162514
#define SECOND_LOG_PACKET_SIZE				45
#define LAST_MEM_ADDRESS                    2162687
////////////////////////////////////////////////////////////////////////////////
//DEFINATION						LOCATION	NO.OF.BYTE		//FIELD.NAME	DATA.TYPE	UNIT

#define LT_SECOND_DATEp					0						//DATE			int			val
	#define LT_SECOND_DATEn							3
#define LT_SECOND_TIMEp					3						//TIME			int			val
	#define LT_SECOND_TIMEn							3										
#define LT_SECOND_CYCLEp				6						//CYCLE			INT			YYMMDD
	#define LT_SECOND_CYCLEn						2										
#define LT_SECOND_STEPp					8						//STEP			INT			HHMMSS
	#define SECOND_STEPn							1									
#define LT_SECOND_RUN_SECp				9                       //RUN SEC       INT         0-65535
	#define LT_SECOND_RUN_SECn						2
#define LT_SECOND_PAE_VOLTp				11						//VOLTAGE		INT		    0-65535     //DURING DISPLAY MUST BE DIVIDED BY 100
	#define LT_SECOND_PAE_VOLTn						2									
#define LT_SECOND_T_FLOWp				13						//PURE VOL		INT			0-0xFFFF FFFF    //DURING DISPLAY MUST BE DIVIDE BY 100
	#define SECOND_T_FLOWn							4									
#define LT_SECOND_CURRENT_FLOWp			17						//CUR FLOW		INT		    0-65535      //DURING DISPLAY MUST BE DIVIDED BY 100
	#define LT_SECOND_CURRENT_FLOWn					2									
#define LT_SECOND_AVERAGE_ECp			19						//EC		    INT		    0-0xFF FFFF  //DURING DISPLAY MUST BE DIVIDED BY 100
	#define LT_SECOND_AVERAGE_ECn					3
#define LT_SECOND_ALARMp				22						//ALARM		    HEX		    0-0xFF FFFF
	#define LT_SECOND_ALARMn						3
#define LT_SECOND_CIP_PUMP_RUN_TIMEp	25						//CIP PUMP 		INT			0-65535
	#define LT_SECOND_CIP_PUMP_RUN_TIMEn			2
//************added for our server ******************************************************************
#define LT_SECOND_WATER_TEMPERATUREp	27						//W.TEMP 		INT			0-65535
	#define LT_SECOND_WATER_TEMPERATUREn			2
#define LT_SECOND_CABINET_TEMPERATUREp	29						//C.TEMP 		INT			0-65535
	#define LT_SECOND_CABINET_TEMPERATUREn			2
#define LT_SECOND_PRESSUREp	            31						//PRESSURE 		INT			0-65535
	#define LT_SECOND_PRESSUREn			            2
#define LT_SECOND_ANALOG_OUT_MILLIVOLTp	33						//C.TEMP 		INT			0-65535
	#define LT_SECOND_ANALOG_OUT_MILLIVOLTn			2
#define LT_DIGITAL_INPUTp               35
    #define LT_DIGITAL_INPUTn                       1
#define LT_DIGITAL_OUTPUTp              36
    #define LT_DIGITAL_OUTPUTn                      2
#define LT_PERCENTAGE_RECOVERYp         38
    #define LT_PERCENTAGE_RECOVERYn                 1
#define LT_MODEp                        39
    #define LT_MODEn                                1
#define LT_SECOND_LIVE_ECp			    40						//EC		    INT		    0-0xFF FFFF  //DURING DISPLAY MUST BE DIVIDED BY 100
	#define LT_SECOND_LIVE_ECn					    3
#define LT_SECOND_PAE_CURRENTp			43						//VOLTAGE		INT		    0-65535     //DURING DISPLAY MUST BE DIVIDED BY 100
	#define LT_SECOND_PAE_CURRENTn					2									
/*******************************************************************************
LOG TABLE
*******************************************************************************/
/*#define LOG_TABLE_START_PAGE		36
#define LOG_START_ADD 				19008	//USER_TABLE_START_ADDRESS+MAX_NO_OF_USER*USER_TABLE_RECORD_SIZE
#define LOG_END_ADD					2162622	//2162688	// -1 // MEMORY END ADDRESS
#define LOG_PACKET_SIZE				24
////////////////////////////////////////////////////////////////////////////////
//DEFINATION			LOCATION	NO.OF.BYTE		//FIELD.NAME	NO.OF.BYTE	DATA.TYPE	UNIT

#define LT_DATEp			0						//DATE			3			int			val
	#define LT_DATEn					3
#define LT_TIMEp			3						//TIME			3			int			val
	#define LT_TIMEn					3										
#define LT_CYCLEp			6						//CYCLE			2			INT			YYMMDD
	#define LT_CYCLEn					2										
#define LT_STEPp			8						//STEP			1			INT			HHMMSS
	#define LT_STEPn					1									
#define LT_RUN_SECp			9
	#define LT_RUN_SECn					2
#define LT_VOLTAGEp			11						//VOLTAGE		1			BITS		0-255
	#define LT_VOLTAGEn					1									
#define LT_TOT_FLOWp		12						//TOT.FLOW		4			BITS		0-255
	#define LT_TOT_FLOWn				4									
#define LT_CUR_FLOWp		16						//CUR.FLOW		2			int	
	#define LT_CUR_FLOWn				2
#define LT_CONDUCTIVITYp	18						//CONDUCTIVITY	2			INT			
	#define LT_CONDUCTIVITYn			2
#define LT_TEMPERATUREp		20						//TEMPERATURE	2			INT			
	#define LT_TEMPERATUREn				2
#define LT_CAB_TEMPERATUREp	22						//TEMPERATURE	2			INT			
	#define LT_CAB_TEMPERATUREn			1
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern boolean_t MemDebugFlag;

extern char AddOtherUserMobile(long long mobno,char pid);
extern void CheckTriggers();
extern void ConfigureVariable();
extern void ClearAlarmDate();
extern void DeleteAllUsers();
extern char DeleteUser(long long mobno,char pid);
//extern void GetParameterMem2Var(char pid);//to be deleted
//extern void GetPidMem2Var(char pid);
//extern void GetParameterVar2Buff(char pid);
extern void InitializeDefaultSetting();
extern void LoadSetting();
extern void LogRecord();
extern void PeriodicSave();
//extern int Pid2Constant(char *str);
extern void RegisterAlarm(uc alarm_code, boolean_t state);
//extern void Report(uc table, uc respond_to, ui condition);
//extern int SearchUserMobileNumber(long long mobno,char pid);//pid=0 if search with mobile only
//extern void SerialNoCheck();
extern void ReadWriteMemoryCommand(int mid, int sid, char data_format,uli address,int no_of_bytes);
//extern void ReadWriteParameterControl(int mid, int sid,char pid);
//extern void ReadWriteParameterLog(int mid, int sid, char pid);
//extern void ReadWriteParameterTrigPriority(int mid,int sid,char pid);
//extern void ReadWriteParameterTrigCondition(int mid,int sid,char pid);
//extern void ReadWriteParameterTrigPara1Val(int mid,int sid,char pid,char data_format);
//extern void ReadWriteParameterTrigMsg(int mid,int sid,char pid);
//extern char ReadWriteParameterLogUnit(char pid);
//extern char SetParameterTrigger(char pid, char parano);
//extern char SetParameterTrigCondition(char pid, char parano);
//extern char SetParameterTrigPara1(char pid, char parano);
//extern char SetParameterTrigPara2(char pid, char parano);
//extern char SetParameterTrigPara3(char pid, char parano);
//extern char SetParameterTrigMobile(char pid, char parano);
//extern char SetParameterTrigMessage(char pid, char parano);

//extern void CheckAndInsertDailyRecord();
//extern char* strcatPid(ui pid);
extern void strcatTrigCondition(char *str,char condition);//function to add Trigger Condition to string given
//extern void strcatTrigPriority(char *str,char condition);
//extern char* TrigPriority2str(char priority);
extern void strcatUnit(char *str,char unit);//function to add unit to string given
extern char TrigCondition2Constant(char*str);
//extern char TrigPriority2Constant(char*str);
//extern char Unit2Constant(char*str);
//extern void strcatUserAssociatedWithPid(char *dstr,char pid);

//extern void UpdateProjectParameter2Memory(char pid);

#endif

