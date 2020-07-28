// Header
#include "Diagnostic.h"

// Include
#include "Delay.h"
#include "LowLevel.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"

// Functions
//
bool DIAG_HandleDiagnosticAction(uint16_t ActionID, uint16_t *pUserError)
{
	switch (ActionID)
	{
		case ACT_DBG_PULSE_IG_FEEDBACK:
			if(CONTROL_State == DS_None)
			{
				LL_IgFeedback(true);
				DELAY_MS(1000);
				LL_IgFeedback(false);
			}
			break;
			
		case ACT_DBG_PULSE_ID_LOW_RANGE:
			if(CONTROL_State == DS_None)
			{
				LL_IdLowRange(true);
				DELAY_MS(1000);
				LL_IdLowRange(false);
			}
			break;
			
		case ACT_DBG_PULSE_SYNC_SCPC:
			if(CONTROL_State == DS_None)
			{
				LL_SyncPowerCell(true);
				DELAY_MS(1000);
				LL_SyncPowerCell(false);
			}
			break;
			
		case ACT_DBG_PULSE_IG:
			if(CONTROL_State == DS_None)
			{
				LL_IgPulse(true);
				DELAY_MS(1000);
				LL_IgPulse(false);
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
