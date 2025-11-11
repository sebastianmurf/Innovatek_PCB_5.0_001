#ifndef __PROJECT_CONSTANTS_H__
#define __PROJECT_CONSTANTS_H__

#define DEBUG	0
#define RELEASED 1
#define PROJECT_MODE RELEASED

#define WATCH_DOG

#define CORETIME 0
#define WATCHMAN 1
#define HIMVAT	 2
#define MANUFACTURER CORETIME 

#if(MANUFACTURER == CORETIME)
	#define DEVICE_NAME "  CORE TIME V.2500  "
	#define MANUFACTURED_BY " CORE TIME SOLUTION "
	#define WEBSITE "coretimesolution.in "
	#define PHONE_NO "Ph: 9910464351      "
	#define MOBILE_NO "Ph: 9891081133      "
#else
	#if(MANUFACTURER == WATCHMAN)
		#define DEVICE_NAME "      SR V.2500     "
		#define MANUFACTURED_BY "ELECTRONIC WATCHMAN "
		#define WEBSITE "srtechnologiesindia "
		#define PHONE_NO "Ph : 011-25051140   "
		#define MOBILE_NO "Mob: 9313941149     "
	#else
		#define DEVICE_NAME "  HIMVAT    V.2500  "
		#define MANUFACTURED_BY "Himvat Business Sol."
		#define WEBSITE " support@himvat.in  "
		#define PHONE_NO "                    "
		#define MOBILE_NO "                    "
	#endif
#endif

#define STANDARD 	0
#define MP3 		1
#define MODEL STANDARD   
#define PRODUCT_ID 0x01	//RF based bell timer system node product

//COMMUNICATION MODE
#define RF TRUE

#if(MODEL == MP3)
	#if(MANUFACTURER == WATCHMAN)
		#define VERSION_NO  "EWM-34 P1.B0.H2.F1.0"
	#else
		#define VERSION_NO  "CTS-34 P1.B0.H2.F1.0"
	#endif
#else
	#if(MODEL == STANDARD)
		#if(MANUFACTURER == WATCHMAN)
			#define VERSION_NO  "EWM-34 P1.B0.H1.F1.0"
		#else
			#define VERSION_NO  "CTS-34 P1.B0.H1.F1.0"
		#endif
	#endif
#endif
#define REVISION_NO "1.08"
#define MADE_IN     "   MADE IN INDIA    "

//COMMUNICATION MODE
#define RF TRUE

//RTC PART NO
#define DS3231 0
#define DS1307 1
#define RTC_PART_NO DS3231

//SHORT FORMS
typedef unsigned char uc;
typedef unsigned int ui;
typedef unsigned long int uli;
typedef signed long int sli;

//ACCESS RIGHT CONSTANTS
#define ACCESS_RIGHT_SUPERADMIN 255
#define ACCESS_RIGHT_ADMIN		254
#define ACCESS_RIGHT_GETSET		253
#define ACCESS_RIGHT_USER		252
#define ACCESS_RIGHT_SET		251
#define ACCESS_RIGHT_GET		250
#define ACCESS_RIGHT_DEBUG		249
#define ACCESS_RIGHT_NONE		0

#define SUPER_PASSWORD "SREE"

//project constansts
#define LOW 0
#define HIGH 1
#define MY_FALSE 0
#define MY_TRUE 1
#define OFF 0
#define ON 1

// RETURN/ERROR CODES
#define UNKNOWN_COMMAND				255
//#define NOTHING								0
#define GIVEN									1
#define DONE 									2
#define MEMORY_WRITE_ERROR		11
#define ILLEGAL_VALUE					12
#define RTC_ERROR							13

#define DIS_HLD_SEC   2
#define ERROR_MSG_DIS_HOLD_SEC 5
//KEY CONSTANTS
#define NORMAL		0
#define PASSWORD	1
#define CAPITAL		0
#define SMALL			1
#define SPECIAL		2

#define OK					0XFF
#define VALIDITY_OVER		0X55
//validity/defect type error codes
#define NONE				0
#define DISABLE_CLM			1
//CODE RECEIVED TYPES
#define PURCHASE 0
#define WARRANTY 1
#define MANUFACTURING 2
/*##################################################################################*/
/*					GENERAL CONTANTS												*/
/************************************************************************************/
/************************************************************************************/
/*	LOCATION NAME	ADDRESS		 DEFINATION     		BYTE						*/
/************************************************************************************/

/*##################################################################################*/
/*##################################################################################*/
/*					NAME OF SETTING													*/
/************************************************************************************/
/*					GENERAL MENU KEY CONTANTS												*/
/************************************************************************************/
/************************************************************************************/
/*	LOCATION NAME		ADDRESS		DEFINATION     			BYTE					*/
/************************************************************************************/

/*##################################################################################*/
/************************************************************************************/
/*	LOCATION NAME		ADDRESS		DEFINATION     			BYTE					*/
/************************************************************************************/
/*##################################################################################*/


/*##################################################################################*/
/*					NAME OF SETTING													*/
/************************************************************************************/
/*					GENERAL CONTANTS												*/
/************************************************************************************/
/************************************************************************************/
/*	LOCATION NAME		ADDRESS		DEFINATION     			BYTE					*/
/************************************************************************************/
/*##################################################################################*/


#endif
