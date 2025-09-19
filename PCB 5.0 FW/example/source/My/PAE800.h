#ifndef __PAE800_H__
#define __PAE800_H__

#include "common.h"
#include "i2c.h"

#define PAE800_MODULE_ADDRESS	0xA0

#define PAE_OUTPUT_VOLT_MAX_NO_OF_SAMPLE_COLLECTION 50
#define ChangeOutputPAEvolt(volt_multiplied_by_100) {u.i[0] = volt_multiplied_by_100;WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x70, u.c, 2);}
#define ChangeOutputPAEcurrent(current_multiplied_by_100) {u.i[0] = current_multiplied_by_100;WritePowerSolveModuleRegister(PAE800_MODULE_ADDRESS,0x72, u.c, 2);}

extern boolean_t PAE_I2C_ErrorFlag;
extern boolean_t PAEcommandFlag;
extern int CurrentPAEvolt;
extern int CurrentPAEcurrent;
extern int SwitchPAEvoltWaste;
extern int SwitchPAEvoltPure;
extern uc PaeErrorSecCounter;
extern uc PaeAlarm;
extern uc TempPaeError;

extern void DisablePAEoutput();
extern void EnablePAEoutput(ui volt_multiplied_by_100, ui current_multiplied_by_100);
extern void InitPAE800();
extern void PAE_OnOffRelayControl();
extern void PAE_OperationAsPerModeAndProcessStep();
extern void StepsBeforeSwitchingPolarity();
extern void ReadPowerSolveModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_read);
extern void ReadPowerSolveModuleOutputVoltage();
extern void WritePowerSolveModuleRegister(uc device_address,uc mem_start_add, char *buff, uc no_of_byte_to_write);

//temp function
extern void PAE_ForceI2CStop();

#endif