//COUNTRY ISD CODE AS PER COUNTRY WISE SETTING
#define OR ||
#define AND &&

#define INDIA 91
#define NEPAL 977
//-----------------
#define COUNTRY INDIA
#define COUNTRY_STR "INDIA"

#define MANUFACTURER_VOLTEA     0
#define MANUFACTURER_INNOVATECH 1
#define MANUFACTURER MANUFACTURER_INNOVATECH
/**********************************/
//PRODUCT CONSTANTS
#if(MANUFACTURER==MANUFACTURER_VOLTEA)
    #define MODEL_NO 		"  VOLTEA V2.00  "	//must be 16 char long as it is center justified for LCD
#else
    #define MODEL_NO 		"  Electra V2.00 "	//must be 16 char long as it is center justified for LCD
#endif
#define EXCEL_FILE_VERSION "XFV0"
#define SUPER_PASSWORD "@a"
#define FACTORY_SET_DEVICE_PASSWORD "1010"
#define SUPER_ADMIN_DEFAULT_MOBILE_NO 0//919717579964
#define SERVER_COMM_PROTOCOL_VERSION "1.0"
#define SERVER_FOTA_PROTOCOL_VERSION "SFV1.0"

#define PRODUCT_DIUSE       0
#define PRODUCT_DIENTRY     1
#define PRODUCT_DISERIES    2
#define PRODUCT_CFO         3
#define PRODUCT_TYPE PRODUCT_DIENTRY

#define OLD_CONTACTOR 0
#define NEW_CONTACTOR 1
#define CONTACTOR_TYPE NEW_CONTACTOR

#define CVS_CIP_YES 0
#define CVS_CIP_NO 1
#define CVS_CIP CVS_CIP_NO //for CFO, always CVS CIP


#if (PRODUCT_TYPE == PRODUCT_DIUSE)
    #if(CONTACTOR_TYPE==OLD_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P3.B4.H3.F1.T5.4"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P3.B4.H3.F2.T5.4"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(CONTACTOR_TYPE==NEW_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P3.B4.H4.F1.T5.5"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P3.B4.H4.F2.T5.5"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(MANUFACTURER==MANUFACTURER_VOLTEA)
        #define PRODUCT_NAME 	"  VOLTEA DiUse  "	//must be 16 char long as it is center justified for LCD
    #else
        #define PRODUCT_NAME 	"Electra FlexMini"	//must be 16 char long as it is center justified for LCD
    #endif
#endif

#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #if(CONTACTOR_TYPE==OLD_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P4.B4.H3.F1.T5.4"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P4.B4.H3.F2.T5.4"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(CONTACTOR_TYPE==NEW_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P4.B4.H4.F1.T5.4"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P4.B4.H4.F2.T5.4"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(MANUFACTURER==MANUFACTURER_VOLTEA)
        #define PRODUCT_NAME 	" VOLTEA DiEntry "	//must be 16 char long as it is center justified for LCD
    #else
        #define PRODUCT_NAME 	" Electra FlexF1 "	//must be 16 char long as it is center justified for LCD
    #endif
#endif

#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #if(CONTACTOR_TYPE==OLD_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P5.B4.H3.F1.T4.9"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P5.B4.H3.F2.T4.9"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(CONTACTOR_TYPE==NEW_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P5.B4.H4.F1.T4.9"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P5.B4.H4.F2.T4.9"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #define PRODUCT_NAME 	" VOLTEA DiSeries"	//must be 16 char long as it is center justified for LCD
#endif

#if(PRODUCT_TYPE==PRODUCT_CFO)
    #if(CONTACTOR_TYPE==OLD_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P6.B4.H3.F1.T4.9"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P6.B4.H3.F2.T4.9"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #if(CONTACTOR_TYPE==NEW_CONTACTOR)
        #if(CVS_CIP==CVS_CIP_NO)
            #define DEVICE_VERSION 	"P6.B4.H4.F1.T4.9"	//must be 16 char long as it is center justified for LCD
        #else
            #define DEVICE_VERSION 	"P6.B4.H4.F2.T4.9"	//must be 16 char long as it is center justified for LCD
        #endif
    #endif
    #define PRODUCT_NAME 	"  VOLTEA DiCFO  "	//must be 16 char long as it is center justified for LCD
#endif

//PCB FUNCTIONALITY ACTIVE
#define WATCH_DOG_TIMER_ENABLED
#define PAE_FUNCTION_ENABLE
#define GSM_FUNCTION_ENABLE
#define ATLAS_REPLACEMENT_FUNCTION_ENABLE
#define ATLAS_I2C_FUNCTION_ENABLE

//TESTING CODE ACTIVATION
#define DEBUG 0
#define RELEASED 1
#define RELEASE_STATUS DEBUG
//#define DEBUG_MODE_TIMINGS
#define DEBUG_I2C_ERROR
//#define DEBUG_ONE_WIRE
#define DEBUG_PAE
#define DEBUG_EC_CALC    //this not working

//FEATURES
#define ONE_WIRE

//PROTOCOL VERSIONS
#define ASCII		100

#define PROTOCOL_VERSION ASCII

#define PI 3.1415926535897932384626433832795028841971
#define VOLT_STEP_SIZE		0.000413 //ok checked
//#define VOLT_STEP_SIZE	0.0003970444483 this is actual
#define DIODE_FORWARD_VOLTAGE 0.25
#define ANALOG_VOLTAGE_DIVIDER 31
#define MAX_CALL_ATTEMPT 10
#define MIN_REC_COUNTER_4_PERIODIC_SAVE 1	//LOG_REC_SEC * MIN_REC_COUNTER_4_PERIODIC_SAVE
#define TIMEOUT_SEC 60
#define LOG_REC_SEC 10//60
#ifdef  DEBUG_MODE_TIMINGS
	#define FLOW_CHECK_TIME_IN_SEC 30	//0.5 min
#else
	#define FLOW_CHECK_TIME_IN_SEC 300	//5 min
#endif

#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define WASTE_MAX_CURRENT   6600	//5940
    #define PURE_MAX_CURRENT    6600	//5940
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define WASTE_MAX_CURRENT   6600	//5940
    #define PURE_MAX_CURRENT    4200	//3780
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define WASTE_MAX_CURRENT   12500	//5940
    #define PURE_MAX_CURRENT    12500	//3780
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define WASTE_MAX_CURRENT   6600	
    #define PURE_MAX_CURRENT    4200	
#endif

#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define SHUNT_TIME_IN_SEC_DEFAULT_VAL 20
#else
    #define SHUNT_TIME_IN_SEC_DEFAULT_VAL 40
#endif
//input output constants
#define ON     	1
#define OFF    	0

//Prefered communication Media constants
#define COMM_MEDIA_NONE 0
#define COMM_MEDIA_AUTO_SWITCH 1
#define COMM_MEDIA_ETHERNET 2
#define COMM_MEDIA_WIFI 3

//SERVER TYPE
#define SERVER_DATA 0
#define SERVER_FOTA 3
enum response_to
{	USB_COM,
	SMS,
	EMAIL,
};
enum date_constants
{	ALL,
	TODAY,
	YESTERDAY,
	DAY_BEFORE_YESTERDAY,
	SPECIFIC_DATE_TIME,
        REBOOT_LOG,
};
enum mode//this constants should never change else in server wrong MODE will be seen.
{	MODE_LOGOUT         =0,
	MODE_AUTO           =1,
        MODE_MANUAL_FLUSH   =2,
        MODE_MANUAL_CIP     =3,
};
enum ProcessStep//this constants should never change else in server wrong step will be seen.
{	FREE_RUN_SEC            = 0,	
	IDLE                    = 1,			
	PREPURIFY               = 2,		
	PURIFY                  = 3,
        WASTE                   = 4,			
	HF_WASTE                = 5,		
	WAIT                    = 6,			
	CIP_DOSING_ON           = 7,	
	CIP_DOSING_OFF          = 8,
	CIP_PULSE_ON            = 9,    //only for DiUse	
	CIP_PULSE_OFF           = 10,	//only for DiUse
	CIP_FLUSH               = 11,		
	HIGH_TEMP               = 12,		
	WAIT_HT                 = 13,	
	SHUNT                   = 14,
        WAIT_BEFORE_CIP_START   = 15,
        SHUNT_POST_PURE         = 16,
        SHUNT_POST_WASTE        = 17,
        TEST_STEP               = 18,
        //WV_CHECK              = x,		
	//FLOW_CHECK            = y,
    
};
/*
enum MyPid
{	MY_PID_TIME 		=0,
	MY_PID_TEMPERATURE	=4,
#define MY_PID_AI_START	MY_PID_EXT_BATTERY
	MY_PID_EXT_BATTERY 	=5,
	MY_PID_INT_BATTERY	=6,
	MY_PID_AI1 			=7,
#define MY_PID_AI_END	MY_PID_AI1
#define MY_PID_DI_START	MY_PID_DI1
	MY_PID_DI1			=8,
	MY_PID_DI2			=9,
	MY_PID_DI3			=10,
	MY_PID_DI4			=11,
#define MY_PID_DI_END	MY_PID_DI4
	
	MY_PID_INVALID_UPLOAD_PTR	=254,
	MY_PID_RESTART		=255,
	
};
*/
/*
enum WorldPid//once issued do not change the world pid, instead use the unused id
{	WORLD_PID_TIME 		=0,
	WORLD_PID_EXT_BATTERY 	=20,
	WORLD_PID_INT_BATTERY	=21,
	WORLD_PID_AI1 			=22,
	
	WORLD_PID_TEMPERATURE 	=25, 
	
	WORLD_PID_DI1			=30,
	WORLD_PID_DI2			=31,
	WORLD_PID_DI3			=32,
	WORLD_PID_DI4			=33,
	
	WORLD_PID_INVALID_UPLOAD_PTR	=254,
	WORLD_PID_RESTART		=255,
	
};
*/
enum ErrorCode
{	PASSWORD_ERROR = 0,
	COMMAND_ERROR = 1,
	ACCESS_RIGHT_ERROR = 2,
	PARAMETER_ID_ERROR = 3,
	SYNTAX_ERROR	= 4,
	READ_ONLY_WRITE_ERROR = 5,
	ILLEGAL_VALUE_ERROR = 6,
	I2C_COMM_ERROR = 7,
};
enum DataType
{	DT_STR = 0,
	DT_INT = 1,
	DT_FLOAT = 2, 
	DT_LONG = 3,
	DT_LONG_LONG = 4,
	DT_DOUBLE = 5,
};
enum ResponseCode2ServerDetails
{	RESPONSE2SERVER_UNDIFINED = 0,
	RESPONSE2SERVER_ENGINE_DISABLED	=4,
	RESPONSE2SERVER_ENGINE_ENABLED	=5,
	RESPONSE2SERVER_AC_DISABLED		=6,
	RESPONSE2SERVER_AC_ENABLED		=7,
	RESPONSE2SERVER_DISABLE_ENGINE_COMMAND_RECEIVED 	=8,
	//RESPONSE2SERVER_DOOR_DISABLED	=9,
	//RESPONSE2SERVER_DOOR_ENABLED	=10,
	//RESPONSE2SERVER_ALARM_DISABLED	=11,
	//RESPONSE2SERVER_ALARM_ENABLED	=12,
	
};

//#define PID_STR_CALL		"MISSCALL"
#define PID_STR_TIME		"TIME"
#define PID_STR_EXT_BATTERY	"EXTBAT"
#define PID_STR_AI1			"AI1"
#define PID_STR_DI1			"DI1"	//IGNITION & DI1 IS THE SAME
#define PID_STR_DI1			"DI1"
#define PID_STR_DI2			"DI2"
#define PID_STR_DI3			"DI3"
#define PID_STR_DI4			"DI4"
//OUTPUT ALLOCATION LIST
#define PID_OUTPUT_ALLOCATION_STR_NONE		"NONE"
#define PID_OUTPUT_ALLOCATION_STR_CALL		"CALL"

//UNIT

#define UNIT_STR_NONE 	"NONE"
#define UNIT_STR_MTR	"MTR"
#define UNIT_STR_KM 	"KM"
#define UNIT_STR_MILE	"MILE"
#define UNIT_STR_MPS	"MPS"
#define UNIT_STR_KMPH	"KMPH"
#define UNIT_STR_MPH	"MPH"
#define UNIT_STR_KNOTS	"KNOTS"
#define UNIT_STR_SEC	"SEC"
#define UNIT_STR_MIN	"MIN"
#define UNIT_STR_HR		"HR"
#define UNIT_STR_DAYS	"DAYS"
#define UNIT_STR_MV		"MV"
#define UNIT_STR_VOLT	"VOLT"
#define UNIT_STR_DEG	"DEG"
#define UNIT_STR_DEG_C	"DEG_C"
#define UNIT_STR_DEG_F	"DEG_F"
#define UNIT_STR_PERCENTAGE "%"

enum unit
{	UNIT_NONE,			//=0,
	UNIT_MTR,			//=1,
	UNIT_KM,			//=2,
	UNIT_MILE,			//=3,
	UNIT_MPS,			//=4,
	UNIT_KMPH,			//=5,
	UNIT_MPH,			//=6,
	UNIT_KNOTS,			//=7,
	UNIT_SEC,			//=8,
	UNIT_MIN,			//=9,
	UNIT_HR,			//=10,
	UNIT_DAYS,			//=11,
	UNIT_MV,			//=12,
	UNIT_VOLT,			//=13,
	UNIT_DEG,			//=14,//ANGLE DEGREE
	UNIT_DEG_C,			//=15,//DEGREE CENTIGRADE
	UNIT_DEG_F,			//=16,//DEGREE FARENHITE
	UNIT_PERCENTAGE,	//=17,
	UNIT_MAX_VAL		//
};

/*
#define TRIG_CONDITION_STR_NONE 				"NONE"
#define TRIG_CONDITION_STR_CHANGE_HIGH 			"LOWTOHIGH"
#define TRIG_CONDITION_STR_CHANGE_LOW 			"HIGHTOLOW"
#define TRIG_CONDITION_STR_CHANGE_BOTH 			"CHANGE"
#define TRIG_CONDITION_STR_WINDOW_OUT			"WINDOWOUT"
#define TRIG_CONDITION_STR_WINDOW_IN			"WINDOWIN"
#define TRIG_CONDITION_STR_WINDOW_IN_OR_OUT		"WINDOWINOROUT"
#define TRIG_CONDITION_STR_GREATER_THAN			">"
#define TRIG_CONDITION_STR_SMALLER_THAN			"<"
#define TRIG_CONDITION_STR_CHANGE_GREATER_THAN	"CHANGE>"
*/
enum trig_condition
{	TRIG_CONDITION_NONE,					//=0,
	TRIG_CONDITION_CHANGE_HIGH,				//=1,
	TRIG_CONDITION_CHANGE_LOW,				//=2,
	TRIG_CONDITION_CHANGE_BOTH, 			//=3,
	TRIG_CONDITION_WINDOW_IN,				//=4,
	TRIG_CONDITION_WINDOW_OUT,				//=5,
	TRIG_CONDITION_WINDOW_IN_OR_OUT,		//=6,
	TRIG_CONDITION_CHANGE_GREATER_THAN,		//=7,
	TRIG_CONDITION_GREATER_THAN,			//=8,
	TRIG_CONDITION_SMALLER_THAN,			//=9,
	
	//AND ANY CONDTION HERE
	
	TRIG_CONDITION_MAX_VAL				
};

#define TRIG_PRIORITY_STR_NORMAL	"NORMAL"
#define TRIG_PRIORITY_STR_HIGH 	"HIGH"
#define TRIG_PRIORITY_STR_PANIC	"PANIC"
enum trig_priority
{	TRIG_PRIORITY_NORMAL	=0,
  	TRIG_PRIORITY_HIGH		=1,
	TRIG_PRIORITY_PANIC		=2,
};
enum callaction
{	CALL_ACTION_NONE			=0,
	CALL_ACTION_TOGGLE_DO1		=1,
	CALL_ACTION_TOGGLE_DO2		=2,
	CALL_ACTION_TOGGLE_BOTH		=3,
	CALL_ACTION_SMS_LOCATION	=4,
};
#define CALL_ACTION_STR_NONE			"NONE"
#define CALL_ACTION_STR_TOGGLE_DO1		"DO1"
#define CALL_ACTION_STR_TOGGLE_DO2		"DO2"
#define CALL_ACTION_STR_TOGGLE_BOTH		"BOTH"
#define CALL_ACTION_STR_SMS_LOCATION	        "LOCATION"

enum fota_constants
{	FIRMWARE_AVAILABLE					=0,
	FIRMWARE_DOWNLOADING 				=1,
	FIRMWARE_UPGRADE_COMMAND_RECEIVED 	=2,
	FIRMWARE_NOT_AVAILABLE 				=255,
};
//COMMAND IDENTIFIER
#define GET_COMMAND 	0
#define SET_COMMAND 	1
#define ENABLE_COMMAND 	2
#define DISABLE_COMMAND 3
#define ON_COMMAND 		4
#define OFF_COMMAND 	5
#define DEBUG_COMMAND 	6

//COMMAND RESPONSE TYPE
#define MY_DONE 				0
#define MY_UNKNOWN_COMMAND 		1
#define MY_ILLEGAL_VALUE 		2
#define MY_RTO 					3
#define MY_UNAUTHORIZED_SENDER 	4
#define MY_PASSWORD_ERROR		5
#define MY_ERROR 				6
#define MY_GIVEN 				7
#define MY_NO_CHANGE 			8
#define MY_USER_FOUND			9
#define MY_USER_NOT_FOUND		10
#define MY_USER_TABLE_FULL		11
#define MY_CRC_ERROR			12
//ANT STATUS CONSTANTS
#define ANT_STAT_NOT_KNOWN 0
#define ANT_OK	1
#define ANT_OPEN 2
#define ANT_SHORT 3
//LIST OF DEBUG COMMANDS
#define DEBUG_TIME "DEBUGTIME"
#define DEBUG_PC_ECHO_ON "DEBUGPCECHOON"
#define DEBUG_PC_ECHO_OFF "DEBUGPCECHOOFF"
#define DEBUG_GPS_RECV_ECHO_ON "DEBUGGPSRECVECHOON"
#define DEBUG_GPS_RECV_ECHO_OFF "DEBUGGPSRECVECHOOFF"
//#define DEBUG_GPS_UART_INIT "DEBUGGPSUARTINIT"
#define DEBUG_GPS_EN_LOW "DEBUGGPSENLOW"
#define DEBUG_GPS_EN_HIGH "DEBUGGPSENHIGH"
#define DEBUG_GPS_RESET_LOW "DEBUGGPSRESETLOW"
#define DEBUG_GPS_RESET_HIGH "DEBUGGPSRESETHIGH"
#define DEBUG_GPS_NORMAL_MODE "DEBUGGPSMODEN"
#define DEBUG_GPS_PERIODIC_BACKUP_MODE "DEBUGGPSMODEPB"
#define DEBUG_GPS_PERIODIC_STANDBY_MODE "DEBUGGPSMODEPS"
#define DEBUG_GPS_PERIODIC_PERPETUAL_BACKUP_MODE "DEBUGGPSMODEPPB"
#define DEBUG_GPS_ALWAYS_LOCATE_STANDBY_MODE "DEBUGGPSMODEALS"
#define DEBUG_GPS_ALWAYS_LOCATE_BACKUP_MODE "DEBUGGPSMODEALB"
#define DEBUG_GPS_RAW_DATA_DEBUG_ON "DEBUGGPSRDDON"
#define DEBUG_GPS_RAW_DATA_DEBUG_OFF "DEBUGGPSRDDOFF"
#define DEBUG_GPS_PKT_SET "DEBUGSETGPSPACKET"
#define DEBUG_GPS_PKT_LAST_VALID_GET "DEBUGGETGPSLASTVALIDPACKET" 
#define DEBUG_GPS_PKT_CURRENT_GET "DEBUGGETGPSCURRENTPACKET"
#define DEBUG_GPRS_ECHO_ON "DEBUGGPRSECHOON"
#define DEBUG_GPRS_ECHO_OFF "DEBUGGPRSECHOOFF"
#define DEBUG_GPRS_FIREFREE "DEBUGAT"
#define DEBUG_GPRS_PWR_KEY_HIGH_COMMAND "DEBUGGPRSPWRKEYHIGH"
#define DEBUG_GPRS_PWR_KEY_LOW_COMMAND "DEBUGGPRSPWRKEYLOW"
#define DEBUG_GPRS_DTR_HIGH_COMMAND "DEBUGGPRSDTRHIGH"
#define DEBUG_GPRS_DTR_LOW_COMMAND "DEBUGGPRSDTRLOW"
#define DEBUG_GPRS_CHECK_DTR "DEBUGGETDTR"
#define DEBUG_GPS_STATIC_NAVIGATION "DEBUGGPSSTATICNAVIGATION"
#define DEBUG_GPRS_POWER_UP "DEBUGGPRSPOWERUP"
#define DEBUG_GPRS_POWER_DOWN "DEBUGGPRSPOWERDOWN"
#define DEBUG_GPRS_INIT_STAT "DEBUGGPRSSTAGE"
#define DEBUG_GPRS_SET_INIT_STAT "DEBUGSETINITSTAT"
//#define DEBUG_GPRS_UART_INIT "DEBUGGPRSUARTINIT"
#define DEBUG_GPRS_BUFFER "DEBUGGPRSBUFF"
//#define DEBUG_MEM_IDENTIFICATION "DEBUGMEMID"
//#define DEBUG_MEM_STAT "DEBUGMEMSTAT"
#define DEBUG_MEMORY_WRITE "DEBUGMW"
#define DEBUG_MEMORY_READ "DEBUGMR"
//#define DEBUG_MEM2PAGE_READ "DEBUGPR"
#define DEBUG_CHIP_ERASE "DEBUGCHIPERASE"
#define DEBUG_DATA_POINTER "DEBUGDATAPTR"
#define DEBUG_DELETE_LOG "DEBUGDELETELOG"
#define DEBUG_UPLOAD_LOGPTR "DEBUGUPLOADPTR="
#define DEBUG_LOG_PTR "DEBUGLOGPTR="
#define DEBUG_EMAIL_PTR "DEBUGEMAILPTR="
//speed conversion
#define MPS2CMPS	100
#define KNOTS2CMPS 	51.4444444443
#define KMPH2CMPS	27.7777777778
#define MPH2CMPS	44.4444444444
#define CMPS2KMPH 	0.036
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//user setpoint min max val
//setpoint id = 410.0 is TargetOutletConductivity : Pure EC Target

#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #if(CVS_CIP==CVS_CIP_NO)
        #define PURE_EC_TARGET_MIN_VAL 0
        #define PURE_EC_TARGET_DEF_VAL 150
        #define PURE_EC_TARGET_MAX_VAL 5000
    #else
        #define PURE_EC_TARGET_MIN_VAL 0
        #define PURE_EC_TARGET_DEF_VAL 50
        #define PURE_EC_TARGET_MAX_VAL 5000
    #endif
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define PURE_EC_TARGET_MIN_VAL 0
    #define PURE_EC_TARGET_DEF_VAL 150
    #define PURE_EC_TARGET_MAX_VAL 5000
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define PURE_EC_TARGET_MIN_VAL 0
    #define PURE_EC_TARGET_DEF_VAL 50
    #define PURE_EC_TARGET_MAX_VAL 5000
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.1 is PrepurifyTimeInSec
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define PREPURIFY_TIME_MIN_VAL  2
    #define PREPURIFY_TIME_DEF_VAL  10
    #define PREPURIFY_TIME_MAX_VAL  30
#else
    #define PREPURIFY_TIME_MIN_VAL  2
    #define PREPURIFY_TIME_DEF_VAL  5
    #define PREPURIFY_TIME_MAX_VAL  30
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.2 PurifyTimeInSec
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define PURIFY_TIME_MIN_VAL 10
    #define PURIFY_TIME_DEF_VAL 80
    #define PURIFY_TIME_MAX_VAL 500
#else
    #define PURIFY_TIME_MIN_VAL 10
    #define PURIFY_TIME_DEF_VAL 110
    #define PURIFY_TIME_MAX_VAL 500
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.3 WasteTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define WASTE_TIME_MIN_VAL 10
    #define WASTE_TIME_DEF_VAL 75
    #define WASTE_TIME_MAX_VAL 500
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define WASTE_TIME_MIN_VAL 10
    #define WASTE_TIME_DEF_VAL 90
    #define WASTE_TIME_MAX_VAL 500
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define WASTE_TIME_MIN_VAL 10
    #define WASTE_TIME_DEF_VAL 79
    #define WASTE_TIME_MAX_VAL 500
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define WASTE_TIME_MIN_VAL 10
    #define WASTE_TIME_DEF_VAL 79
    #define WASTE_TIME_MAX_VAL 500
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.4 HF_WasteTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define HF_WASTE_TIME_MIN_VAL   0
    #define HF_WASTE_TIME_DEF_VAL   5
    #define HF_WASTE_TIME_MAX_VAL   500
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define HF_WASTE_TIME_MIN_VAL   0
    #define HF_WASTE_TIME_DEF_VAL   5
    #define HF_WASTE_TIME_MAX_VAL   500
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define HF_WASTE_TIME_MIN_VAL   0
    #define HF_WASTE_TIME_DEF_VAL   1
    #define HF_WASTE_TIME_MAX_VAL   500
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define HF_WASTE_TIME_MIN_VAL   0
    #define HF_WASTE_TIME_DEF_VAL   1
    #define HF_WASTE_TIME_MAX_VAL   500
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.5 CIP_DosingOnTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_DOSE_ON_TIME_MIN_VAL 1
    #define CIP_DOSE_ON_TIME_DEF_VAL 6
    #define CIP_DOSE_ON_TIME_MAX_VAL 200
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_DOSE_ON_TIME_MIN_VAL 1
    #define CIP_DOSE_ON_TIME_DEF_VAL 6
    #define CIP_DOSE_ON_TIME_MAX_VAL 200
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define CIP_DOSE_ON_TIME_MIN_VAL 1
    #define CIP_DOSE_ON_TIME_DEF_VAL 110
    #define CIP_DOSE_ON_TIME_MAX_VAL 2000
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define CIP_DOSE_ON_TIME_MIN_VAL 1
    #define CIP_DOSE_ON_TIME_DEF_VAL 110
    #define CIP_DOSE_ON_TIME_MAX_VAL 2000
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.6 CIP_DosingOffTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_DOSE_OFF_TIME_MIN_VAL 1
    #define CIP_DOSE_OFF_TIME_DEF_VAL 7
    #define CIP_DOSE_OFF_TIME_MAX_VAL 200
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_DOSE_OFF_TIME_MIN_VAL 1
    #define CIP_DOSE_OFF_TIME_DEF_VAL 7
    #define CIP_DOSE_OFF_TIME_MAX_VAL 200
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY) //FOR DIENTRY  this is also called as CIP RECIRCULATE
    #define CIP_DOSE_OFF_TIME_MIN_VAL 1
    #define CIP_DOSE_OFF_TIME_DEF_VAL 900
    #define CIP_DOSE_OFF_TIME_MAX_VAL 2000
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES) //FOR PRODUCT_DISERIES  this is also called as CIP RECIRCULATE
    #define CIP_DOSE_OFF_TIME_MIN_VAL 1
    #define CIP_DOSE_OFF_TIME_DEF_VAL 900
    #define CIP_DOSE_OFF_TIME_MAX_VAL 2000
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.7 CIP_DosingTotalTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_DOSE_TOTAL_TIME_MIN_VAL 1
    #define CIP_DOSE_TOTAL_TIME_DEF_VAL 78
    #define CIP_DOSE_TOTAL_TIME_MAX_VAL 200
#endif
//though there is no cip in dientry for uniformity defining it
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define CIP_DOSE_TOTAL_TIME_MIN_VAL 1
    #define CIP_DOSE_TOTAL_TIME_DEF_VAL CIP_DOSE_ON_TIME_DEF_VAL + CIP_DOSE_OFF_TIME_DEF_VAL
    #define CIP_DOSE_TOTAL_TIME_MAX_VAL 2000
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.9 CIP_PulseOnTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_PULSE_ON_TIME_MIN_VAL 1
    #define CIP_PULSE_ON_TIME_DEF_VAL 8
    #define CIP_PULSE_ON_TIME_MAX_VAL 20
#endif
//though there is no cip in dientry for uniformity defining it
#if(PRODUCT_TYPE==PRODUCT_DIENTRY) //FOR DIENTRY  this is also called as CIP FLUSH. LCD=CIP FLUSH
    #define CIP_PULSE_ON_TIME_MIN_VAL 0
    #define CIP_PULSE_ON_TIME_DEF_VAL 0
    #define CIP_PULSE_ON_TIME_MAX_VAL 240
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.10 CIP_PulseOffTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_PULSE_OFF_TIME_MIN_VAL 1
    #define CIP_PULSE_OFF_TIME_DEF_VAL 18
    #define CIP_PULSE_OFF_TIME_MAX_VAL 60
#endif
//though there is no cip in dientry for uniformity defining it
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define CIP_PULSE_OFF_TIME_MIN_VAL 0
    #define CIP_PULSE_OFF_TIME_DEF_VAL 0
    #define CIP_PULSE_OFF_TIME_MAX_VAL 60
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.8 CIP_PulseTotalTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_PULSE_TOTAL_TIME_MIN_VAL 300
    #define CIP_PULSE_TOTAL_TIME_DEF_VAL 1800
    #define CIP_PULSE_TOTAL_TIME_MAX_VAL 7200
#endif
//though there is no cip in dientry for uniformity defining it
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define CIP_PULSE_TOTAL_TIME_MIN_VAL 0
    #define CIP_PULSE_TOTAL_TIME_DEF_VAL 0
    #define CIP_PULSE_TOTAL_TIME_MAX_VAL 7200
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.11 CIP_FlushTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define CIP_FLUSH_TIME_MIN_VAL 50
    #define CIP_FLUSH_TIME_DEF_VAL 480
    #define CIP_FLUSH_TIME_MAX_VAL 3600
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define CIP_FLUSH_TIME_MIN_VAL 50
    #define CIP_FLUSH_TIME_DEF_VAL 480
    #define CIP_FLUSH_TIME_MAX_VAL 3600
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define CIP_FLUSH_TIME_MIN_VAL 50
    #define CIP_FLUSH_TIME_DEF_VAL 180
    #define CIP_FLUSH_TIME_MAX_VAL 3600
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define CIP_FLUSH_TIME_MIN_VAL 50
    #define CIP_FLUSH_TIME_DEF_VAL 180
    #define CIP_FLUSH_TIME_MAX_VAL 3600
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.12 WV_CheckTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define WV_CHECK_TIME_MIN_VAL 10
    #define WV_CHECK_TIME_DEF_VAL 40
    #define WV_CHECK_TIME_MAX_VAL 60
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define WV_CHECK_TIME_MIN_VAL 10
    #define WV_CHECK_TIME_DEF_VAL 40
    #define WV_CHECK_TIME_MAX_VAL 60
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define WV_CHECK_TIME_MIN_VAL 10
    #define WV_CHECK_TIME_DEF_VAL 40
    #define WV_CHECK_TIME_MAX_VAL 80
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define WV_CHECK_TIME_MIN_VAL 10
    #define WV_CHECK_TIME_DEF_VAL 40
    #define WV_CHECK_TIME_MAX_VAL 80
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.13 WaitHT_TimeInSec
#define WAIT_HT_TIME_MIN_VAL 30
#define WAIT_HT_TIME_DEF_VAL 600
#define WAIT_HT_TIME_MAX_VAL 3600
//------------------------------------------------------------------------------
//setpoint id = 410.14 TargetPureFlowLperM * 10 TO OMIT DECIMAL POINT i.e 100 = 10.0 & 900 = 90.0
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define TARGET_PURE_FLOW_MIN_VAL 0
    #define TARGET_PURE_FLOW_DEF_VAL 26
    #define TARGET_PURE_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define TARGET_PURE_FLOW_MIN_VAL 0
    #define TARGET_PURE_FLOW_DEF_VAL 26
    #define TARGET_PURE_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define TARGET_PURE_FLOW_MIN_VAL 0
    #define TARGET_PURE_FLOW_DEF_VAL 53
    #define TARGET_PURE_FLOW_MAX_VAL 900
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define TARGET_PURE_FLOW_MIN_VAL 0
    #define TARGET_PURE_FLOW_DEF_VAL 55
    #define TARGET_PURE_FLOW_MAX_VAL 900
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.15 LowFlowPurifyAlarmSetpoint * 10 TO OMIT DECIMAL POINT i.e 100 = 10.0 
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define PURIFY_LOW_FLOW_MIN_VAL 0
    #define PURIFY_LOW_FLOW_DEF_VAL 10  //0   //10
    #define PURIFY_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define PURIFY_LOW_FLOW_MIN_VAL 0
    #define PURIFY_LOW_FLOW_DEF_VAL 0   //10
    #define PURIFY_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define PURIFY_LOW_FLOW_MIN_VAL 0
    #define PURIFY_LOW_FLOW_DEF_VAL 30  //0   //30
    #define PURIFY_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define PURIFY_LOW_FLOW_MIN_VAL 0
    #define PURIFY_LOW_FLOW_DEF_VAL 0   //30
    #define PURIFY_LOW_FLOW_MAX_VAL 100
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.16 LowFlowWasteAlarmSetpoint * 10 TO OMIT DECIMAL POINT i.e 100 = 10.0 
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define WASTE_LOW_FLOW_MIN_VAL 0
    #define WASTE_LOW_FLOW_DEF_VAL 5//0    //3
    #define WASTE_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define WASTE_LOW_FLOW_MIN_VAL 0
    #define WASTE_LOW_FLOW_DEF_VAL 0    //3
    #define WASTE_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define WASTE_LOW_FLOW_MIN_VAL 0
    #define WASTE_LOW_FLOW_DEF_VAL 10//0    //10
    #define WASTE_LOW_FLOW_MAX_VAL 100
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define WASTE_LOW_FLOW_MIN_VAL 0
    #define WASTE_LOW_FLOW_DEF_VAL 0    //10
    #define WASTE_LOW_FLOW_MAX_VAL 100
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.17 MaxCycleCountForCIP
#if(CVS_CIP==CVS_CIP_NO)
    #define MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL 0   //change requested by guillermo previously its 500
    #define MAX_CYCLE_COUNT_FOR_CIP_DEF_VAL 500
    #define MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL 2000
#else
    #define MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL 1
    #define MAX_CYCLE_COUNT_FOR_CIP_DEF_VAL 432
    #define MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL 5000
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.18 CabinetHighTemperatureSetVal
#define CABINET_HIGH_TEMP_MIN_VAL 20
#define CABINET_HIGH_TEMP_DEF_VAL 40
#define CABINET_HIGH_TEMP_MAX_VAL 60
//------------------------------------------------------------------------------
//setpoint id = 410.19 MAX_CIP_PumpRunTimeInSec
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define MAX_CIP_PUMP_RUN_TIME_MIN_VAL 5
    #define MAX_CIP_PUMP_RUN_TIME_DEF_VAL 1000
    #define MAX_CIP_PUMP_RUN_TIME_MAX_VAL 2500
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define MAX_CIP_PUMP_RUN_TIME_MIN_VAL 5
    #define MAX_CIP_PUMP_RUN_TIME_DEF_VAL 1000
    #define MAX_CIP_PUMP_RUN_TIME_MAX_VAL 2500
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define MAX_CIP_PUMP_RUN_TIME_MIN_VAL 5
    #define MAX_CIP_PUMP_RUN_TIME_DEF_VAL 2600
    #define MAX_CIP_PUMP_RUN_TIME_MAX_VAL 4000
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define MAX_CIP_PUMP_RUN_TIME_MIN_VAL 5
    #define MAX_CIP_PUMP_RUN_TIME_DEF_VAL 2600
    #define MAX_CIP_PUMP_RUN_TIME_MAX_VAL 4000
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.20 AnaOutProportionalFactor is * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#define ANA_OUT_P_FACTOR_MIN_VAL 25
#define ANA_OUT_P_FACTOR_DEF_VAL 40
#define ANA_OUT_P_FACTOR_MAX_VAL 200
//------------------------------------------------------------------------------
//setpoint id = 410.21 DynaProportionalFactor is * 100 TO OMIT DECIMAL POINT i.e 999 = 9.99
#define DYNA_P_FACTOR_MIN_VAL 0
#define DYNA_P_FACTOR_DEF_VAL 5
#define DYNA_P_FACTOR_MAX_VAL 999
//------------------------------------------------------------------------------
//setpoint id = 410.22 PAE_PureStepVoltageSetpoint is * 10 TO OMIT DECIMAL POINT i.e 120 = 12.0 VOLT 
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define PAE_PURE_STEP_VOLT_MIN_VAL 0
    #define PAE_PURE_STEP_VOLT_DEF_VAL 36   //3.6
    #define PAE_PURE_STEP_VOLT_MAX_VAL 120
#else
    #define PAE_PURE_STEP_VOLT_MIN_VAL 0
    #define PAE_PURE_STEP_VOLT_DEF_VAL 19   //17   //14=1.4
    #define PAE_PURE_STEP_VOLT_MAX_VAL 120
#endif

//------------------------------------------------------------------------------
//setpoint id = 410.23 PAE_WasteStepVoltageSetpoint is * 10 TO OMIT DECIMAL POINT i.e 120 = 12.0 VOLT 
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define PAE_WASTE_STEP_VOLT_MIN_VAL 0
    #define PAE_WASTE_STEP_VOLT_DEF_VAL 36  //3.6
    #define PAE_WASTE_STEP_VOLT_MAX_VAL 120
#else
    #define PAE_WASTE_STEP_VOLT_MIN_VAL 0
    #define PAE_WASTE_STEP_VOLT_DEF_VAL 19  //14  //1.4
    #define PAE_WASTE_STEP_VOLT_MAX_VAL 120
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.24 CurrentProportionalFactor is * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#define CURRENT_P_FACTOR_MIN_VAL 50
#define CURRENT_P_FACTOR_DEF_VAL 105
#define CURRENT_P_FACTOR_MAX_VAL 200
//------------------------------------------------------------------------------
//setpoint id = 410.25 FlowMeter.K_Factor
#define FLOWMETER_K_FACTOR_MIN_VAL 10
#define FLOWMETER_K_FACTOR_DEF_VAL 1400
#define FLOWMETER_K_FACTOR_MAX_VAL 2000
//------------------------------------------------------------------------------
//setpoint id = 410.26 VolumeUnit
#define LITER_TO_GALLON 0.2642007926
#define VOLUME_UNIT_LITTER   0
#define VOLUME_UNIT_GALLON  1
#define VOLUME_UNIT_MIN_VAL VOLUME_UNIT_LITTER
#define VOLUME_UNIT_DEF_VAL VOLUME_UNIT_GALLON
#define VOLUME_UNIT_MAX_VAL VOLUME_UNIT_GALLON
//------------------------------------------------------------------------------
//setpoint id = 410.27 BypassOption
#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
    enum bypass_option
    {   BYPASS_INTERNAL,
        BYPASS_EXTERNAL_NC,
        BYPASS_EXTERNAL_NO,
    };
    #define BYPASS_OPTION_MIN_VAL BYPASS_INTERNAL
    #define BYPASS_OPTION_DEF_VAL BYPASS_EXTERNAL_NC
    #define BYPASS_OPTION_MAX_VAL BYPASS_EXTERNAL_NO
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.28 is AnalogPressureStartLevel * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    //#if(CVS_CIP==CVS_CIP_NO)
        #define START_PRESSURE_MIN_VAL 50
        #define START_PRESSURE_DEF_VAL 350//400
        #define START_PRESSURE_MAX_VAL 590
    /*#else
        #define START_PRESSURE_MIN_VAL 50
        #define START_PRESSURE_DEF_VAL 350
        #define START_PRESSURE_MAX_VAL 590
    #endif*/
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    //#if(CVS_CIP==CVS_CIP_NO)
        #define START_PRESSURE_MIN_VAL 50
        #define START_PRESSURE_DEF_VAL 350//400
        #define START_PRESSURE_MAX_VAL 590
    /*#else
        #define START_PRESSURE_MIN_VAL 50
        #define START_PRESSURE_DEF_VAL 350
        #define START_PRESSURE_MAX_VAL 590
    #endif*/
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
    #define START_PRESSURE_MIN_VAL 50
    #define START_PRESSURE_DEF_VAL 350//400
    #define START_PRESSURE_MAX_VAL 590
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.29 is AnalogPressureStopLevel * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    //#if(CVS_CIP==CVS_CIP_NO)
        #define STOP_PRESSURE_MIN_VAL 50
        #define STOP_PRESSURE_DEF_VAL 520//480
        #define STOP_PRESSURE_MAX_VAL 590
    /*#else
        #define STOP_PRESSURE_MIN_VAL 50
        #define STOP_PRESSURE_DEF_VAL 550
        #define STOP_PRESSURE_MAX_VAL 590
    #endif*/
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    //#if(CVS_CIP==CVS_CIP_NO)
        #define STOP_PRESSURE_MIN_VAL 50
        #define STOP_PRESSURE_DEF_VAL 420//480
        #define STOP_PRESSURE_MAX_VAL 590
    /*#else
        #define STOP_PRESSURE_MIN_VAL 50
        #define STOP_PRESSURE_DEF_VAL 550
        #define STOP_PRESSURE_MAX_VAL 590
    #endif*/
#endif  
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define STOP_PRESSURE_MIN_VAL 50
    #define STOP_PRESSURE_DEF_VAL 420//480
    #define STOP_PRESSURE_MAX_VAL 590
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define STOP_PRESSURE_MIN_VAL 50
    #define STOP_PRESSURE_DEF_VAL 420//480
    #define STOP_PRESSURE_MAX_VAL 590
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.30 is AnalogPressureBypassLevel * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#if(PRODUCT_TYPE==PRODUCT_DIUSE)
    #define BYPASS_PRESSURE_DEF_VAL 300//350
    #define BYPASS_PRESSURE_MAX_VAL 590
#endif
#if(PRODUCT_TYPE==PRODUCT_CFO)
    #define BYPASS_PRESSURE_DEF_VAL 300//350
    #define BYPASS_PRESSURE_MAX_VAL 590
#endif
#if(PRODUCT_TYPE==PRODUCT_DIENTRY)
    #define BYPASS_PRESSURE_DEF_VAL 300//350
    #define BYPASS_PRESSURE_MAX_VAL 590
#endif
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define BYPASS_PRESSURE_DEF_VAL 300//350
    #define BYPASS_PRESSURE_MAX_VAL 590
#endif
//------------------------------------------------------------------------------
//setpoint id = 410.31 is AnalogPressureCipLevel * 100 TO OMIT DECIMAL POINT i.e 25 = 0.25 200 = 2.00
#define CIP_PRESSURE_MIN_VAL 50
#define CIP_PRESSURE_DEF_VAL 420
#define CIP_PRESSURE_MAX_VAL 590
//------------------------------------------------------------------------------
//setpoint id = 410.32 is CIP_WaitTimeInSec
#define CIP_WAIT_TIME_MIN_VAL 0
#define CIP_WAIT_TIME_DEF_VAL 3600
#define CIP_WAIT_TIME_MAX_VAL 9999
//------------------------------------------------------------------------------
//setpoint id = 410.33 is WaitTimebB4BypassAlarmInSec
#define WAIT_TIME_B4_BYPASS_ALARM_MIN_VAL 0
#define WAIT_TIME_B4_BYPASS_ALARM_DEF_VAL 900
#define WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL 9999
//------------------------------------------------------------------------------
//setpoint id = 410.34 is CIP_LowerLimitCycle
#define CIP_LOWER_LIMIT_CYCLE_MIN_VAL 50
#define CIP_LOWER_LIMIT_CYCLE_DEF_VAL 240
#define CIP_LOWER_LIMIT_CYCLE_MAX_VAL 1000
//------------------------------------------------------------------------------
//setpoint id = 410.35 is CIP_FavorableTime
#define CIP_FAVORABLE_TIME_MIN_VAL 0
#define CIP_FAVORABLE_TIME_DEF_VAL 700
#define CIP_FAVORABLE_TIME_MAX_VAL 2359
//------------------------------------------------------------------------------
//setpoint id = 410.36 is CIP_FavorableTimeGraceMin
#define CIP_FAVORABLE_TIME_GRACE_MINUTES_MIN_VAL 5
#define CIP_FAVORABLE_TIME_GRACE_MINUTES_DEF_VAL 15
#define CIP_FAVORABLE_TIME_GRACE_MINUTES_MAX_VAL 30
//------------------------------------------------------------------------------
//setpoint id = 410.39 PAE_PrepurifyStepVoltageSetpoint is * 10 TO OMIT DECIMAL POINT i.e 120 = 12.0 VOLT 
#if(PRODUCT_TYPE==PRODUCT_DISERIES)
    #define PAE_PREPURIFY_STEP_VOLT_MIN_VAL 0
    #define PAE_PREPURIFY_STEP_VOLT_DEF_VAL 36   //3.6
    #define PAE_PREPURIFY_STEP_VOLT_MAX_VAL 120
#else
    #define PAE_PREPURIFY_STEP_VOLT_MIN_VAL 0
    #define PAE_PREPURIFY_STEP_VOLT_DEF_VAL 17   //17   //14=1.4
    #define PAE_PREPURIFY_STEP_VOLT_MAX_VAL 120
#endif
//setpoint id = 410.42 is dynamic mode on  
#define DYNAMIC_CAPDI_MODE_ON 0
#define DYNAMIC_CAPDI_MODE_DEF_VAL 0
#define DYNAMIC_CAPDI_MODE_OFF 1
    
#define SHUNT_PRE_CHARGE_TIME_MIN_VAL 1
#define SHUNT_PRE_CHARGE_TIME_DEF_VAL 5
#define SHUNT_PRE_CHARGE_TIME_MAX_VAL 500

#define SHUNT_POST_CHARGE_TIME_MIN_VAL 1
#define SHUNT_POST_CHARGE_TIME_DEF_VAL 5
#define SHUNT_POST_CHARGE_TIME_MAX_VAL 500
    
#define PRE_PURE_CURRENT_MIN_VAL 10
#define PRE_PURE_CURRENT_DEF_VAL 30
#define PRE_PURE_CURRENT_MAX_VAL 120
    
#define PURE_CURRENT_MIN_VAL 10
#define PURE_CURRENT_DEF_VAL 35
#define PURE_CURRENT_MAX_VAL 120
    
#define PAE_MANUAL_CURRENT_MODE_ON 0
#define PAE_MANUAL_CURRENT_MODE_DEF_VAL 0
#define PAE_MANUAL_CURRENT_MODE_OFF 1
    
#define PAE_MANUAL_CURRENT_MIN_VAL 0
#define PAE_MANUAL_CURRENT_DEF_VAL 0
#define PAE_MANUAL_CURRENT_MAX_VAL 120  

#define PUMP_MANUAL_MODE_ON 1
#define PUMP_MANUAL_MODE_DEF_VAL 0
#define PUMP_MANUAL_MODE_OFF 0
    
#define PUMP_MANUAL_SPEED_MIN_VAL 0
#define PUMP_MANUAL_SPEED_DEF_VAL 0
#define PUMP_MANUAL_SPEED_MAX_VAL 100 

#define WASTE_CURRENT_MIN_VAL 10
#define WASTE_CURRENT_DEF_VAL 80
#define WASTE_CURRENT_MAX_VAL 120