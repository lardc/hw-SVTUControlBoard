// Header
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

void LL_IdLowRange(bool State)
{
	GPIO_SetState(GPIO_ID_LOW_RANGE, State);
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

void LL_WriteDAC(uint16_t Data)
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
