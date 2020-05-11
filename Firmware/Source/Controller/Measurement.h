#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"

// Functions
//
float MEASURE_BatteryVoltage1();
float MEASURE_BatteryVoltage2();
//
void MEASURE_ConvertVoltageArr(volatile uint16_t *InputArray, float *OutputArray, uint16_t DataLength);
void MEASURE_ConvertCurrentArr(volatile uint16_t *InputArray, float *OutputArray, uint16_t DataLength);

#endif // __MEASUREMENT_H
