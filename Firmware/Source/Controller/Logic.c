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
#include "math.h"

// Definitions
//
#define ARR_START_INDEX_SHIFT			700

// Types
//
typedef struct __LCSUStructData
{
	bool IsActive;
	LCSUState State;
	float Current;
	Int16U PulseMode;
} LCSUData, *pLCSUData;

// Variables
//
LCSUData LCSU_DataArray[LCSU_COUNT_MAX] = {0};
static uint16_t ActiveLCSUCounter = 0, CachedLCSUStartNid = 0, CachedLCSUMaxCurrent = 0;

// Forward functions
//
void LOGIC_ResetLCSUCurrent();

// Functions
//
bool LOGIC_FindLCSU()
{
	ActiveLCSUCounter = 0;
	CachedLCSUStartNid = DataTable[REG_LCSU_START_NID];
	CachedLCSUMaxCurrent = DataTable[REG_LCSU_MAX_CURRENT];
	
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_DEV_STATE, NULL))
		{
			LCSU_DataArray[i].IsActive = true;
			++ActiveLCSUCounter;
		}
		else
			BHL_ResetError();
	}
	
	DataTable[REG_TOTAL_LCSU] = ActiveLCSUCounter;
	DataTable[REG_CURRENT_MAX] = CachedLCSUMaxCurrent * ActiveLCSUCounter;
	
	return ActiveLCSUCounter;
}
// ----------------------------------------

bool LOGIC_UpdateLCSUState()
{
	uint16_t Register;
	
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_DEV_STATE, &Register))
				LCSU_DataArray[i].State = Register;
			else
				return false;
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_CallCommandForLCSU(uint16_t Command)
{
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(!BHL_Call(i + CachedLCSUStartNid, Command))
				return false;
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_PowerEnableLCSU()
{
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(LCSU_DataArray[i].State == LCSU_None)
			{
				if(!BHL_Call(i + CachedLCSUStartNid, ACT_LCSU_ENABLE_POWER))
					return false;
			}
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_AreLCSUInStateX(uint16_t State)
{
	bool result = true;
	
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive && LCSU_DataArray[i].State != State)
			result = false;
	}
	
	return result;
}
// ----------------------------------------

bool LOGIC_IsLCSUInFaultOrDisabled(uint16_t Fault, uint16_t Disabled)
{
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].State == Fault || LCSU_DataArray[i].State == Disabled)
			return true;
	}
	
	return false;
}
// ----------------------------------------

bool LOGIC_WriteLCSUConfig()
{
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(!BHL_WriteRegister(i + CachedLCSUStartNid, REG_LCSU_PULSE_VALUE, LCSU_DataArray[i].Current))
				return false;

			if(!BHL_WriteRegister(i + CachedLCSUStartNid, REG_LCSU_PULSE_MODE, LCSU_DataArray[i].PulseMode ? 1 : 0))
				return false;
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_SetCurrentForCertainLCSU(uint16_t Nid, float Current)
{
	// Nid вне диапазона
	if(Nid < CachedLCSUStartNid || Nid >= (CachedLCSUStartNid + LCSU_COUNT_MAX))
		return false;
	
	// Выбранный блок LCSU не активен
	if(!LCSU_DataArray[Nid - CachedLCSUStartNid].IsActive)
		return false;
	
	// Ток превышает допустимый диапазон
	if(Current > DataTable[REG_LCSU_MAX_CURRENT])
		return false;
	
	// Очистка уставки тока для всех LCSU
	LOGIC_ResetLCSUCurrent();
	
	// Конфигурация требуемого блока LCSU
	LCSU_DataArray[Nid - CachedLCSUStartNid].Current = Current;
	
	return true;
}
// ----------------------------------------

bool LOGIC_DistributeCurrent(float Current)
{
	uint16_t IntCurrent = (uint16_t)Current;
	
	// Ток превышает допустимый диапазон
	if(IntCurrent > (CachedLCSUMaxCurrent * ActiveLCSUCounter))
		return false;
	
	// Определение целой и дробной частей уставки тока
	uint16_t FractionCurrent = IntCurrent % CachedLCSUMaxCurrent;
	uint16_t NoFractionCurrent = IntCurrent - FractionCurrent;

	// Очистка уставки тока для всех LCSU
	LOGIC_ResetLCSUCurrent();
	
	// Запись флага модифицированного синус сигнала
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
			LCSU_DataArray[i].PulseMode = (Int16U)DataTable[REG_PULSE_MODE];
	}

	// Запись значений тока
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(FractionCurrent > 0)
			{
				LCSU_DataArray[i].Current = FractionCurrent;
				FractionCurrent = 0;
			}
			else if(NoFractionCurrent > 0)
			{
				LCSU_DataArray[i].Current = CachedLCSUMaxCurrent;
				NoFractionCurrent -= CachedLCSUMaxCurrent;
			}
			else
				LCSU_DataArray[i].Current = 0;
		}
	}
	
	return true;
}
// ----------------------------------------

float LOGIC_GetCurrentSetpoint()
{
	float P0 = DataTable[REG_ISET_P0];
	float P1 = DataTable[REG_ISET_P1];
	float P2 = DataTable[REG_ISET_P2];
	
	float current = DataTable[REG_CURRENT_SETPOINT];
	current = current * (100 + DataTable[REG_CURRENT_OVERSHOOT]) / 100;
	current = current * current * P2 + current * P1 + P0;

	return (current > 0) ? current : 0;
}
// ----------------------------------------

void LOGIC_ResetLCSUCurrent()
{
	for(uint16_t i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
			LCSU_DataArray[i].Current = 0;
	}
}
// ----------------------------------------

void LOGIC_SelectCurrentRange(float Current)
{
	if(Current <= DataTable[REG_I_LOW_RANGE_LIMIT])
	{

	}
}
// ----------------------------------------

void LOGIC_ProcessPulse()
{
	// Задание напряжения в цепи управления
	GATE_SetVg(DataTable[REG_VG_VALUE]);

	// Подготовка оцифровки
	IT_DMAFlagsReset();
	DMA_ChannelReload(DMA_ADC_IGBT_GATE_CH, VALUES_GATE_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_ID_CH, VALUES_POWER_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_VD_CH, VALUES_POWER_DMA_SIZE);
	DMA_ChannelEnable(DMA_ADC_IGBT_GATE_CH, true);
	DMA_ChannelEnable(DMA_ADC_ID_CH, true);
	DMA_ChannelEnable(DMA_ADC_VD_CH, true);

	// Запуск оцифровки импульса тока и напряжения в силовой цепи
	TIM_Start(TIM1);

	// Запуск импульса тока в силовой цепи
	LL_SyncScope(true);
	LL_SyncLCSU(true);

	// Синхронизация по вершине
	DELAY_US(TIME_PULSE_WIDTH / 2 + (Int16U)DataTable[REG_OSC_SYNC_TUNE_DELAY]);

	// Синхронизация осциллографа, если не задано превышение уставки
	if(!DataTable[REG_CURRENT_OVERSHOOT])
		LL_SyncScope(false);
	else
		TIM_Start(TIM6);	// Запуск таймера определения момента синхронизации

	// Завершение оцифровки
	while(!IT_DMASampleCompleted()){}

	TIM_Stop(TIM1);

	LL_SyncLCSU(false);

	// Пересчёт значений
	MEASURE_ConvertVd((uint16_t *)MEMBUF_DMA_Vd, VALUES_POWER_DMA_SIZE);
	MEASURE_ConvertIdLow((uint16_t *)MEMBUF_DMA_Id, VALUES_POWER_DMA_SIZE);
	MEASURE_ConvertVg((uint16_t *)MEMBUF_DMA_Vg, VALUES_GATE_DMA_SIZE);
}
// ----------------------------------------

void LOGIC_SaveToEndpoint(volatile pFloat32 InputArray, pFloat32 OutputArray, uint16_t InputArraySize)
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
	float Current;

	DataTable[REG_GATE_VOLTAGE] = MEASURE_ExtractMaxValues((uint16_t *)MEMBUF_DMA_Vg, VALUES_GATE_DMA_SIZE);
	//
	Current = MEASURE_ExtractMaxValues((uint16_t *)MEMBUF_DMA_Id, VALUES_POWER_DMA_SIZE);

	if(DataTable[REG_CURRENT_OVERSHOOT])
		Current = Current / ((float)(100 + DataTable[REG_CURRENT_OVERSHOOT]) / 100);

	DataTable[REG_DUT_CURRENT] = Current;
	//
	DataTable[REG_DUT_VOLTAGE] = MEASURE_ExtractVoltage((pFloat32)MEMBUF_DMA_Vd, (pFloat32)MEMBUF_DMA_Id, Current, VALUES_POWER_DMA_SIZE);

	if((DataTable[REG_DUT_VOLTAGE] > VOLTAGE_MAX_VALUE) || (DataTable[REG_DUT_VOLTAGE] < VOLTAGE_MIN_VALUE))
		DataTable[REG_WARNING] = WARNING_VOLTAGE_OUT_OF_RANGE;
}
// ----------------------------------------
