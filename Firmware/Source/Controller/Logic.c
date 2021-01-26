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
#include "LowLevel.h"
#include "Interrupts.h"
#include "SysConfig.h"
#include "Delay.h"
#include "GateDriver.h"
#include "MemBuffers.h"
#include "Measurement.h"
#include "ZwDMA.h"
#include "ZwADC.h"
#include "ZwTIM.h"
#include "Global.h"

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
			if(!BHL_WriteRegister(i + CachedStartNid, REG_LSLPC_PULSE_VALUE, PC_DataArray[i].Current * 10))
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

float LOGIC_GetCurrentSetpoint()
{
	float P0 = (float)((int16_t)DataTable[REG_ISET_P0]);
	float P1 = (float)DataTable[REG_ISET_P1] / 1000;
	float P2 = (float)((int16_t)DataTable[REG_ISET_P2]) / 1e6;
	
	float current = (float)DataTable[REG_CURRENT_SETPOINT];
	current = current * (100 + DataTable[REG_CURRENT_OVERSHOOT]) / 100;
	current = current * current * P2 + current * P1 + P0;

	return (current > 0) ? current : 0;
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

void LOGIC_SelectCurrentRange(float Current)
{
	LL_IdLowRange((uint16_t)Current <= DataTable[REG_I_LOW_RANGE_LIMIT]);
}
// ----------------------------------------

void LOGIC_ProcessPulse()
{
	uint16_t GatePulseDelay = DataTable[REG_GATE_PULSE_DELAY];
	uint16_t GatePulseTime = DataTable[REG_GATE_PULSE_TIME];

	// Задание напряжения в цепи управления
	GATE_SetVg(DataTable[REG_VG_VALUE]);
	DELAY_US(TIME_VG_STAB);

	// Подготовка оцифровки
	IT_DMAFlagsReset();
	DMA_ChannelReload(DMA_ADC_IG_CHANNEL, VALUES_IG_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_VG_CHANNEL, VALUES_VG_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_ID_CHANNEL, VALUES_ID_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_VD_CHANNEL, VALUES_VD_DMA_SIZE);
	DMA_ChannelEnable(DMA_ADC_IG_CHANNEL, true);
	DMA_ChannelEnable(DMA_ADC_VG_CHANNEL, true);
	DMA_ChannelEnable(DMA_ADC_ID_CHANNEL, true);
	DMA_ChannelEnable(DMA_ADC_VD_CHANNEL, true);

	// Запуск оцифровки импульса тока и напряжения в силовой цепи
	TIM_Start(TIM1);

	// Запуск импульса тока в силовой цепи
	LL_SyncScope(true);
	LL_SyncPowerCell(true);

	// Задержка сигнала управления
	if(GatePulseDelay)
		DELAY_US(GatePulseDelay);

	// Запуск оцифровки импульса тока и напряжения в цепи управления
	TIM_Start(TIM2);

	// Сигнал отпирания DUT
	GATE_IgPulse(DataTable[REG_IG_VALUE], GatePulseTime);

	// Синхронизация по вершине
	DELAY_US(TIME_PULSE_WIDTH / 2 - GatePulseDelay - GatePulseTime);
	LL_SyncScope(false);

	// Синхронизация ячеек
	DELAY_US(TIME_PULSE_WIDTH / 2);
	LL_SyncPowerCell(false);

	// Завершение оцифровки
	TIM_Stop(TIM1);
	TIM_Stop(TIM2);

	// Пересчёт значений
	MEASURE_ConvertVd((uint16_t *)MEMBUF_DMA_Vd, VALUES_VD_DMA_SIZE);
	if(LL_IsIdLowRange())
		MEASURE_ConvertIdLow((uint16_t *)MEMBUF_DMA_Id, VALUES_ID_DMA_SIZE);
	else
		MEASURE_ConvertId((uint16_t *)MEMBUF_DMA_Id, VALUES_ID_DMA_SIZE);
	MEASURE_ConvertVg((uint16_t *)MEMBUF_DMA_Vg, VALUES_VG_DMA_SIZE);
	MEASURE_ConvertIg((uint16_t *)MEMBUF_DMA_Ig, VALUES_IG_DMA_SIZE);
}
// ----------------------------------------

void LOGIC_SaveToEndpoint(volatile Int16U *InputArray, Int16U *OutputArray, uint16_t InputArraySize)
{
	uint16_t BufferCompression;

	if(InputArraySize >= VALUES_x_SIZE)
		BufferCompression = InputArraySize / VALUES_x_SIZE;
	else
		BufferCompression = 1;

	for(int i = 0; i < InputArraySize; i += BufferCompression)
		*(OutputArray + i / BufferCompression) = *(InputArray + i);
}
// ----------------------------------------

void LOGIC_SaveResults()
{
	DataTable[REG_DUT_VOLTAGE] = MEASURE_InstantVd((uint16_t *)MEMBUF_DMA_Мd);
	DataTable[REG_DUT_CURRENT] = MEASURE_InstantId((uint16_t *)MEMBUF_DMA_Id);

	if((DataTable[REG_DUT_VOLTAGE] > VOLTAGE_MAX_VALUE) || (DataTable[REG_DUT_VOLTAGE] < VOLTAGE_MIN_VALUE))
		DataTable[REG_WARNING] = WARNING_VOLTAGE_OUT_OF_RANGE;
}
// ----------------------------------------
