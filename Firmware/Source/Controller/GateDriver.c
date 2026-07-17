// Header
#include "GateDriver.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DeviceObjectDictionary.h"
#include "LowLevel.h"
#include "Delay.h"
#include "math.h"
#include "MemBuffers.h"
#include "Controller.h"
#include "Constraints.h"

// Variables
//
volatile RegulatorState GATE_RegulatorState = RS_None;
//
float RegulatorQp = 0;
float RegulatorQi = 0;
float RegulatorQimax = 0;
float GateVoltageSetpoint = 0;
float dUg = 0;
float dUdiag = 0;
float RegulatorAlowedError = 0;
float GateVoltage = 0; 						// Напряжение, которое должно быть при работе регулятора
float GateVoltageDiag = 0;					// Напряжение, которое должно быть при работе диагностики
Int16U FollowingErrorCounterMax = 0;
Int16U FollowingErrorCounter = 0;
Int16U RegulatorCounter = 0;
Int16U GateValues_Counter = 0;
float DiagVoltThreshold = 0;
float DiagCurrentThreshold = 0;
float DiagCounterThreshold = 0;
float DiagVoltage = 0;						// Устанавливаемое напряжение с которым идет сравнение в процессе диагностики
float DiagCurrent = 0;
Int16U DiagErrorCounter = 0;

// Forward functions
Int16U GATE_ConvertUgToDAC(float Value);
void GATE_SaveToEndpoints(float Voltage, float Current, float Error);

// Functions
//
Int16U GATE_ConvertUgToDAC(float Value)
{
	float Offset = DataTable[REG_UG_SET_B];
	float K = DataTable[REG_UG_SET_K];
	
	float P0 = DataTable[REG_UG_SET_P0];
	float P1 = DataTable[REG_UG_SET_P1];
	float P2 = DataTable[REG_UG_SET_P2];
	
	// Квадратичная корректировка
	float tmp = (float)Value;
	tmp = tmp * tmp * P2 + tmp * P1 + P0;
	
	// Пересчёт в значения ЦАП
	tmp = tmp * K / DAC_REF_VOLTAGE * DAC_RESOLUTION;
	tmp = tmp - Offset;
	
	if(tmp > DAC_RESOLUTION)
		tmp = DAC_RESOLUTION;

	return (tmp > 0) ? (Int16U)tmp : 0;
}
//------------------------------------

void GATE_SetUg(float Value)
{
	Value ? LL_WriteDAC(GATE_ConvertUgToDAC(Value)) : LL_WriteDAC(0);
}
//------------------------------------

void GATE_StartProcess()
{
	// Сначала остановить источник триггера и ADC — иначе OVR при перезарядке DMA
	TIM_Stop(TIM2);
	ADC_SamplingStop(ADC2);
	ADC_InterruptClearFlag(ADC2, OVR);

	DMA_ChannelEnable(DMA_ADC_IGBT_UGIG, false);
	DMA_TransferCompleteReset(DMA2, DMA_IFCR_CGIF1);
	DMA_ChannelReload(DMA_ADC_IGBT_UGIG, 2);
	DMA_ChannelEnable(DMA_ADC_IGBT_UGIG, true);

	// Для IGBT gate идёт только через ADC2/DMA2_CH1.
	// UT2 (DMA1_CH1) заряжается в LOGIC_StartPulse под TIM1.
	if ((Int16U)DataTable[REG_PCB_TIRIS_IGBT] != PCB_IGBT)
	{
		DMA_ChannelEnable(DMA_ADC_UT2_UGIG, false);
		DMA_TransferCompleteReset(DMA1, DMA_IFCR_CGIF1);
		DMA_ChannelReload(DMA_ADC_UT2_UGIG, 2);
		DMA_ChannelEnable(DMA_ADC_UT2_UGIG, true);
		ADC_SamplingStop(ADC1);
		ADC_InterruptClearFlag(ADC1, OVR);
		ADC_SamplingStart(ADC1);
	}

	ADC_SamplingStart(ADC2);
	TIM_Start(TIM2);
}
//------------------------------------

void GATE_StopProcess()
{
	// TIM/ADC стоп до Disable DMA — иначе триггер при EN=0 даёт OVR
	TIM_Stop(TIM2);
	ADC_SamplingStop(ADC2);

	DMA_ChannelEnable(DMA_ADC_IGBT_UGIG, false);
	DMA_ChannelEnable(DMA_ADC_UT2_UGIG, false);

	DMA_TransferCompleteReset(DMA2, DMA_IFCR_CGIF1);
	DMA_TransferCompleteReset(DMA1, DMA_IFCR_CGIF1);
	ADC_InterruptClearFlag(ADC2, OVR);

	GATE_SetUg(0);
}
//------------------------------------

void GATE_CacheVariables()
{
	RegulatorQp = DataTable[REG_REGULATOR_QP];
	RegulatorQi = DataTable[REG_REGULATOR_QI];
	RegulatorQimax = DataTable[REG_REGULATOR_QI_MAX];
	GateVoltageSetpoint = DataTable[REG_UG_SETPOINT];
	dUg = DataTable[REG_UG_SETPOINT]/(DataTable[REG_UG_EDGE_TIME] / TIMER2_uS);
	dUdiag = DataTable[REG_EXT_DIAG_U_REF]/(DataTable[REG_UG_EDGE_TIME] / TIMER2_uS);
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	FollowingErrorCounterMax = (Int16U)DataTable[REG_FOLLOWING_ERR_CNT];
	DiagVoltThreshold = DataTable[REG_EXT_DIAG_U_THRESHOLD]*0.01f;
	DiagCurrentThreshold = DataTable[REG_EXT_DIAG_I_THRESHOLD]*0.01f;
	DiagVoltage = DataTable[REG_EXT_DIAG_U_REF];
	DiagCurrent = DataTable[REG_EXT_DIAG_I_REF];

	//
	GateVoltage = 0;
	GateVoltageDiag = 0;
	RegulatorCounter = 0;
	FollowingErrorCounter = 0;
	GateValues_Counter = 0;
	DiagErrorCounter = 0;

	DiagCounterThreshold = (DataTable[REG_EXT_DIAG_DURATION] * 1000 / TIMER2_uS) * 0.1; // Взята 1\10 часть от макс ошибки,
																						// т.к иначе она не успевает накопиться за время работы

	GATE_RegulatorState = RS_None;
	DataTable[REG_DBG_GATE_STATE] = GATE_RegulatorState;
	DataTable[REG_DBG_GATE_COUNTER] = GateValues_Counter;
}
//------------------------------------

void GATE_RegulatorProcess(float VoltageSample, float CurrentSample)
{
	float RegulatorError = 0, RegulatorOut, Qp, RegulatorVoltage = 0;
	static float Qi = 0;

	// Формирование линейно нарастающего фронта импульса напряжения
	switch(GATE_RegulatorState)
	{
		case RS_None:
		case RS_InProcess:
		case RS_TargetReached:
		case RS_FollowingError:
		case RS_GateShort:
		{
			if(GateVoltage < GateVoltageSetpoint)
			{
				GateVoltage += dUg;
				GATE_RegulatorState = RS_InProcess;
			}
			else
			{
				GateVoltage = GateVoltageSetpoint;
				GATE_RegulatorState = RS_TargetReached;
			}

			RegulatorVoltage = GateVoltage;
			RegulatorError = (RegulatorCounter == 0) ? 0 : (RegulatorVoltage - VoltageSample);

			if(fabsf(RegulatorError / RegulatorVoltage * 100) < RegulatorAlowedError)
			{
				if(FollowingErrorCounter)
					FollowingErrorCounter--;
			}
			else
			{
				FollowingErrorCounter++;

				if(CurrentSample >= DataTable[REG_IG_THRESHOLD])
					GATE_RegulatorState = RS_GateShort;

				else if(FollowingErrorCounter >= FollowingErrorCounterMax && !DataTable[REG_FOLLOWING_ERR_MUTE])
					GATE_RegulatorState = RS_FollowingError;
			}
		}
		break;

		case RS_Diagnostic:
		case RS_DiagShort:
		case RS_DiagDisconnected:
		{
			if(GateVoltageDiag < DiagVoltage)
				GateVoltageDiag += dUdiag;
			else
			{
				GateVoltageDiag = DiagVoltage;

				if((VoltageSample < DiagVoltage * (DiagVoltThreshold + 1))	&& (CurrentSample > DiagCurrent * (DiagCurrentThreshold + 1)))
				{
					if(DiagErrorCounter < DiagCounterThreshold)
						DiagErrorCounter++;
					if(DiagErrorCounter >= DiagCounterThreshold)
					{
						DiagErrorCounter = 0;
						GATE_RegulatorState = RS_DiagShort;
					}
				}

				if((VoltageSample > DiagVoltage * (DiagVoltThreshold + 1))	&& (CurrentSample < DiagCurrent * (DiagCurrentThreshold + 1)))
				{
					if(DiagErrorCounter < DiagCounterThreshold)
						DiagErrorCounter++;
					if(DiagErrorCounter >= DiagCounterThreshold)
					{
						DiagErrorCounter = 0;
						GATE_RegulatorState = RS_DiagDisconnected;
					}
				}
			}
			RegulatorVoltage = GateVoltageDiag;
			RegulatorError = (RegulatorCounter == 0) ? 0 : (RegulatorVoltage - VoltageSample);
		}
		break;
	}

	if (RegulatorCounter == 0)
		Qi = 0;

	Qi += RegulatorError * RegulatorQi;

	if(Qi > RegulatorQimax)
		Qi = RegulatorQimax;

	if(Qi < (-1) * RegulatorQimax)
		Qi = (-1) * RegulatorQimax;

	Qp = RegulatorError * RegulatorQp;

	RegulatorOut = RegulatorVoltage + Qp + Qi;

	GATE_SetUg(RegulatorOut);

	RegulatorCounter++;

	GATE_SaveToEndpoints(VoltageSample, CurrentSample, RegulatorError);
}
//------------------------------------

void GATE_SaveToEndpoints(float Voltage, float Current, float Error)
{
	if(GateValues_Counter < VALUES_x_SIZE)
	{
		MEMBUF_EP_Ug[GateValues_Counter] = Voltage;
		MEMBUF_EP_Ig[GateValues_Counter] = Current;
		MEMBUF_EP_UgErr[GateValues_Counter] = Error;

		GateValues_Counter++;
	}
	DataTable[REG_DBG_GATE_STATE] = GATE_RegulatorState;
	DataTable[REG_DBG_GATE_COUNTER] = GateValues_Counter;
}
//------------------------------------
