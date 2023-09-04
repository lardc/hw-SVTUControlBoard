// Header
#include "Measurement.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "stdlib.h"

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

void MEASURE_ConvertVg(pFloat32 InputArray, Int16U DataLength)
{
	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_VG_B, REG_VG_K, REG_VG_P0, REG_VG_P1,
			REG_VG_P2, 0);
}
//------------------------------------

void MEASURE_ConvertId(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange)
{
	float RShunt = DataTable[REG_R_SHUNT] / 1000;

	if(CurrentRange)
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

float MEASURE_ExtractMaxValues(pFloat32 InputArray, Int16U Size)
{
	float AverageValue = 0;
	static float InputArrayCopy[VALUES_POWER_DMA_SIZE];

	for (int i = 0; i < Size; i++)
		{
			// Из-за процесса отпирания прибора 1/3 массива содержит ненужные данные, которые необходимо обнулить
			if(i < (Size / 3))
				InputArrayCopy[i] = 0;
			else
				InputArrayCopy[i] = *(InputArray + i);
		}

	qsort(InputArrayCopy, Size, sizeof(*InputArrayCopy), MEASURE_SortCondition);

	for (int i = Size - SAMPLING_AVG_NUM - MAX_SAMPLES_CUTOFF_NUM; i < Size - MAX_SAMPLES_CUTOFF_NUM; ++i)
		AverageValue += *(InputArrayCopy + i);

	return (AverageValue / SAMPLING_AVG_NUM);
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
