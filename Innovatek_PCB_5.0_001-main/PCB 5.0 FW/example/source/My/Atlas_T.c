#include "Atlas_T.h"
#include "common.h"
#include "VARIABLE.h"
#include "Atlas_TC.h"
#include "MyUsb.h"

#define RECALIBRATION_SEC 5

boolean_t AtlasTemperatureErrorFlag = FALSE;
int AtlasTemperature = 0;
ui AtlasTemperatureContinuousErrorSec = 0;
/*
void RecalibrateTemperatureIfNeeded()
{   boolean_t result = FALSE;
    if(AtlasTemperatureErrorFlag == TRUE)
    {   if(AtlasTemperatureContinuousErrorSec % RECALIBRATION_SEC == 0)
        {   AtlasTemperatureErrorFlag = FALSE;
            //AtlasTemperatureContinuousErrorSec = 0;
            //Set Atlas Temp to ACTIVE state
            u.c[0] = 1;
            WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,6,u.c,1);
			if(I2C_ErrorFlag == FALSE)
			{   //Clear Atlas Temp^ Mod Calibration
                u.c[0] = 1;
                WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
                if(I2C_ErrorFlag == FALSE)
                {   //Set Atlas Temp^ Mod Calibration
                    u.i[1] = AtlasTemperature;
                    //arrange the 4 byte integer value to send in MSB first 
                    u.c[0] = u.c[7];
                    u.c[1] = u.c[6];
                    u.c[2] = u.c[5];
                    u.c[3] = u.c[4];
                    WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,8,u.c,4);//USE MSB FIRST VARIABLE
                    if(I2C_ErrorFlag == FALSE)
                    {    //Set Atlas Temp^ Mod Calibration Request-Single Point
                        u.c[0] = 2;
                        WriteAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,12,u.c,1);
                        if(I2C_ErrorFlag == FALSE)
                        {   //Get Atlas Temp^ Mod Calibration Confirmation
                            ReadAtlasModuleRegister(ATLAS_TEMPERATURE_MODULE_ADDRESS,13,u.c,1);
                            if(I2C_ErrorFlag == FALSE && u.c[0] == 1)
                            {    result = TRUE;
                            }
                        }
                    }
                }
            }
                
            if(result == FALSE)
            {   PutStringUsb("\r\nATLAS Temperature-> Auto Calibration Error");
            }
        }
    }
}
*/