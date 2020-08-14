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
static uint16_t ActiveCellsCounter = 0, CachedStartNid = 0;

// Functions
//
bool LOGIC_FindCells()
{
	ActiveCellsCounter = 0;
	CachedStartNid = DataTable[REG_PC_START_NID];
	
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
	DataTable[REG_CURRENT_MAX] = DataTable[REG_PC_MAX_CURRENT] * ActiveCellsCounter;
	
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
	/*
	// Nid вне диапазона
	if(Nid < PC_START_ADDR || Nid >= (PC_START_ADDR + LSLPC_COUNT_MAX))
		return false;
	
	// Выбранная ячейка неактивна
	if(!PC_DataArray[Nid - PC_START_ADDR].IsActive)
		return false;
	
	// Ток превышает допустимый диапазон
	if((uint16_t)Current > PC_MAX_CURRENT)
		return false;
	
	// Очистка уставки тока для всех ячеек
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
			PC_DataArray[i].Current = 0;
	}
	
	// Конфигурация требуемой ячейки
	PC_DataArray[Nid - PC_START_ADDR] = (uint16_t)Current;
	*/
	return true;
}
// ----------------------------------------

bool LOGIC_DistributeCurrent(float Current)
{
	/*
	// Определение дробной части уставки тока
	uint16_t IntCurrent = (uint16_t)Current;
	uint16_t FractionCurrent = IntCurrent - (IntCurrent / PC_MAX_CURRENT) * PC_MAX_CURRENT;
	
	// Ток превышает допустимый диапазон
	if(IntCurrent > (PC_MAX_CURRENT * ActiveCellsCounter))
		return false;
	
	// Очистка уставки тока для всех ячеек
	for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
	{
		if(PC_DataArray[i].IsActive)
			PC_DataArray[i].Current = 0;
	}
	
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
				PC_DataArray[i].Current = PC_MAX_CURRENT;
				IntCurrent -= PC_MAX_CURRENT;
			}
		}
	}
	*/
	return true;
}
// ----------------------------------------

bool LOGIC_PulseProcess()
{
	/*
	static uint16_t CellPointer = 0;
	static uint64_t Timeout = 0;
	
	if(!LOGIC_UpdateCellsState())
	{
		LOGIC_HandleCommunicationError();
		return false;
	}
	
	switch (LOGIC_State)
	{
		case LS_PulsePrepare:
			{
				CellPointer = 0;
				LOGIC_State = LS_PulseSetCurrent;
			}
			break;
			
		case LS_PulseSetCurrent:
			{
				// Обработка текущего индекса ячейки
				// Ячейка настраивается, если она активна и для неё определён ток
				if(PC_DataArray[CellPointer].IsActive && PC_DataArray[CellPointer].Current)
				{
					if(PC_DataArray[CellPointer].State == LPC_Ready)
					{
						if(PC_WriteRegister(CellPointer + PC_START_ADDR, LSLPC_REG_PULSE_VALUE,
								PC_DataArray[CellPointer].Current))
							if(PC_Call(CellPointer + PC_START_ADDR, LSLPC_ACT_PULSE_CONFIG))
							{
								Timeout = CONTROL_TimeCounter + PC_CONFIG_PAUSE;
								LOGIC_State = LS_PulseSetCurrentPause;
							}
					}
					else
					{
						LOGIC_State = LS_Error;
						CONTROL_SwitchToFault(FAULT_PC_UNEXPECTED_STATE);
					}
				}
				
				if(++CellPointer == LSLPC_COUNT_MAX && LOGIC_State != LS_Error)
					LOGIC_State = LS_PulseCheckConfig;
			}
			break;
			
		case LS_PulseSetCurrentPause:
			{
				if(CONTROL_TimeCounter > Timeout)
					LOGIC_State = LS_PulseSetCurrent;
			}
			break;
			
		case LS_PulseCheckConfig:
			{
				// Проверка настройки ячеек
				for(uint16_t i = 0; i < LSLPC_COUNT_MAX; ++i)
				{
					if(PC_DataArray[i].IsActive && PC_DataArray[CellPointer].Current)
					{
						if(PC_DataArray[i].State != LPC_PulseConfigReady)
						{
							LOGIC_State = LS_Error;
							CONTROL_SwitchToFault(FAULT_PC_UNEXPECTED_STATE);
						}
					}
				}
				
				// Проверка условия заряда всех ячеек
				if(LOGIC_State != LS_Error)
					LOGIC_State = LS_None;
			}
			break;
	}
	*/
	return true;
}
// ----------------------------------------
