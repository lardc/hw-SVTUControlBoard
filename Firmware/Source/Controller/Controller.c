// Header
#include "Controller.h"

// Includes
#include "DataTable.h"
#include "Board.h"
#include "Logic.h"
#include "Controller.h"
#include "Diagnostic.h"
#include "MemBuffers.h"
#include "BCCIMHighLevel.h"
#include "GateDriver.h"
#include "LowLevel.h"
#include "BCCIxParams.h"

// Types
//
typedef void (*FUNC_AsyncDelegate)();
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
	SS_CurrentPulseProcess = 8,
	SS_PostPulseCheck = 9,

	SS_PowerOff = 10
} SubState;

// Variables
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = false;
SubState SUB_State = SS_None;

volatile Int16U CONTROL_PowerValues_Counter = 0;
volatile Int16U CONTROL_GateValues_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0;

// Forward functions
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SetDeviceState(DeviceState NewState, SubState NewSubState);
void CONTROL_ResetToDefaults();
void CONTROL_ResetData();
void CONTROL_ResetHardware();
void CONTROL_WatchDogUpdate();
void CONTROL_LCSUStateUpdate();
void CONTROL_HandleFaultLCSUEvents(Int64U Timeout);
void CONTROL_HandlePowerOn();
void CONTROL_HandlePulse();
void CONTROL_HandlePowerOff();
void CONTROL_SaveDataToEndpoint();
void CONTROL_SaveResults();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U FEPIndexes[FEP_COUNT] = {EP_ID, EP_VD, EP_VG, EP_VG_ERR};
	Int16U FEPSized[FEP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U FEPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_PowerValues_Counter, (pInt16U)&CONTROL_PowerValues_Counter,
														(pInt16U)&CONTROL_GateValues_Counter, (pInt16U)&CONTROL_GateValues_Counter};
	pFloat32 FEPDatas[FEP_COUNT] = {(pFloat32)MEMBUF_EP_Id, (pFloat32)MEMBUF_EP_Vd, (pFloat32)MEMBUF_EP_Vg,
			(pFloat32)MEMBUF_EP_VgErr};
	
	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, CAN_MASTER_NID);
	
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitFEPService(FEPIndexes, FEPSized, FEPCounters, FEPDatas);
	
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaults();

	LOGIC_FindLCSU();
}
//-----------------------------------------------

void CONTROL_ResetToDefaults()
{
	CONTROL_ResetData();
	CONTROL_ResetHardware();
	CONTROL_SetDeviceState(DS_None, SS_None);
}
//-----------------------------------------------

void CONTROL_ResetData()
{
	DataTable[REG_FAULT_REASON] = DF_NONE;
	DataTable[REG_DISABLE_REASON] = DF_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;

	DataTable[REG_DUT_VOLTAGE] = 0;
	DataTable[REG_DUT_CURRENT] = 0;

	DataTable[REG_BHL_ERROR_CODE] = 0;
	DataTable[REG_BHL_DEVICE] = 0;
	DataTable[REG_BHL_FUNCTION] = 0;
	DataTable[REG_BHL_EXT_DATA] = 0;

	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//-----------------------------------------------

void CONTROL_ResetHardware()
{
	LL_SyncLCSU(false);
	LL_SyncScope(false);
	LL_AnalogInputsSelftTest(false);
	LL_ExtIndication(false);
	LL_SetIdRange(false);

	GATE_StopProcess();
}
//-----------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case ACT_ENABLE_POWER:
			{
				if(CONTROL_State == DS_None)
					CONTROL_SetDeviceState(DS_InProcess, SS_PowerOn);
				else if(CONTROL_State != DS_Ready)
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;
			
		case ACT_DISABLE_POWER:
			{
				if(CONTROL_State == DS_Ready)
					CONTROL_SetDeviceState(DS_None, SS_PowerOff);
				else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;
			
		case ACT_FAULT_CLEAR:
			{
				if(CONTROL_State == DS_Fault)
					CONTROL_ResetToDefaults();
			}
			break;
			
		case ACT_WARNING_CLEAR:
			DataTable[REG_WARNING] = 0;
			break;
			
		case ACT_START_TEST:
			{
				if(CONTROL_State == DS_Ready)
					CONTROL_SetDeviceState(DS_InProcess, SS_PulseInit);
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			}
			break;
			
		case ACT_STOP_TEST:
			if(CONTROL_State == DS_InProcess)
			{
				CONTROL_ResetToDefaults();
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			break;
			
		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
	}
	
	return true;
}
//-----------------------------------------------

void CONTROL_Idle()
{
	// Обработка мастер-запросов по интерфейсу
	DEVPROFILE_ProcessRequests();
	
	// Считывание состояний блоков LCSU
	CONTROL_LCSUStateUpdate();
	
	// Обработка логики мастер-команд
	CONTROL_HandlePowerOn();
	CONTROL_HandlePulse();
	CONTROL_HandlePowerOff();
	
	CONTROL_WatchDogUpdate();
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_ResetToDefaults();

	if(Reason == DF_INTERFACE)
	{
		BHLError Error = BHL_GetError();
		DataTable[REG_BHL_ERROR_CODE] = Error.ErrorCode;
		DataTable[REG_BHL_DEVICE] = Error.Device;
		DataTable[REG_BHL_FUNCTION] = Error.Func;
		DataTable[REG_BHL_EXT_DATA] = Error.ExtData;
	}
	
	CONTROL_SetDeviceState(DS_Fault, SS_None);
	DataTable[REG_FAULT_REASON] = Reason;
}
//-----------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, SubState NewSubState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
	
	SUB_State = NewSubState;
	DataTable[REG_DEV_SUB_STATE] = NewSubState;
}
//-----------------------------------------------

void CONTROL_WatchDogUpdate()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//-----------------------------------------------

void CONTROL_LCSUStateUpdate()
{
	static uint64_t NextUpdate = 0;
	
	if(SUB_State != SS_None)
	{
		if(CONTROL_TimeCounter > NextUpdate)
		{
			NextUpdate = CONTROL_TimeCounter + TIME_LCSU_UPDATE;
			if(!LOGIC_UpdateLCSUState())
				CONTROL_SwitchToFault(DF_INTERFACE);
		}
	}
}
//-----------------------------------------------

void CONTROL_HandlePowerOn()
{
	static Int64U Timeout = 0;
	
	if(CONTROL_State == DS_InProcess)
	{
		switch (SUB_State)
		{
			case SS_PowerOn:
				{
					if(LOGIC_PowerEnableLCSU())
					{
						Timeout = CONTROL_TimeCounter + DataTable[REG_LCSU_LONG_TIMEOUT];
						CONTROL_SetDeviceState(DS_InProcess, SS_WaitCharge);
					}
					else
						CONTROL_SwitchToFault(DF_INTERFACE);
				}
				break;
				
			case SS_WaitCharge:
				{
					if(LOGIC_AreLCSUInStateX(LCSU_Ready))
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					else
						CONTROL_HandleFaultLCSUEvents(Timeout);
				}
				break;
				
			default:
				break;
		}
	}
}
//-----------------------------------------------

void CONTROL_HandlePulse()
{
	static Int64U Timeout = 0;
	
	if(CONTROL_State == DS_InProcess)
	{
		switch (SUB_State)
		{
			case SS_PulseInit:
				{
					CONTROL_ResetData();
					Timeout = CONTROL_TimeCounter + DataTable[REG_LCSU_LONG_TIMEOUT];
					CONTROL_SetDeviceState(DS_InProcess, SS_WaitPulsePause);
				}
				break;
				
			case SS_WaitPulsePause:
				{
					if(LOGIC_AreLCSUInStateX(LCSU_Ready))
						CONTROL_SetDeviceState(DS_InProcess, SS_ConfigPulse);
					else
						CONTROL_HandleFaultLCSUEvents(Timeout);
				}
				break;
				
			case SS_ConfigPulse:
				{
					float CurrentAmplitude = LOGIC_GetCurrentSetpoint();

					if(LOGIC_DistributeCurrent(CurrentAmplitude))
					{
						bool NoError = false;
						LOGIC_SelectCurrentRange(CurrentAmplitude);
						
						if(LOGIC_WriteLCSUConfig())
						{
							if(LOGIC_CallCommandForLCSU(ACT_LCSU_PULSE_CONFIG))
							{
								NoError = true;
								Timeout = CONTROL_TimeCounter + TIMEOUT_LCSU_SHORT;
								CONTROL_SetDeviceState(DS_InProcess, SS_WaitConfig);
							}
						}
						
						if(!NoError)
							CONTROL_SwitchToFault(DF_INTERFACE);
					}
					else
						CONTROL_SwitchToFault(DF_LCSU_CURRENT_CONFIG);
				}
				break;
				
			case SS_WaitConfig:
				{
					if(LOGIC_AreLCSUInStateX(LCSU_PulseConfigReady))
					{
						GATE_CacheVariables();
						GATE_StartProcess();
						Timeout = CONTROL_TimeCounter + TIME_VG_STAB;

						CONTROL_SetDeviceState(DS_InProcess, SS_GateVoltageProcess);
					}
					else
						CONTROL_HandleFaultLCSUEvents(Timeout);
				}
				break;
				
			case SS_GateVoltageProcess:
				if(GATE_WaitingVoltage())
					CONTROL_SetDeviceState(DS_InProcess, SS_CurrentPulseProcess);
				else
				{
					if(CONTROL_TimeCounter >= Timeout || GATE_FollowingErrorCheck())
						CONTROL_SwitchToFault(DF_GATE_VOLTAGE);
				}
				break;

			case SS_CurrentPulseProcess:
				LOGIC_ProcessPulse();
				CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseCheck);
				break;

			case SS_PostPulseCheck:
				{
					CONTROL_SaveDataToEndpoint();
					CONTROL_SaveResults();
					DataTable[REG_OP_RESULT] = OPRESULT_OK;
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				}
				break;

			default:
				break;
		}
	}
}
//-----------------------------------------------

void CONTROL_SaveDataToEndpoint()
{
	LOGIC_SaveToEndpoint((pFloat32)MEMBUF_DMA_Vd, (pFloat32)MEMBUF_EP_Vd, VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint((pFloat32)MEMBUF_DMA_Id, (pFloat32)MEMBUF_EP_Id, VALUES_POWER_DMA_SIZE);
	CONTROL_PowerValues_Counter = VALUES_x_SIZE;
}
//-----------------------------------------------

void CONTROL_SaveResults()
{
	LOGIC_SaveResults();
}
//-----------------------------------------------

void CONTROL_HandlePowerOff()
{
	if(CONTROL_State == DS_None && SUB_State == SS_PowerOff)
	{
		CONTROL_ResetToDefaults();

		if(LOGIC_CallCommandForLCSU(ACT_LCSU_DISABLE_POWER))
			CONTROL_SetDeviceState(DS_None, SS_None);
		else
			CONTROL_SwitchToFault(DF_INTERFACE);
	}
}
//-----------------------------------------------

void CONTROL_HandleFaultLCSUEvents(Int64U Timeout)
{
	if(LOGIC_IsLCSUInFaultOrDisabled(LCSU_Fault, LCSU_Disabled))
	{
		CONTROL_SwitchToFault(DF_LCSU_UNEXPECTED_STATE);
	}
	else if(CONTROL_TimeCounter > Timeout)
		CONTROL_SwitchToFault(DF_LCSU_STATE_TIMEOUT);
}
//-----------------------------------------------

void CONTROL_SafetyProcess()
{
	if(!LL_SafetyIsActive())
	{
		CONTROL_ResetData();
		CONTROL_ResetHardware();

		DataTable[REG_WARNING] = WARNING_SAFETY;

		CONTROL_SetDeviceState(DS_Ready, SS_None);
	}
}
// ----------------------------------------
