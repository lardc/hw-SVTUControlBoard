#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "stdinc.h"

// Functions
//
void LL_ToggleBoardLED();
void LL_SyncLCSU(bool State);
void LL_SyncScope(bool State);
void LL_AnalogInputsSelftTest(bool State);
void LL_ExtIndication(bool State);
void LL_ExtIndicationToggle();
void LL_WriteDAC(Int16U Data);
bool LL_GetSafetyState();
void LL_SetIdRange(bool Range);
bool LL_IdGetRange();

#endif // __LOWLEVEL_H
