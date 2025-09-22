// Header
#include "Measurement.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "stdlib.h"
#include "MemBuffers.h"
#include "Constraints.h"

// Definitions
//
#define SAMPLING_AVG_NUM			15
#define MAX_SAMPLES_CUTOFF_NUM		10

// Forward functions
void MEASURE_ConvertADCtoValx(pFloat32 InputArray, Int16U DataLength, Int16U RegisterOffset,
		Int16U RegisterK, Int16U RegisterP0, Int16U RegisterP1, Int16U RegisterP2, float RShunt);
int MEASURE_SortCondition(const void *A, const void *B);
float MEASURE_ConvertX(Int16U SampleADC, Int16U P2reg, Int16U P1reg, Int16U P0reg, Int16U Kreg, Int16U Breg);
float MEASURE_ConvertX_Array(pFloat32 MEMBUF, Int16U Index, Int16U P2reg, Int16U P1reg, Int16U P0reg, Int16U Kreg, Int16U Breg);


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

void MEASURE_ConvertUt(pFloat32 InputArray, Int16U DataLength)
{
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_UT_B, REG_UT_K, REG_UT_P0, REG_UT_P1,
					REG_UT_P2, 0);
}

void MEASURE_ConvertUt2(pFloat32 InputArray, Int16U DataLength)
{
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_UT2_B, REG_UT2_K, REG_UT2_P0, REG_UT2_P1,
				REG_UT2_P2, 0);
}
//------------------------------------

void MEASURE_ConvertIt(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange)
{
	float RShunt = DataTable[REG_R_SHUNT] / 1000;

	if(!CurrentRange)
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_IT_R1_B, REG_IT_R1_K, REG_IT_R1_P0, REG_IT_R1_P1, REG_IT_R1_P2, RShunt);
	else
		MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_IT_R0_B, REG_IT_R0_K, REG_IT_R0_P0, REG_IT_R0_P1, REG_IT_R0_P2, RShunt);
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

float MEASURE_ConvertX_Array(pFloat32 MEMBUF, Int16U Index, Int16U P2reg, Int16U P1reg, Int16U P0reg, Int16U Kreg, Int16U Breg)
{
	float Result = 0;

	Result = MEMBUF[Index] * DataTable[Kreg] + DataTable[Breg];
	Result = Result * Result * DataTable[P2reg] + Result * DataTable[P1reg] + DataTable[P0reg];

	return (Result > 0) ? Result : 0;
}
//------------------------------------

float MEASURE_Ug_ADC_Direct(Int16U SampleADC)
{
	return MEASURE_ConvertX(SampleADC, REG_UG_P2, REG_UG_P1, REG_UG_P0, REG_UG_K, REG_UG_B);
}
//------------------------------------

float MEASURE_Ug_DMA(pFloat32 MEMBUF, Int16U Index)
{
	return MEASURE_ConvertX_Array(MEMBUF, Index, REG_UG_P2, REG_UG_P1, REG_UG_P0, REG_UG_K, REG_UG_B);
}
//------------------------------------

float MEASURE_Ig_ADC_Direct(Int16U SampleADC)
{
	return MEASURE_ConvertX(SampleADC, REG_IG_P2, REG_IG_P1, REG_IG_P0, REG_IG_K, REG_IG_B);
}
//------------------------------------

float MEASURE_Ig_DMA(pFloat32 MEMBUF, Int16U Index)
{
	return MEASURE_ConvertX_Array(MEMBUF, Index, REG_IG_P2, REG_IG_P1, REG_IG_P0, REG_IG_K, REG_IG_B);
}
//------------------------------------

float MEASURE_GateAverageVoltage()
{
	Int16U StartIndex, Points;

	//Умножение на 1000, чтобы преобразовать мс в мкс
	StartIndex = DataTable[REG_UG_EDGE_TIME] / TIMER2_uS + DataTable[REG_PULSE_DURATION] * 1000 / TIMER2_uS - DataTable[REG_MSR_TIME];
	Points = DataTable[REG_MSR_TIME];

	return MEASURE_ExtractAverageValues((pFloat32)MEMBUF_EP_Ug, StartIndex, Points);
}
//------------------------------------

float MEASURE_GateAverageCurrent()
{
	Int16U StartIndex, Points;

	//Умножение на 1000, чтобы преобразовать мс в мкс
	StartIndex = DataTable[REG_UG_EDGE_TIME] / TIMER2_uS + DataTable[REG_PULSE_DURATION] * 1000 / TIMER2_uS - DataTable[REG_MSR_TIME];
	Points = DataTable[REG_MSR_TIME];

	return MEASURE_ExtractAverageValues((pFloat32)MEMBUF_EP_Ig, StartIndex, Points);
}
//------------------------------------

float MEASURE_CollectorAverageValue(pFloat32 MEMBUF_DMA_Intermediary, bool TimerConversion)
{
	Int16U StartIndex, Points;

	if(TimerConversion)
	{
		StartIndex = DataTable[REG_PULSE_DURATION] * 1000 / TIMER1_uS;
		Points = DataTable[REG_MSR_TIME] * TIMER2_uS / TIMER1_uS;
	}
	else
	{
		StartIndex = DataTable[REG_PULSE_DURATION] * 1000;
		Points = DataTable[REG_MSR_TIME];
	}
	return MEASURE_ExtractAverageValues(MEMBUF_DMA_Intermediary, StartIndex, Points);
}
//------------------------------------

float MEASURE_ExtractAverageValues(pFloat32 InputArray, Int16U StartAverage, Int16U Points)
{
	float SumArray = 0;

	for (int i = StartAverage; i < (StartAverage + Points); i++)
		SumArray += InputArray[i];

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

