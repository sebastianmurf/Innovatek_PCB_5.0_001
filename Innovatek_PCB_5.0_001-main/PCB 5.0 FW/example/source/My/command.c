#include<string.h>
#include<ctype.h>
#include<stdio.h>
#include<math.h>
#include "command.h"
#include "constants.c"
#include "MyUsb.h"
#include "GpsUart.h"
#include "GprsUart.h"
#include "gprs.h"
#include"VARIABLE.h"
#include"AT45DB161.h"
#include"mmu.h"
#include"myfun.h"
#include"gpio.h"
#include"DualTimer.h"
#include"adc.h"
#include"delay.h"
#include"SerialHmi.h"
#include "AtlasReplacement.h"
#include "Atlas_TC.h"
#include "PAE800.h"
#include "DS18B20.h"
#include "ppg.h"
#include "SerialPort.h"
#include "ProjectFunc.h"

//#include"DAC_MCP48FEBx1.h"
//#include"pc.h"
//JUST FOR TESTING

#include "usbconfig.h"
#include "usb.h"
#ifndef USB_DISBALE_DEVICE_FUNCTIONALITY
	#include "usbdevice.h"
	#include "UsbDeviceCdcCom.h"
#endif
#ifndef USB_DISBALE_HOST_FUNCTIONALITY
	#include "usbhost.h"
	#include "usbclasssupervisor.h"
#endif 
#include "usbdevicehw.h"
#include "usbhosthw.h"


//VARIABLE LOCAL TO THE FILE STARTS --------------------------------------------
struct commandformat CommandFormat;
uc command_comma_counter;
ui sp,dp,main_id,sub_id;
char resbuff[FLASH_PAGE_SIZE];
	
//uli HexMemPtr;
				
//VARIABLE LOCAL TO THE FILE ENDS **********************************************

//VARIABLE EXTERNAL TO THE FILE STARTS -----------------------------------------
//VARIABLE EXTERNAL TO THE FILE ENDS *******************************************

///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */

/*****************************************************************************/
void ArrangeCurrentStatus2PasteBuff()
{   strcpy(CopyBuff,Gmt2LocalTimeStamp(Device.TimeStamp));
    sprintf(PasteBuff,"\r\nMode=%d, TS=%s, Cy=%d, Step=%d, P.Sec=%d, PAE.V:=%.2f, TPV=%.2f, C.Flow=%.2f, A.EC=%.2f, Alarm=%d, WT=%.2f, CT=%.2f, P=%.2f, AV=%.2f, IP=%d, OP=%d, %%R=%d, L.EC=%.2f, PAE.I=%.2f",
			Mode,
			CopyBuff,
			CycleCounter,
			CurrentProcessStep,
			ProcessStepRunSec,
			(float)CurrentPAEvolt/100,
			TotalPureVolume,
			FlowMeter.CurrentFlow,
			(float)LastFiveCyclePureAverageConductivityAverage/100,//Till D7.3 it was (float)AtlasConductivity/100,
			AlarmRegister24Bit,
            (float)AtlasTemperature/1000,
            (float)Temperature.i/10,
            AnaPressureAval,
            (float)DAC_FB_Aval/100,
            CurDigitalInput,
            #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                DigitalOutput,
            #else //PRODUCT_DIENTRY or PRODUCT_DISERIES
                (BypassOption==BYPASS_EXTERNAL_NC)? DigitalOutput^0X02: DigitalOutput,
            #endif
            (ui)((PercentageRecovery[0]+PercentageRecovery[1]+PercentageRecovery[2]+PercentageRecovery[3]+PercentageRecovery[4])/5),
            (float)AtlasConductivity/100,
            (float)CurrentPAEcurrent/100);
}
	
void CheckAuthority(){	
	if(CommandFormat.Sender == SENDER_SERVER)
        CommandFormat.AccessRight = ACCESS_RIGHT_SUPERADMIN;
    else 
    {   strcpy(CommandFormat.Pass,CommandFormat.Para);
        if(strcmp(CommandFormat.Pass,Device.Password)==0)
            CommandFormat.AccessRight = ACCESS_RIGHT_USER;
        else if(strcmp(CommandFormat.Pass,SUPER_PASSWORD)==0)
            CommandFormat.AccessRight = ACCESS_RIGHT_SUPERADMIN;
        else if(strcmp(CommandFormat.Pass,"d")==0)
            CommandFormat.AccessRight = ACCESS_SYSTEM_DEBUG;
        else {
            CommandFormat.AccessRight = ACCESS_DEVICE_GET;
            /*switch(SearchUserMobileNumber(CommandFormat.Sender,0)){
                case -1:CommandFormat.AccessRight = ACCESS_NONE;break;
                case 0:	CommandFormat.AccessRight = ACCESS_RIGHT_SUPERADMIN;break;//super admin
                case 1:	CommandFormat.AccessRight = ACCESS_RIGHT_ADMIN;break;//admin
                case 2: CommandFormat.AccessRight = ACCESS_DEVICE_OUTPUT_GET;break;//driver
                default:CommandFormat.AccessRight = ACCESS_DEVICE_GET;break;
            }*/
        }
	}
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
char Command(char* commbuff)
{	/* just to generate the serial no below variable is required */
	//TEST CODE
	//char uid_str[21],octstr[21],oct9compli[21];
	/***************************************************************/
	strcpy(resbuff,"");
	/*
	PutStringUsb("\r\n\r\nSERVER TO DEVICE COMMAND in Command function = ");
	PutStringUsb(&GprsBuff[u.i[0]+1]);
	PutStringUsb("\r\n\r\n");
	*/
	for(command_comma_counter = 0,sp=0,dp=0; dp<500 && *(commbuff+sp)!='\0' && *(commbuff+sp)!='\r';sp++,dp++){
		CommandFormat.Para[dp] = *(commbuff+sp);
		CommandFormat.Para[dp+1] = '\0';
		if(*(commbuff+sp) == ',' || *(commbuff+sp) == '*' || *(commbuff+sp) == '#' || *(commbuff+sp+1) == '\0' || *(commbuff+sp+1)=='\r' || *(commbuff+sp) == '='){
			if(*(commbuff+sp) == ',' || *(commbuff+sp) == '*' || *(commbuff+sp) == '#' || *(commbuff+sp) == '=')
				CommandFormat.Para[dp] = '\0';
			
			RemoveCharFromString(' ',CommandFormat.Para);
			dp = -1;//it will be zero once roll back to next for
			if(*(commbuff+sp) == '='){
				sscanf(CommandFormat.Para,"%d.%d",&main_id,&sub_id);
				/*
				sprintf(CopyBuff,"\r\n*********\r\nmain_id=%d, sub_id=%d\n\r**********\r\n",main_id,sub_id);
				PutStringUsb(CopyBuff);
				*/
				continue;
			}
			command_comma_counter++;
			strcpy(PasteBuff,"");
			switch(command_comma_counter){
				case 1:{ //extract password
			   		//strcpy(CommandFormat.Pass,CommandFormat.Para);
					CheckAuthority();
					if(CommandFormat.AccessRight == ACCESS_NONE){
						sprintf(resbuff,"%s=!%d",CommandFormat.Pass,PASSWORD_ERROR);
						PutStringUsb(resbuff);
						//SendCommandReplyToSender(resbuff);
						return 0;
					}
					else
					{	sprintf(PasteBuff,"2,R%s",CommandFormat.Para);//this 2 is the Read/Write pkt_type required by server in response
                                                                        //R is for response to the command id from server or response using password
                        //strcpy(PasteBuff,"2,x");//this 2 is the Read/Write pkt_type required by server in response
                    }
				}
				break;
				case 2:{//write only - command
					strcpy(CommandFormat.Cmd,CommandFormat.Para); 
					sprintf(PasteBuff,",%s",CommandFormat.Para);
					//no argument command
					//if(strcmp(CommandFormat.Para,"M2C") == 0)
					//	sprintf(PasteBuff,",%s",CommandFormat.Para);
					//else 
					if(strcmp(CommandFormat.Para ,"RF") == 0){//reset factory set
						InitializeDefaultSetting();
						LoadSetting();
						strcpy(PasteBuff,",RF=1");
						strcat(resbuff,PasteBuff);
						SendCommandReplyToSender(resbuff);
						return 0;
					}
					else if(strcmp(CommandFormat.Para ,"RD") == 0){//reset device
						strcat(PasteBuff,"=1");
						strcat(resbuff,PasteBuff);
						SendCommandReplyToSender(resbuff);
						DelayInMilliSec(2000);
						PeriodicSave();
						RESET_DEVICE;
						return 0;
					}
					else if(strcmp(CommandFormat.Para ,"RC") == 0){//reset CONTROLLER
						strcat(PasteBuff,"=1");
						strcat(resbuff,PasteBuff);
						SendCommandReplyToSender(resbuff);
						PutStringUsb("\r\nRESTART DUE TO COMMAND");//RESET TEST CODE
                        SoftwareResetProcessor();
						return 0;
					}
					else if(strcmp(CommandFormat.Para ,"RP") == 0){//reset password
						strcat(PasteBuff,"=1");
						strcat(resbuff,PasteBuff);
						strcpy(Device.Password,FACTORY_SET_DEVICE_PASSWORD);
						RandomMemWrite(DEVICE_PASSWORDa,Device.Password,DEVICE_PASSWORDn);
						return 0;
					}
					else if(strcmp(CommandFormat.Cmd,"AT") == 0){//
						strcat(resbuff,",AT,");
						strcat(resbuff,commbuff+sp+1);
						strcat(resbuff,",");
						//AT COMMAND
						SendGPRScommand(commbuff+sp+1);
                        //PutStringGprs(commbuff+sp+1);
                        DelayInMilliSec(1000);
						strcat(resbuff,GprsBuff);
						SendCommandReplyToSender(resbuff);
						return 0;
					}
                    
					else if(strcmp(CommandFormat.Cmd,"AFM") == 0){//ATLAS FLOWMETER EXPERT FIRE FREE COMMAND
						strcat(resbuff,",AFM,");
						strcat(resbuff,commbuff+sp+1);
						strcat(resbuff,",");
						//ATLAS FIRE FREE COMMAND
						AtlasReplacementReceiveCounter = 0;
						AtlasReplacementBuff[AtlasReplacementReceiveCounter] = '\0';
						PutStringAtlasReplacement(commbuff+sp+1);
						DelayInSec(2);
						//DelayInMicroSec(2000000);
                        strcat(resbuff,AtlasReplacementBuff);
						SendCommandReplyToSender(resbuff);
						return 0;
					}
                    else if(strcmp(CommandFormat.Para,"AbOuT Us") == 0){																													strcpy(PasteBuff,",About Us: NABEEN LAL AMATYA\r\nDesigned & Developed By:PPS Technonogies\r\nMarketed By: Jiyanjani Pvt. Ltd");
						strcat(resbuff,PasteBuff);
						SendCommandReplyToSender(resbuff);
						return 0;
					}
				}
				break;
				default:{//
					/*
                    if(strcmp(CommandFormat.Cmd,"PAE") == 0){//
						PutStringUsb("\r\nPAE <- ");
						switch(command_comma_counter){
							case 3:{//address from where to read
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									sprintf(PasteBuff,",%d",u.i[0]);
								}
							}
							break;
							case 4:{//no of byte to read
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									sprintf(CopyBuff,",%d,",u.i[1]);
									strcat(resbuff,CopyBuff);
									if(u.i[1]>200)
										u.i[1] = 200;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,u.i[0], CopyBuff,u.i[1]);//max 200 byte buffer
									CopyBuff[u.i[1]] = '\0';
									for(uiTemp=0;uiTemp<u.i[1];uiTemp++){
										sprintf(PasteBuff," 0x%X",CopyBuff[uiTemp]);
										strcat(resbuff,PasteBuff);
									}
									PasteBuff[0] = '\0';//so that it doesnot add again at the end of this function
								}
							}
							break;
						}
					}
					
					if(strcmp(CommandFormat.Cmd,"MR") == 0){//
						switch(command_comma_counter){
							case 3:{//address from where to read
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									sprintf(PasteBuff,",%d",u.i[0]);
								}
							}
							break;
							case 4:{//no of byte to read
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									sprintf(CopyBuff,",%d,",u.i[1]);
									strcat(resbuff,CopyBuff);
									if(u.i[1]>200)
										u.i[1] = 200;
									DirectMemRead(u.i[0],CopyBuff,u.i[1]);//max 200 byte buffer
									CopyBuff[u.i[1]] = '\0';
									for(uiTemp=0;uiTemp<u.i[1];uiTemp++){
										sprintf(PasteBuff," 0x%2x",CopyBuff[uiTemp]);
										strcat(resbuff,PasteBuff);
									}
									PasteBuff[0] = '\0';//so that it doesnot add again at the end of this function
								}
							}
							break;
						}
					}
					else if(strcmp(CommandFormat.Cmd,"MW") == 0){//
						switch(command_comma_counter){
							case 3:{//address from where to write
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									sprintf(PasteBuff,",%d",u.i[0]);
								}
							}
							break;
							case 4:{//no of byte to write
								if(strlen(CommandFormat.Para)>0){
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									sprintf(PasteBuff,",%d,",u.i[1]);
								}
							}
							break;
							case 5:{//data to write
								u.i[1] = strlen(CommandFormat.Para);
								if(u.i[1]>0){
									for(uiTemp=0;uiTemp/2<u.i[1];uiTemp+=2){
										PasteBuff[uiTemp/2] = AsciiHex2Hex(&CommandFormat.Para[uiTemp]);
									}
									PasteBuff[uiTemp/2] = '\0';
									RandomMemWrite(u.i[0],PasteBuff,u.i[1]);
									PasteBuff[0] = '\0';//so that it doesnot add again at the end of this function
									strcat(resbuff,"#0");
								}
							}
							break;
                            case 10:{//page erase
                                if(strlen(CommandFormat.Para)>0){
                                    sscanf(CommandFormat.Para,"%d",&u.i[0]);//page no to erase
                                    sprintf(PasteBuff,",%d",u.i[0]);
                                    PageErase(u.i[0]);
                                }
                            }
                            break;
						}
					}
					
					else*/ if(strcmp(CommandFormat.Cmd,"DBG") == 0){//
						switch(main_id){
							case 1000: 	//super admin
										if(CommandFormat.AccessRight > ACCESS_RIGHT_SUPERADMIN){
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ACCESS_RIGHT_ERROR);	
											break;
										}
										
							case 1001:	if(CommandFormat.AccessRight > ACCESS_RIGHT_ADMIN){
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ACCESS_RIGHT_ERROR);	
											break;
										}
							case 1002:
							case 1003:
							case 1004:
							case 1005:
							case 1006:
							case 1007:
							case 1008:
							case 1009:
							case 1010:
							case 1011:{
								uiTemp = main_id-1000;
								if(uiTemp < 12){
									switch(sub_id){
										case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_LONG_LONG,USER_TABLE_START_ADDRESS+(uiTemp*USER_TABLE_RECORD_SIZE),UT_MOBn); break;//user table mobile no
										case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,USER_TABLE_START_ADDRESS+(uiTemp*USER_TABLE_RECORD_SIZE)+UT_EMAILp,UT_EMAILn);break;//user table email
										case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,USER_TABLE_START_ADDRESS+(uiTemp*USER_TABLE_RECORD_SIZE)+UT_PP_T_IDp,UT_PP_T_IDn); break;
									}
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);
							}
							break;
                            /*    
                            case 902:{//hmi test code
                                PutStringSerialHmi(CommandFormat.Para);
                                sprintf(PasteBuff,",%d.%d=SENT",main_id,sub_id);
                            }
                            break;
                            */
                            /*
                            case 901:{  //HMI LCD FUNCTIONS
                                switch(sub_id){
									case 0:{//SerialHmiDebugTarget
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											SerialHmiDebugTarget = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,SerialHmiDebugTarget);
									}
									break;
                                    case 1:{//lcdline 1
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,LcdLine1);
										}
										else{
                                          sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
									}
                                    break;
                                    case 2:{//lcdline 2
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,LcdLine2);
										}
										else{
                                          sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
									}
                                    break;
                                    case 3:{//CURSOR POSITION 1ST CHAR LINE, LAST 2 CHAR COLUMN
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CurPos);
										}
										else{
                                          sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
									}
                                    break;
                                    case 4:{//CURSOR ACTIVE/INACTIVE
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%c",main_id,sub_id,(CursorActiveFlag==TRUE)? 'A':'I');
                                        }
										else{
                                          sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
									}
                                    break;
                                    case 5: {//DisplayPage
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											DisplayPage = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,DisplayPage);
                                    }
									break;
                                }
                                break;
						    }
                            break;
                            case 900:{ //HMI Key functions
								switch(sub_id){
									case 1: KeyPressFlag = TRUE; Key = KEY_UP;break;
									case 2: KeyPressFlag = TRUE; Key = KEY_DOWN;break;
									case 3: KeyPressFlag = TRUE; Key = KEY_ENTER;break;
									case 4: KeyPressFlag = TRUE; Key = KEY_EXIT;break;
								}	
                                sprintf(PasteBuff,",%d.%d=DONE",main_id,sub_id);
							}
							break;
                            */
							/*
                            case 870: {//pae garbage command
								WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0xff, u.c, 4);
								sprintf(PasteBuff,",%d.%d=SENT",main_id,sub_id);
							}
							break;
							case 869:{ //pae forced i2c stop command
								PAE_ForceI2CStop();
								sprintf(PasteBuff,",%d.%d=DONE",main_id,sub_id);
							}
							break;
                            */
                            /*
                            case 868:{ //pae forced error generation
								if(strcmp(CommandFormat.Para,"?")!=0){
                                    sscanf(CommandFormat.Para,"%d",&u.i[0]);
									TempPaeError = u.i[0];
                                }
                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TempPaeError);
							}
                            break;
                            */
							/*
							case 867: {//power solve manual voltage and current sentting command
								switch(sub_id){
									case 0://voltage
										sscanf(CommandFormat.Para,"%d",&u.i[0]);
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									break;
									case 1://current
										sscanf(CommandFormat.Para,"%d",&u.i[1]);
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[1]);
									break;
									case 2:
										sscanf(CommandFormat.Para,"%d",&uiTemp);
										if(uiTemp == 1)
											EnablePAEoutput(u.i[0], u.i[1]);
										else
											DisablePAEoutput();
										if(PAE_I2C_ErrorFlag == FALSE)
											sprintf(PasteBuff,",%d.%d=DONE",main_id,sub_id);
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
									break;
								}
							}
							break;
							*/
							/*
                            case 866: { //power solve  Control command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x7c, u.c,1);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else{
									u.i[0]=0;
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x7C, u.c, 1);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							case 865: { //power solve  Output Current Setting command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0] =0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x72, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else{
									u.i[0]=0;
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x72, u.c, 2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							case 864: { //power solve  Output Voltage Setting command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x70, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else{
									u.i[0]=0;
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x70, u.c, 2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
                            */
							/*
							case 863: { //power solve Status:1 command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0] =0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x6f, u.c,1);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
                            /*
							case 862: { //power solve Status:0 command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0] = 0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x6c, u.c,1);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            */
							/*
							case 861: { //power solve Internal Temperature command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]= 0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x68, u.c,1);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d DegC",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
                            /*
							case 860: { //power solve  Output Current command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0] = 0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x62, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fA",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
                            /*
							case 859: { //power solve 2-byte Output Voltage command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0] =0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x60, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fV",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            */
							/*
							case 858: { //power solve Max Output Current command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x56, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fA",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
							/*
							case 857: { //power solve Max Output Voltage command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x54, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fV",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
                            /*
							case 856: { //power solve Rate Output Current command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x52, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fA",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            */
                            /*
							case 855: { //power solve Rate Output Voltage command
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x50, u.c,2);
									if(PAE_I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%.2fV",main_id,sub_id,(float)u.i[0]/100);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            */
                            /*
							case 854: { //power solve Output Voltage command
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x20, CopyBuff,4);
									if(PAE_I2C_ErrorFlag == FALSE)
									{	sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
										CopyBuff[4]='\0';
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            */
							/*
							case 853: { //power solve Serial No. command
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x30, CopyBuff,16);
									if(PAE_I2C_ErrorFlag == FALSE)
									{	sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
										CopyBuff[16]='\0';
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
							/*
							case 852: { //power solve Mfg. Date command
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x28, CopyBuff,8);
									if(PAE_I2C_ErrorFlag == FALSE)
									{	sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
										CopyBuff[8]='\0';
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
							/*
							case 851: { //power solve REVISION command
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x24, CopyBuff,4);
									if(PAE_I2C_ErrorFlag == FALSE)
									{	sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
										CopyBuff[4]='\0';
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
							/*
							case 850: { //power solve MODEL NAME command
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadPowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x10, CopyBuff,16);
									if(PAE_I2C_ErrorFlag == FALSE)
									{	sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
										CopyBuff[16]='\0';
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
							*/
                            
                            /*
							case 800:{//Flowmeter Debug Flag
								if(strcmp(CommandFormat.Para,"?")!=0){
                                    sscanf(CommandFormat.Para,"%d",&uiTemp);
									FlowMeterDebugFlag = uiTemp;
                                }
                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FlowMeterDebugFlag);
                            }
							break;
									
							case 801:{	//Atlas Flowmeter LED command
								if(strcmp(CommandFormat.Para,"?")==0){
									PutStringAtlasFlowmeter("L,?\r");
									//while(AtlasFlowmeterReceivePacketCompleteFlag == FALSE);
									DelayInSec(2);
									//DelayInMicroSec(2000000);
                                    sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										sscanf(CommandFormat.Para,"%d",&u.i[0]);
										if(u.i[0] == 0 || u.i[0] == 1)
										{	sprintf(CopyBuff,"L,%d\r",u.i[0]);
											PutStringAtlasFlowmeter(CopyBuff);
											DelayInSec(2);
											//DelayInMicroSec(2000000);
                                            sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break; 
                            */
                            
							/*
							case 802:{	//Atlas Flowmeter continous reading mode
								if(strcmp(CommandFormat.Para,"?")==0){
									PutStringAtlasFlowmeter("C,?\r");
									//while(AtlasFlowmeterReceivePacketCompleteFlag == FALSE);
									DelayInSec(2);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										sscanf(CommandFormat.Para,"%d",&u.i[0]);
										if(u.i[0] == 0 || u.i[0] == 1)
										{	sprintf(CopyBuff,"C,%d\r",u.i[0]);
											PutStringAtlasFlowmeter(CopyBuff);
											DelayInSec(2);
											sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
										}										
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
							*/
							/*
							case 803:{ //SINGLE READ VALUE
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%.2f,%.1f",main_id,sub_id,FlowMeter.TotalFlowAfterRestart,FlowMeter.CurrentFlow);
								
									//sprintf(PasteBuff,",%d.%d=%.2f,%.2f",main_id,sub_id,(float)AtlasReplacement.TotalFlowAfterRestart/100,(float)AtlasReplacement.CurrentFlow/100);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
							}
							break;
							*/
                            
							case 804:{	//Atlas Flowmeter clear reading 
								if(strcmp(CommandFormat.Para,"CLEAR")==0){
									ClearAllFlow();
                                    sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,"DONE");
                                    
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
							}
							break;
                            
                            /*
							case 805:{	//Atlas Flowmeter K value command
								switch(sub_id){
									case 0:	//Read Flowmeter K value
										if(strcmp(CommandFormat.Para,"?")==0){
											//PutStringAtlasFlowmeter("K,?\r");
											AtlasReplacementCommand("[302,?");
									
                                            //while(AtlasFlowmeterReceivePacketCompleteFlag == FALSE);
											DelayInSec(2);
											//DelayInMicroSec(2000000);
                                            sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									break;
									case 1:	//volume per n pulse
										if(strlen(CommandFormat.Para) > 0) 
										{	sscanf(CommandFormat.Para,"%f",&u.f[0]);
											sprintf(PasteBuff,",%d.%d=%f",main_id,sub_id,u.f[0]);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									break;
									case 2: //no of pulse per volume
										if(strlen(CommandFormat.Para) > 0) 
										{	sscanf(CommandFormat.Para,"%d",&u.i[1]);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[1]);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									break;
									case 3: //set K factor as volume per no of pulse 
										sscanf(CommandFormat.Para,"%d",&uiTemp);
										if(uiTemp == 1)
										{	sprintf(CopyBuff,"K,%f,%d\r",u.f[0],u.i[1]);
											PutStringAtlasFlowmeter(CopyBuff);
											DelayInSec(2);
											//DelayInMicroSec(2000000);
                                            sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									break;
								}
							}
							break;
                            */
                            case 806:{	//K value command
								if(strcmp(CommandFormat.Para,"?")!=0){
                                    sscanf(CommandFormat.Para,"%d",&u.i[0]);
                                    RandomMemWrite(FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
                                    FlowMeter.K_Factor = u.i[0];
                                }
                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FlowMeter.K_Factor);
                            }
							break;
                            
                            /*
							case 806:{	//Clear Atlas Flowmeter K value command
								sscanf(CommandFormat.Para,"%d",&u.i[0]);
								if(u.i[0]>0)
									sprintf(CopyBuff,"K,CLEAR,%d\r",u.i[0]);
								else
									strcpy(CopyBuff,"K,CLEAR\r");
								PutStringAtlasFlowmeter(CopyBuff);
								DelayInSec(2);
                                //DelayInMicroSec(2000000);
                                sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
							}
							break;
                            */
                            /*
							case 807:{	//Atlas Flowmeter time base
								if(strcmp(CommandFormat.Para,"?")==0){
									PutStringAtlasFlowmeter("TK,?\r");
									DelayInSec(2);
                                    //DelayInMicroSec(2000000);
                                    sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										sscanf(CommandFormat.Para,"%c",&u.c[0]);
										switch(u.c[0])
										{	case 'S': 
											case 'M':
											case 'H':	sprintf(CopyBuff,"TK,%c\r",u.c[0]);
														PutStringAtlasFlowmeter(CopyBuff);
														DelayInSec(2);
                                                        //DelayInMicroSec(2000000);
                                                        sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
											break;
											default:	sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
                            */
                            /*
							case 808:{	//Atlas Flowmeter pull up/down resistor
								if(strcmp(CommandFormat.Para,"?")==0){
									PutStringAtlasFlowmeter("P,?\r");
									DelayInSec(2);
                                    //DelayInMicroSec(2000000);
                                    sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										sscanf(CommandFormat.Para,"%d",&iTemp);
										switch(iTemp)
										{	case 0:	//DISABLE 
											case 1: 
											case -1:
											case 10:
											case -10:
											case 100:
											case -100:
														sprintf(CopyBuff,"P,%d\r",iTemp);
														PutStringAtlasFlowmeter(CopyBuff);
														DelayInSec(2);
                                                        //DelayInMicroSec(2000000);
                                            			sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasReplacementBuff);
											break;
											default:	sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
                            */
							/*
							case 809: {//setting response code
								if(strcmp(CommandFormat.Para,"?")==0){
									PutStringAtlasFlowmeter("RESPONSE,?\r");
									DelayInSec(2);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										sscanf(CommandFormat.Para,"%d",&u.i[0]);
										if(u.i[0] == 0 || u.i[0] == 1)
										{	sprintf(CopyBuff,"RESPONSE,%d\r",u.i[0]);
											PutStringAtlasFlowmeter(CopyBuff);
											DelayInSec(2);
											sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);
										}
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
							*/
							/*
							case 811: {//Atlas Flowmeter power mode
								if(strlen(CommandFormat.Para)>0){
								sscanf(CommandFormat.Para,"%d",&u.i[0]);
								switch(u.i[0])
								{	case 0: PutStringAtlasFlowmeter("SLEEP\r");DelayInSec(2);sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);break;
									case 1: PutStringAtlasFlowmeter("\r");DelayInSec(2);sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasFlowmeterBuff);break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);break;
								}
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
							}
							break;
							*/
                            case 812:   { //AtlasReplacementDebugFlag
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,AtlasReplacementDebugFlag? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												AtlasReplacementDebugFlag = FALSE;
											else
												AtlasReplacementDebugFlag = TRUE;
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,AtlasReplacementDebugFlag? 1:0);
										}
                                    }
                            break;
                            /*
                            case 813:{ //hang child processor for reboot test
								if(strcmp(CommandFormat.Para,"?")==0){
									//GetFlowMeterReading();
                                    SendAtlasReplacementCommand("{000=0");
                                    DelayInMilliSec(200);
                                    sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,AtlasReplacementBuff);
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
							}
							break;
							*/
					//////////////////////////////////////////////////////////////////////////////////////////////////////		
							/*case 700: {//atlas Temperature Device Type
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,0,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}	
							}
							break;
							case 701: {	//Atlas Temperature Device Version
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,1,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;*/
							/*case 705: {	//Atlas Temperature LED control
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,5,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									switch(u.i[0]){
										case 0:	//default config 0x00 = LED off
										case 1:	//Blink each time a reading is taken
												WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,5,u.c,1);
												if(I2C_ErrorFlag == FALSE)
													sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
												else
													sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
												break;
										default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR); break; 
									}
								}
							}
							break;
                            */
                            /*
							case 706: {	//Temperature probe Active/Hybernate setting
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
                            */
							/*case 707: {	//New Reading Available register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,7,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,7,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
									else
										sprintf(CopyBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;*/
							/*case 708: {	//Atlas Temperature Calibration Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,8,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									//arrange the 4 byte integer value to send in MSB first 
									u.c[0] = u.c[7];
									u.c[1] = u.c[6];
									u.c[2] = u.c[5];
									u.c[3] = u.c[4];
									WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,8,u.c,4);//USE MSB FIRST VARIABLE
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[1]);//USE MSB LAST VARIABLE AS SENT BY USER
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;*/
							/*case 712: {	//Calibration Request Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									if(u.i[0] > 0 && u.i[0] < 3)
									{	WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
										if(I2C_ErrorFlag == FALSE)
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
										else
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;*/
							/*case 713: {	//Calibration confirmation Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,13,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;*/
							case 714: {	//Temperature registers
								if(strcmp(CommandFormat.Para,"?")==0){
									/*
                                    ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,14,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
                                  */
                                  sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AtlasTemperature/1000);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 715: {	
								switch(sub_id){
                                    case 0:{//My TemperatureOffsetRegister
                                            if(strcmp(CommandFormat.Para,"?")!=0){
                                                sscanf(CommandFormat.Para,"%d",&u.i[0]);
                                                TemperatureOffsetRegister = u.i[0] - RawWaterTemperature;
                                                u.i[0] = TemperatureOffsetRegister;
                                                RandomMemWrite(TEMPERATURE_OFFSET_REGISTERa,u.c,TEMPERATURE_OFFSET_REGISTERn);
                                            }
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TemperatureOffsetRegister);
                                    }
                                    break;
                                    case 1:{//My Clear TemperatureOffsetRegister
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&u.i[0]);
                                            TemperatureOffsetRegister = u.i[0];
                                            u.i[0] = TemperatureOffsetRegister;
                                            RandomMemWrite(TEMPERATURE_OFFSET_REGISTERa,u.c,TEMPERATURE_OFFSET_REGISTERn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TemperatureOffsetRegister);
                                    }
                                    break;
                                }
                            }
							break;
							  
							/*case 600: {//conductivity //Device Type
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,0,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 601: {	//Device Version
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,1,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;*/
							/*case 605: {	//LED control
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,5,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									switch(u.i[0]){
										case 0:	//default config 0x00 = LED off
										case 1:	//0x01 = LED on
										case 2:	//0x02 = LED on upon new reading
										case 4:	//0x04 = LED off upon new reading
										case 8:	//0x08 = LED invert upon new reading
												WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,5,u.c,1);
												if(I2C_ErrorFlag == FALSE)
													sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
												else
													sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
												break;
										default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR); break; 
									}
								}
							}
							break;
							*/
                            case 606: { //Conductivity Probe Active/Hybernate Function
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							/*case 607: {	//New Reading Available register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,7,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,7,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
									else
										sprintf(CopyBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;*/
							case 608: {	//Probe Type register
								if(strcmp(CommandFormat.Para,"?")==0){
									u.i[0]=0;
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,8,u.c,2);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.i[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,8,u.c,2);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							case 610: {	//Calibration Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,10,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
                                    }
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									//arrange the 4 byte integer value to send in MSB first 
									u.c[0] = u.c[7];
									u.c[1] = u.c[6];
									u.c[2] = u.c[5];
									u.c[3] = u.c[4];
									WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,10,u.c,4);
									if(I2C_ErrorFlag == FALSE){
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[1]);
                                        AtlasConductivity = u.i[1]; //for displaying conductivity on calibration success
                                    }
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							
                            case 614: {	//Calibration Request Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,14,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[0]);
									if(u.i[0] > 0 && u.i[0] < 6)
									{	WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,14,u.c,1);
										if(I2C_ErrorFlag == FALSE)
											sprintf(PasteBuff,",%d.%d=%d uS",main_id,sub_id,AtlasConductivity);
										else
											sprintf(PasteBuff,",%d.%d=C!%d",main_id,sub_id,I2C_COMM_ERROR);
										//Read Temperature during conductivity Calibration
                                        u.i[1] = AtlasTemperature;
            
                                        //save the temperature at which the calibration was made
                                        //u.i[0] = u.i[1];
                                        u.i[0] = 25000;//25 DEGREE CENTIGRADE
                                        sprintf(CopyBuff," Current Temp = %.2f DegC",(float)u.i[1]/1000);
                                        strcat(PasteBuff,CopyBuff);
                                        TemperatureAtWhichConductivityWasCalibrated = u.i[0];
                                        RandomMemWrite(TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDa,u.c,TEMP_AT_WHICH_CONDUCTIVITY_WAS_CALIBRATEDn);
                                        sprintf(CopyBuff," but Calibrated @ %.2f DegC",(float)u.i[0]/1000);
                                        strcat(PasteBuff,CopyBuff);
										
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
							case 615: {	//Calibration confirmation Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,15,u.c,1);
									if(I2C_ErrorFlag == FALSE)
										sprintf(CopyBuff,"%d",u.c[0]);
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 616: {	//Temperature compensation Register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,16,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sscanf(CommandFormat.Para,"%d",&u.i[1]);
									//arrange the 4 byte integer value to send in MSB first 
									u.c[0] = u.c[7];
									u.c[1] = u.c[6];
									u.c[2] = u.c[5];
									u.c[3] = u.c[4];
									WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,16,u.c,4);
									if(I2C_ErrorFlag == FALSE)
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
								}
							}
							break;
							/*case 620: {	//Temperature confirm register
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,20,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							*/
							case 624: {	//Conductivity registers
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,24,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							/*
							case 628: {	//Total Dissolved Solids (TDS) registers
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,28,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 632: {	//Practical Salinity Scale (PSS) registers
								if(strcmp(CommandFormat.Para,"?")==0){
									ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,32,u.c,4);
									if(I2C_ErrorFlag == FALSE)
									{	//switch MSB <--> LSB. First byte received is MSB
										u.c[4] = u.c[3];
										u.c[5] = u.c[2];
										u.c[6] = u.c[1];
										u.c[7] = u.c[0];
										sprintf(CopyBuff,"%d",u.i[1]);
									}
									else
										sprintf(CopyBuff,"!%d",I2C_COMM_ERROR);
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,CopyBuff);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							*/
							/*case 640: { //dry calibration
								bTemp = FALSE;
								u.i[0] = 1;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);//activate conductivity sensor probe
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0] = 100;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,8,u.c,2);//probe type
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0]=0;
								ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,8,u.c,2);//get probe type
								if(I2C_ErrorFlag == FALSE)
								{	sprintf(CopyBuff,"\r\nAtlas Conductivity Module Probe Type Set to %d",u.i[0]);
									PutStringUsb(CopyBuff);
								}
								else
									bTemp = TRUE;
								u.i[0] = 1;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,14,u.c,1);//Clear calibration
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0] = 0;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,10,u.c,4);//set calibration value register = 0 for dry calibration
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0] = 2;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,14,u.c,1);//set calibration request register = 2 for dry calibration
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(20);
								u.i[0] = 0;
								ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,15,u.c,1);//get calibration confimation register
								if(I2C_ErrorFlag == FALSE)
								{	sprintf(CopyBuff,"\r\nAtlas Conductivity Module DRY Calibration confirmation = %d",u.i[0]);	
									PutStringUsb(CopyBuff);
								}
								else
									bTemp = TRUE;
								
								if(bTemp == FALSE)
									sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);	
							}
							break;
							case 641: { //set conductivity one point calibration
								bTemp = FALSE;
								u.i[0] = 14700;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,10,u.c,4);//set calibration value register = 14700 for single point calibration
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0] = 3;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,14,u.c,1);//set calibration request register = 3 for single point calibration
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								DelayInMilliSec(2);
								u.i[0] = 0;
								ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,15,u.c,1);//get calibration confimation register
								if(I2C_ErrorFlag == FALSE)
								{	sprintf(CopyBuff,"\r\nAtlas Conductivity Module SINGLE POINT Calibration confirmation = %d",u.i[0]);			
									PutStringUsb(CopyBuff);
								}
								else
									bTemp = TRUE;
								DelayInMilliSec(2);
								//GET CONDUCTIVITY
								u.i[0] = 0;
								ReadAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,24,u.c,4);
								if(I2C_ErrorFlag == FALSE)
								{	//switch MSB <--> LSB. First byte received is MSB
									u.c[4] = u.c[3];
									u.c[5] = u.c[2];
									u.c[6] = u.c[1];
									u.c[7] = u.c[0];
									sprintf(CopyBuff,"\r\nCurrent Conductivity Read as %d uS",u.i[1]);
									PutStringUsb(CopyBuff);
								}
								else
									bTemp = TRUE;
								DelayInMilliSec(2);
								//HYBERNATE
								u.i[0] = 0;
								WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);//hybernate conductivity sensor probe
								if(I2C_ErrorFlag == TRUE)
									bTemp = TRUE;
								if(bTemp == FALSE)
									sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,I2C_COMM_ERROR);
							}
							break;
							*/
                            case 530:{//wifi setting
                                switch(sub_id){
                                    case 0: //SSID 
                                      ReadWriteMemoryCommand(main_id,sub_id,DT_STR,WIFI_SSID_NAMEa,WIFI_SSID_NAMEn);
                                    break;
                                    case 1: //SSID password 
                                      ReadWriteMemoryCommand(main_id,sub_id,DT_STR,WIFI_SSID_PASSWORDa,WIFI_SSID_PASSWORDn);
                                    break;
                                }
                            }
                            break;
							case 520:{//server 0
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE0a,UPLOAD_PTRn); break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
							case 515:{//server 5
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*5,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*5+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*5+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE5a,UPLOAD_PTRn); break;
									case 6: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,FOTA_STATUSa,FOTA_STATUSn); break;
									case 7: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,UPDATE_FIRMWARE_FILENAMEa,UPDATE_FIRMWARE_FILENAMEn); break;
									case 8: {//fota page no
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,Fota.PageNo);
										}
										else{
											if(strlen(CommandFormat.Para)>0){
												sscanf(CommandFormat.Para,"%d",&u.i[0]);
												Fota.PageNo = u.i[0];
												sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,Fota.PageNo);
											}
										}
									}
									break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
							/*
							case 514:{//server 4
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*4,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*4+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*4+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE4a,UPLOAD_PTRn); break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
							case 513:{//server 3
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*3,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*3+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*3+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE3a,UPLOAD_PTRn); break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;*/
							case 512:{//server 2
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*2,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*2+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE*2+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE2a,UPLOAD_PTRn); break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
							case 511:{//server 1
								switch(sub_id){
									case 0:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE,ST_SRn); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE+ST_IPorDOMAINp,ST_IPorDOMAINn); break;
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,SERVER_START_ADDRESS+SERVER_RECORD_BYTE_SIZE+ST_PORTp,ST_PORTn); break;
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,UPLOAD_PTR_PROFILE1a,UPLOAD_PTRn); break;
									case 8: {//upload pointer RAM
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,UploadPtr);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&UploadPtr);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,UploadPtr);
										}
									} 
                                    break;
                                    default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
                            /*
                            case 506:{//hardware digital output control
                                switch(sub_id){
                                    case 0://power key
                                       	sscanf(CommandFormat.Para,"%d",&uiTemp);
										if(uiTemp==0)
											GPRS_PWR_KEY_LOW;
										else
											GPRS_PWR_KEY_HIGH;
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,uiTemp);
									break;
                                    case 1://DTR key
                                       	sscanf(CommandFormat.Para,"%d",&uiTemp);
										if(uiTemp==0)
											GPRS_DTR_LOW;
										else
											GPRS_DTR_HIGH;
										sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,uiTemp);
									break;
                                }
                            }
                            break;
                            */
							/*case 505:{//CALL
								switch(sub_id){
									case 0:	{//CONTROL
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & CALL_CONTROL)>0)? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												GprsCR &= ~CALL_CONTROL;
											else
												GprsCR |= CALL_CONTROL;
											u.c[0] = GprsCR;
											RandomMemWrite(GPRS_CRa,u.c,GPRS_CRn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & CALL_CONTROL)>0)? 1:0);
										}
									}
									break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;*/
							/*case 504:{//MOBILE DEEP SLEEP 
								switch(sub_id){
									case 0:	{//CONTROL
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & MOBILE_DEEP_SLEEP_CONTROL)>0)? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												GprsCR &= ~MOBILE_DEEP_SLEEP_CONTROL;
											else
												GprsCR |= MOBILE_DEEP_SLEEP_CONTROL;
											u.c[0] = GprsCR;
											RandomMemWrite(GPRS_CRa,u.c,GPRS_CRn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & MOBILE_DEEP_SLEEP_CONTROL)>0)? 1:0);
										}
									}
									break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
                            */
							/*
                            case 503:{//EMAIL
								switch(sub_id){
									case 0:	{//CONTROL
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & EMAIL_CONTROL)>0)? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												GprsCR &= ~EMAIL_CONTROL;
											else
												GprsCR |= EMAIL_CONTROL;
											u.c[0] = GprsCR;
											RandomMemWrite(GPRS_CRa,u.c,GPRS_CRn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & EMAIL_CONTROL)>0)? 1:0);
										}
									}
									break;
									case 1:	ReadWriteMemoryCommand(main_id,sub_id,DT_STR,EMAIL_USERNAMEa,EMAIL_USERNAMEn); break;//email USERNAME
									case 2:	ReadWriteMemoryCommand(main_id,sub_id,DT_STR,EMAIL_PASSWORDa,EMAIL_PASSWORDn); break;//email password
									case 3:	ReadWriteMemoryCommand(main_id,sub_id,DT_STR,EMAIL_SENDER_EMAIL_ADDRESSa,EMAIL_SENDER_EMAIL_ADDRESSn); break;//email SENDER EMAIL ADDRESS
									case 4:	ReadWriteMemoryCommand(main_id,sub_id,DT_STR,EMAIL_OUTGOING_EMAIL_SERVERa,EMAIL_OUTGOING_EMAIL_SERVERn); break;//email OUTGOING SERVER NAME
									case 5:	ReadWriteMemoryCommand(main_id,sub_id,DT_INT,EMAIL_OUTGOING_EMAIL_SERVER_PORTa,EMAIL_OUTGOING_EMAIL_SERVER_PORTn); break;//email OUTGOING SERVER PORT NUMBER
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
                            */
							case 502:{//gprs
								switch(sub_id){
									case 0:	{//CONTROL
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & GPRS_CONTROL)>0)? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												GprsCR &= ~GPRS_CONTROL;
											else
												GprsCR |= GPRS_CONTROL;
											u.c[0] = GprsCR;
											RandomMemWrite(GPRS_CRa,u.c,GPRS_CRn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & GPRS_CONTROL)>0)? 1:0);
										}
									}
									break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_STR,APNa,APNn); break;
									
									case 2: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,MIN_REC_PER_CONNECTIONa,MIN_REC_PER_CONNECTIONn); break;
									case 3:{//READ ONLY : MAX REC PER CONNECTION
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MAX_REC_PER_CONNECTION);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									/*
                                    case 4:{//READ ONLY : ServerTimeoutSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ServerTimeoutSec);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
                                    */
									case 5: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,LOG_PTRa,LOG_PTRn); break;
									/*
									case 6:{//READ ONLY : LastServerConnectionTime
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,LastServerConnectionTime);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									*/
									/*
									case 7:{//ACTIVE SERVER
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ActiveServer);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp < MAX_NO_OF_SERVER)
												ActiveServer = uiTemp;
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(uiTemp <  MAX_NO_OF_SERVER)? 1:0);
										}
									}
									break;
									*/
									case 8: {//Log pointer RAM
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&LogPtr);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,LogPtr);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,LogPtr);
                                    } 
                                    break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
                            /*
							case 501:{//SMS
								switch(sub_id){
									case 0:	{//CONTROL
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & SMS_CONTROL)>0)? 1:0);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												GprsCR &= ~SMS_CONTROL;
											else
												GprsCR |= SMS_CONTROL;
											u.c[0] = GprsCR;
											RandomMemWrite(GPRS_CRa,u.c,GPRS_CRn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((GprsCR & SMS_CONTROL)>0)? 1:0);
										}
									}
									break;
									default:sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR); break;
								}
							}
							break;
                            */
							case 500:{//Mobile
								switch(sub_id){
									case 0:{//Mobile Debug Flag
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											MobileDebugFlag = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MobileDebugFlag);
                                    }
									break;
									/*
									case 1:{//READ ONLY : SIM MCC NUMBER
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.Mcc);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 2:{//READ ONLY : SIM MNC NUMBER
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.Mnc);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 3:{//READ ONLY : SIM LAC NUMBER
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%X",main_id,sub_id,GprsPacket.Lac);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 4:{//READ ONLY : SIM CELLID NUMBER
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%X",main_id,sub_id,GprsPacket.CellID);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 5:{//READ ONLY : Battery charging status
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.BatteryChargingStatus);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 6:{//READ ONLY : Battery Percentage
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.BatteryPercentage);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
                                    */
                                    /*
									case 7:{//READ ONLY : Signal Strength
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.SignalStrength);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
                                    */
                                    /*
									case 8:{//READ ONLY : Network
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,GprsPacket.Network);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 9:{//READ ONLY : Network time
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d:%d:%d",
													main_id,
													sub_id,
													GprsPacket.TimeStamp.tm_hour,
													GprsPacket.TimeStamp.tm_min,
													GprsPacket.TimeStamp.tm_sec);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 11:{//READ ONLY : Network Date
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d/%d/20%d",
													main_id,
													sub_id,
													GprsPacket.TimeStamp.tm_mday,
													GprsPacket.TimeStamp.tm_mon,
													GprsPacket.TimeStamp.tm_year);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 12:{//READ ONLY : Tower Lat
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.6f",main_id,sub_id,GprsPacket.Lat);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 13:{//READ ONLY : Tower Lng
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.6f",main_id,sub_id,GprsPacket.Lng);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									*/
									
									case 14:{//Mobile Stage
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            MobileStage = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MobileStage);
                                    }
									break;
									
									/*
									case 15:{//ReturnToMobileStage
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ReturnToMobileStage);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 16:{//MobileStageWaitSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MobileStageWaitSec);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 17:{//MobileResponseWaitSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MobileResponseWaitSec);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 21://BALANCE CHECK Flag
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											BalanceCheckFlag = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,BalanceCheckFlag);
                                    break;
									*/
                                    case 23://prefered communiation
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            CommunicationMediaPrefered = uiTemp;
											u.c[0] = CommunicationMediaPrefered;
                                            RandomMemWrite(COMMUNICATION_MEDIA_PREFEREDa,u.c,COMMUNICATION_MEDIA_PREFEREDn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CommunicationMediaPrefered);
                                    break;
                                    case 24://current communiation media
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CommunicationMediaCurrent);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
                                    break;
                                    
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							
							case 420:{//Process variable
								switch(sub_id){
                                    case 0: {//Mode
                                        if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,Mode);
										}
										else{
											sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(Mode != uiTemp)
                                            {   Mode = uiTemp;
                                                switch(Mode)
                                                {   case MODE_LOGOUT:       SwitchingFromAutoToLogout(); Mode = uiTemp;break;
                                                    case MODE_AUTO:         SwitchFromLogoutToAutoMode(1); Mode = uiTemp; break;
                                                    case MODE_MANUAL_FLUSH: DisplayPage = USER_FUNCTIONS_0_0; DisplayMenu(); break;
                                                    case MODE_MANUAL_CIP:   SwitchFromLogoutToManualCIP(); Mode = uiTemp;break;
                                                }
                                            }
                                            else if(uiTemp == MODE_AUTO && CurrentProcessStep == IDLE)//this is to switch from Auto/IDLE to Prepurify
                                            {   SwitchFromLogoutToAutoMode(1); 
                                            }
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,Mode);
										}
                                    }
                                    break;
                                    /*
                                    case 0: {//Jump to step
										sscanf(CommandFormat.Para,"%d",&uiTemp);
										CurrentProcessStep = uiTemp;
										ProcessStepRunSec = 0;
										DisplayPage = AUTO_0;
										Mode = MODE_AUTO;
										//other variable so that don't care no matter which step it is jumping.
										CIP_DosingRunTimeInSec = 0;
										CIP_PulseRunTimeInSec = 0;
										ResetAverageFlow();
                                        //AverageFlow = 0;
										//AverageFlowAccumulator = 0;
										//WasteBypassLowFlag = TRUE;
										//WasteBypassHighFlag = TRUE;
									}
									break;
									*/
                                /*	case 0:	{//CurrentProcessStep
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											CurrentProcessStep = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CurrentProcessStep);
                                    }
									break;*/
									/*case 1:{	//ProcessStepRunSec
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											ProcessStepRunSec = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ProcessStepRunSec);
                                    }
									break;
                            */
									case 2:{	//CycleCounter
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											CycleCounter = uiTemp;
                                            u.i[0] = uiTemp;
                                            RandomMemWrite(LAST_CYCLE_COUNTER_FOR_CIPa,u.c,LAST_CYCLE_COUNTER_FOR_CIPn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CycleCounter);
									}
									break;
                                    /*
									case 3:{	//AverageFlow
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											AverageFlow = uiTemp;
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,AverageFlow);
                                    }
									break;
									case 4: {//DS18B20 Temperature
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)Temperature.i/10);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}	
									}
									break;
									case 5:{//di1
										if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												CurDigitalInput &= ~0x01;
											else
												CurDigitalInput |= 0x01;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput&0x01)>0)? 1:0);
                                    }
									break;
									case 6:{//di2
										if(strcmp(CommandFormat.Para,"?")!=0){
                                        	sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												CurDigitalInput &= ~0x02;
											else
												CurDigitalInput |= 0x02;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput&0x02)>0)? 1:0);
									}
									break;
									case 7:{//di3
										if(strcmp(CommandFormat.Para,"?")!=0){
                                        	sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												CurDigitalInput &= ~0x04;
											else
												CurDigitalInput |= 0x04;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput&0x04)>0)? 1:0);
									}
									break;
									case 8:{//di4
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												CurDigitalInput &= ~0x08;
											else
												CurDigitalInput |= 0x08;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput&0x08)>0)? 1:0);
									}
									break;
                                    case 9:{//di5
                                       	if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&uiTemp);
											if(uiTemp==0)
												CurDigitalInput &= ~0x10;
											else
												CurDigitalInput |= 0x10;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput&0x10)>0)? 1:0);
									}
									break;
                                    */
									case 11:{//PWM DUTY 
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											PwmDuty = u.i[0];
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PwmDuty);
                                    }
									break;
									case 12:{ //PWM CYCLE
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											PwmCycle = u.i[0];
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PwmCycle);
                                    }
									break;
									case 13:{ //PWM UPDATE & GENERATE DUTY & CYCLE
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%s",
													main_id,
													sub_id,
													"DONE");
											Ppg_SetDuty(5,PwmDuty);
											Ppg_SetCycle(5,PwmCycle);
										}
									}
									break;
									/*
                                    case 14:{ //LastCycleOptimizedOutputVoltageCode
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&uiTemp);
											LastCycleOptimizedOutputVoltageCode = uiTemp;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,LastCycleOptimizedOutputVoltageCode);
                                    }
									break;
									*/
                                    case 15:{ //shunt sec
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											ShuntSec = u.i[0];
                                            u.c[0] = ShuntSec;
                                            RandomMemWrite(SHUNT_TIME_IN_SECa,u.c,SHUNT_TIME_IN_SECn);
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ShuntSec);
                                    }
									break;
                                    case 16:{ //pressure offset
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",
													main_id,
													sub_id,
													PressureOffset);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&u.f[0]);
                                            if((u.f[0] >= PHYSICAL_VALUE_HIGH * -0.1) && (u.f[0] <= PHYSICAL_VALUE_HIGH * 0.1))
											{   PressureOffset = u.f[0];
                                                RandomMemWrite(PRESSURE_OFFSETa,u.c,PRESSURE_OFFSETn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",
                                                        main_id,
                                                        sub_id,
                                                        PressureOffset);
                                            }
                                            else
                                            {   sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                            }
                                        }
									}
									break;
                                    case 17:{   // NabeenTestValue for debuging certain conditions
                                        if(strcmp(CommandFormat.Para,"?")==0){
											DirectMemRead(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
                                        }
								    }
                                    break;
                                case 18:{   // NabeenTestValue for debuging certain conditions: FirstWasteCycle
                                        if(strcmp(CommandFormat.Para,"?")==0){
											//DirectMemRead(EC_ZERO_CYCLE_COUNTa,u.c,EC_ZERO_CYCLE_COUNTn);
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FirstWasteCycle);
                                        }
								    }
                                    break;
                                    
                                    
                                case 101:{   // NabeenTestValue for debuging certain conditions
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											NabeenTestValue = u.i[0];
                                            //WaitTimeB4BypassAlarmInSecCounter = NabeenTestValue;
										}
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,NabeenTestValue);
                                    }
                                    break;
                                }	
							}
							break;
                            case 412:{  //variable that are setpoint but not in LCD or in server, can only be set via docklight
                                switch(sub_id){
                                    case 0: //ShuntPostPureStepTimeInSec//WastePreShuntTimeInSec 
                                      if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
                                                ShuntPostPureStepTimeInSec = u.i[0];
                                            RandomMemWrite(SHUNT_PRE_CHARGE_TIME_IN_SECa,u.c,SHUNT_PRE_CHARGE_TIME_IN_SECn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ShuntPostPureStepTimeInSec);
                                    break;
                                    case 1: //ShuntPostWasteStepTimeInSec 
                                      if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											ShuntPostWasteStepTimeInSec = u.i[0];
                                            RandomMemWrite(SHUNT_POST_CHARGE_TIME_IN_SECa,u.c,SHUNT_POST_CHARGE_TIME_IN_SECn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,ShuntPostWasteStepTimeInSec);
                                    break;
                                    case 2: //HF_WasteShuntTimeInSec 
                                      if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&u.i[0]);
											HF_WasteShuntTimeInSec = u.i[0];
                                            RandomMemWrite(HF_WASTE_SHUNT_TIME_IN_SECa,u.c,HF_WASTE_SHUNT_TIME_IN_SECn);
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,HF_WasteShuntTimeInSec);
                                    break;
                                }
                            }
                            break;
                            case 411:{   //reset alarm
                                bTemp = TRUE;
                                switch(sub_id){
                                    case 255: AlarmRegister24Bit = 0; break;
                                    case 0: AlarmRegister24Bit &= ~0x000001; break;
									case 1: AlarmRegister24Bit &= ~0x000002; break;
									case 2: AlarmRegister24Bit &= ~0x000004; break;
									case 3: AlarmRegister24Bit &= ~0x000008; break;
									case 4: AlarmRegister24Bit &= ~0x000010; break;
									case 5: AlarmRegister24Bit &= ~0x000020; break;
									case 6: AlarmRegister24Bit &= ~0x000040; break;
									case 7: AlarmRegister24Bit &= ~0x000080; break;
									case 8: AlarmRegister24Bit &= ~0x000100; break;
									case 9: AlarmRegister24Bit &= ~0x000200; break;
									case 10: AlarmRegister24Bit &= ~0x000400; break;
									case 11: AlarmRegister24Bit &= ~0x000800; break;
									case 12: AlarmRegister24Bit &= ~0x001000; break;
									case 13: AlarmRegister24Bit &= ~0x002000; break;
									case 14: AlarmRegister24Bit &= ~0x004000; break;
									case 15: AlarmRegister24Bit &= ~0x008000; break;
									case 16: AlarmRegister24Bit &= ~0x010000; break;
									case 17: AlarmRegister24Bit &= ~0x020000; break;
									case 18: AlarmRegister24Bit &= ~0x040000; break;
									case 19: AlarmRegister24Bit &= ~0x080000; break;
									case 20: AlarmRegister24Bit &= ~0x100000; break;
									case 21: AlarmRegister24Bit &= ~0x200000; break;
									case 22: AlarmRegister24Bit &= ~0x400000; break;
									case 23: AlarmRegister24Bit &= ~0x800000; break;
                                    default: bTemp = FALSE;break;
                                }
                                if(bTemp == TRUE)
                                  sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);//alarm reseted
                                else
                                  sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);
                            }
                            break;
							case 410:{//Process configurable parameters
								switch(sub_id){
									case 0:	{//Pure EC Target
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TargetOutletConductivity);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= PURE_EC_TARGET_MAX_VAL){
                                                TargetOutletConductivity = u.i[0];
                                                RandomMemWrite(TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECa,u.c,TARGET_OUTLET_CONDUCTIVITY_uS_PER_SECn);
												sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TargetOutletConductivity);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
                                    break;
									case 1:	{//PrepurifyTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PrepurifyTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= PREPURIFY_TIME_MAX_VAL){
                                                PrepurifyTimeInSec = u.i[0];
                                                RandomMemWrite(PREPURIFY_TIME_IN_SECa,u.c,PREPURIFY_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PrepurifyTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
									break;
									case 2:	{//PurifyTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PurifyTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= PURIFY_TIME_MIN_VAL && u.i[0] <= PURIFY_TIME_MAX_VAL){
												PurifyTimeInSec = u.i[0];
                                                RandomMemWrite(PURIFY_TIME_IN_SECa,u.c,PURIFY_TIME_IN_SECn);
												sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,PurifyTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
									break;
									case 3:{	//WasteTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WasteTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= WASTE_TIME_MIN_VAL && u.i[0] <= WASTE_TIME_MAX_VAL){
                                                WasteTimeInSec = u.i[0];
                                                RandomMemWrite(WASTE_TIME_IN_SECa,u.c,WASTE_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WasteTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
										}
									}
									break;
									case 4:{	//HF_WasteTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,HF_WasteTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= HF_WASTE_TIME_MAX_VAL){
												HF_WasteTimeInSec = u.i[0];
                                                RandomMemWrite(HF_WASTE_TIME_IN_SECa,u.c,HF_WASTE_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,HF_WasteTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 5:{	//CIP_DosingOnTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingOnTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= CIP_DOSE_ON_TIME_MIN_VAL && u.i[0] <= CIP_DOSE_ON_TIME_MAX_VAL){
												CIP_DosingOnTimeInSec = u.i[0];
                                                RandomMemWrite(CIP_DOSING_ON_TIME_IN_SECa,u.c,CIP_DOSING_ON_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingOnTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 6:{	//CIP_DosingOffTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingOffTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= CIP_DOSE_ON_TIME_MIN_VAL && u.i[0] <= CIP_DOSE_ON_TIME_MAX_VAL){
												CIP_DosingOffTimeInSec = u.i[0];
                                                RandomMemWrite(CIP_DOSING_OFF_TIME_IN_SECa,u.c,CIP_DOSING_OFF_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingOffTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 7:{	//CIP_DosingTotalTimeInSec
										#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                            if(strcmp(CommandFormat.Para,"?")==0){
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingTotalTimeInSec);
                                            }
                                            else{
                                                sscanf(CommandFormat.Para,"%d",u.i);
                                                if(u.i[0] >= CIP_DOSE_ON_TIME_MIN_VAL && u.i[0] <= CIP_DOSE_ON_TIME_MAX_VAL){
                                                    CIP_DosingTotalTimeInSec = u.i[0];
                                                    RandomMemWrite(CIP_DOSING_TOTAL_TIME_IN_SECa,u.c,CIP_DOSING_TOTAL_TIME_IN_SECn);
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_DosingTotalTimeInSec);
                                                }
                                                else
                                                    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
                                            }
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                        #endif
									}
									break;
                                    case 8:{	//CIP_PulseTotalTimeInSec
										#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                            if(strcmp(CommandFormat.Para,"?")==0){
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseTotalTimeInSec);
                                            }
                                            else{
                                                sscanf(CommandFormat.Para,"%d",u.i);
                                                if(u.i[0] >= CIP_PULSE_TOTAL_TIME_MIN_VAL && u.i[0] <= CIP_PULSE_TOTAL_TIME_MAX_VAL){
                                                    CIP_PulseTotalTimeInSec = u.i[0];
                                                    RandomMemWrite(CIP_FLOW_PULSED_TOTAL_TIME_IN_SECa,u.c,CIP_FLOW_PULSED_TOTAL_TIME_IN_SECn);
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseTotalTimeInSec);
                                                }
                                                else
                                                    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
                                            }
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                        #endif
									}
									break;
                                    case 9:{	//CIP_PulseOnTimeInSec
										#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                            if(strcmp(CommandFormat.Para,"?")==0){
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseOnTimeInSec);
                                            }
                                            else{
                                                sscanf(CommandFormat.Para,"%d",u.i);
                                                if(u.i[0] >= CIP_PULSE_ON_TIME_MIN_VAL && u.i[0] <= CIP_PULSE_ON_TIME_MAX_VAL){
                                                    CIP_PulseOnTimeInSec = u.i[0];
                                                    RandomMemWrite(CIP_FLOW_PULSE_ON_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_ON_TIME_IN_SECn);
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseOnTimeInSec);
                                                }
                                                else
                                                    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                            }
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                        #endif
									}
									break;
                                    case 10:{	//CIP_PulseOffTimeInSec
										#if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                            if(strcmp(CommandFormat.Para,"?")==0){
											    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseOffTimeInSec);
                                            }
                                            else{
                                                sscanf(CommandFormat.Para,"%d",u.i);
                                                if(u.i[0] >= CIP_PULSE_OFF_TIME_MIN_VAL && u.i[0] <= CIP_PULSE_OFF_TIME_MAX_VAL){
                                                    CIP_PulseOffTimeInSec = u.i[0];
                                                    RandomMemWrite(CIP_FLOW_PULSE_OFF_TIME_IN_SECa,u.c,CIP_FLOW_PULSE_OFF_TIME_IN_SECn);
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_PulseOffTimeInSec);
                                                }
                                                else
                                                    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
                                            }
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                        #endif
									}
									break;
                                    case 11:{	//CIP_FlushTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_FlushTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= CIP_FLUSH_TIME_MIN_VAL && u.i[0] <= CIP_FLUSH_TIME_MAX_VAL){
                                                CIP_FlushTimeInSec = u.i[0];
                                                RandomMemWrite(CIP_FLUSH_TIME_IN_SECa,u.c,CIP_FLUSH_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_FlushTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 12:{	//WV_CheckTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WV_CheckTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= WV_CHECK_TIME_MIN_VAL && u.i[0] <= WV_CHECK_TIME_MAX_VAL){
                                                WV_CheckTimeInSec = u.i[0];
                                                RandomMemWrite(WV_CHECK_TIME_IN_SECa,u.c,WV_CHECK_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WV_CheckTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 13:{	//WaitHT_TimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WaitHT_TimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= WAIT_HT_TIME_MIN_VAL && u.i[0] <= WAIT_HT_TIME_MAX_VAL){
                                                WaitHT_TimeInSec = u.i[0];
                                                RandomMemWrite(WAIT_HT_TIME_IN_SECa,u.c,WAIT_HT_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WaitHT_TimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 14:{	//TargetPureFlowLperM
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)TargetPureFlowLperM/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= TARGET_PURE_FLOW_MAX_VAL){
                                                TargetPureFlowLperM = u.i[0];
                                                RandomMemWrite(TARGET_PURE_FLOW_LITER_PER_MINa,u.c,TARGET_PURE_FLOW_LITER_PER_MINn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)TargetPureFlowLperM/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 15:{	//LowFlowPurifyAlarmSetpoint
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)LowFlowPurifyAlarmSetpoint/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= PURIFY_LOW_FLOW_MAX_VAL){ 
                                                LowFlowPurifyAlarmSetpoint = u.i[0];
                                                RandomMemWrite(LOW_FLOW_PURIFY_ALARM_SET_POINTa,u.c,LOW_FLOW_PURIFY_ALARM_SET_POINTn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)LowFlowPurifyAlarmSetpoint/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 16:{	//LowFlowWasteAlarmSetpoint
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)LowFlowWasteAlarmSetpoint/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= WASTE_LOW_FLOW_MAX_VAL){ 
                                                LowFlowWasteAlarmSetpoint = u.i[0];
                                                RandomMemWrite(LOW_FLOW_WASTE_ALARM_SET_POINTa,u.c,LOW_FLOW_WASTE_ALARM_SET_POINTn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)LowFlowWasteAlarmSetpoint/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 17:{	//MaxCycleCountForCIP
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MaxCycleCountForCIP);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL && u.i[0] <= MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL){
                                                MaxCycleCountForCIP = u.i[0];
                                                RandomMemWrite(MAX_CYCLE_RUNCOUNT_FOR_CIPa,u.c,MAX_CYCLE_RUNCOUNT_FOR_CIPn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MaxCycleCountForCIP);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 18:{	//CabinetHighTemperatureSetVal
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CabinetHighTemperatureSetVal);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= CABINET_HIGH_TEMP_MIN_VAL && u.i[0] <= CABINET_HIGH_TEMP_MAX_VAL){
                                                CabinetHighTemperatureSetVal = u.i[0];
                                                RandomMemWrite(CABINET_HIGH_TEMPERATURE_SET_VALa,u.c,CABINET_HIGH_TEMPERATURE_SET_VALn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CabinetHighTemperatureSetVal);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 19:{	//MAX_CIP_PumpRunTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MAX_CIP_PumpRunTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= MAX_CIP_PUMP_RUN_TIME_MIN_VAL && u.i[0] <= MAX_CIP_PUMP_RUN_TIME_MAX_VAL){
                                                MAX_CIP_PumpRunTimeInSec = u.i[0];
                                                RandomMemWrite(MAX_CIP_PUMP_RUNTIMEa,u.c,MAX_CIP_PUMP_RUNTIMEn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MAX_CIP_PumpRunTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 20:{	//AnaOutProportionalFactor
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnaOutProportionalFactor/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
											u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] >= ANA_OUT_P_FACTOR_MIN_VAL && u.i[0] <= ANA_OUT_P_FACTOR_MAX_VAL){       //in actual 0.25<= P.Factor <= 2.00  ..since we are working as integer 200 is needed
                                                AnaOutProportionalFactor = u.i[0];
                                                RandomMemWrite(PUMP_P_FACTORa,u.c,PUMP_P_FACTORn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnaOutProportionalFactor/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 21:{	//DynaProportionalFactor
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)DynaProportionalFactor/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
											u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] <= DYNA_P_FACTOR_MAX_VAL){	//its 9.99 in actual..since we are working as integer 999 is needed
                                                DynaProportionalFactor = u.i[0];
                                                RandomMemWrite(DYNAMIC_P_FACTORa,u.c,DYNAMIC_P_FACTORn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)DynaProportionalFactor/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 22:{	//PAE_PureStepVoltageSetpoint
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_PureStepVoltageSetpoint/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= PAE_PURE_STEP_VOLT_MAX_VAL){//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                PAE_PureStepVoltageSetpoint = u.i[0];
                                                RandomMemWrite(PAE_PURE_CYCLE_VOLTAGEa,u.c,PAE_PURE_CYCLE_VOLTAGEn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_PureStepVoltageSetpoint/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 23:{	//PAE_WasteStepVoltageSetpoint
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_WasteStepVoltageSetpoint/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= PAE_WASTE_STEP_VOLT_MAX_VAL){	//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                PAE_WasteStepVoltageSetpoint = u.i[0];
                                                RandomMemWrite(PAE_WASTE_CYCLE_VOLTAGEa,u.c,PAE_WASTE_CYCLE_VOLTAGEn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_WasteStepVoltageSetpoint/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 24:{	//CurrentProportionalFactor
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)CurrentProportionalFactor/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
											u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] >= CURRENT_P_FACTOR_MIN_VAL && u.i[0] <= CURRENT_P_FACTOR_MAX_VAL){
                                                CurrentProportionalFactor = u.i[0];
                                                RandomMemWrite(WASTE_VALUE_CURRENTa,u.c,WASTE_VALUE_CURRENTn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)CurrentProportionalFactor/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 25:{	//FlowMeter.K_Factor
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FlowMeter.K_Factor);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] >= FLOWMETER_K_FACTOR_MIN_VAL && u.i[0] <= FLOWMETER_K_FACTOR_MAX_VAL){
                                                FlowMeter.K_Factor = u.i[0];
                                                RandomMemWrite(FLOWMETER_K_FACTORa,u.c,FLOWMETER_K_FACTORn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FlowMeter.K_Factor);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 26:{	//VolumeUnit
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,VolumeUnit);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= VOLUME_UNIT_MAX_VAL){
                                                VolumeUnit = u.i[0];
                                                u.c[0] = VolumeUnit;
                                                RandomMemWrite(VOLUME_UNITa,u.c,VOLUME_UNITn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,VolumeUnit);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 27:{	//BypassOption
										if(strcmp(CommandFormat.Para,"?")==0){
											#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,BypassOption);
                                            #else
                                                sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                            #endif
										}
										else{
											#if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                                sscanf(CommandFormat.Para,"%d",u.i);
                                                if(u.i[0] <= BYPASS_OPTION_MAX_VAL){
                                                    BypassOption = u.i[0];
                                                    u.c[0] = BypassOption;
                                                    RandomMemWrite(BYPASS_OPTIONa,u.c,BYPASS_OPTIONn);
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,BypassOption);
                                                }
                                                else
                                                    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR); 
                                            #else
                                                sprintf(PasteBuff,",%d.%d=0",main_id,sub_id);
                                            #endif
										}
									}
									break;
                                    case 28:{	////AnalogPressureStartLevel
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureStartLevel/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] >= START_PRESSURE_MIN_VAL && u.i[0] <= START_PRESSURE_MAX_VAL){
                                                AnalogPressureStartLevel = u.i[0];
                                                RandomMemWrite(ANALOG_PRESSURE_START_LEVELa,u.c,ANALOG_PRESSURE_START_LEVELn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureStartLevel/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 29:{	////AnalogPressureStopLevel
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureStopLevel/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] >= STOP_PRESSURE_MIN_VAL && u.i[0] <= STOP_PRESSURE_MAX_VAL){
                                                AnalogPressureStopLevel = u.i[0];
                                                RandomMemWrite(ANALOG_PRESSURE_STOP_LEVELa,u.c,ANALOG_PRESSURE_STOP_LEVELn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureStopLevel/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 30:{	////AnalogPressureBypassLevel
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureBypassLevel/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] <= BYPASS_PRESSURE_MAX_VAL){
                                                AnalogPressureBypassLevel = u.i[0];
                                                RandomMemWrite(ANALOG_PRESSURE_BYPASS_LEVELa,u.c,ANALOG_PRESSURE_BYPASS_LEVELn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureBypassLevel/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 31:{	////AnalogPressureCipLevel
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureCipLevel/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											if(u.i[0] >= CIP_PRESSURE_MIN_VAL && u.i[0] <= CIP_PRESSURE_MAX_VAL){
                                                AnalogPressureCipLevel = u.i[0];
                                                RandomMemWrite(ANALOG_PRESSURE_CIP_LEVELa,u.c,ANALOG_PRESSURE_CIP_LEVELn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)AnalogPressureCipLevel/100);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 32:{	//CIP_WaitTimeInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_WaitTimeInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= CIP_WAIT_TIME_MAX_VAL){
                                                CIP_WaitTimeInSec = u.i[0];
                                                RandomMemWrite(CIP_WAIT_TIME_IN_SECa,u.c,CIP_WAIT_TIME_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,CIP_WaitTimeInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 33:{	//WaitTimeB4BypassAlarmInSec
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WaitTimeB4BypassAlarmInSec);
										}
										else{
											sscanf(CommandFormat.Para,"%d",u.i);
											if(u.i[0] <= WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL){
                                                WaitTimeB4BypassAlarmInSec = u.i[0];
                                                RandomMemWrite(WAIT_TIME_B4_BYPASS_ALARM_IN_SECa,u.c,WAIT_TIME_B4_BYPASS_ALARM_IN_SECn);
                                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,WaitTimeB4BypassAlarmInSec);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 34:{	//SP_EC_ZERO_CYCLEa
										if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",u.i);
											RandomMemWrite(SP_EC_ZERO_CYCLEa,u.c,SP_EC_ZERO_CYCLEn);
                                        }
                                        DirectMemRead(SP_EC_ZERO_CYCLEa,u.c,SP_EC_ZERO_CYCLEn);
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.c[0]);
                                    }
									break;
                                    
                                    case 38:{	//SwitchPAEvoltWaste
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)SwitchPAEvoltWaste/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.c[0] = (ui)(fTemp * 100);//to omit decimal point
                                            SwitchPAEvoltWaste = u.c[0];
                                            RandomMemWrite(SWITCH_PAE_VOLT_WASTEa,u.c,SWITCH_PAE_VOLT_WASTEn);
                                            sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)SwitchPAEvoltWaste/100);
                                        }
									}
                                    case 39:{	//PAE_PrepurifyStepVoltageSetpoint
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_PrepurifyStepVoltageSetpoint/10);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 10);//to omit decimal point
											if(u.i[0] <= PAE_PREPURIFY_STEP_VOLT_MAX_VAL){//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                PAE_PrepurifyStepVoltageSetpoint = u.i[0];
                                                RandomMemWrite(PAE_PREPURIFY_CYCLE_VOLTAGEa,u.c,PAE_PREPURIFY_CYCLE_VOLTAGEn);
                                                sprintf(PasteBuff,",%d.%d=%.1f",main_id,sub_id,(float)PAE_PrepurifyStepVoltageSetpoint/10);
                                            }
                                            else
                                                sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
									}
									break;
                                    case 40:{   //OptimizedPAE_CurrentForPrepureStep
                                      if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)OptimizedPAE_CurrentForPrepureStep/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.i[0] = (ui)(fTemp * 100);//to omit decimal point
											//if(u.i[0] <= PAE_PREPURIFY_STEP_VOLT_MAX_VAL){//min val 00.0 max val 12.0 in actual so for actual value divide by 10
                                                OptimizedPAE_CurrentForPrepureStep = u.i[0];
                                                RandomMemWrite(OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPa,u.c,OPTIMIZED_PAE_CURRENT_FOR_PREPURE_STEPn);
                                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)OptimizedPAE_CurrentForPrepureStep/100);
                                            //}
                                            //else
                                            //    sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);    
										}
                                    }
                                    break;
                                    case 41:{	//SwitchPAEvoltPure
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)SwitchPAEvoltPure/100);
										}
										else{
											sscanf(CommandFormat.Para,"%f",&fTemp);
                                            u.c[0] = (ui)(fTemp * 100);//to omit decimal point
                                            SwitchPAEvoltPure = u.c[0];
                                            RandomMemWrite(SWITCH_PAE_VOLT_PUREa,u.c,SWITCH_PAE_VOLT_PUREn);
                                            sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,(float)SwitchPAEvoltPure/100);
                                        }
									}
                                    
                                    /*
									case 99: {//testing default values
										
									}
									break;
									*/
								}
							}
							break;
							
							/*case 402://Reporting Time
								ReadWriteMemoryCommand(main_id,sub_id,DT_INT,REPORTING_TIMEa,REPORTING_TIMEn); 
							break;*/
							/*case 401://OPERATOR name
								ReadWriteMemoryCommand(main_id,sub_id,DT_STR,OPERATOR_NAMEa,OPERATOR_NAMEn); 
							break;*/
							/*
							case 400://PRODUCT registration number
								ReadWriteMemoryCommand(main_id,sub_id,DT_STR,PRODUCT_REG_NOa,PRODUCT_REG_NOn); 
							break;
							*/
                            /*
							case 351:{//email report
								switch(sub_id){
									case 0://email all mem
									break;
									case 1: {//report of today log only
										//Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, TODAY);
										//sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 2: {//report of yesterday log only
										//Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, YESTERDAY);
										//sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 3: {//date wise report
										SendMailFlag = TRUE;
										DailySummaryReportMailFlag = TRUE;
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 4: {//MONTH wise report
										SendMailFlag = TRUE;
										MonthlySummaryReportMailFlag = TRUE;
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 5: {//YEAR wise report
										SendMailFlag = TRUE;
										YearlySummaryReportMailFlag = TRUE;
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 6: {//specific date-time onwards
										sscanf(CommandFormat.Para,"%lld",&SpecificReportDateTime);
										SpecificDateTimeReportMailFlag = TRUE;
										SendMailFlag = TRUE;
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
								}
							}
							break;
							*/
                            /*
							case 350:{//report
								switch(sub_id){
									case 0://download all mem
										Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, ALL);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									break;
									case 1: {//report of today log only
										Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, TODAY);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 2: {//report of yesterday log only
										Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, YESTERDAY);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 3: {//date wise report
										Report(DAILY_LOG_TABLE_START_PAGE, USB_COM, ALL);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									
									case 4: {//MONTH wise report
										Report(MONTHLY_LOG_TABLE_START_PAGE, USB_COM, ALL);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									case 5: {//YEAR wise report
										Report(YEARLY_LOG_TABLE_START_PAGE, USB_COM, ALL);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
									
									case 6: {//10 min record from specific date time
										sscanf(CommandFormat.Para,"%lld",&SpecificReportDateTime);
										Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, SPECIFIC_DATE_TIME);
										sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
                                    case 7:
                                        Report(SECOND_LOG_TABLE_START_PAGE, USB_COM, REBOOT_LOG);
										sprintf(PasteBuff,",%d.%d=DONE",main_id,sub_id);
                                    break;
								}
							}
							break;
                           */
                            case 326:{//digital output
                                switch(sub_id){
									case 1:	{//MIV
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                    		sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0001;
                                            else
                                              DigitalOutput &= ~0x0001;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0001)>0)? 1:0));
                                    }
                                    break;
                                    case 2:	{//BYPASS
                                        #if(PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_CFO)
                                            if(strcmp(CommandFormat.Para,"?")!=0){
                                                sscanf(CommandFormat.Para,"%d",&uiTemp);
                                                if(uiTemp > 0)
                                                  DigitalOutput |= 0x0002;
                                                else
                                                  DigitalOutput &= ~0x0002;
                                                PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                            }
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0002)>0)? 1:0));
                                        #endif
                                        #if(PRODUCT_TYPE==PRODUCT_DIENTRY||PRODUCT_TYPE==PRODUCT_DISERIES)
                                            switch(BypassOption)
                                            {   case BYPASS_INTERNAL:
                                                case BYPASS_EXTERNAL_NO:
                                                    if(strcmp(CommandFormat.Para,"?")!=0){
                                                        sscanf(CommandFormat.Para,"%d",&uiTemp);
                                                        if(uiTemp > 0)
                                                          DigitalOutput |= 0x0002;
                                                        else
                                                          DigitalOutput &= ~0x0002;
                                                        PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                                    }
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0002)>0)? 1:0));
                                                break;
                                                case BYPASS_EXTERNAL_NC:
                                                    if(strcmp(CommandFormat.Para,"?")!=0){
                                                        sscanf(CommandFormat.Para,"%d",&uiTemp);
                                                        if(uiTemp == 0)
                                                          DigitalOutput |= 0x0002;
                                                        else
                                                          DigitalOutput &= ~0x0002;
                                                        PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                                    }
                                                    sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0002)>0)? 0:1));
                                                break;
                                            }
                                        #endif
                                    }
                                    break;
                                    case 3:	{//POV
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0004;
                                            else
                                              DigitalOutput &= ~0x0004;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0004)>0)? 1:0));
                                    }
                                    break;
                                    case 4:	{//WOV
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0008;
                                            else
                                              DigitalOutput &= ~0x0008;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0008)>0)? 1:0));
                                    }
                                    break;
                                    case 5:	{//CIP
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0010;
                                            else
                                              DigitalOutput &= ~0x0010;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0010)>0)? 1:0));
                                    }
                                    break;
                                    case 6:	{//SHUNT
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0020;
                                            else
                                              DigitalOutput &= ~0x0020;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0020)>0)? 1:0));
                                    }
                                    break;
                                    case 7:	{//POLARITY
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0040;
                                            else
                                              DigitalOutput &= ~0x0040;
                                            PrevDigitalOutput = DigitalOutput;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0040)>0)? 1:0));
                                    }
                                    break;
                                    case 8:	{//PAE
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0080;
                                            else
                                              DigitalOutput &= ~0x0080;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0080)>0)? 1:0));
                                    }
                                    break;
                                    case 9:	{//SPARE BUZZER
                                        if(strcmp(CommandFormat.Para,"?")!=0){
                                            sscanf(CommandFormat.Para,"%d",&uiTemp);
                                            if(uiTemp > 0)
                                              DigitalOutput |= 0x0100;
                                            else
                                              DigitalOutput &= ~0x0100;
                                            PrevDigitalOutput = DigitalOutput;//for manual control from server while PCB in IDLE step, But buzzer cannot be operated in this step.
                                        }
                                        sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(((DigitalOutput & 0x0100)>0)? 1:0));
                                    }
                                    break;
                                }
                            }
                            break;
							case 325: {//current system status
								ArrangeCurrentStatus2PasteBuff();							
							}
							break;
							/*
							case 324: {//delete all users
								DeleteAllUsers();
								sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
							}
							break;
							*/
							case 323: {//Reinitialized 
								switch(sub_id){
									case 0:	{//factory setting without chip erase
										InitializeDefaultSetting();
                                        LoadSetting();
                                        sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
                                    case 1:{//factory setting with chip erase//response time is almost 1 min later.
										ChipErase();
                                        InitializeDefaultSetting();
                                        LoadSetting();
                                        sprintf(PasteBuff,",%d.%d=1",main_id,sub_id);
									}
									break;
                                }
								
							}
							break;
							
							case 322: {//MemDebugFlag
                                if(strcmp(CommandFormat.Para,"?")!=0){
                                    sscanf(CommandFormat.Para,"%d",&uiTemp);
                                    MemDebugFlag = uiTemp;
                                }
                                sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,MemDebugFlag);
                            }
							break;
							case 321: {//flash memory unique ID number
								if(strcmp(CommandFormat.Para,"?")==0)
							  		sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,ReadUniqueIdentification());
							  	else
							  		sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
							}
							break;
                            /*
                            case 320:   {//read last log saved
                                //read last log record
                                if(LogPtr > SECOND_LOG_START_ADD)
                                  uiTemp = LogPtr-SECOND_LOG_PACKET_SIZE;
                                else
                                  uiTemp = SECOND_LOG_START_ADD;
                                DirectMemRead(uiTemp,PasteBuff,SECOND_LOG_PACKET_SIZE);
                                u.c[1] = PasteBuff[LT_SECOND_DATEp+1];
                                if(u.c[1] > 0 && u.c[1] < 13)//in case of valid month data
                                {	//date
                                    sprintf(CopyBuff,"\r\n\r\n%d/%d/%d %d:%d:%d",
                                            PasteBuff[LT_SECOND_DATEp],
                                            PasteBuff[LT_SECOND_DATEp+1],
                                            PasteBuff[LT_SECOND_DATEp+2],
                                            PasteBuff[LT_SECOND_TIMEp],
                                            PasteBuff[LT_SECOND_TIMEp+1],
                                            PasteBuff[LT_SECOND_TIMEp+2]);
                                    PutStringUsb(CopyBuff);
                                    //PAE volt
                                    u.i[0] = 0;//CurrentPAEvolt;
                                    u.c[0] = PasteBuff[LT_SECOND_PAE_VOLTp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_PAE_VOLTp+1];// = u.c[1];
                                    sprintf(CopyBuff,",PAE.V=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    // Current Flow
                                    u.i[0] = 0;//AtlasReplacement.CurrentFlow;
                                    u.c[0] = PasteBuff[LT_SECOND_CURRENT_FLOWp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_CURRENT_FLOWp+1];// = u.c[1];
                                    sprintf(CopyBuff,",C.Flow=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    //Avg EC
                                    u.i[0] = 0;//AtlasConductivity;
                                    u.c[0] = PasteBuff[LT_SECOND_AVERAGE_ECp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_AVERAGE_ECp+1];// = u.c[1];
                                    u.c[2] = PasteBuff[LT_SECOND_AVERAGE_ECp+2];// = u.c[2];
                                    sprintf(CopyBuff,",A.EC=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    //Alarm
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_ALARMp];// =  AlarmRegister24Bit;
                                    u.c[1] = PasteBuff[LT_SECOND_ALARMp+1];// =  AlarmRegister24Bit;
                                    u.c[2] = PasteBuff[LT_SECOND_ALARMp+2];// =  AlarmRegister24Bit;
                                    sprintf(CopyBuff,",ALARM=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    ////////////////////////////////////////////////////////////////////////
                                    // CIP PUMP RUN TIME
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_CIP_PUMP_RUN_TIMEp+1];// = u.c[1];
                                    sprintf(CopyBuff,",CIPRUNTIME=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    // WATER TEMPERATURE
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_WATER_TEMPERATUREp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_WATER_TEMPERATUREp+1];// = u.c[1];
                                    sprintf(CopyBuff,",W.TEMP=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    // CABINET TEMPERATURE
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_CABINET_TEMPERATUREp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_CABINET_TEMPERATUREp+1];// = u.c[1];
                                    sprintf(CopyBuff,",C.TEMP=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    // PRESSURE
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_PRESSUREp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_PRESSUREp+1];// = u.c[1];
                                    if(u.i[0] > 32767) //-ve
                                        fTemp  = ((float)(65535 - u.i[0]))/100.0f;
                                    else
                                        fTemp = u.i[0]/100.0f;
                                    sprintf(CopyBuff,",PRESSURE=%.2f",fTemp);
                                    PutStringUsb(CopyBuff);
                                    // ANALOG OUTPUT VOLTAGE MILLI VOLT
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_ANALOG_OUT_MILLIVOLTp+1];// = u.c[1];
                                    sprintf(CopyBuff,",DAC=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    
                                    // DIGITAL INPUT
                                    sprintf(CopyBuff,",IP=%d",PasteBuff[LT_DIGITAL_INPUTp]);
                                    PutStringUsb(CopyBuff);
                                    // DIGITAL OUTPUT
                                    u.i[0] = 0;
                                    u.c[0] = PasteBuff[LT_DIGITAL_OUTPUTp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_DIGITAL_OUTPUTp+1];// = u.c[1];
                                    sprintf(CopyBuff,",DO=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    // % RECOVERY
                                    ucTemp = PasteBuff[LT_PERCENTAGE_RECOVERYp];
                                    sprintf(CopyBuff,",%%R=%d",ucTemp);
                                    PutStringUsb(CopyBuff);
                                    // MODE
                                    ucTemp = PasteBuff[LT_MODEp];
                                    sprintf(CopyBuff,",Mode=%d",ucTemp);
                                    PutStringUsb(CopyBuff);
                                    //Live EC
                                    u.i[0] = 0;//AtlasConductivity;
                                    u.c[0] = PasteBuff[LT_SECOND_LIVE_ECp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_LIVE_ECp+1];// = u.c[1];
                                    u.c[2] = PasteBuff[LT_SECOND_LIVE_ECp+2];// = u.c[2];
                                    sprintf(CopyBuff,",L.EC=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                    //PAE Current
                                    u.i[0] = 0;//CurrentPAEvolt;
                                    u.c[0] = PasteBuff[LT_SECOND_PAE_CURRENTp];// = u.c[0];
                                    u.c[1] = PasteBuff[LT_SECOND_PAE_CURRENTp+1];// = u.c[1];
                                    sprintf(CopyBuff,",PAE.I=%d",u.i[0]);
                                    PutStringUsb(CopyBuff);
                                }
                            }
                            break;
                            */
                            /*case 319:{//flow pulse counter
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FlowPulseCounter);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							*/
                            case 318:{//CurDigitalInput
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%X",main_id,sub_id,CurDigitalInput);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							
							case 317:{//Excel file version
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,EXCEL_FILE_VERSION);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
							case 316:{//password
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,Device.Password);
								}
								else{
									if(strlen(CommandFormat.Para)==4){
										strcpy(Device.Password, CommandFormat.Para);
										RandomMemWrite(DEVICE_PASSWORDa,Device.Password,DEVICE_PASSWORDn);
										sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,Device.Password);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
								}
							}
							break;
							/*case 314:{//ADMIN_EMAIL ID
								if(CommandFormat.AccessRight == ACCESS_RIGHT_ADMIN){
									ReadWriteMemoryCommand(main_id,sub_id,DT_STR,ADMIN_EMAILa,ADMIN_EMAILn); 
								}
								else
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ACCESS_RIGHT_ERROR);
							}
							break;*/
							/*
							case 312:{//SUPER_ADMIN_DEFAULT_MOBILE_NO
								if(strcmp(CommandFormat.Para,"?")==0){
									if(CommandFormat.AccessRight == ACCESS_RIGHT_SUPERADMIN){
										sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,(long long)SUPER_ADMIN_DEFAULT_MOBILE_NO);
									}
									else
										sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ACCESS_RIGHT_ERROR);
								}
								else{
									sscanf(CommandFormat.Para,"%lld",&u.ll);
									ReadWriteMemoryCommand(main_id,sub_id,DT_LONG_LONG,USER_SUPER_ADMIN_MOBa,UT_MOBn); 
									sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,u.ll);
								}
							}
							break;
							*/
							/*
							case 311:{//SecAfterTimeSync
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,SecAfterTimeSync);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 310:{//runtime after reset
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,FreeRunSec);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							*/
							case 309:{//timezone
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=",main_id,sub_id);
									strcatTimeZone(PasteBuff);
								}
								else{
									if(strlen(CommandFormat.Para)>0){
										if(SetDeviceTimeZone(CommandFormat.Para)==0){
											sprintf(PasteBuff,",%d.%d=",main_id,sub_id);
											strcatTimeZone(PasteBuff);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,SYNTAX_ERROR);
										}
									}
									else{
										#if (COUNTRY == INDIA)
										strcpy(CommandFormat.Para,"+05:30");
										#endif
										#if (COUNTRY == NEPAL)
										strcpy(CommandFormat.Para,"+05:45");
										#endif
										SetDeviceTimeZone(CommandFormat.Para);
										sprintf(PasteBuff,",%d.%d=",main_id,sub_id);
										strcatTimeZone(PasteBuff);
									}
								}
							}
							break;
							case 308:{//device date
								switch(sub_id){
									case 0:	{//date
                                                if(strcmp(CommandFormat.Para,"?")==0){
                                                    _ttemp = TimeStamp2Sec(Device.TimeStamp);
                                                    _ttemp += SecAfterTimeSync;
                                                    TimeStampPtr = localtime(&_ttemp);
                                                    TimeStampPtr->tm_year -= 100;
                                                    TimeStampPtr->tm_mon += 1; 	
                                                    sprintf(PasteBuff,",%d.%d=%2d%2d%2d",//YYMMDD
                                                            main_id,
                                                            sub_id,
                                                            TimeStampPtr->tm_year,
                                                            TimeStampPtr->tm_mon,
                                                            TimeStampPtr->tm_mday);
                                                    FindAndReplaceCharInString(PasteBuff,' ','0');
                                                }
                                                else{
                                                    //sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                                    sscanf(CommandFormat.Para,"%d",&uiTemp);//in yymmdd format
                                                    u.c[0] = uiTemp%100;//date
                                                    uiTemp /= 100;
                                                    u.c[1] = uiTemp%100;//month
                                                    uiTemp /= 100;
                                                    u.c[2] = uiTemp;//year
                                                    if(ValidateDate(u.c[0],u.c[1],u.c[2]) == TRUE)
                                                    {   Device.TimeStamp.tm_year = u.c[2];
                                                        Device.TimeStamp.tm_mon = u.c[1];
                                                        Device.TimeStamp.tm_mday = u.c[0];
                                                        sprintf(PasteBuff,",%d.%d=%2d%2d%2d",//YYMMDD
                                                                main_id,
                                                                sub_id,
                                                                Device.TimeStamp.tm_year,
                                                                Device.TimeStamp.tm_mon,
                                                                Device.TimeStamp.tm_mday);
                                                        FindAndReplaceCharInString(PasteBuff,' ','0');
                                                        DateValidFlag = TRUE;
                                                    }
                                                    else
                                                    {   sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                                    }
                                                }
							
                                    }
                                    break;
                                    case 1:{    //read only Date valid flag
                                        if(strcmp(CommandFormat.Para,"?")==0){
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,DateValidFlag);
                                        }
                                        else{
                                            sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
                                    }
                                    break;
                                }
                            }
							break;
							case 307:{// device time
								switch(sub_id){
									case 0:	{   //time related
                                                if(strcmp(CommandFormat.Para,"?")==0){
                                                    _ttemp = TimeStamp2Sec(Device.TimeStamp);
                                                    _ttemp += SecAfterTimeSync;
                                                    TimeStampPtr = localtime(&_ttemp);
                                                    TimeStampPtr->tm_year -= 100;
                                                    TimeStampPtr->tm_mon += 1; 	
                                                    sprintf(PasteBuff,",%d.%d=%2d%2d%2d",
                                                            main_id,
                                                            sub_id,
                                                            TimeStampPtr->tm_hour,
                                                            TimeStampPtr->tm_min,
                                                            TimeStampPtr->tm_sec);
                                                    FindAndReplaceCharInString(PasteBuff,' ','0');
                                                }
                                                else{
                                                    //sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                                    sscanf(CommandFormat.Para,"%d",&uiTemp);
                                                    u.c[0] = uiTemp%100;//sec
                                                    uiTemp /= 100;
                                                    u.c[1] = uiTemp%100;//min
                                                    uiTemp /= 100;
                                                    u.c[2] = uiTemp;//hour
                                                    if(u.c[2] < 24)
                                                    {	if(u.c[1] < 60)
                                                        {	if(u.c[0] < 60)
                                                            {	Device.TimeStamp.tm_hour = u.c[2];
                                                                Device.TimeStamp.tm_min = u.c[1];
                                                                Device.TimeStamp.tm_sec = u.c[0];
                                                                SecAfterTimeSync = 0;
                                                                sprintf(PasteBuff,",%d.%d=%2d%2d%2d",
                                                                        main_id,
                                                                        sub_id,
                                                                        TimeStampPtr->tm_hour,
                                                                        TimeStampPtr->tm_min,
                                                                        TimeStampPtr->tm_sec);
                                                                FindAndReplaceCharInString(PasteBuff,' ','0');
                                                                TimeValidFlag = TRUE;
                                                            }
                                                            else
                                                            {   sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                                            }
                                                        }
                                                        else
                                                        {   sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                                        }
                                                    }
                                                    else
                                                    {   sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,ILLEGAL_VALUE_ERROR);
                                                    }
                                                }
                                    }
                                    break;
                                    case 1:{    //read only Time valid flag
                                        if(strcmp(CommandFormat.Para,"?")==0){
                                            sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,TimeValidFlag);
                                        }
                                        else{
                                            sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
                                        }
                                    }
                                    break;
                                }
							}
							break;
							/*case 306:{//READ ONLY : SIM IMSI NUMBER
								if(strcmp(CommandFormat.Para,"?")==0){
									DirectMemRead(MOBILE_SUBSCRIBER_IDENTITYa,u.c,MOBILE_SUBSCRIBER_IDENTITYn);
									sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,u.ll);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;*/
							/*case 305:{//READ ONLY : IMEI NO.
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,GprsPacket.Imei);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;*/
							/*case 304:{//READ ONLY = COUNTRY
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,COUNTRY_STR);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}*/
							break;
							case 303:{//get serial Info
								ReadSerialInfoInCopyBuff();
								switch(sub_id){
									case 0://serial Valid Flag
										if(strcmp(CommandFormat.Para,"?")==0){
										  	if(CopyBuff[0] == 0)//BYDEFAULT IF MEMORY IS NOT SERIAL CODED THEN IT WILL HAVE 255 else 0
												SerialNoValidFlag = TRUE;
											else
												SerialNoValidFlag = FALSE;
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(SerialNoValidFlag==TRUE)?1:0);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 1://serial no
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,Device.SerialNo);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 2://serial code
									  if(strcmp(CommandFormat.Para,"?")==0){
											u.ll = 0;
                                            u.c[0] = CopyBuff[1];
											u.c[1] = CopyBuff[2];
											u.c[2] = CopyBuff[3];
											u.c[3] = CopyBuff[4];
											sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,u.ll);
										}
										else{
											//validate serial code given by user and if valid program in Security Register
											sscanf(CommandFormat.Para,"%lld",&llTemp);
											ut.ll = llTemp;
                                            CopyBuff[0] = 0;//serial no valid flag
                                            CopyBuff[1] = ut.c[0];
                                            CopyBuff[2] = ut.c[1];
                                            CopyBuff[3] = ut.c[2];
                                            CopyBuff[4] = ut.c[3];
                                            WriteValidSerialInfoFromCopyBuffToSecurityRegister();
                                            SerialNoValidFlag = TRUE;
                                            sprintf(PasteBuff,",%d.%d=%lld",main_id,sub_id,ut.ll);//DONE SUCCESSFUL
                                            //strcpy(PasteBuff,"\r\nSerial Number Set SUCCESSFUL.");	//TEST CODE
                                            //PutStringUsb(PasteBuff);////TEST CODE
                                        }
									break;
								}
							}
							break;
							//temp command code ends
							
							/*
							case 303:{//Serial NO.
								switch(sub_id){
									case 0:	//Serial Valid Flag READ ONLY
										if(strcmp(CommandFormat.Para,"?")==0){
										  sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,(SerialNoValidFlag==TRUE)?1:0);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 1: {//SERIAL NO 
										if(strcmp(CommandFormat.Para,"?")==0){
											DirectMemRead(D_SERIAL_NOa,u.c,D_SERIAL_NOn);
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,u.i[0]);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									}
									break;
									case 2: {//serial Code
										ReadWriteMemoryCommand(main_id,sub_id,DT_LONG_LONG,SERIAL_NUMBER_CODEa,SERIAL_NUMBER_CODEn);
									}
									break;
									case 3: {//check serial No:
										SerialNoCheck();
										sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,"SERIAL NO. CHECKED");
									}
									break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
							case 302:{//READ ONLY = DEVICE_VERSION
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,DEVICE_VERSION);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
							case 301:{//READ ONLY = Model No
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,MODEL_NO);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}	
							}
							break;
							case 300:{//READ ONLY = PRODUCT_NAME
								if(strcmp(CommandFormat.Para,"?")==0){
									sprintf(PasteBuff,",%d.%d=%s",main_id,sub_id,PRODUCT_NAME);
								}
								else{
									sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
								}
							}
							break;
                            case 200:{//spare serial port functions
                                sscanf(CommandFormat.Para,"%s",CopyBuff);
								PutStringSerialPort(CopyBuff);
                                sprintf(PasteBuff,",%d.%d=SENT",main_id,sub_id);
                            }
                            break;
                            /*case 34:{//DI 4
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_DI4); break;
									case 1: 
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput & (1<<Input[3].RegisterBitId))? 1:0));
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 2:	
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=P",main_id,sub_id);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 3: ReadWriteMemoryCommand(main_id,sub_id,DT_STR, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 3 + IT_NAMEp,IT_NAMEn); break;
									case 4: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 3 + IT_REG_BIT_ADDRESSp,IT_REG_BIT_ADDRESSn); break;
									case 5:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_DI4);break;
									case 6: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_DI4); break;
									case 7: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_DI4);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							case 33:{//DI 3
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_DI3); break;
									case 1: 
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput & (1<<Input[2].RegisterBitId))? 1:0));
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 2:	
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=G",main_id,sub_id);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 3: ReadWriteMemoryCommand(main_id,sub_id,DT_STR, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 2 + IT_NAMEp,IT_NAMEn); break;
									case 4: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 2 + IT_REG_BIT_ADDRESSp,IT_REG_BIT_ADDRESSn); break;
									case 5:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_DI3);break;
									case 6: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_DI3); break;
									case 7: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_DI3);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							case 32:{//DI 2
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_DI2); break;
									case 1: 
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput & (1<<Input[1].RegisterBitId))? 1:0));
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 2:	
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=Y",main_id,sub_id);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 3: ReadWriteMemoryCommand(main_id,sub_id,DT_STR, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 1 + IT_NAMEp,IT_NAMEn); break;
									case 4: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 1 + IT_REG_BIT_ADDRESSp,IT_REG_BIT_ADDRESSn); break;
									case 5:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_DI2);break;
									case 6: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_DI2); break;
									case 7: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_DI2);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							case 31:{//DI 1
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_DI1); break;
									case 1: 
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=%d",main_id,sub_id,((CurDigitalInput & (1<<Input[0].RegisterBitId))? 1:0));
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 2:	
										if(strcmp(CommandFormat.Para,"?")==0){
											sprintf(PasteBuff,",%d.%d=O",main_id,sub_id);
										}
										else{
											sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,READ_ONLY_WRITE_ERROR);
										}
									break;
									case 3: ReadWriteMemoryCommand(main_id,sub_id,DT_STR, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 0 + IT_NAMEp,IT_NAMEn); break;
									case 4: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, INPUT_TABLE_START_ADDRESS + INPUT_TABLE_RECORD_SIZE * 0 + IT_REG_BIT_ADDRESSp,IT_REG_BIT_ADDRESSn); break;
									case 5:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_DI1);break;
									case 6: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_DI1); break;
									case 7: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_DI1);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
							/*case 25:{//Temperature
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_TEMPERATURE); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_FLOAT, PP_START_LOC + MY_PID_TEMPERATURE * PP_MAX_BYTE + PP_LOG_VALp,PP_LOG_VALn); break;
									case 2:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_TEMPERATURE);break;
									case 3: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_TEMPERATURE); break;
									case 4: ReadWriteParameterTrigPara1Val(main_id, sub_id, MY_PID_TEMPERATURE, DT_FLOAT); break;
									case 5: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_TEMPERATURE);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
                            
                            case 23:{//Analog Input AnaPressureAval
                                if(strcmp(CommandFormat.Para,"?")!=0){
                                    sscanf(CommandFormat.Para,"%f",&u.f[0]);
									AnaPressureAval = u.f[0];
                                }
                                sprintf(PasteBuff,",%d.%d=%.2f",main_id,sub_id,AnaPressureAval);
                            }
                            break;
                            /*case 22:{//Analog Input 1
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_AI1); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, PP_START_LOC + MY_PID_AI1 * PP_MAX_BYTE + PP_LOG_VALp,PP_LOG_VALn); break;
									case 2:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_AI1);break;
									case 3: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_AI1); break;
									case 4: ReadWriteParameterTrigPara1Val(main_id, sub_id, MY_PID_AI1, DT_INT); break;
									case 5: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_AI1);break;
									case 11: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, PP_START_LOC + MY_PID_AI1 * PP_MAX_BYTE + PP_TRIG_PARA2_VALp,PP_TRIG_PARA2_VALn); break;
									case 12: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, PP_START_LOC + MY_PID_AI1 * PP_MAX_BYTE + PP_TRIG_PARA3_VALp,PP_TRIG_PARA3_VALn); break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
							/*case 21:{//Int power
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_INT_BATTERY); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, PP_START_LOC + MY_PID_INT_BATTERY * PP_MAX_BYTE + PP_LOG_VALp,PP_LOG_VALn); break;
									case 2:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_INT_BATTERY);break;
									case 3: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_INT_BATTERY); break;
									case 4: ReadWriteParameterTrigPara1Val(main_id, sub_id, MY_PID_INT_BATTERY, DT_INT); break;
									case 5: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_INT_BATTERY);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
							/*case 20:{//Ext power
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_EXT_BATTERY); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_INT, PP_START_LOC + MY_PID_EXT_BATTERY * PP_MAX_BYTE + PP_LOG_VALp,PP_LOG_VALn); break;
									case 2:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_EXT_BATTERY);break;
									case 3: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_EXT_BATTERY); break;
									case 4: ReadWriteParameterTrigPara1Val(main_id, sub_id, MY_PID_EXT_BATTERY, DT_INT); break;
									case 5: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_EXT_BATTERY);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;
							*/
							/*case 0:{//time
								switch(sub_id){
									case 0:	ReadWriteParameterControl(main_id,sub_id,MY_PID_TIME); break;
									case 1: ReadWriteMemoryCommand(main_id,sub_id,DT_INT,PP_START_LOC+MY_PID_TIME*PP_MAX_BYTE+PP_LOG_VALp,PP_LOG_VALn); break;
									case 2:	ReadWriteParameterTrigPriority(main_id,sub_id,MY_PID_TIME);break;
									case 3: ReadWriteParameterTrigCondition(main_id,sub_id,MY_PID_TIME); break;
									case 4: ReadWriteParameterTrigPara1Val(main_id, sub_id, MY_PID_TIME, DT_INT); break;
									case 5: ReadWriteParameterTrigMsg(main_id, sub_id, MY_PID_TIME);break;
									default: sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);	break;
								}
							}
							break;*/
							default:{
								sprintf(PasteBuff,",%d.%d=!%d",main_id,sub_id,PARAMETER_ID_ERROR);
							}
							break;
						}
					}
					else{
						sprintf(PasteBuff,",%s=!%d",CommandFormat.Para,COMMAND_ERROR);
						strcat(resbuff,PasteBuff);
						SendCommandReplyToSender(resbuff);
						return 0;
					}
				}
				break;
			}
			if(strlen(resbuff)+strlen(PasteBuff)<FLASH_PAGE_SIZE)
				strcat(resbuff,PasteBuff);
			main_id = 0;
			sub_id = 0;
		}
	}
	SendCommandReplyToSender(resbuff);
	if(LoadSettingFlag==TRUE){
		LoadSettingFlag = FALSE;
		PeriodicSave();	
		LoadSetting();
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void strcatTimeZone(char *strptr)
{	iTemp = Device.TimeZone;
	if(iTemp < 0)
	{	strcat(strptr,"-");
		iTemp *= -1;
	}
	else
		strcat(strptr,"+");
	uiTemp = iTemp/3600;	//hour
	if(uiTemp<10)
		strcat(strptr,"0");
	//I2A(uiTemp,u.c);
	sprintf(u.c,"%d",uiTemp);
	//strcat(PasteBuff,u.c);
	strcat(strptr,u.c);
	//strcat(PasteBuff,":");
	strcat(strptr,":");
	uiTemp = (iTemp - uiTemp*3600)/60;	//min 
	if(uiTemp<10)
		strcat(strptr,"0");
	//I2A(uiTemp,u.c);
	sprintf(u.c,"%d",uiTemp);
	//strcat(PasteBuff,u.c);
	strcat(strptr,u.c);
}	
///////////////////////////////////////////////////////////////////////////////
void SendCommandReplyToSender(char* msg)
{	//PutStringUsb("\r\nInside SendCommandReplyToSender");
  	/*if(CommandFormat.Sender == SENDER_PC)
	{  PutStringUsb(msg);
	}
	else*/ if(CommandFormat.Sender == SENDER_USB)
	{	PutCharUsb('{');
		PutStringUsb(msg);
		PutCharUsb('}');
		//UsbDeviceCdcCom_SendString(msg);
	}
	else if(CommandFormat.Sender == SENDER_SERVER)
	{	PutCharGprs('[');
        GprsCrcCodeOut = 0;
		PutStringGprs(msg);
        PutCharGprs('*');
        sprintf(PasteBuff,"%d",GprsCrcCodeOut);
        PutStringGprs(PasteBuff);
		PutCharGprs(']');
        if(MobileDebugFlag == TRUE)
		{	sprintf(PasteBuff,"\r\nSERVER <- [%s*%d]",msg,GprsCrcCodeOut);
			PutStringUsb(PasteBuff);
		}
	}
	else
	{	if((GprsCR & SMS_CONTROL) > 0)
		{	if(SendSms(CommandFormat.Sender,TRUE,msg)==SUCCESS)
			{	//if(MobileDebugFlag == TRUE)
				//	PutStringUsb("\r\nSMS response sent to User SUCCESSFULLY");
				/*
                //email sms sent
				if(ReturnToMobileStage == MOBILE_STAGE_READY_TO_OPEN_CONNECTION)
				//if((MobileStage == MOBILE_STAGE_READY_TO_OPEN_CONNECTION) && GprsDataModeFlag == FALSE)
				{	//check if admin email id is valid
					DirectMemRead(USER_TABLE_START_ADDRESS+USER_TABLE_RECORD_SIZE+UT_EMAILp,CopyBuff,UT_EMAILn);
					CopyBuff[UT_EMAILn] = '\0';
					bTemp = FALSE;
					if(strlen(CopyBuff) > 5)
					{	if(SearchString(CopyBuff,"@") != -1)
						{	if(SearchString(CopyBuff,".") != -1)
							{	bTemp = TRUE;
							}
						}
					}
					if(bTemp == TRUE)
					{	//email body
						sprintf(PasteBuff,"SMS REPORT\r\nResponse to %lld: %s",CommandFormat.Sender,msg);
					  	//PutStringUsb(PasteBuff);//TESTING CODE
						//email this to admin for record
						MobileStage = MOBILE_STAGE_SIGNAL_STRENGTH_CHECK_EMAIL_SMS;
					}
					else
					{	MobileStage = MOBILE_STAGE_READY_TO_OPEN_CONNECTION;
					}
				}*/
			}
			/*else if(MobileDebugFlag == TRUE)
			{	PutStringUsb("\r\nSMS response sent ERROR");
			}*/
		}
		else//testing code
		  PutStringUsb("\r\nSMS control is INACTIVE");//TESTING CODE
	}
}
///////////////////////////////////////////////////////////////////////////////
char SetDeviceTimeZone(char* para)
{	if(strlen(CommandFormat.Para)>0)
	{	if((CommandFormat.Para[0] == '+' || CommandFormat.Para[0] == '-') && CommandFormat.Para[3] == ':')
		{   u.c[2] = ((CommandFormat.Para[1]-48)*10 + (CommandFormat.Para[2]-48));
			if(u.c[2] < 14)
			{   u.c[3] = ((CommandFormat.Para[4]-48)*10 + (CommandFormat.Para[5]-48));
				if(u.c[3] < 60)
				{   u.i[0] = u.c[2]*3600 + u.c[3]*60;
					if(CommandFormat.Para[0]=='-')
						u.i[0] *= -1;
					Device.TimeZone = u.i[0];
					RandomMemWrite(TIME_ZONEa,u.c,TIME_ZONEn);
					return 0;
				}
			}
		}
	}
	return -1;
}
///////////////////////////////////////////////////////////////////////////////
void CheckCallResponse(uc callerloc)
{	static uc RingCounter=0;
  	RingCounter++;
	switch(callerloc)
	{	case 0://superadmin call
			//treat him as other users
	  		//PutStringUsb("\r\nSUPER Admin Called");
		  	u.c[0] = CALL_ACTION_SMS_LOCATION;
	  		break;
		case 1://admin call
			PutStringUsb("\r\nAdmin Called");
		  	DirectMemRead(ADMIN_CALL_ACTIONa,u.c,ADMIN_CALL_ACTIONn);
			break;
		case 2://driver call
			PutStringUsb("\r\nOperator Called");
		  	DirectMemRead(OPERATOR_CALL_ACTIONa,u.c,OPERATOR_CALL_ACTIONn);
			break;
		case -1://unknown user call
			u.c[0] = -1;//do nothiing let the call come
		  	break;
		default://registered user call
			PutStringUsb("\r\nUser Called");
		  	DirectMemRead(USER_CALL_ACTIONa,u.c,USER_CALL_ACTIONn);
			break;
	}
	
	switch(u.c[0])
	{	case -1://do nothiing let the call ring
	  		RingCounter=0;
		  	break;
		default://treat admin call as user call
			SendGPRScommand("ATH");	//disconnect call
			CallResponsePendingFlag = TRUE;
			CallResponsePendingCaller = CommandFormat.Sender;
			RingCounter=0;
			MobileCommandFlag = FALSE;
			MobileStageWaitSec = 0;
			break;
	}
}