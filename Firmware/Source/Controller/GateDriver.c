// Header
#include "GateDriver.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "LowLevel.h"

// Forward functions
uint16_t GATE_ConvertValueToDAC(uint16_t Value, uint16_t RegisterOffset, uint16_t RegisterK, uint16_t RegisterP0,
		uint16_t RegisterP1, uint16_t RegisterP2);
uint16_t GATE_ConvertVgToDAC(uint16_t Value);
uint16_t GATE_ConvertIgToDAC(uint16_t Value);

// Functions
//
void GATE_PulseOutput(bool State)
{
	GPIO_SetState(GPIO_IG_PULSE, !State);
}
//------------------------------------

uint16_t GATE_ConvertValueToDAC(uint16_t Value, uint16_t RegisterOffset, uint16_t RegisterK, uint16_t RegisterP0,
		uint16_t RegisterP1, uint16_t RegisterP2)
{
	float Offset = (float)((int16_t)DataTable[RegisterOffset]);
	float K = (float)DataTable[RegisterK] / 1000;
	
	float P0 = (float)((int16_t)DataTable[RegisterP0]);
	float P1 = (float)DataTable[RegisterP1] / 1000;
	float P2 = (float)((int16_t)DataTable[RegisterP2]) / 1e6;
	
	// Квадратичная корректировка
	float tmp = (float)Value;
	tmp = tmp * tmp * P2 + tmp * P1 + P0;
	
	// Пересчёт в значения ЦАП
	tmp = tmp * K / DAC_REF_VOLTAGE * DAC_RESOLUTION;
	tmp = tmp - Offset;
	
	return (tmp > 0) ? (uint16_t)tmp : 0;
}
//------------------------------------

uint16_t GATE_ConvertVgToDAC(uint16_t Value)
{
	return GATE_ConvertValueToDAC(Value, REG_VG_OFFSET, REG_VG_K, REG_VG_P0, REG_VG_P1, REG_VG_P2);
}
//------------------------------------

uint16_t GATE_ConvertIgToDAC(uint16_t Value)
{
	return GATE_ConvertValueToDAC(Value, REG_IG_OFFSET, REG_IG_K, REG_IG_P0, REG_IG_P1, REG_IG_P2);
}
//------------------------------------

void GATE_SetVg(uint16_t Value)
{
	LL_WriteDAC_ChB(GATE_ConvertVgToDAC(Value));
}
//------------------------------------

void GATE_SetIg(uint16_t Value)
{
	LL_WriteDAC_ChA(GATE_ConvertIgToDAC(Value));
}
//------------------------------------
