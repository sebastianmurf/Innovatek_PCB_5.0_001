#ifndef __ATLAS_C_H__
#define __ATLAS_C_H__

#include "common.h"
#define CONDUCTIVITY_MAX_NO_OF_SAMPLE_COLLECTION 11
extern int AtlasConductivity;
extern long long AverageConductivityAccumulator;
extern uc AverageConductivityCounter;

extern void ReadAtlasConductivity();

#endif