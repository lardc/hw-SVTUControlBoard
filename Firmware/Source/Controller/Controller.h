#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Includes
#include "stdinc.h"

// Types
typedef enum __SubState
{
	SS_None				= 0,
	SS_Charge			= 1,
	SS_ChargeWait		= 2,
	SS_PulsePrepStep1	= 3,
	SS_PulsePrepCheckV	= 4,
	SS_PulsePrepStep2	= 5,
	SS_PulsePrepStep3	= 6,
	SS_PulsePrepStep4	= 7,
	SS_PulseFinishWait	= 8
} SubState;
typedef enum __DeviceState
{
	DS_None				= 0,
	DS_Fault			= 1,
	DS_Disabled			= 2,
	DS_BatteryCharge	= 3,
	DS_Ready			= 4,
	DS_InProcess		= 5
} DeviceState;

// Variables
extern DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;

// Functions
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SwitchToFault(Int16U Reason);

#endif // __CONTROLLER_H
