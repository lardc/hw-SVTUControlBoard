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
#include "Constraints.h"
#include "JSONDescription.h"
#include "SaveToFlash.h"
#include "Delay.h"

// Types
//
typedef void (*FUNC_AsyncDelegate)();


// Variables
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = false;
SubState SUB_State = SS_None;
bool IsImpulse = false;
bool SelfTest = false;
bool Diagnostic = false;
static Boolean RequestSaveToFlash = false;

volatile Int16U CONTROL_PowerValues_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U CONTROL_Timeout = 0;
volatile Int16U CONTROL_ExtInfoCounter = 0;

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
Int16U CONTROL_CheckSelfTestResults();
bool CONTROL_IsSafetyEvent();
void CONTROL_FinishedWithProblem(Int16U Problem);
void CONTROL_InitStoragePointers();
void CONTROL_InitJSONPointers();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U FEPIndexes[FEP_COUNT] = {EP_IT, EP_UT, EP_UG, EP_UG_ERR, EP_IG, EP_UT2, EP_ExtInfoData};

	Int16U FEPSized[FEP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_EXT_INFO_SIZE};

	pInt16U FEPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_PowerValues_Counter, (pInt16U)&CONTROL_PowerValues_Counter,
			(pInt16U)&GateValues_Counter, (pInt16U)&GateValues_Counter, (pInt16U)&GateValues_Counter,
			(pInt16U)&CONTROL_PowerValues_Counter, (pInt16U)&CONTROL_ExtInfoCounter};

	pFloat32 FEPDatas[FEP_COUNT] = {(pFloat32)MEMBUF_EP_It, (pFloat32)MEMBUF_EP_Ut, (pFloat32)MEMBUF_EP_Ug,
			(pFloat32)MEMBUF_EP_UgErr, (pFloat32)MEMBUF_EP_Ig, (pFloat32)MEMBUF_EP_Ut_Ch2, (pFloat32)&CONTROL_ExtInfoData};
	
	// Конфигурация сервиса работы DataTable и EEPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	
	// Инициализация DataTable, DMA и АЦП
	DT_Init(EPROMService, false);
	INITCFG_ConfigADC();
	INITCFG_ConfigDMA();

	// Инициализация функций связанных с CAN NodeID
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, CAN_MASTER_NID);
	
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitFEPService(FEPIndexes, FEPSized, FEPCounters, FEPDatas);
	
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaults();

	LOGIC_FindLCSU();
	// Инициализация указателей на сохраняемые данные
	CONTROL_InitStoragePointers();
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

	DataTable[REG_RESULT_UT] = 0;
	DataTable[REG_RESULT_IT] = 0;
	DataTable[REG_RESULT_UG] = 0;
	DataTable[REG_RESULT_IG] = 0;

	DataTable[REG_BHL_ERROR_CODE] = 0;
	DataTable[REG_BHL_DEVICE] = 0;
	DataTable[REG_BHL_FUNCTION] = 0;
	DataTable[REG_BHL_EXT_DATA] = 0;

	Diagnostic = false;
	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//-----------------------------------------------

void CONTROL_ResetHardware()
{
	LL_SyncLCSU(false);
	LL_SyncScope(false);
	LL_AnalogInputsSelfTest(false);
	LL_ExtIndication(false);
	LL_SetItRange(false);
	GATE_StopProcess();
	TIM_Stop(TIM1);
	TIM_Stop(TIM6);
	TIM_Stop(TIM7);
}
//-----------------------------------------------

void CONTROL_WriteDAC(Int16U Value, bool Channel)
{
	// CS
	Channel ? GPIO_SetState(GPIO_AIN_ST, false) : GPIO_SetState(GPIO_UGT_SELF_TEST, false);
	DELAY_US(1);

	for(int i = 15; i >= 0; i--)
	{
		// CLOCK вниз
		GPIO_SetState(GPIO_IND_CTRL, false);

		// Данные
		GPIO_SetState(GPIO_SYNC_LCSU, (Value >> i) & 0x1);
		DELAY_US(1);

		// CLOCK вверх
		GPIO_SetState(GPIO_IND_CTRL, true);
		DELAY_US(1);
	}

	// Данные, CS, CLOCK вверх
	GPIO_SetState(GPIO_AIN_ST, true);
	GPIO_SetState(GPIO_UGT_SELF_TEST, true);
	GPIO_SetState(GPIO_SYNC_LCSU, true);
	GPIO_SetState(GPIO_IND_CTRL, true);
	DELAY_US(1);
}
//-----------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case 21:
			CONTROL_WriteDAC(DataTable[REG_DAC_PLUS_VALUE], false);
			CONTROL_WriteDAC(DataTable[REG_DAC_MINUS_VALUE], true);
			break;

		case 22:
			//GPIO_SetState(GPIO_SAFETY, true);
			//DELAY_US(50);

			GPIO_SetState(GPIO_IT_RANGE, true);
			DELAY_US(DataTable[REG_PULSE1_LEN]);
			GPIO_SetState(GPIO_IT_RANGE, false);

			//DELAY_US(50);
			//GPIO_SetState(GPIO_SAFETY, false);
			break;

		case 23:
			//GPIO_SetState(GPIO_SAFETY, true);
			//DELAY_US(50);

			GPIO_SetState(GPIO_IT_RANGE, true);
			DELAY_US(DataTable[REG_PULSE1_LEN]);
			GPIO_SetState(GPIO_IT_RANGE, false);
			DELAY_US(DataTable[REG_PAUSE_LEN]);
			GPIO_SetState(GPIO_IT_RANGE, true);
			DELAY_US(DataTable[REG_PULSE2_LEN]);
			GPIO_SetState(GPIO_IT_RANGE, false);

			//DELAY_US(50);
			//GPIO_SetState(GPIO_SAFETY, false);
			break;

		case 29:
			GPIO_SetState(GPIO_AIN_ST, true);
			GPIO_SetState(GPIO_IND_CTRL, true);
			GPIO_SetState(GPIO_SYNC_LCSU, true);
			GPIO_SetState(GPIO_IT_RANGE, true);
			GPIO_SetState(GPIO_UGT_SELF_TEST, true);
			GPIO_SetState(GPIO_SAFETY, true);
			DELAY_US(1000);
			GPIO_SetState(GPIO_AIN_ST, false);
			GPIO_SetState(GPIO_IND_CTRL, false);
			GPIO_SetState(GPIO_SYNC_LCSU, false);
			GPIO_SetState(GPIO_IT_RANGE, false);
			GPIO_SetState(GPIO_UGT_SELF_TEST, false);
			GPIO_SetState(GPIO_SAFETY, false);
			break;

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
				{
					CONTROL_ResetData();
					if(CONTROL_IsSafetyEvent())
						CONTROL_FinishedWithProblem(PROBLEM_SAFETY);
					else
						CONTROL_SetDeviceState(DS_InProcess, SS_PulseInit);
				}
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			}
			break;
			
		case ACT_STOP_TEST:
			if(CONTROL_State == DS_InProcess)
			{
				LOGIC_CallCommandForLCSU(ACT_LCSU_STOP_PROCESS);
				CONTROL_ResetToDefaults();
				CONTROL_FinishedWithProblem(PROBLEM_FORCED_STOP);
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

		case ACT_START_DIAGNOSTIC:
			{
				if(CONTROL_State == DS_Ready)
				{
					CONTROL_ResetData();
					Diagnostic = true;
					CONTROL_SetDeviceState(DS_InProcess, SS_CheckResultAndPostPulseConfig);
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
	// Проверка флага сохранения во Flash память
	if (RequestSaveToFlash)
	{
		RequestSaveToFlash = false;
		STF_SaveDiagData();
	}

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
					if(LOGIC_PowerEnableLCSU() || DataTable[REG_EMULATION])
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
					if(LOGIC_AreLCSUInStateX(LCSU_Ready) || DataTable[REG_EMULATION])
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
	static float UtResult, UtCh2Result, ItResult;
	static Int64U Timeout = 0;
	
	if(CONTROL_State == DS_InProcess)
	{
		switch (SUB_State)
		{
			case SS_PulseInit:
				{
					CONTROL_ResetData();
					UtResult = UtCh2Result = ItResult = 0.0f;

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
					float CurrentAmplitude = SelfTest ? DataTable[REG_IT_READ_MAX] : LOGIC_GetCurrentSetpoint();

					if(LOGIC_DistributeCurrent(CurrentAmplitude)||DataTable[REG_EMULATION])
					{
						bool NoError = false;
						LOGIC_SelectCurrentRange(CurrentAmplitude);
						LL_AnalogInputsSelfTest(SelfTest);
						
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
				switch (GATE_RegulatorState)
				{
					case RS_InProcess:
						Timeout = CONTROL_TimeCounter + DataTable[REG_PULSE_TIME_DELAY];
						break;

					case RS_TargetReached:
						if(CONTROL_TimeCounter >= Timeout)
							CONTROL_SetDeviceState(DS_InProcess, SS_CurrentPulseStart);
						break;

					case RS_FollowingError:
						if (SelfTest)
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
							CONTROL_SwitchToFault(DF_SELFTEST_GATE);
						}
						else
						{
							CONTROL_ResetHardware();
							CONTROL_FinishedWithProblem(PROBLEM_GATE_VOLTAGE);
							CONTROL_SetDeviceState(DS_Ready, SS_None);
						}
						break;

					case RS_GateShort:
						if (SelfTest)
						{
							DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
							CONTROL_SwitchToFault(DF_SELFTEST_GATE);
						}
						else
						{
							GATE_StopProcess();
							CONTROL_FinishedWithProblem(PROBLEM_GATE_SHORT);
							CONTROL_SetDeviceState(DS_Ready, SS_None);
						}
						break;

					default:
						break;
				}
				break;

			case SS_CurrentPulseStart:
				LOGIC_StartPulse();

				CONTROL_Timeout = CONTROL_TimeCounter + DataTable[REG_SVTU_WAIT_FINISH_TIME];
				CONTROL_SetDeviceState(DS_InProcess, SS_WaitFinishProcess);
				break;

			case SS_WaitFinishProcess:
				if(CONTROL_TimeCounter < CONTROL_Timeout)
				{
					if(LOGIC_FinishProcess())
						CONTROL_SetDeviceState(DS_InProcess, SS_CheckResultAndPostPulseConfig);
				}
				else
				{
					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
					CONTROL_SwitchToFault(DF_SVTU_WAIT_TIMEOUT);
				}
				break;

			case SS_CheckResultAndPostPulseConfig:
				{
					CONTROL_SaveDataToEndpoint();
					LOGIC_GetResults(&UtResult, &UtCh2Result, &ItResult);

					if((DataTable[REG_PCB_VERSION] != PCB_VERSION_10) && DataTable[REG_DIAG_ACT])
					{
						if((LOGIC_CheckResults(UtResult)) || Diagnostic)
						{
							TIM_Stop(TIM15);
							GATE_CacheVariables();
							GATE_RegulatorState = RS_Diagnostic;
							LL_AnalogInputsDiagGate(true);
							LL_AnalogInputsSelfTest(true);
							DELAY_MS(3);
							Timeout = CONTROL_TimeCounter + DataTable[REG_EXT_DIAG_DURATION];
							GATE_StartProcess();
							CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseProcess);
						}
						else
							CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseSaveResults);
					}
					else
						CONTROL_SetDeviceState(DS_InProcess, SS_PostPulseSaveResults);
				}
				break;

			case SS_PostPulseProcess:
				if(CONTROL_TimeCounter >= Timeout)
				{
					GATE_StopProcess();
					LL_AnalogInputsDiagGate(false);
					LL_AnalogInputsSelfTest(false);
					TIM_Start(TIM15);
					Diagnostic = false;

					if(GATE_RegulatorState == RS_DiagDisconnected)
					{
						CONTROL_ResetHardware();
						CONTROL_FinishedWithProblem(PROBLEM_EXT_DIAG_LINE_DISCON);
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
					else if(GATE_RegulatorState == RS_DiagShort)
					{
						CONTROL_ResetHardware();
						CONTROL_FinishedWithProblem(PROBLEM_EXT_DIAG_SHORT);
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
					else
					{
						DataTable[REG_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
				}
				break;

			case SS_PostPulseSaveResults:
				if(SelfTest)
				{
					SelfTest = false;
					LL_AnalogInputsSelfTest(SelfTest);

					Int16U SelfTestResult = CONTROL_CheckSelfTestResults();

					if(SelfTestResult)
					{
						DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
						CONTROL_SwitchToFault(SelfTestResult);
					}
					else
					{
						DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
				}
				if(DataTable[REG_PCB_VERSION] == PCB_VERSION_10)
				{
					if((UtResult > UT_MAX_VALUE) || (UtResult < UT_MIN_VALUE))
					{
						CONTROL_FinishedWithProblem(PROBLEM_VOLTAGE_OUT_OF_RANGE);
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
					if((ItResult > IT_MAX_VALUE) || (ItResult < IT_MIN_VALUE))
					{
						CONTROL_FinishedWithProblem(PROBLEM_CURRENT_OUT_OF_RANGE);
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
				}
				LOGIC_SaveResults(UtResult, ItResult);
				CONTROL_SetDeviceState(DS_Ready, SS_None);
				DataTable[REG_OP_RESULT] = OPRESULT_OK;
				break;

			default:
				break;
		}
	}
}
//-----------------------------------------------

Int16U CONTROL_CheckSelfTestResults()
{
	if(fabsf((1 - DataTable[REG_RESULT_IT] / DataTable[REG_IT_READ_MAX]) * 100) > SELFTEST_ALLOWED_ERROR)
		return DF_SELFTEST_IT;

	if(fabsf(1 - (DataTable[REG_RESULT_UT] / (DataTable[REG_RESULT_IT] * DataTable[REG_R_SHUNT] / 1000))) * 100 > SELFTEST_ALLOWED_ERROR)
		return DF_SELFTEST_UT;

	return 0;
}
//-----------------------------------------------

void CONTROL_SaveDataToEndpoint()
{
	LOGIC_SaveToEndpoint(MEMBUF_DMA_Ut, MEMBUF_EP_Ut, VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint(MEMBUF_DMA_It, MEMBUF_EP_It, VALUES_POWER_DMA_SIZE);
	LOGIC_SaveToEndpoint(MEMBUF_DMA_Ut2_UgIg, MEMBUF_EP_Ut_Ch2, VALUES_POWER_DMA_SIZE);
	CONTROL_PowerValues_Counter = VALUES_x_SIZE;
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

bool CONTROL_IsSafetyEvent()
{
	if (DataTable[REG_MUTE_SAFETY])
	{
		LL_SetSafetyState(false);
		return false;
	}
	else
	{
		LL_SetSafetyState(true);
		return LL_GetSafetyState();
	}
}
//-----------------------------------------------

void CONTROL_FinishedWithProblem(Int16U Problem)
{
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
	DataTable[REG_PROBLEM] = Problem;
}
//-----------------------------------------------

void CONTROL_SafetyProcess()
{
	if(CONTROL_IsSafetyEvent() && CONTROL_State == DS_InProcess && SUB_State != SS_PowerOn && SUB_State != SS_WaitCharge
			&& SUB_State != SS_PowerOff)
	{
		CONTROL_ResetHardware();
		CONTROL_FinishedWithProblem(PROBLEM_SAFETY);
		CONTROL_SetDeviceState(DS_Ready, SS_None);
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

void CONTROL_InitStoragePointers()
{
	STF_AssignPointer(0, (Int32U)&DataTable[REG_IT_SETPOINT]);
	STF_AssignPointer(1, (Int32U)&DataTable[REG_UG_SETPOINT]);

	STF_AssignPointer(2, (Int32U)&DataTable[REG_DEV_STATE]);
	STF_AssignPointer(3, (Int32U)&DataTable[REG_FAULT_REASON]);
	STF_AssignPointer(4, (Int32U)&DataTable[REG_DISABLE_REASON]);
	STF_AssignPointer(5, (Int32U)&DataTable[REG_WARNING]);
	STF_AssignPointer(6, (Int32U)&DataTable[REG_PROBLEM]);
	STF_AssignPointer(7, (Int32U)&DataTable[REG_OP_RESULT]);
	STF_AssignPointer(8, (Int32U)&DataTable[REG_SELF_TEST_OP_RESULT]);
	STF_AssignPointer(9, (Int32U)&DataTable[REG_SUB_STATE]);

	STF_AssignPointer(10, (Int32U)MEMBUF_EP_It);
	STF_AssignPointer(11, (Int32U)MEMBUF_EP_Ut);
	STF_AssignPointer(12, (Int32U)MEMBUF_EP_Ug);
	STF_AssignPointer(13, (Int32U)MEMBUF_EP_UgErr);
	STF_AssignPointer(14, (Int32U)MEMBUF_EP_Ig);

	STF_AssignPointer(15, (Int32U)&CONTROL_PowerValues_Counter);
	STF_AssignPointer(16, (Int32U)&GateValues_Counter);
}
//-----------------------------------------------

void CONTROL_InitJSONPointers()
{
	Utm1Min = DataTable[REG_UT_MIN] ? DataTable[REG_UT_MIN] : UT_MIN_VALUE;
	Utm1Max = DataTable[REG_UT_MAX] ? DataTable[REG_UT_MAX] : UT_MAX_VALUE;

	Utm2Min = DataTable[REG_UT_MAX] ? DataTable[REG_UT_MAX] : UT_MAX_VALUE;
	Utm2Max = DataTable[REG_UT2_MAX];

	ItmSetMin = DataTable[REG_IT_MIN] ? DataTable[REG_IT_MIN] : IT_MIN_VALUE;
	ItmSetMax = DataTable[REG_IT_MAX] ? DataTable[REG_IT_MAX] : IT_MAX_VALUE;

	ItmMeas1Min = DataTable[REG_IT_MIN] ? DataTable[REG_IT_MIN] : IT_MIN_VALUE;
	ItmMeas1Max = DataTable[REG_I_R0_THRESHOLD];

	ItmMeas2Min = DataTable[REG_I_R0_THRESHOLD];
	ItmMeas2Max = DataTable[REG_IT_MAX] ? DataTable[REG_IT_MAX] : IT_MAX_VALUE;

	Utm2Active = DataTable[REG_UT2_MAX] ? 1 : 0;

	JSON_AssignPointer(0, &Utm1Min);
	JSON_AssignPointer(1, &Utm1Max);

	JSON_AssignPointer(2, &Utm2Active);

	JSON_AssignPointer(3, &Utm2Min);
	JSON_AssignPointer(4, &Utm2Max);

	JSON_AssignPointer(5, &ItmSetMin);
	JSON_AssignPointer(6, &ItmSetMax);

	JSON_AssignPointer(7, &ItmMeas1Min);
	JSON_AssignPointer(8, &ItmMeas1Max);

	JSON_AssignPointer(9, &ItmMeas2Min);
	JSON_AssignPointer(10, &ItmMeas2Max);
}
//------------------------------------------
