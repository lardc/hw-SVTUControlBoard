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
#include "math.h"
#include "InitConfig.h"
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
	SS_CurrentPulseStart = 8,
	SS_WaitFinishProcess = 9,
	SS_PostPulseCheck = 10,

	SS_PowerOff = 11
} SubState;

// Variables
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = false;
SubState SUB_State = SS_None;
bool IsImpulse = false;
bool SelfTest = false;

volatile Int16U CONTROL_PowerValues_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U CONTROL_Timeout = 0;

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
Int16U CONTROL_CheckSelfTestResults();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U FEPIndexes[FEP_COUNT] = {EP_ID, EP_VD, EP_VG, EP_VG_ERR, EP_IG};
	Int16U FEPSized[FEP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U FEPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_PowerValues_Counter, (pInt16U)&CONTROL_PowerValues_Counter,
										(pInt16U)&GateValues_Counter, (pInt16U)&GateValues_Counter, (pInt16U)&GateValues_Counter};
	pFloat32 FEPDatas[FEP_COUNT] = {(pFloat32)MEMBUF_EP_Id, (pFloat32)MEMBUF_EP_Vd, (pFloat32)MEMBUF_EP_Vg,
			(pFloat32)MEMBUF_EP_VgErr, (pFloat32)MEMBUF_EP_Ig};
	
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

	DataTable[REG_RESULT_VD] = 0;
	DataTable[REG_RESULT_ID] = 0;
	DataTable[REG_RESULT_VG] = 0;

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
	TIM_Stop(TIM1);
	TIM_Stop(TIM6);
	TIM_Stop(TIM7);

	GATE_StopProcess();
	LL_SyncLCSU(false);
	LL_SyncScope(false);
	LL_AnalogInputsSelftTest(false);
	LL_ExtIndication(false);
	LL_SetIdRange(false);
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
				LOGIC_CallCommandForLCSU(ACT_LCSU_STOP_PROCESS);
				CONTROL_ResetToDefaults();

				DataTable[REG_PROBLEM] = PROBLEM_FORCED_STOP;

				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			break;
			
		case ACT_START_SELF_TEST:
			{
				if(CONTROL_State == DS_Ready)
				{
					SelfTest = true;
					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
					CONTROL_SetDeviceState(DS_InProcess, SS_PulseInit);
				}
				else
					*pUserError = ERR_DEVICE_NOT_READY;
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
	
	// Обработка сигнала безопасности
	CONTROL_SafetyProcess();

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
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
}
//-----------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, SubState NewSubState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
	
	SUB_State = NewSubState;
	DataTable[REG_SUB_STATE] = NewSubState;
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
	static Int64U NextUpdate = 0;
	
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
					INITCFG_ConfigTimer6(DataTable[REG_OSC_SYNC_DELAY]);
					INITCFG_ConfigTimer7(DataTable[REG_OSC_SYNC_TIME]);

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
					float CurrentAmplitude = SelfTest ? DataTable[REG_ID_MAX] : LOGIC_GetCurrentSetpoint();

					if(LOGIC_DistributeCurrent(CurrentAmplitude))
					{
						bool NoError = false;
						LOGIC_SelectCurrentRange(CurrentAmplitude);
						LL_AnalogInputsSelftTest(SelfTest);
						
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
						Timeout = 0;

						CONTROL_SetDeviceState(DS_InProcess, SS_GateVoltageProcess);
					}
					else
						CONTROL_HandleFaultLCSUEvents(Timeout);
				}
				break;
				
			case SS_GateVoltageProcess:
				if(GATE_RegulatorStatusCheck(RS_InProcess))
					Timeout = CONTROL_TimeCounter + TIME_VG_STAB;

				if(GATE_RegulatorStatusCheck(RS_TargetReached))
				{
					if(CONTROL_TimeCounter >= Timeout)
						CONTROL_SetDeviceState(DS_InProcess, SS_CurrentPulseStart);
				}
				else
				{
					if(GATE_RegulatorStatusCheck(RS_FollowingError))
					{
						if (SelfTest)
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
							CONTROL_SwitchToFault(DF_SELFTEST_GATE);
						}
						else
						{
							CONTROL_ResetData();
							CONTROL_ResetHardware();
							DataTable[REG_PROBLEM] = PROBLEM_GATE_VOLTAGE;
							CONTROL_SetDeviceState(DS_Ready, SS_None);
						}
					}

					if(GATE_RegulatorStatusCheck(RS_GateShort))
					{
						if (SelfTest)
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
							CONTROL_SwitchToFault(DF_SELFTEST_GATE);
						}
						else
						{
							GATE_StopProcess();
							DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
							DataTable[REG_PROBLEM] = PROBLEM_GATE_SHORT;
							CONTROL_SetDeviceState(DS_Ready, SS_None);
						}
					}
				}
				break;

			case SS_CurrentPulseStart:
				LOGIC_StartPulse();

				CONTROL_Timeout = CONTROL_TimeCounter + SVTU_WAIT_FINISH_TIME;
				CONTROL_SetDeviceState(DS_InProcess, SS_WaitFinishProcess);
				break;

			case SS_WaitFinishProcess:
				if(CONTROL_TimeCounter < CONTROL_Timeout)
				{
					if(LOGIC_FinishProcess())
						CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseCheck);
				}
				else
				{
					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
					CONTROL_SwitchToFault(DF_SVTU_WAIT_TIMEOUT);
				}
				break;

			case SS_PostPulseCheck:
				{
					CONTROL_SaveDataToEndpoint();
					CONTROL_SaveResults();

					if(SelfTest)
					{
						SelfTest = false;
						LL_AnalogInputsSelftTest(SelfTest);

						Int16U SelfTestResult = CONTROL_CheckSelfTestResults();

						if(SelfTestResult)
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
							CONTROL_SwitchToFault(SelfTestResult);
							break;
						}
						else
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
							CONTROL_SetDeviceState(DS_Ready, SS_None);
						}
					}
					else
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

Int16U CONTROL_CheckSelfTestResults()
{
	if(fabs((1-DataTable[REG_RESULT_ID] / DataTable[REG_ID_MAX]) * 100) > SELFTEST_ALLOWED_ERROR)
		return DF_SELFTEST_ID;

	if(fabs(1-(DataTable[REG_RESULT_VD] / (DataTable[REG_RESULT_ID] * DataTable[REG_R_SHUNT]/1000)))*100 > SELFTEST_ALLOWED_ERROR)
		return DF_SELFTEST_VD;

	return 0;
}
//-----------------------------------------------

void CONTROL_SaveDataToEndpoint()
{
	LOGIC_SaveToEndpoint(&MEMBUF_DMA_Vd[0], &MEMBUF_EP_Vd[0], VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint(&MEMBUF_DMA_Id[0], &MEMBUF_EP_Id[0], VALUES_POWER_DMA_SIZE);
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

		DataTable[REG_PROBLEM] = PROBLEM_SAFETY;

		CONTROL_SetDeviceState(DS_None, SS_None);
	}
}
// ----------------------------------------

void CONTROL_HandleExternalLamp(bool IsImpulse)
{
	static Int64U ExternalLampCounter = 0;

	if(DataTable[REG_LAMP_CTRL])
	{
		if(CONTROL_State == DS_Fault)
		{
			if(++ExternalLampCounter > TIME_FAULT_LED_BLINK)
			{
				LL_ExtIndicationToggle();
				ExternalLampCounter = 0;
			}
		}
		else
			{
				if(IsImpulse)
				{
					LL_ExtIndication(true);
					ExternalLampCounter = CONTROL_TimeCounter + TIME_EXT_LAMP_ON_STATE;
				}
				else
				{
					if(CONTROL_TimeCounter >= ExternalLampCounter)
						LL_ExtIndication(false);
				}
			}
	}
}
//-----------------------------------------------
