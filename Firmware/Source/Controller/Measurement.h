#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
void MEASURE_ConvertVd(pFloat32 InputArray, Int16U DataLength);
void MEASURE_ConvertId(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange);
void MEASURE_ConvertVg(pFloat32 InputArray, Int16U DataLength);
//
float MEASURE_Vg(Int16U SampleADC);
float MEASURE_Ig(Int16U SampleADC);
//
float MEASURE_ExtractAverageValues(pFloat32 InputArray, Int16U Size, Int16U ADCPeriod, bool Gate);
//
void MEASURE_ArrayEMA(pFloat32 InputArray, Int16U DataLength);

#endif // __MEASUREMENT_H
