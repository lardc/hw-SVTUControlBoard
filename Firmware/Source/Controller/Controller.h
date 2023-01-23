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

// Variables
extern DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;

// Functions
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SwitchToFault(Int16U Reason);

#endif // __CONTROLLER_H
