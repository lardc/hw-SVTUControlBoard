#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
void MEASURE_ConvertVd(pFloat32 InputArray, uint16_t DataLength);
void MEASURE_ConvertId(pFloat32 InputArray, uint16_t DataLength);
void MEASURE_ConvertIdLow(pFloat32 InputArray, uint16_t DataLength);
void MEASURE_ConvertVg(pFloat32 InputArray, uint16_t DataLength);
//
float MEASURE_Vg(uint16_t SampleADC);
//
float MEASURE_ExtractMaxValues(pFloat32 InputArray, Int16U Size);
Int16U MEASURE_ExtractVoltage(pFloat32 VoltageArray, pFloat32 CurrentArray, Int16U CurrentPoint, Int16U Size);
//
void MEASURE_ArrayEMA(pFloat32 InputArray, uint16_t DataLength);

#endif // __MEASUREMENT_H
