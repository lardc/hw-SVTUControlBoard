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
bool DIAG_HandleDiagnosticAction(Int16U ActionID, Int16U *pUserError)
{
	switch (ActionID)
	{
	case ACT_DBG_PULSE_V_GATE:
		LL_WriteDAC(DataTable[REG_DBG]);
		DELAY_MS(10);
		LL_WriteDAC(0);
		break;

	case ACT_DBG_PULSE_SYNC_OSC:
		LL_SyncScope(true);
		DELAY_MS(10);
		LL_SyncScope(false);
		break;

	case ACT_DBG_VIN_ST:
		LL_AnalogInputsSelftTest(DataTable[REG_DBG]);
		break;

	case ACT_DBG_PULSE_SYNC_LCSU:
		LL_SyncLCSU(true);
		DELAY_MS(10);
		LL_SyncLCSU(false);
		break;

	case ACT_DBG_EXT_INDICATION:
		LL_ExtIndication(true);
		DELAY_MS(500);
		LL_ExtIndication(false);
		break;
			
		default:
			return false;
	}
	
	return true;
}
//-----------------------------
