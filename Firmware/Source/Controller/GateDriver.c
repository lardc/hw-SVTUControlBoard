// Header
#include "GateDriver.h"

// Include
#include "DataTable.h"
#include "Board.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "LowLevel.h"

// Forward functions


// Functions
//
void GATE_PulseOutput(bool State)
{
	GPIO_SetState(GPIO_IG_PULSE, State);
}
//------------------------------------

void GATE_CurrentFeedback(bool State)
{
	GPIO_SetState(GPIO_IG_FEEDBACK, State);
}
//------------------------------------
