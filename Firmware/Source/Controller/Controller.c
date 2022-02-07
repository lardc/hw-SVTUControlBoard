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
	SS_PostPulseCheck = 7,

	SS_PowerOff = 8
} SubState;

// Variables
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = false;
SubState SUB_State = SS_None;

volatile Int16U CONTROL_Values_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0;

// Forward functions
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SetDeviceState(DeviceState NewState, SubState NewSubState);
void CONTROL_ResetToDefaults();
void CONTROL_ResetData();
void CONTROL_ResetHardware();
void CONTROL_WatchDogUpdate();
void CONTROL_CellsStateUpdate();
void CONTROL_HandleFaultCellsEvents(Int64U Timeout);
void CONTROL_HandlePowerOn();
void CONTROL_HandlePulse();
void CONTROL_HandlePowerOff();
void CONTROL_SaveDataToEndpoint();
void CONTROL_SaveResults();

// Functions
//
void CONTROL_Init()
{
	pInt16U cnt = (pInt16U)&CONTROL_Values_Counter;
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[EP_COUNT] = {EP16_DATA_ID, EP16_DATA_VD, EP16_DATA_IG, EP16_DATA_VG};
	Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[EP_COUNT] = {cnt, cnt, cnt, cnt};
	pInt16U EPDatas[EP_COUNT] = {(pInt16U)MEMBUF_EP_Id, (pInt16U)MEMBUF_EP_Vd, (pInt16U)MEMBUF_EP_Ig,
			(pInt16U)MEMBUF_EP_Vg};
	
	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, CAN_MASTER_NID);
	
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaults();

	LOGIC_FindCells();
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
	GATE_PulseOutput(false);
	LL_SyncPowerCell(false);
	LL_SyncScope(false);
	LL_IdLowRange(false);

	GATE_SetVg(0);
	GATE_SetIg(0);
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
			{
				if(CONTROL_State == DS_InProcess)
				{
					CONTROL_ResetToDefaults();
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				}
				else
					*pUserError = ERR_OPERATION_BLOCKED;
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
	
	// Считывание состояний силовых ячеек
	CONTROL_CellsStateUpdate();
	
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

void CONTROL_CellsStateUpdate()
{
	static uint64_t NextUpdate = 0;
	
	if(SUB_State != SS_None)
	{
		if(CONTROL_TimeCounter > NextUpdate)
		{
			NextUpdate = CONTROL_TimeCounter + TIME_PC_UPDATE;
			if(!LOGIC_UpdateCellsState())
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
					if(LOGIC_PowerEnableCells())
					{
						Timeout = CONTROL_TimeCounter + DataTable[REG_PC_LONG_TIMEOUT];
						CONTROL_SetDeviceState(DS_InProcess, SS_WaitCharge);
					}
					else
						CONTROL_SwitchToFault(DF_INTERFACE);
				}
				break;
				
			case SS_WaitCharge:
				{
					if(LOGIC_AreCellsInStateX(PCDS_Ready))
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					else
						CONTROL_HandleFaultCellsEvents(Timeout);
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
					Timeout = CONTROL_TimeCounter + DataTable[REG_PC_LONG_TIMEOUT];
					CONTROL_SetDeviceState(DS_InProcess, SS_WaitPulsePause);
				}
				break;
				
			case SS_WaitPulsePause:
				{
					if(LOGIC_AreCellsInStateX(PCDS_Ready))
						CONTROL_SetDeviceState(DS_InProcess, SS_ConfigPulse);
					else
						CONTROL_HandleFaultCellsEvents(Timeout);
				}
				break;
				
			case SS_ConfigPulse:
				{
					float CurrentAmplitude = LOGIC_GetCurrentSetpoint();

					if(LOGIC_DistributeCurrent(CurrentAmplitude))
					{
						bool NoError = false;
						LOGIC_SelectCurrentRange(CurrentAmplitude);
						
						if(LOGIC_WriteCellsConfig())
						{
							if(LOGIC_CallCommandForCells(ACT_LSLPC_PULSE_CONFIG))
							{
								NoError = true;
								Timeout = CONTROL_TimeCounter + TIMEOUT_PC_SHORT;
								CONTROL_SetDeviceState(DS_InProcess, SS_WaitConfig);
							}
						}
						
						if(!NoError)
							CONTROL_SwitchToFault(DF_INTERFACE);
					}
					else
						CONTROL_SwitchToFault(DF_PC_CURRENT_CONFIG);
				}
				break;
				
			case SS_WaitConfig:
				{
					if(LOGIC_AreCellsInStateX(PCDS_PulseConfigReady))
					{
						LOGIC_ProcessPulse();
						CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseCheck);
					}
					else
						CONTROL_HandleFaultCellsEvents(Timeout);
				}
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
	LOGIC_SaveToEndpoint((uint16_t *)MEMBUF_DMA_Vd, (uint16_t *)MEMBUF_EP_Vd, VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint((uint16_t *)MEMBUF_DMA_Id, (uint16_t *)MEMBUF_EP_Id, VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint((uint16_t *)MEMBUF_DMA_Vg, (uint16_t *)MEMBUF_EP_Vg, VALUES_GATE_DMA_SIZE);
	LOGIC_SaveToEndpoint((uint16_t *)MEMBUF_DMA_Ig, (uint16_t *)MEMBUF_EP_Ig, VALUES_GATE_DMA_SIZE);
	CONTROL_Values_Counter = VALUES_x_SIZE;
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

		if(LOGIC_CallCommandForCells(ACT_LSLPC_DISABLE_POWER))
			CONTROL_SetDeviceState(DS_None, SS_None);
		else
			CONTROL_SwitchToFault(DF_INTERFACE);
	}
}
//-----------------------------------------------

void CONTROL_HandleFaultCellsEvents(Int64U Timeout)
{
	if(LOGIC_IsCellInFaultOrDisabled(PCDS_Fault, PCDS_Disabled))
	{
		CONTROL_SwitchToFault(DF_PC_UNEXPECTED_STATE);
	}
	else if(CONTROL_TimeCounter > Timeout)
		CONTROL_SwitchToFault(DF_PC_STATE_TIMEOUT);
}
//-----------------------------------------------
