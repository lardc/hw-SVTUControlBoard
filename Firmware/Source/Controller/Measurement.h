#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
void MEASURE_ConvertVd(pFloat32 InputArray, Int16U DataLength);
void MEASURE_ConvertId(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange);
//
float MEASURE_CollectorAverageCurrent();
float MEASURE_CollectorAverageVoltage();
float MEASURE_GateAverageVoltage();
//
float MEASURE_Vg(Int16U SampleADC);
float MEASURE_Ig(Int16U SampleADC);
//
float MEASURE_ExtractAverageValues(pFloat32 InputArray, Int16U StartAverage, Int16U Points);
//
void MEASURE_ArrayEMA(pFloat32 InputArray, Int16U DataLength);

#endif // __MEASUREMENT_H
