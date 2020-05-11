#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "ZwGPIO.h"
#include "ZwSPI.h"

// Functions
//
void LL_ToggleBoardLED();
//
void LL_IFB_EN(bool State);
void LL_RangeSelect(bool State);
void LL_SYNC_SCPC(bool State);
void LL_GATE_EN(bool State);
void LL_SYNC_OSC(bool State);
void LL_DAC_Write(uint16_t Port, uint16_t Data);

#endif //__LOWLEVEL_H
