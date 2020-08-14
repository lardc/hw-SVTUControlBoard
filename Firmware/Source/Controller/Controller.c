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

// Types
//
typedef void (*FUNC_AsyncDelegate)();
typedef enum __SubState
{
	SS_None = 0,
	SS_PowerOn = 1,
	SS_WaitCharge = 2,

	SS_PowerOff = 3,

	SS_ConfigSlaves = 4,
	SS_ConfigSlavesApply = 5,
	SS_ConfigHardware = 6,
	SS_CommPause = 7,
	SS_StartPulse = 8
} SubState;

// Variables
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = false;
SubState SUB_State = SS_None;

volatile Int16U CONTROL_Values_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U CONTROL_TimeCounterDelay = 0;

// Forward functions
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SetDeviceState(DeviceState NewState, SubState NewSubState);
void CONTROL_ResetToDefaults();
void CONTROL_WatchDogUpdate();
void CONTROL_CellsStateUpdate();
void CONTROL_HandlePowerOn();

// Functions
//
void CONTROL_Init()
{
	pInt16U cnt = (pInt16U)&CONTROL_Values_Counter;
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[EP_COUNT] = {EP16_DATA_ID, EP16_DATA_VD, EP16_DATA_IG, EP16_DATA_VG};
	Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[EP_COUNT] = {cnt, cnt, cnt, cnt};
	pInt16U EPDatas[EP_COUNT] = {(pInt16U)MEMBUF_EP_Vd, (pInt16U)MEMBUF_EP_Id, (pInt16U)MEMBUF_EP_Vg,
			(pInt16U)MEMBUF_EP_Ig};
	
	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};

	// Инициализация data table
	DT_Init(EPROMService, false);

	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);

	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaults();
}
//-----------------------------------------------

void CONTROL_ResetToDefaults()
{
}
//-----------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case ACT_FAULT_CLEAR:
			{
			}
			break;
			
		case ACT_WARNING_CLEAR:
			DataTable[REG_WARNING] = 0;
			break;
			
		case ACT_ENABLE_POWER:
			{
			}
			break;
			
		case ACT_DISABLE_POWER:
			{
			}
			break;
			
		case ACT_START_TEST:
			{
			}
			break;
			
		case ACT_STOP_TEST:
			{
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
	//CONTROL_HandlePowerOff();
	//CONTROL_HandlePulseConfig();

	CONTROL_WatchDogUpdate();
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	if(Reason == DF_INTERFACE)
	{
		BHLError Error = BHL_GetError();
		DataTable[REG_BHL_ERROR_CODE] = Error.ErrorCode;
		DataTable[REG_BHL_DEVICE] = Error.Device;
		DataTable[REG_BHL_FUNCTION] = Error.Func;
		DataTable[REG_BHL_EXT_DATA] = Error.ExtData;
	}

	CONTROL_ResetToDefaults();

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
			NextUpdate = CONTROL_TimeCounter + TIME_CELLS_UPDATE;
			if(!LOGIC_UpdateCellsState())
				CONTROL_SwitchToFault(DF_INTERFACE);
		}
	}
}
//-----------------------------------------------

void CONTROL_HandlePowerOn()
{
	if(CONTROL_State == DS_InProcess)
	{
		switch (SUB_State)
		{
			case SS_PowerOn:
				{
					if(LOGIC_PowerEnableCells())
					{
						CONTROL_TimeCounterDelay = CONTROL_TimeCounter + DataTable[REG_PC_CHARGE_TIMEOUT];
						CONTROL_SetDeviceState(DS_InProcess, SS_WaitCharge);
					}
					else
						CONTROL_SwitchToFault(DF_INTERFACE);
				}
				break;

			case SS_WaitCharge:
				{
					if(LOGIC_AreCellsInStateX(PCDS_Ready))
					{
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
					else if(LOGIC_IsCellInFaultOrDisabled(PCDS_Fault, PCDS_Disabled))
					{
						CONTROL_SwitchToFault(DF_PC_UNEXPECTED_STATE);
					}
					else if(CONTROL_TimeCounter > CONTROL_TimeCounterDelay)
						CONTROL_SwitchToFault(DF_PC_STATE_TIMEOUT);
				}
				break;

			default:
				break;
		}
	}
}
//-----------------------------------------------
