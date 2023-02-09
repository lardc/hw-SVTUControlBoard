#ifndef __GATE_DRIVER_H
#define __GATE_DRIVER_H

#include "stdinc.h"

typedef enum __RegulatorState
{
	RS_None				= 0,
	RS_InProcess		= 1,
	RS_TargetReached	= 2,
	RS_FollowingError	= 3,
	RS_GateShort		= 4
} RegulatorState;

// Variables
//
extern Int16U GateValues_Counter;
extern RegulatorState GATE_RegulatorState;

// Functions
//
void GATE_SetVg(float Value);
void GATE_StartProcess();
void GATE_RegulatorProcess(float GateVoltageSample, float CurrentSample);
void GATE_StopProcess();
void GATE_CacheVariables();
bool GATE_RegulatorStatusCheck(RegulatorState State);

#endif // __GATE_DRIVER_H
