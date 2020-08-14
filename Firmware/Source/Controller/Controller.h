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
typedef enum __PCDeviceState
{
	PCDS_None = 0,
	PCDS_Fault = 1,
	PCDS_Disabled = 2,
	PCDS_BatteryCharge = 3,
	PCDS_Ready = 4,

	PCDS_WaitTimeOut = 6,
	PCDS_PulseConfigReady = 7,
	PCDS_PulseStart = 8
} PCDeviceState;

// Variables
extern DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;

// Functions
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SwitchToFault(Int16U Reason);

#endif // __CONTROLLER_H
