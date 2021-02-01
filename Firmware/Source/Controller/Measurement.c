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
#define MAX_SAMPLES_CUTOFF_NUM		5

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

Int16U MEASURE_InstantValues(Int16U *InputArray, Int16U Size)
{
	Int32U AverageValue = 0;

	qsort(InputArray, Size, sizeof(*InputArray), MEASURE_SortCondition);

	for (int i = Size - SAMPLING_AVG_NUM - MAX_SAMPLES_CUTOFF_NUM; i < Size - MAX_SAMPLES_CUTOFF_NUM; ++i)
		AverageValue += *(InputArray + i);

	return (AverageValue / SAMPLING_AVG_NUM);
}
//------------------------------------

Int16U MEASURE_InstantValuesOnFallEdge(Int16U *Voltage, Int16U *Current, Int16U Size)
{
	Int32U AverageValue = 0;
	Int16U Index = 0;

	qsort((Voltage + Size / 2), (Size / 2), sizeof(*Voltage), MEASURE_SortCondition);
	qsort((Current + Size / 2), (Size / 2), sizeof(*Current), MEASURE_SortCondition);

	for(int i = Size / 2; i < Size; i++)
	{
		if(DataTable[REG_CURRENT_SETPOINT] <= *(Current + i))
		{
			Index = i;
			break;
		}
	}

	for (int i = Index; i < (Index + SAMPLING_AVG_NUM); i++)
		AverageValue += *(Voltage + i);

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
