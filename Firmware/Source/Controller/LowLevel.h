#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "stdinc.h"

// Functions
//
void LL_ToggleBoardLED();
void LL_IdLowRange(bool State);
void LL_SyncPowerCell(bool State);
void LL_SyncScope(bool State);
void LL_WriteDAC(uint16_t Data);

#endif // __LOWLEVEL_H
