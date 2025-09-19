#ifndef __ATLAS_T_H__
#define __ATLAS_T_H__

#include "common.h"

extern int AtlasTemperature;
extern boolean_t AtlasTemperatureErrorFlag;
extern ui AtlasTemperatureContinuousErrorSec;

extern void RecalibrateTemperatureIfNeeded();
                                    
#endif