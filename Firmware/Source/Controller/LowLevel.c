// Header
#include "LowLevel.h"

// Include
#include "Board.h"
#include "Delay.h"

// Definitions
#define DAC_CHANNEL_B		BIT15

// Forward functions
void LL_WriteDACx(uint16_t Data);

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//------------------------------------

void LL_IdLowRange(bool State)
{
	GPIO_SetState(GPIO_ID_LOW_RANGE, !State);
}
//------------------------------------

bool LL_IsIdLowRange()
{
	return GPIO_GetState(GPIO_ID_LOW_RANGE);
}
//------------------------------------

void LL_SyncPowerCell(bool State)
{
	GPIO_SetState(GPIO_SYNC_POWER_CELL, State);
}
//------------------------------------

void LL_SyncScope(bool State)
{
	GPIO_SetState(GPIO_SYNC_SCOPE, State);
}
//------------------------------------

void LL_PulseIg(bool State)
{
	GPIO_SetState(GPIO_IG_PULSE, !State);
}
//------------------------------------

void LL_WriteDACx(uint16_t Data)
{
	GPIO_SetState(GPIO_DAC_CS, false);
	SPI_WriteByte(SPI1, Data);
	GPIO_SetState(GPIO_DAC_CS, true);
	DELAY_US(1);

	GPIO_SetState(GPIO_DAC_LDAC, false);
	DELAY_US(1);
	GPIO_SetState(GPIO_DAC_LDAC, true);
	DELAY_US(1);
}
//-----------------------------

void LL_WriteDAC_ChA(uint16_t Data)
{
	LL_WriteDACx(Data & (~DAC_CHANNEL_B));
}
//-----------------------------

void LL_WriteDAC_ChB(uint16_t Data)
{
	LL_WriteDACx(Data | DAC_CHANNEL_B);
}
//-----------------------------
