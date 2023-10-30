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

// Types
//
typedef struct __LCSUStructData
{
	bool IsActive;
	LCSUState State;
	float Current;
} LCSUData, *pLCSUData;

// Variables
//
LCSUData LCSU_DataArray[LCSU_COUNT_MAX] = {0};
static Int16U ActiveLCSUCounter = 0, CachedLCSUStartNid = 0, CachedLCSUMaxCurrent = 0;

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
	
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_DEV_STATE, NULL))
		{
			LCSU_DataArray[i].IsActive = true;
			++ActiveLCSUCounter;
		}
		else
			BHL_ResetError();
	}
	
	DataTable[REG_LCSU_DETECTED] = ActiveLCSUCounter;
	DataTable[REG_ID_MAX] = CachedLCSUMaxCurrent * ActiveLCSUCounter;
	
	return ActiveLCSUCounter;
}
// ----------------------------------------

bool LOGIC_UpdateLCSUState()
{
	Int16U Register;
	
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
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

bool LOGIC_CallCommandForLCSU(Int16U Command)
{
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
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
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
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

	if(!DataTable[REG_LCSU_DETECTED])
		return false;
	
	return true;
}
// ----------------------------------------

bool LOGIC_AreLCSUInStateX(Int16U State)
{
	bool result = true;
	
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive && LCSU_DataArray[i].State != State)
			result = false;
	}
	
	return result;
}
// ----------------------------------------

bool LOGIC_IsLCSUInFaultOrDisabled(Int16U Fault, Int16U Disabled)
{
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].State == Fault || LCSU_DataArray[i].State == Disabled)
			return true;
	}
	
	return false;
}
// ----------------------------------------

bool LOGIC_WriteLCSUConfig()
{
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(!BHL_WriteRegister(i + CachedLCSUStartNid, REG_LCSU_PULSE_VALUE, LCSU_DataArray[i].Current))
				return false;
		}
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_SetCurrentForCertainLCSU(Int16U Nid, float Current)
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
	// Ток превышает допустимый диапазон
	if(Current > (CachedLCSUMaxCurrent * ActiveLCSUCounter))
		return false;

	// Очистка уставки тока для всех LCSU
	LOGIC_ResetLCSUCurrent();

	// Запись значений и формы импульса тока
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(Current >= CachedLCSUMaxCurrent)
			{
				LCSU_DataArray[i].Current = CachedLCSUMaxCurrent;
				Current -= CachedLCSUMaxCurrent;
			}
			else
				LCSU_DataArray[i].Current = Current;
		}
	}
	
	return true;
}
// ----------------------------------------

float LOGIC_GetCurrentSetpoint()
{
	float P0, P1, P2;
	float current = DataTable[REG_ID_SETPOINT];

	if (current <= DataTable[REG_I_R0_THRESHOLD])
	{
		P0 = DataTable[REG_ISET_R0_P0];
		P1 = DataTable[REG_ISET_R0_P1];
		P2 = DataTable[REG_ISET_R0_P2];
	}
	else
	{
		P0 = DataTable[REG_ISET_R1_P0];
		P1 = DataTable[REG_ISET_R1_P1];
		P2 = DataTable[REG_ISET_R1_P2];
	}
	
	current = current * current * P2 + current * P1 + P0;

	return (current > 0) ? current : 0;
}
// ----------------------------------------

void LOGIC_ResetLCSUCurrent()
{
	for(Int16U i = 0; i < LCSU_COUNT_MAX; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
			LCSU_DataArray[i].Current = 0;
	}
}
// ----------------------------------------

void LOGIC_SelectCurrentRange(float Current)
{
	(Current <= DataTable[REG_I_R0_THRESHOLD]) ? LL_SetIdRange(true) : LL_SetIdRange(false);
}
// ----------------------------------------

void LOGIC_StartPulse()
{
	// Подготовка оцифровки
	IT_DMAFlagsReset();
	DMA_ChannelReload(DMA_ADC_ID_CH, VALUES_POWER_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_VD_CH, VALUES_POWER_DMA_SIZE);
	DMA_ChannelEnable(DMA_ADC_ID_CH, true);
	DMA_ChannelEnable(DMA_ADC_VD_CH, true);

	// Запуск оцифровки импульса тока и напряжения в силовой цепи
	ADC_SamplingStart(ADC3);
	TIM_Start(TIM1);

	// Запуск процесса формирования синхронизации для осциллографа
	TIM_Start(TIM6);
	TIM_Start(TIM7);

	// Запуск импульса тока в силовой цепи
	LL_SyncLCSU(true);
	IsImpulse = true;
}
// ----------------------------------------

bool LOGIC_FinishProcess()
{
	// Завершение оцифровки
	if(IT_DMASampleCompleted())
	{
		TIM_Stop(TIM1);

		TIM_Stop(TIM7);

		IsImpulse = false;
		LL_SyncLCSU(false);
		GATE_StopProcess();

		// Пересчёт значений
		MEASURE_ConvertVd(&MEMBUF_DMA_Vd[0], VALUES_POWER_DMA_SIZE);
		MEASURE_ConvertId(&MEMBUF_DMA_Id[0], VALUES_POWER_DMA_SIZE, LL_IdGetRange());

		return true;
	}
	else
		return false;
}
// ----------------------------------------

void LOGIC_SaveToEndpoint(volatile pFloat32 InputArray, pFloat32 OutputArray, Int16U InputArraySize)
{
	Int16U BufferCompression;

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
	DataTable[REG_RESULT_VD] = MEASURE_ExtractAverageValues((pFloat32)MEMBUF_DMA_Vd, VALUES_POWER_DMA_SIZE, TIMER1_uS);
	DataTable[REG_RESULT_ID] = MEASURE_ExtractAverageValues((pFloat32)MEMBUF_DMA_Id, VALUES_POWER_DMA_SIZE, TIMER1_uS);
	DataTable[REG_RESULT_VG] = MEASURE_ExtractAverageValues((pFloat32)MEMBUF_EP_Vg, VALUES_x_SIZE, TIMER2_uS);

	if((DataTable[REG_RESULT_VD] > VD_MAX_VALUE) || (DataTable[REG_RESULT_VD] < VD_MIN_VALUE))
		DataTable[REG_WARNING] = WARNING_VOLTAGE_OUT_OF_RANGE;

	if((DataTable[REG_RESULT_ID] > ID_MAX_VALUE) || (DataTable[REG_RESULT_ID] < ID_MIN_VALUE))
			DataTable[REG_WARNING] = WARNING_CURRENT_OUT_OF_RANGE;
}
// ----------------------------------------
