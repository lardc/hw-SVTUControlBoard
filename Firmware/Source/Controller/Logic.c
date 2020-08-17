// Header
//
#include "Logic.h"

// Includes
//
#include "Controller.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "BCCIMHighLevel.h"
#include "Constraints.h"

// Types
//
typedef struct __PCStructData
{
	bool IsActive;
	PCDeviceState State;
	uint16_t Current;
} PCData, *pPCData;

// Variables
//
PCData PC_DataArray[LSLPC_COUNT_MAX] = {0};
static uint16_t ActiveCellsCounter = 0, CachedStartNid = 0, CachedCellMaxCurrent = 0;

// Forward functions
//
void LOGIC_ResetCellsCurrent();

// Functions
//
bool LOGIC_FindCells()
{
	ActiveCellsCounter = 0;
	CachedStartNid = DataTable[REG_PC_START_NID];
	CachedCellMaxCurrent = DataTable[REG_PC_MAX_CURRENT];
	
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(BHL_ReadRegister(i + CachedStartNid, REG_LSLPC_DEV_STATE, NULL))
		{
			PC_DataArray[i].IsActive = true;
			++ActiveCellsCounter;
		}
		else
			BHL_ResetError();
	}
	
	DataTable[REG_TOTAL_LSLPC] = ActiveCellsCounter;
	DataTable[REG_CURRENT_MAX] = CachedCellMaxCurrent * ActiveCellsCounter;
	
	return ActiveCellsCounter;
}
// ----------------------------------------

bool LOGIC_UpdateCellsState()
{
	uint16_t Register;
	
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
		{
			if(BHL_ReadRegister(i + CachedStartNid, REG_LSLPC_DEV_STATE, &Register))
				PC_DataArray[i].State = Register;
			else
				return false;
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_CallCommandForCells(uint16_t Command)
{
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
		{
			if(!BHL_Call(i + CachedStartNid, Command))
				return false;
		}
	}

	return true;
}
// ----------------------------------------

bool LOGIC_PowerEnableCells()
{
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
		{
			if(PC_DataArray[i].State == PCDS_None)
			{
				if(!BHL_Call(i + CachedStartNid, ACT_LSLPC_ENABLE_POWER))
					return false;
			}
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_AreCellsInStateX(uint16_t State)
{
	bool result = true;
	
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive && PC_DataArray[i].State != State)
			result = false;
	}
	
	return result;
}
// ----------------------------------------

bool LOGIC_IsCellInFaultOrDisabled(uint16_t Fault, uint16_t Disabled)
{
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].State == Fault || PC_DataArray[i].State == Disabled)
			return true;
	}
	
	return false;
}
// ----------------------------------------

bool LOGIC_WriteCellsConfig()
{
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
		{
			if(!BHL_WriteRegister(i + CachedStartNid, REG_LSLPC_PULSE_VALUE, PC_DataArray[i].Current))
				return false;
		}
	}

	return true;
}
// ----------------------------------------

bool LOGIC_SetCurrentForCertainBlock(uint16_t Nid, float Current)
{
	// Nid вне диапазона
	if(Nid < CachedStartNid || Nid >= (CachedStartNid + LSLPC_COUNT_MAX))
		return false;
	
	// Выбранная ячейка неактивна
	if(!PC_DataArray[Nid - CachedStartNid].IsActive)
		return false;
	
	// Ток превышает допустимый диапазон
	if((uint16_t)Current > DataTable[REG_PC_MAX_CURRENT])
		return false;
	
	// Очистка уставки тока для всех ячеек
	LOGIC_ResetCellsCurrent();
	
	// Конфигурация требуемой ячейки
	PC_DataArray[Nid - CachedStartNid].Current = (uint16_t)Current;

	return true;
}
// ----------------------------------------

bool LOGIC_DistributeCurrent(float Current)
{
	// Определение дробной части уставки тока
	uint16_t IntCurrent = (uint16_t)Current;
	uint16_t FractionCurrent = IntCurrent - (IntCurrent / CachedCellMaxCurrent) * CachedCellMaxCurrent;
	
	// Ток превышает допустимый диапазон
	if(IntCurrent > (CachedCellMaxCurrent * ActiveCellsCounter))
		return false;
	
	// Очистка уставки тока для всех ячеек
	LOGIC_ResetCellsCurrent();
	
	// Запись значений тока
	for(uint16_t i = 0; (i < LSLPC_COUNT_MAX) && (FractionCurrent > 0) && (IntCurrent > 0); ++i)
	{
		if(PC_DataArray[i].IsActive)
		{
			if(FractionCurrent > 0)
			{
				PC_DataArray[i].Current = FractionCurrent;
				IntCurrent -= FractionCurrent;
				FractionCurrent = 0;
				continue;
			}
			
			if(IntCurrent > 0)
			{
				PC_DataArray[i].Current = CachedCellMaxCurrent;
				IntCurrent -= CachedCellMaxCurrent;
			}
		}
	}

	return true;
}
// ----------------------------------------

void LOGIC_ResetCellsCurrent()
{
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
			PC_DataArray[i].Current = 0;
	}
}
// ----------------------------------------
