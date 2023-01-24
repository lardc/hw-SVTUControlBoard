// Header
#include "GateDriver.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "LowLevel.h"
#include "Delay.h"
#include "math.h"
#include "MemBuffers.h"

// Variables
//
bool GateVoltageReched = false;
bool FollowingErrorFlag = false;
float RegulatorQp = 0;
float RegulatorQi = 0;
float RegulatorQimax = 0;
float GateVoltageSetpoint = 0;
float dVg = 0;
float RegulatorAlowedError = 0;
float GateVoltage = 0;
Int16U FollowingErrorCounterMax = 0;
Int16U FollowingErrorCounter = 0;
Int16U RegulatorCounter = 0;
Int16U GateValues_Counter = 0;

// Forward functions
Int16U GATE_ConvertVgToDAC(float Value);
void GATE_SaveToEndpoints(float Voltage, float Error);

// Functions
//
Int16U GATE_ConvertVgToDAC(float Value)
{
	float Offset = DataTable[REG_VG_SET_B];
	float K = DataTable[REG_VG_SET_K];
	
	float P0 = DataTable[REG_VG_SET_P0];
	float P1 = DataTable[REG_VG_SET_P1];
	float P2 = DataTable[REG_VG_SET_P2];
	
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

void GATE_SetVg(float Value)
{
	Value ? LL_WriteDAC(GATE_ConvertVgToDAC(Value)) : LL_WriteDAC(0);
}
//------------------------------------

bool GATE_WaitingVoltage()
{
	return GateVoltageReched;
}
//------------------------------------

void GATE_StartProcess()
{
	TIM_Start(TIM2);
}
//------------------------------------

void GATE_StopProcess()
{
	TIM_Stop(TIM2);
	GATE_SetVg(0);
}
//------------------------------------

void GATE_CacheVariables()
{
	RegulatorQp = DataTable[REG_REGULATOR_QP];
	RegulatorQi = DataTable[REG_REGULATOR_QI];
	RegulatorQimax = DataTable[REG_REGULATOR_QI_MAX];
	GateVoltageSetpoint = DataTable[REG_VG_SETPOINT];
	dVg = DataTable[REG_VG_EDGE_TIME] / TIMER2_uS;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	FollowingErrorCounterMax = (Int16U)DataTable[REG_FOLLOWING_ERR_CNT];
	//
	GateVoltageReched = false;
	FollowingErrorFlag = false;
	GateVoltage = 0;
	RegulatorCounter = 0;
	FollowingErrorCounter = 0;
	GateValues_Counter = 0;
}
//------------------------------------

void GATE_RegulatorProcess(float GateVoltageSample)
{
	float RegulatorError, RegulatorOut, Qp, Qi = 0;

	// Формирование линейно нарастающего фронта импульса напряжения
	if(GateVoltage < GateVoltageSetpoint)
		GateVoltage += dVg;
	else
	{
		GateVoltage = GateVoltageSetpoint;
		GateVoltageReched = true;
	}

	RegulatorError = (RegulatorCounter == 0) ? 0 : (GateVoltage - GateVoltageSample);

	if(fabsf(RegulatorError / GateVoltage * 100) < RegulatorAlowedError)
	{
		if(FollowingErrorCounter)
			FollowingErrorCounter--;
	}
	else
	{
		FollowingErrorCounter++;

		if(FollowingErrorCounter >= FollowingErrorCounterMax && !DataTable[REG_FOLLOWING_ERR_MUTE])
			FollowingErrorFlag = true;
	}

	Qi += RegulatorError * RegulatorQi;

	if(Qi > RegulatorQimax)
		Qi = RegulatorQimax;

	if(Qi < (-1) * RegulatorQimax)
		Qi = (-1) * RegulatorQimax;

	Qp = RegulatorError * RegulatorQp;

	RegulatorOut = GateVoltage + Qp +Qi;

	GATE_SetVg(RegulatorOut);

	RegulatorCounter++;

	GATE_SaveToEndpoints(GateVoltageSample, RegulatorError);
}
//------------------------------------

void GATE_SaveToEndpoints(float Voltage, float Error)
{
	if(GateValues_Counter < VALUES_x_SIZE)
	{
		MEMBUF_EP_Vg[GateValues_Counter] = Voltage;
		MEMBUF_EP_VgErr[GateValues_Counter] = Error;

		GateValues_Counter++;
	}
}

bool GATE_FollowingErrorCheck()
{
	return FollowingErrorFlag;
}
//------------------------------------
