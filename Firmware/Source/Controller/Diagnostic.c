// Header
#include "Diagnostic.h"

// Include
#include "Delay.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "GateDriver.h"

// Functions
//
bool DIAG_HandleDiagnosticAction(uint16_t ActionID, uint16_t *pUserError)
{
	switch (ActionID)
	{
		case ACT_DBG_PULSE_ID_LOW_RANGE:
			if(CONTROL_State == DS_None)
			{
				LL_IdLowRange(DataTable[REG_DBG]);
			}
			break;
			
		case ACT_DBG_PULSE_SYNC_SCPC:
			if(CONTROL_State == DS_None)
			{
				LL_SyncPowerCell(true);
				DELAY_MS(10);
				LL_SyncPowerCell(false);
			}
			break;
			
		case ACT_DBG_PULSE_IG:
			if(CONTROL_State == DS_None)
			{
				GATE_SetVg(DataTable[REG_VG_VALUE]);
				GATE_SetIg(DataTable[REG_IG_VALUE]);
				LL_SyncScope(true);
				GATE_PulseOutput(true);
				DELAY_US(DataTable[REG_GATE_PULSE_TIME] / 2);
				LL_SyncScope(false);
				DELAY_US(DataTable[REG_GATE_PULSE_TIME] / 2);
				GATE_PulseOutput(false);
			}
			break;
			
		case ACT_DBG_PULSE_SYNC_SCOPE:
			if(CONTROL_State == DS_None)
			{
				LL_SyncScope(true);
				DELAY_MS(1000);
				LL_SyncScope(false);
			}
			break;
			
		default:
			return false;
	}
	
	return true;
}
//-----------------------------
