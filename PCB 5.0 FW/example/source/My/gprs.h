#ifndef __GPRS_H__
#define __GPRS_H__

#include "common.h"
#include "mmu.h"
#include <time.h>

#define SERVER_CONNECTION_CLOSE_TIME 60	//this is the max sec the server will be in open connection state
#define SERVER_RESPONSE_TIME 10	//once the pkt is send, server should respond within this sec else connection will be closed
#define SERVER_COMMAND_TIME 2	//Once the response is received, server can send any command within this sec else connection will be closed
#define CLOSING_CONNECTION_TIMEOUT_TIME 10	//within this sec connection should be close
#define OPENING_CONNECTION_TIMEOUT_TIME 10	//within this sec connection should be open
#define EMAIL_RESPONSE_TIMEOUT_TIME	300		//within this sec +QSMTPPUT response should come
#define GPRS_SLEEP_SEC 840					//once sleep active, gprs will wake up only after this much sec

//GPRS CONTROL REGISTER
#define CALL_CONTROL 		0x01
#define SMS_CONTROL 		0x02
#define GPRS_CONTROL 		0x04
#define EMAIL_CONTROL		0X08
#define MOBILE_DEEP_SLEEP_CONTROL	0X10

/*enum gprs_stage
{	GPRS_STAGE_SLEEP,						//0
  	GPRS_STAGE_SHUT_DOWN_SEQ_1,				//1
	GPRS_STAGE_SHUT_DOWN_SEQ_2,				//2
	GPRS_STAGE_POWER_UP_SEQ_1,				//3
	GPRS_STAGE_CALL_READY,					//4
	GPRS_STAGE_BAUD_RATE,					//5
	GPRS_STAGE_IMEI,						//6 
	GPRS_STAGE_MSG_FORMAT,					//7
	GPRS_STAGE_CALLER_ID,					//8
	GPRS_STAGE_PHONE_FUNCTIONALITY,			//9
	GPRS_STAGE_SIM_PIN,						//10
	GPRS_STAGE_NETWORK_REGISTRATION,		//11
	GPRS_STAGE_CLOSE_CONNECTION,			//12
	GPRS_STAGE_DEACTIVATE,					//13
	GPRS_STAGE_OPERATING_MODE,				//14
	GPRS_MULTIPLE_SESSION,					//15
	GPRS_STAGE_TRANSFER_MODE,				//16
	GPRS_STAGE_BEARER,						//17
	GPRS_STAGE_APN,							//18
	GPRS_STAGE_ACTIVATE,					//19
	GPRS_STAGE_LOCAL_IP,					//20
	GPRS_STAGE_EMAIL_USERNAME,				//21
	GPRS_STAGE_EMAIL_PASSWORD,				//22
	GPRS_STAGE_EMAIL_CONTEXT,				//23
	GPRS_STAGE_EMAIL_SMTP_NAME,				//24
	GPRS_STAGE_EMAIL_SMTP_ADDRESS,			//25
	GPRS_STAGE_EMAIL_SMTP_SERVER,			//26
	GPRS_STAGE_EMAIL_SMTP_DESTINATION,		//27
	GPRS_STAGE_EMAIL_SMTP_SUBJECT,			//28
	GPRS_STAGE_CUSD,						//29
	GPRS_STAGE_READY_TO_OPEN_CONNECTION,	//30
	GPRS_STAGE_OPENING_CONNECTION,			//31
	GPRS_STAGE_CONNECTION_OPEN,				//32
	GPRS_STAGE_WAITING_SERVER_RESPONSE,		//33
	GPRS_STAGE_CLOSING_CONNECTION,			//34
	GPRS_STAGE_EMAIL_SMTP_BODY,				//35
	GPRS_STAGE_SEND_EMAIL,					//36
	GPRS_STAGE_SENDING_EMAIL,				//37
	GPRS_STAGE_WAITING_EMAIL_BODY_RESPONSE,	//38
	GPRS_STAGE_FTP_CONNECTION, 				//39
	GPRS_STAGE_FTP_PASS,					//40
	GPRS_STAGE_FTP_PATH,					//41
	GPRS_STAGE_FTP_OPEN,					//42
	GPRS_STAGE_FTP_CFG,						//43
	GPRS_STAGE_FTP_GET,						//44
	GPRS_STAGE_FTP_GET_WAIT,				//45
	GPRS_STAGE_WAIT		
};*/
/*
#define MOBILE_STAGE_SLEEP 								0
#define MOBILE_STAGE_CHECK_MSG_B4_SHUTDOWN				1
#define MOBILE_STAGE_SHUT_DOWN_SEQ_1 					2
#define MOBILE_STAGE_SHUT_DOWN_SEQ_2 					3
#define MOBILE_STAGE_POWER_UP_SEQ_1 					4
#define MOBILE_STAGE_CHECK_AT_RESPONSE 					5
#define MOBILE_STAGE_ECHO_OFF 							6
#define MOBILE_STAGE_BAUD_RATE 							7
#define MOBILE_STAGE_SAVE_BAUDRATE 						8
#define MOBILE_STAGE_IMEI 								9
#define MOBILE_STAGE_MSG_INIT 							10
#define MOBILE_STAGE_MSG_FORMAT 						11
#define MOBILE_STAGE_MSG_INDICATION_OFF 				12
#define MOBILE_STAGE_MSG_SMS_TEXT_MODE_PARAMETERS		13
#define MOBILE_STAGE_CALLER_ID 							14
#define MOBILE_STAGE_CALLER_ID_FORMAT 					15
#define MOBILE_STAGE_CALLER_ID_FORMAT_SAVE 				16
#define MOBILE_STAGE_CHECK_PHONE_FUNCTIONALITY 			17
#define MOBILE_STAGE_CHECK_SIM_PIN 						18
#define MOBILE_STAGE_SWITCH2_MIN_PHONE_FUNCTIONALITY 	19
#define MOBILE_STAGE_SWITCH2_FULL_PHONE_FUNCTIONALITY 	20
#define MOBILE_STAGE_CHECK_CALL_READY 					21
#define MOBILE_STAGE_SET_NETWORK_REG_WITH_CODE 			22
#define MOBILE_STAGE_CHECK_NETWORK_REG 					23
#define MOBILE_STAGE_CLOSE_CONNECTION_INIT				24
#define MOBILE_STAGE_CLOSE_CONNECTION_OPEN				25 
#define MOBILE_STAGE_DEACTIVATE 						26
#define MOBILE_STAGE_RTC_INIT 							27
#define MOBILE_STAGE_SUBSCRIBER_IDENTITY				28 
#define MOBILE_STAGE_OPERATING_MODE 					29
#define MOBILE_STAGE_CHECK_TRANSFER_MODE 				30
#define MOBILE_STAGE_SET_DATA_TRANSFER_MODE 			31
#define MOBILE_STAGE_FORGROUND_CONTEXT 					32
#define MOBILE_STAGE_BEARER 							33
#define MOBILE_STAGE_START_TCPIP_TASK 					34
#define MOBILE_STAGE_ACTIVATE 							35
#define MOBILE_STAGE_LOCAL_IP 							36
#define MOBILE_STAGE_CUSD 								37
#define MOBILE_STAGE_EMAIL_USERNAME 					38
#define MOBILE_STAGE_EMAIL_PASSWORD 					39
#define MOBILE_STAGE_EMAIL_SENDER_NAME 					40
#define MOBILE_STAGE_EMAIL_SMTP_ADDRESS 				41
#define MOBILE_STAGE_EMAIL_SMTP_SERVER 					42
#define MOBILE_STAGE_EMAIL_SMTP_DESTINATION 			43
#define MOBILE_STAGE_READY_TO_OPEN_CONNECTION 			44
#define MOBILE_STAGE_SIGNAL_STRENGTH_CHECK_SERVER 		45
#define MOBILE_STAGE_SIGNAL_STRENGTH_CHECK_EMAIL_TRACKING 46
#define MOBILE_STAGE_SIGNAL_STRENGTH_CHECK_EMAIL_BALANCE 47
#define MOBILE_STAGE_SIGNAL_STRENGTH_CHECK_EMAIL_SMS 	48
#define MOBILE_STAGE_SIGNAL_STRENGTH_CHECK              86
#define MOBILE_STAGE_DOMAIN_IP_CHECK 					49
#define MOBILE_STAGE_CONNECT_SERVER_WITH_IP 			50
#define MOBILE_STAGE_CONNECT_SERVER_WITH_DOMAIN 		51
#define MOBILE_STAGE_OPENING_SERVER_CONNECTION 			52
#define MOBILE_STAGE_WAITING_SERVER_CONNECTION 			53
#define MOBILE_STAGE_CONNECTION_OPEN 					54
#define MOBILE_STAGE_WAITING_SERVER_RESPONSE 			55
#define MOBILE_STAGE_SERVER_CONNECTION_STATUS_CLOSE		56
#define MOBILE_STAGE_SERVER_CONNECTION_STATUS_OPEN		57
#define MOBILE_STAGE_EMAIL_SMTP_BODY 					58
#define MOBILE_STAGE_EMAIL_SMTP_SUBJECT_TRACKING 		59
#define MOBILE_STAGE_EMAIL_SMTP_SUBJECT_BALANCE 		60
#define MOBILE_STAGE_EMAIL_SMTP_SUBJECT_SMS 			61
#define MOBILE_STAGE_EMAIL_SMTP_BODY_TRACKING 			62
#define MOBILE_STAGE_EMAIL_SMTP_BODY_BALANCE 			63
#define MOBILE_STAGE_EMAIL_SMTP_BODY_SMS 				64
#define MOBILE_STAGE_EMAIL_SMTP_BODY_TRACKING_DATA 		65
#define MOBILE_STAGE_EMAIL_SMTP_BODY_BALANCE_DATA 		66
#define MOBILE_STAGE_EMAIL_SMTP_BODY_SMS_DATA 			67
#define MOBILE_STAGE_EMAIL_SMTP_BODY_TRACKING_DATA_RESPONSE 68
#define MOBILE_STAGE_EMAIL_SMTP_BODY_BALANCE_DATA_RESPONSE 69
#define MOBILE_STAGE_EMAIL_SMTP_BODY_SMS_DATA_RESPONSE 	70
#define MOBILE_STAGE_SEND_EMAIL_TRACKING 				71
#define MOBILE_STAGE_SEND_EMAIL_BALANCE 				72
#define MOBILE_STAGE_SEND_EMAIL_SMS 					73
#define MOBILE_STAGE_SEND_EMAIL_TRACKING_RESPONSE_WAIT 	74
#define MOBILE_STAGE_SEND_EMAIL_BALANCE_RESPONSE_WAIT 	75
#define MOBILE_STAGE_SEND_EMAIL_SMS_RESPONSE_WAIT 		76
#define MOBILE_STAGE_BALANCE_CHECK 						77
#define MOBILE_STAGE_BALANCE_CHECK_RESPONSE 			78
#define MOBILE_STAGE_SMS_INQUIRY						79
#define MOBILE_STAGE_READ_SMS							80
#define MOBILE_STAGE_DELETE_READ_SMS					81
#define MOBILE_STAGE_GET_RTC 							82
#define MOBILE_STAGE_GET_CELL_LOC						83
#define MOBILE_STAGE_FTP_CONNECTION						84
#define MOBILE_STAGE_BATTERY							85
//#define MOBILE_STAGE_
*/

#define MOBILE_STAGE_POWER_UP_WAIT  200
#define MOBILE_STAGE_SWITCH_TO_ETHERNET  202
#define MOBILE_STAGE_SWITCHING_WAIT 203
#define MOBILE_STAGE_SEND_DATA 204
#define MOBILE_STAGE_RESPONSE_WAIT 205
#define MOBILE_STAGE_ANALYSE_RESPOND 206
#define MOBILE_STAGE_SWITCH_TO_WIFI 207 
#define MOBILE_STAGE_WIFI_COMMAND_MODE 208
#define MOBILE_STAGE_WIFI_AT_COMMAND 209
#define MOBILE_STAGE_WIFI_ROUTER_SSID_AND_PASSWORD 210
#define MOBILE_STAGE_WIFI_NETWORK_QUERY 211
#define MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_MAKING 212
#define MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT 213
#define MOBILE_STAGE_WIFI_ENABLE_AUTO_RECEIVE 214
#define MOBILE_STAGE_WIFI_COMMAND_TO_DATA_MODE 215
#define MOBILE_STAGE_ROUTING_DECISION_MAKING 216
#define MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_SWITCHING 217
#define MOBILE_STAGE_WIFI_DISCONNECT_ALL_CONNECTION_BEFORE_PACKET_GENERATION 218
#define MOBILE_STAGE_WIFI_WAIT_FOR_SERVER_RECONNECTION 219
#define MOBILE_STAGE_WAIT 								250

enum at_command_response
{	AT_COMMAND_RESPONSE_OK,
	AT_COMMAND_RESPONSE_CME_ERROR,
	AT_COMMAND_RESPONSE_CMS_ERROR
};
enum server_response
{	SERVER_RESPONSE_REQ_REJECTED 		=0,	//0
	SERVER_RESPONSE_ACK					=1,	//1
	SERVER_RESPONSE_CRC_ERROR			=2,	//2
	SERVER_FOTA_REQ_REJECTED			=3,	//3
	SERVER_FOTA_RESPONSE_ACK			=4, //4
	SERVER_FOTA_FILE_TRANSFER_COMPLETE 	=5,	//5
};
enum server_packet
{	SERVER_LOG_PACKET,	        //0
	SERVER_HEART_BEAT_PACKET,	//1
	SERVER_READ_WRITE_PACKET,   //2
	SERVER_FOTA_PACKET,	        //3
};
enum network
{	NETWORK_NOT_REGISTERED_NOR_SEARCHING,
	NETWORK_REGISTERED_HOME,
	NETWORK_NOT_REGISTERED_BUT_SEARCHING,
	NETWORK_REGISTERATION_DENIED,
	NETWORK_UNKNOWN,
	NETWORK_REGISTERED_ROAMING
};
enum batter_charge_status
{	NOT_CHARGING,		//0
	CHARGING,			//1
	CHARGING_COMPLETE	//2
};
struct gprspacket
{   long long Imei;
	ui Mcc;
	ui Mnc;
	ui Lac;
    ui CellID;
	uc BatteryChargingStatus;	//0=NOT_CHARGING 1=CHARGING 2=CHARGING COMPLETED
	uc BatteryPercentage;
	ui BatteryVoltage;	//in mV
    char SignalStrength;
	char Network;	//C enum network : home, roaming, others
	struct tm TimeStamp;
  	float Lat;		//current in degree through mobile tower
	float Lng;		//current in degree through mobile tower
};


//SERVER STATUS REGISTER (Sr) bit definations
struct server_profile_status_register
{
	uint8_t EN       	:1;	//0 = DISABLE 1=ENABLE 
	uint8_t CONNECTION  :1; //0=DISCONNECTED 1=CONNECTED 
	uint8_t LISTENER    :1;	//0=LISTENER OK 1=LISTENER FAIL
	//uint8_t SM3       :1;
	//uint8_t SM4       :1;
	//uint8_t SM5       :1;
	//uint8_t SM6       :1;
	//uint8_t EN        :1; 
};
//server profile status register defination
#define SERVER_PROFILE_ENABLE_DISABLE_CONTROL_LOC		0x01
#define SERVER_PROFILE_CONNECTED_DISCONNECTED_LOC 		0x02
#define SERVER_PROFILE_ATTEMPT_DONT_ATTEMPT_LOC 		0x04
#define SERVER_PROFILE_RESPONDING_NOT_RESPONDING_LOC 	0x08
//server profile status register value defination
#define SERVER_PROFILE_CONTROL_ENABLE 			0X01
#define SERVER_PROFILE_CONTROL_DISABLE			0
#define SERVER_PROFILE_CONNECTED				0X02
#define SERVER_PROFILE_DISCONNECTED				0
#define SERVER_PROFILE_ATTEMPT_CONNECTION		0
#define SERVER_PROFILE_DONT_ATTEMPT_CONNECTION	0X04
#define SERVER_PROFILE_RESPONDING				0
#define SERVER_PROFILE_NOT_RESPONDING			0X08

struct server_profile
{	//struct server_profile_status_register Sr;
	char Sr;	//status register
	char IpOrDomainName[26];
	ui Port;
	uc ServerCommProtocol;
	ui UploadPtr;
	/*
	char Apn[15];
	long long AccessNo;
	char UserName[10];
	char Password[10];
	*/
};
union SIM_IP
{   char c[4];
    int i[2];
    unsigned long int l;
};

//extern boolean_t BalanceCheckFlag;
extern boolean_t CallResponsePendingFlag;
extern long long CallResponsePendingCaller;
//extern boolean_t FtpFlag;
extern ui CallTimeOutSec;
extern uc GprsCR;
//extern ui GprsPowerWaitSec;//it counts the second elapsed after it was set zero
extern ui LastServerConnectionTime;
extern ui GprsEmailSendingResponseTimeOutTime;
extern boolean_t CountryCodeFlag;
extern boolean_t GprsDataModeFlag;
//extern boolean_t SmsCheckFlag;
extern boolean_t MobileCommandFlag;
extern boolean_t MobileDeepSleepFlag;
//----------------------------------
extern uc MobileResponseWaitSec;
extern ui MobileStageWaitSec;
//extern boolean_t GetMobileRtcTimeFlag;//Flag to indicate GPS time sync not possible from last 5 min so sync with mobile tower
//----------------------------------
extern uc ReturnToMobileStage;

extern struct gprspacket GprsPacket;
extern unsigned char MobileStage, ReturnToMobileStage;
extern uc MinRecPerConnection;
extern unsigned char PortOpenErrorCounter;
extern boolean_t CallReadyChkFlag;
extern struct server_profile Server;//,Server4Check[MAX_NO_OF_SERVER];
extern boolean_t SendMailFlag;
extern char MsgBuff[529];

extern void AnalyzeResponse();
//extern void CheckCall();
extern void CloseConnection(unsigned char profile);
extern char CheckGPRSResponse(char* response,unsigned int gstate);
//extern void GetMobileInfo();
extern void MobileRoutine();
extern void SendGPRScommand(char *command);
extern unsigned char SendSms(long long mobileno,boolean_t countrycodeflag,char* buff);
extern void SendStatusThroughSMS(long long mobileno);
extern void Upload1Pkt2Server();
extern void SendFotaCommandToServer();
extern void ServerResponseAnalysis();

#endif