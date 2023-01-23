﻿// Header
#include "LowLevel.h"

// Include
#include "Board.h"
#include "Delay.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//------------------------------------

void LL_SyncLCSU(bool State)
{
	GPIO_SetState(GPIO_SYNC_LCSU, State);
}
//------------------------------------

void LL_SyncScope(bool State)
{
	GPIO_SetState(GPIO_SYNC_OSC, State);
}
//------------------------------------

bool LL_SyncScopeGetState()
{
	return GPIO_GetState(GPIO_SYNC_OSC);
}
//------------------------------------

void LL_AnalogInputsSelftTest(bool State)
{
	GPIO_SetState(GPIO_AIN_ST, State);
}
//------------------------------------

void LL_ExtIndication(bool State)
{
	GPIO_SetState(GPIO_IND_CTRL, State);
}
//------------------------------------

void LL_ExtIndicationToggle()
{
	GPIO_Toggle(GPIO_IND_CTRL);
}
//------------------------------------

void LL_WriteDAC(Int16U Data)
{
	DAC_SetValueCh2(DAC1, Data);
}
//-----------------------------
