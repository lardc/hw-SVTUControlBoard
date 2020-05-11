// Header
#include "LowLevel.h"

// Functions
//
// LED on board
void LL_ToggleBoardLED()
{
	GPIO_Bit_Toggle(GPIOB, Pin_7);
}
//------------------------------------------------------------------------------

// IFB_EN
void LL_IFB_EN(bool State)
{
	State ? GPIO_Bit_Set(GPIOA, Pin_3) : GPIO_Bit_Rst(GPIOA, Pin_3);
}
//------------------------------------------------------------------------------

// RangeSelect
void LL_RangeSelect(bool State)
{
	State ? GPIO_Bit_Set(GPIOA, Pin_8) : GPIO_Bit_Rst(GPIOA, Pin_8);
}
//------------------------------------------------------------------------------

// SYNC_SCPC
void LL_SYNC_SCPC(bool State)
{
	State ? GPIO_Bit_Set(GPIOA, Pin_15) : GPIO_Bit_Rst(GPIOA, Pin_15);
}
//------------------------------------------------------------------------------

// GATE_EN
void LL_GATE_EN(bool State)
{
	State ? GPIO_Bit_Set(GPIOB, Pin_0) : GPIO_Bit_Rst(GPIOB, Pin_0);
}
//------------------------------------------------------------------------------

// SYNC_OSC
void LL_SYNC_OSC(bool State)
{
	State ? GPIO_Bit_Set(GPIOB, Pin_6) : GPIO_Bit_Rst(GPIOB, Pin_6);
}
//------------------------------------------------------------------------------

// DAC
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
