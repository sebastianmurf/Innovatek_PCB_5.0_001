#ifndef __ATLAS_TC_H__
#define __ATLAS_TC_H__

#include "common.h"
#include "i2c.h"
#include "Atlas_C.h"

#define ATLAS_TEMPERATURE_MODULE_ADDRESS	0xD0
#define ATLAS_CONDUCTIVITY_MODULE_ADDRESS	0xC8

#define TEMPERATURE_COMPENSATED_CONDUCTIVITY
#define DEBUG_EC_CALC    //this not working

extern void ReadAtlasModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_read);
extern void WriteAtlasModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_write);

#endif