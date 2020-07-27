// Header
#include "LowLevel.h"

// Include
#include "Board.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//------------------------------------

void LL_IgFeedback(bool State)
{
	GPIO_SetState(GPIO_IG_FEEDBACK, State);
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

void LL_IgPulse(bool State)
{
	GPIO_SetState(GPIO_IG_PULSE, State);
}
//------------------------------------

void LL_SyncScope(bool State)
{
	GPIO_SetState(GPIO_SYNC_SCOPE, State);
}
//------------------------------------

void LL_DAC_Write(uint16_t Port, uint16_t Data)
{
	Data |= 1<<14;

	Data |= Port;

	GPIO_Bit_Rst(GPIOA, Pin_1);
	SPI_WriteByte(SPI1, Data);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	GPIO_Bit_Set(GPIOA, Pin_1);

	GPIO_Bit_Rst(GPIOA, Pin_2);
	GPIO_Bit_Set(GPIOA, Pin_2);
}
//------------------------------------
