#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Includes
#include "stdinc.h"

// Types
typedef enum __DeviceState
{
	DS_None				= 0,
	DS_Fault			= 1,
	DS_Disabled			= 2,
	DS_BatteryCharge	= 3,
	DS_Ready			= 4,
	DS_InProcess		= 5
} DeviceState;

typedef enum __LCSUState
{
	LCSU_None = 0,
	LCSU_Fault = 1,
	LCSU_Disabled = 2,
	LCSU_Ready = 3,
	LCSU_PulseConfigReady = 4,
	LCSU_InProcess = 5
} LCSUState;

typedef enum __SubState
{
	SS_None = 0,

	SS_PowerOn = 1,
	SS_WaitCharge = 2,

	SS_PulseInit = 3,
	SS_WaitPulsePause = 4,
	SS_ConfigPulse = 5,
	SS_WaitConfig = 6,
	SS_GateVoltageProcess = 7,
	SS_CurrentPulseStart = 8,
	SS_WaitFinishProcess = 9,
	SS_PostPulseCheck = 10,

	SS_PowerOff = 11
} SubState;

// Variables
extern DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;
extern bool IsImpulse;
extern SubState SUB_State;

// Functions
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_SafetyProcess();
void CONTROL_HandleExternalLamp(bool IsImpulse);

#endif // __CONTROLLER_H
