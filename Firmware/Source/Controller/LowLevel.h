#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "stdinc.h"

// Functions
//
void LL_ToggleBoardLED();
void LL_SyncLCSU(bool State);
void LL_SyncScope(bool State);
void LL_AnalogInputsSelfTest(bool State);
void LL_ExtIndication(bool State);
void LL_ExtIndicationToggle();
void LL_WriteDAC(Int16U Data);
bool LL_GetSafetyState();
void LL_SetItRange(bool Range);
bool LL_ItGetRange();
void LL_AnalogInputsDiagGate(bool State);
void LL_SetSafetyState (bool State);

#endif // __LOWLEVEL_H
