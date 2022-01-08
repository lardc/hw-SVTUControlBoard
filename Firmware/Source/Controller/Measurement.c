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
void MEASURE_ConvertADCtoValx(uint16_t *InputArray, uint16_t DataLength, uint16_t RegisterOffset,
		uint16_t RegisterK, uint16_t RegisterP0, uint16_t RegisterP1, uint16_t RegisterP2, float RShunt);
int MEASURE_SortCondition(const void *A, const void *B);

// Functions
//
void MEASURE_ConvertADCtoValx(uint16_t *InputArray, uint16_t DataLength, uint16_t RegisterOffset,
		uint16_t RegisterK, uint16_t RegisterP0, uint16_t RegisterP1, uint16_t RegisterP2, float RShunt)
{
	float Offset = (float)((int16_t)DataTable[RegisterOffset]);
	float K = (float)DataTable[RegisterK] / 1000;
	
	float P0 = (float)((int16_t)DataTable[RegisterP0]);
	float P1 = (float)DataTable[RegisterP1] / 1000;
	float P2 = (float)((int16_t)DataTable[RegisterP2]) / 1e6;
	
	for(uint16_t i = 0; i < DataLength; i++)
	{
		float tmp = ((float)InputArray[i] + Offset) * ADC_REF_VOLTAGE / ADC_RESOLUTION * K;

		if(RShunt)
			tmp = tmp / RShunt;

		tmp = tmp * tmp * P2 + tmp * P1 + P0;
		tmp = (tmp > 0) ? tmp : 0;

		InputArray[i] = (uint16_t)tmp;
	}
}
//------------------------------------

void MEASURE_ConvertVd(uint16_t *InputArray, uint16_t DataLength)
{
	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_VD_OFFSET, REG_VD_K, REG_VD_P0, REG_VD_P1,
			REG_VD_P2, 0);
}
//------------------------------------

void MEASURE_ConvertId(uint16_t *InputArray, uint16_t DataLength)
{
	float RShunt = (float)DataTable[REG_R_SHUNT] / 1000;

	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_ID_OFFSET, REG_ID_K, REG_ID_P0, REG_ID_P1,
			REG_ID_P2, RShunt);
}
//------------------------------------

void MEASURE_ConvertIdLow(uint16_t *InputArray, uint16_t DataLength)
{
	float RShunt = (float)DataTable[REG_R_SHUNT] / 1000;

	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_IDL_OFFSET, REG_IDL_K, REG_IDL_P0, REG_IDL_P1,
			REG_IDL_P2, RShunt);
}
//------------------------------------

void MEASURE_ConvertVg(uint16_t *InputArray, uint16_t DataLength)
{
	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_VG_OFFSET, REG_VG_K, REG_VG_P0, REG_VG_P1,
			REG_VG_P2 , 0);
}
//------------------------------------

void MEASURE_ConvertIg(uint16_t *InputArray, uint16_t DataLength)
{
	MEASURE_ConvertADCtoValx(InputArray, DataLength, REG_IG_OFFSET, REG_IG_K, REG_IG_P0, REG_IG_P1,
			REG_IG_P2, 0);
}
//------------------------------------

float MEASURE_ExtractMaxValues(Int16U *InputArray, Int16U Size)
{
	float AverageValue = 0;
	static Int16U InputArrayCopy[VALUES_POWER_DMA_SIZE];

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

Int16U MEASURE_ExtractVoltage(Int16U *VoltageArray, Int16U *CurrentArray, Int16U CurrentPoint, Int16U Size)
{
	Int32U AverageValue = 0;
	Int16U Index = 0, IndexMax = 0;
	Int16U CurrentMax = 0;

	// Поиск значения CurrentPoint в массиве CurrentArray
	for(int i = (Size / 3); i < Size; i++)
	{
		if(CurrentMax < *(CurrentArray + i))
		{
			CurrentMax = *(CurrentArray + i);
			IndexMax = i;
		}

		if(*(CurrentArray + i) <= CurrentPoint)
		{
			if(!Index)
				Index = i;
		}
		else
			Index = 0;
	}

	if(Index <= (Size / 3))
			Index = IndexMax;

	DataTable[100] = AverageValue;

	// Усредение в точке измерения
	for (int i = Index; i < (Index + SAMPLING_AVG_NUM); i++)
		AverageValue += *(VoltageArray + i);

	DataTable[101] = *(VoltageArray + Index);
	DataTable[102] = Index;
	DataTable[103] = CurrentPoint;
	DataTable[104] = (Int16U) (AverageValue / SAMPLING_AVG_NUM);

	return (AverageValue / SAMPLING_AVG_NUM);
}
//------------------------------------

int MEASURE_SortCondition(const void *A, const void *B)
{
	return (int)(*(uint16_t *)A) - (int)(*(uint16_t *)B);
}
//-----------------------------------------

void MEASURE_ArrayEMA(uint16_t *InputArray, uint16_t DataLength)
{
	for(uint16_t i = 1; i < DataLength; ++i)
		InputArray[i] = (uint16_t)(InputArray[i] * ADC_EMA_FACTOR + (1 - ADC_EMA_FACTOR) * InputArray[i - 1]);
}
//------------------------------------
