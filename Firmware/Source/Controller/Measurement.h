#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
void MEASURE_ConvertVd(uint16_t *InputArray, uint16_t DataLength);
void MEASURE_ConvertId(uint16_t *InputArray, uint16_t DataLength);
void MEASURE_ConvertIdLow(uint16_t *InputArray, uint16_t DataLength);
//
void MEASURE_ConvertVg(uint16_t *InputArray, uint16_t DataLength);
void MEASURE_ConvertIg(uint16_t *InputArray, uint16_t DataLength);
//
float MEASURE_ExtractMaxValues(Int16U *InputArray, Int16U Size);
Int16U MEASURE_ExtractVoltage(Int16U *VoltageArray, Int16U *CurrentArray, Int16U CurrentPoint, Int16U Size);
//
void MEASURE_ArrayEMA(uint16_t *InputArray, uint16_t DataLength);

#endif // __MEASUREMENT_H
