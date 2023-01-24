#ifndef __GATE_DRIVER_H
#define __GATE_DRIVER_H

#include "stdinc.h"

// Variables
//
extern Int16U GateValues_Counter;

// Functions
//
void GATE_SetVg(float Value);
bool GATE_WaitingVoltage();
void GATE_StartProcess();
void GATE_RegulatorProcess(float GateVoltageSample);
void GATE_StopProcess();
void GATE_CacheVariables();
bool GATE_FollowingErrorCheck();

#endif // __GATE_DRIVER_H
