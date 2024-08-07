﻿// Header
#include "Measurement.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "stdlib.h"
#include "MemBuffers.h"

// Definitions
//
#define SAMPLING_AVG_NUM			15
#define MAX_SAMPLES_CUTOFF_NUM		10

// Forward functions
void MEASURE_ConvertADCtoValx(pFloat32 InputArray, Int16U DataLength, Int16U RegisterOffset,
		Int16U RegisterK, Int16U RegisterP0, Int16U RegisterP1, Int16U RegisterP2, float RShunt);
int MEASURE_SortCondition(const void *A, const void *B);
float MEASURE_ConvertX(Int16U SampleADC, Int16U P2reg, Int16U P1reg, Int16U P0reg, Int16U Kreg, Int16U Breg);


// Functions
//
void MEASURE_ConvertADCtoValx(pFloat32 InputArray, Int16U DataLength, Int16U RegisterOffset,
		Int16U RegisterK, Int16U RegisterP0, Int16U RegisterP1, Int16U RegisterP2, float RShunt)
{
	float Offset = DataTable[RegisterOffset];
	float K = DataTable[RegisterK];
	
	float P0 = DataTable[RegisterP0];
	float P1 = DataTable[RegisterP1];
	float P2 = DataTable[RegisterP2];
	
	for(Int16U i = 0; i < DataLength; i++)
	{
		float tmp = (float)(*((pInt32U)(InputArray + i))) * K + Offset;

		if(RShunt)
			tmp = tmp / RShunt;

		tmp = tmp * tmp * P2 + tmp * P1 + P0;
		tmp = (tmp > 0) ? tmp : 0;

		InputArray[i] = tmp;
	}
}
//------------------------------------

void MEASURE_ConvertVd(pFloat32 InputArray, Int16U DataLength)
{
	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_VD_B, REG_VD_K, REG_VD_P0, REG_VD_P1,
			REG_VD_P2, 0);
}
//------------------------------------

void MEASURE_ConvertId(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange)
{
	float RShunt = DataTable[REG_R_SHUNT] / 1000;

	if(!CurrentRange)
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_ID_R1_B, REG_ID_R1_K, REG_ID_R1_P0, REG_ID_R1_P1, REG_ID_R1_P2, RShunt);
	else
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_ID_R0_B, REG_ID_R0_K, REG_ID_R0_P0, REG_ID_R0_P1, REG_ID_R0_P2, RShunt);
}
//------------------------------------

float MEASURE_ConvertX(Int16U SampleADC, Int16U P2reg, Int16U P1reg, Int16U P0reg, Int16U Kreg, Int16U Breg)
{
	float Result = 0;

	Result = SampleADC * DataTable[Kreg] + DataTable[Breg];
	Result = Result * Result * DataTable[P2reg] + Result * DataTable[P1reg] + DataTable[P0reg];

	return (Result > 0) ? Result : 0;
}
//------------------------------------

float MEASURE_Vg(Int16U SampleADC)
{
	return MEASURE_ConvertX(SampleADC, REG_VG_P2, REG_VG_P1, REG_VG_P0, REG_VG_K, REG_VG_B);
}
//------------------------------------

float MEASURE_Ig(Int16U SampleADC)
{
	return MEASURE_ConvertX(SampleADC, REG_IG_P2, REG_IG_P1, REG_IG_P0, REG_IG_K, REG_IG_B);
}
//------------------------------------

float MEASURE_GateAverageVoltage()
{
	Int16U StartIndex, Points;

	StartIndex = DataTable[REG_VG_EDGE_TIME] / TIMER2_uS + DataTable[REG_MSR_DELAY];
	Points = DataTable[REG_MSR_TIME];

	return MEASURE_ExtractAverageValues((pFloat32)MEMBUF_EP_Vg, StartIndex, Points);
}
//------------------------------------

float MEASURE_CollectorAverageVoltage()
{
	Int16U StartIndex, Points;

	StartIndex = DataTable[REG_MSR_DELAY] * TIMER2_uS / TIMER1_uS;
	Points = DataTable[REG_MSR_TIME] * TIMER2_uS / TIMER1_uS;

	return MEASURE_ExtractAverageValues((pFloat32)MEMBUF_DMA_Vd, StartIndex, Points);
}
//------------------------------------

float MEASURE_CollectorAverageCurrent()
{
	Int16U StartIndex, Points;

	StartIndex = DataTable[REG_MSR_DELAY] * TIMER2_uS / TIMER1_uS;
	Points = DataTable[REG_MSR_TIME] * TIMER2_uS / TIMER1_uS;

	return MEASURE_ExtractAverageValues((pFloat32)MEMBUF_DMA_Id, StartIndex, Points);
}
//------------------------------------

float MEASURE_ExtractAverageValues(pFloat32 InputArray, Int16U StartAverage, Int16U Points)
{
	float SumArray = 0;

	for (int i = StartAverage; i < (StartAverage + Points); i++)
		SumArray += *(InputArray + i);

	return (SumArray / Points);
}
//------------------------------------

int MEASURE_SortCondition(const void *A, const void *B)
{
	return (int)(*(Int16U *)A) - (int)(*(Int16U *)B);
}
//-----------------------------------------

void MEASURE_ArrayEMA(pFloat32 InputArray, Int16U DataLength)
{
	for(Int16U i = 1; i < DataLength; ++i)
		InputArray[i] = InputArray[i] * ADC_EMA_FACTOR + (1 - ADC_EMA_FACTOR) * InputArray[i - 1];
}
//------------------------------------
