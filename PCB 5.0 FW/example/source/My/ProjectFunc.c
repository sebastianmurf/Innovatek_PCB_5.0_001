#include "ProjectFunc.h"
#include "Variable.h"
#include "SerialHmi.h"
#include "Atlas_TC.h"
#include "gpio.h"
#include "constants.c"
#include "PAE800.h"

void SwitchFromLogoutToAutoMode(char commandfrom)
{   if(SerialNoValidFlag == TRUE)
    {   StopTimer = 0;
        FirstWasteCycle = TRUE;
        DisplayPage=AUTO_0;
        //AlarmRegister24Bit = 0;//ALL ALARM OFF
        
        // Activate Conductivity Probe by NABEEN on Nov 15 start
        u.c[0] = 1; //Activate Conductivity Probe sensor
        WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
        // Activate Conductivity Probe by NABEEN on Nov 15 end
        
        // Activate TEMPERATURE Probe by NABEEN on Nov 14 2018 start
        //u.c[0] = 1;//Activate
        //WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
        // Activate TEMPERATURE Probe by NABEEN on Nov 14 2018 end
        
        //DigitalOutput = 0xFFFF;//Switch off all output
        DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step.
        Mode = MODE_AUTO;
        if(commandfrom == 0)//FROM KEYPAD
        {   CurrentProcessStep = IDLE;//FREE_RUN_SEC;//IDLE;
        }
        else
            CurrentProcessStep = WAIT;
        
        //reset all PAE alarms if any
        for(ucTemp=0;ucTemp<8;ucTemp++)
        {	if((PaeAlarm & (1<<ucTemp)) > 0)
                RegisterAlarm(8+ucTemp, ALARM_RESOLVED);	
        }
        
        ProcessStepRunSec = 0;
        PeriodicSave();
    }
    else
        DisplayPage = DEV_INFO_3_1;
}
void SwitchFromLogoutToManualCIP()
{   CurrentProcessStep = CIP_DOSING_ON;
    Mode = MODE_MANUAL_CIP;
    DisplayPage=AUTO_0;
    ProcessStepRunSec = 0;
    CIP_DosingRunTimeInSec = 0;
}

void SwitchingFromAutoToLogout()
{   Mode = MODE_LOGOUT;
    //DisablePAEoutput(); //this action is already included in PAE_OperationAsPerModeAndProcessStep() function.

    // Hybernate Conductivity Probe by NABEEN on Nov 15 start
    u.c[0] = 0;//hybernate
    WriteAtlasModuleRegister(ATLAS_CONDUCTIVITY_MODULE_ADDRESS,6,u.c,1);
    // Hybernate Conductivity Probe by NABEEN on Nov 15 end
    
    // Hybernate temperature Probe by NABEEN on Nov 14 2018 start
    //u.c[0] = 0;//hybernate
    //WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
    // Hybernate Temperature Probe by NABEEN on Nov 14 2018 end
    
    DisplayPage = LOGOUT_0;
    CurrentProcessStep = FREE_RUN_SEC;
    DigitalOutput = 0xFFFF;//ALL OUTPUT OFF
    PeriodicSave();
}