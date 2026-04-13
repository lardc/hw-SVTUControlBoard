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
	float RiseRate;
	Int8U Fault;
	Int8U Problem;
} LCSUData, *pLCSUData;

// Variables
//
LCSUData LCSU_DataArray[LCSU_AMOUNT_MAX] = {0};
static Int16U ActiveLCSUCounter = 0, CachedLCSUStartNid = 0, CachedLCSUMaxCurrent = 0;

// Forward functions
//
void LOGIC_ResetLCSUCurrent();

// Functions
//
void LOGIC_FindLCSU()
{
	ActiveLCSUCounter = 0;
	CachedLCSUStartNid = DataTable[REG_LCSU_START_NID];
	CachedLCSUMaxCurrent = DataTable[REG_LCSU_MAX_CURRENT];
	
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
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
	DataTable[REG_IT_READ_MAX] = CachedLCSUMaxCurrent * ActiveLCSUCounter;
}
// ----------------------------------------

bool LOGIC_UpdateLCSUState()
{
	Int16U Register;
	
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_DEV_STATE, &Register))
				LCSU_DataArray[i].State = Register;
			else
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
		}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_CallCommandForLCSU(Int16U Command)
{
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
			if(!BHL_Call(i + CachedLCSUStartNid, Command))
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_PowerEnableLCSU()
{
	if(!DataTable[REG_LCSU_DETECTED])
	{
		CONTROL_SwitchToFault(DF_INTERFACE);
		return false;
	}

	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive && LCSU_DataArray[i].State == LCSU_None)
			if(!BHL_Call(i + CachedLCSUStartNid, ACT_LCSU_ENABLE_POWER))
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_AreLCSUInStateX(Int16U State)
{
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive && LCSU_DataArray[i].State != State)
			return false;
	}
	
	return true;
}
// ----------------------------------------

bool LOGIC_IsLCSUInFaultOrDisabled()
{
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive && (LCSU_DataArray[i].State == LCSU_Fault || LCSU_DataArray[i].State == LCSU_Disabled))
			return true;
	}
	
	return false;
}
// ----------------------------------------

bool LOGIC_UpdateProblemsOrFaults()
{
	Int16U Register;

	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_PROBLEM, &Register))
			{
				LCSU_DataArray[i].Problem = Register;
				if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_FAULT_REASON, &Register))
					LCSU_DataArray[i].Fault = Register;
				else
				{
					CONTROL_SwitchToFault(DF_INTERFACE);
					return false;
				}
			}
			else
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
		}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_NoIssuesFromLCSU()
{
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(LCSU_DataArray[i].Problem == PROBLEM_LCSU_FOLLOWING_ERROR)
			{
				CONTROL_FinishedWithProblem(PROBLEM_FOLLOWING_ERROR_LCSU);
				return false;
			}

			else if(LCSU_DataArray[i].Problem == PROBLEM_LCSU_SYNC_STOP)
			{
				CONTROL_FinishedWithProblem(PROBLEM_SYNC_STOP_LCSU);
				return false;
			}

			else if(LCSU_DataArray[i].Problem == PROBLEM_LCSU_MANUAL_STOP)
			{
				CONTROL_FinishedWithProblem(PROBLEM_MANUAL_STOP_LCSU);
				return false;
			}

			else if(LCSU_DataArray[i].Problem == PROBLEM_LCSU_TRAPEZE_INDEX)
			{
				CONTROL_FinishedWithProblem(PROBLEM_TRAPEZE_INDEX_LCSU);
				return false;
			}

			else if(LCSU_DataArray[i].Problem == PROBLEM_LCSU_SIN_CALC_FAIL)
			{
				CONTROL_FinishedWithProblem(PROBLEM_SIN_CALC_FAIL_LCSU);
				return false;
			}

			else if(LCSU_DataArray[i].Problem != PROBLEM_NONE)
			{
				CONTROL_FinishedWithProblem(PROBLEM_LCSU_UNKNOWN_PROBLEM);
				return false;
			}

			if(LCSU_DataArray[i].Fault == DF_LCSU_PROBLEM_BATTERY)
			{
				CONTROL_SwitchToFault(DF_PROBLEM_BATTERY_LCSU);
				return false;
			}
		}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_GetLCSURiseRate()
{
	Int16U Register;

	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(BHL_ReadRegister(i + CachedLCSUStartNid, REG_LCSU_RISE_RATE, &Register))
				LCSU_DataArray[i].RiseRate = Register;
			else
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
		}
	}
	return true;
}
// ----------------------------------------

void LOGIC_CalcSyncTime(Int32U *SyncTime, Int32U *OscSyncTime)
{
	float TrapezeTime , RisingPart = 0, Flattop;

	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive && LCSU_DataArray[i].Current != 0)
			RisingPart = fmaxf(RisingPart, LCSU_DataArray[i].Current / LCSU_DataArray[i].RiseRate);
	}

	Flattop =  DataTable[REG_PULSE_DURATION];
	TrapezeTime = RisingPart * 2 + Flattop;

	*SyncTime = (Int32U)(TIME_LCSU_DELAY_AFTER_SYNC + TrapezeTime + TIME_DELAY_AFTER_PULSE);
	*OscSyncTime = (Int32U)(TIME_LCSU_DELAY_AFTER_SYNC + RisingPart + Flattop - TIME_START_FOR_OSC);
}
// ----------------------------------------

bool LOGIC_WriteLCSUConfig()
{
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(!BHL_WriteRegisterFloat(i + CachedLCSUStartNid, REG_LCSU_PULSE_VALUE, LCSU_DataArray[i].Current))
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
			if(!BHL_WriteRegister(i + CachedLCSUStartNid, REG_LCSU_TRAPEZE_DURATION, DataTable[REG_PULSE_DURATION]))
			{
				CONTROL_SwitchToFault(DF_INTERFACE);
				return false;
			}
		}
	}
	return true;
}
// ----------------------------------------

bool LOGIC_SetCurrentForCertainLCSU(Int16U Nid, float Current)
{
	// Nid вне диапазона
	if(Nid < CachedLCSUStartNid || Nid >= (CachedLCSUStartNid + DataTable[REG_LCSU_COUNT_MAX]))
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
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
		{
			if(Current >= CachedLCSUMaxCurrent)
			{
				LCSU_DataArray[i].Current = CachedLCSUMaxCurrent;
				Current -= CachedLCSUMaxCurrent;
			}
			else
			{
				LCSU_DataArray[i].Current = Current;
				Current = 0;
			}
		}
	}
	
	return true;
}
// ----------------------------------------

float LOGIC_GetCurrentSetpoint()
{
	float P0, P1, P2;
	float current = DataTable[REG_IT_SETPOINT];

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
	for(Int16U i = 0; i < DataTable[REG_LCSU_COUNT_MAX]; ++i)
	{
		if(LCSU_DataArray[i].IsActive)
			LCSU_DataArray[i].Current = 0;
	}
}
// ----------------------------------------

void LOGIC_SelectCurrentRange(float Current)
{
	(Current <= DataTable[REG_I_R0_THRESHOLD]) ? LL_SetItRange(true) : LL_SetItRange(false);
}
// ----------------------------------------

void LOGIC_StartPulse()
{
	// Подготовка оцифровки
	IT_DMAFlagsReset();

	DMA_ChannelReload(DMA_ADC_IT_CH, VALUES_POWER_DMA_SIZE);
	DMA_ChannelReload(DMA_ADC_UT_CH, VALUES_POWER_DMA_SIZE);
	if (DataTable[REG_PCB_TIRIS_IGBT] == PCB_IGBT)
	{
		DMA_ChannelReload(DMA_ADC_UT2_UGIG, VALUES_POWER_DMA_SIZE);
		DMA_ChannelEnable(DMA_ADC_UT2_UGIG, true);
	}
	DMA_ChannelEnable(DMA_ADC_IT_CH, true);
	DMA_ChannelEnable(DMA_ADC_UT_CH, true);

	// Запуск оцифровки импульса тока и напряжения в силовой цепи
	ADC_SamplingStart(ADC3);
	TIM_Start(TIM1);

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
			LL_SyncScope(false);
			LL_SyncLCSU(false);
			GATE_StopProcess();

			// Пересчёт значений
			MEASURE_ConvertUt(MEMBUF_DMA_Ut, VALUES_POWER_DMA_SIZE);
			MEASURE_ConvertIt(MEMBUF_DMA_It, VALUES_POWER_DMA_SIZE, LL_ItGetRange());
			if ((DataTable[REG_PCB_VERSION] == PCB_VERSION_20) && (DataTable[REG_PCB_TIRIS_IGBT] == PCB_IGBT))
				MEASURE_ConvertUt2(MEMBUF_DMA_Ut2_UgIg, VALUES_POWER_DMA_SIZE);
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

void LOGIC_GetResults(float *UtResult, float *UtCh2Result, float *ItResult, Int16U UtIndex, Int16U ItIndex)
{
	float UtMaxVal = DataTable[REG_UT_MAX] ? DataTable[REG_UT_MAX] : UT_MAX_VALUE;

	*UtResult = MEASURE_CollectorAverageValue(MEMBUF_DMA_Ut, UtIndex);
	if(((Int16U)DataTable[REG_PCB_VERSION] == PCB_VERSION_20) && (DataTable[REG_PCB_TIRIS_IGBT] == PCB_IGBT))
	{
		*UtCh2Result = MEASURE_CollectorAverageValue(MEMBUF_DMA_Ut2_UgIg, UtIndex);
		*UtResult = (*UtResult > UtMaxVal) ? *UtCh2Result : *UtResult;
	}
	*ItResult = MEASURE_CollectorAverageValue(MEMBUF_DMA_It, ItIndex);
}
// ----------------------------------------

void LOGIC_SaveResults(float UtResult, float ItResult)
{
	DataTable[REG_RESULT_UT] = UtResult;
	DataTable[REG_RESULT_IT] = ItResult;
	DataTable[REG_RESULT_UG] = MEASURE_GateAverageVoltage();
	DataTable[REG_RESULT_IG] = MEASURE_GateAverageCurrent();
}
// ----------------------------------------

bool LOGIC_CheckResults(float UtResult)
{
	float UtMaxVal, UtMinVal;
	UtMaxVal = DataTable[REG_UT_MAX] ? DataTable[REG_UT_MAX] : UT_MAX_VALUE;
	UtMinVal = DataTable[REG_UT_MIN] ? DataTable[REG_UT_MIN] : UT_MIN_VALUE;

	return ((UtResult > UtMaxVal) || (UtResult < UtMinVal));
}
// ----------------------------------------
