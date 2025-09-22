#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
void MEASURE_ConvertUt(pFloat32 InputArray, Int16U DataLength);
void MEASURE_ConvertUt2(pFloat32 InputArray, Int16U DataLength);
void MEASURE_ConvertIt(pFloat32 InputArray, Int16U DataLength, Int16U CurrentRange);
//
float MEASURE_CollectorAverageValue(pFloat32 MEMBUF_DMA_Intermediary, bool TimerConversion);
float MEASURE_GateAverageVoltage();
float MEASURE_GateAverageCurrent();
//
float MEASURE_Ug_ADC_Direct(Int16U SampleADC);
float MEASURE_Ig_ADC_Direct(Int16U SampleADC);
float MEASURE_Ug_DMA(pFloat32 MEMBUF, Int16U Index);
float MEASURE_Ig_DMA(pFloat32 MEMBUF, Int16U Index);
//
float MEASURE_ExtractAverageValues(pFloat32 InputArray, Int16U StartAverage, Int16U Points);
//
void MEASURE_ArrayEMA(pFloat32 InputArray, Int16U DataLength);

#endif // __MEASUREMENT_H
