//-----------------------------------------------
// Основная логика
//-----------------------------------------------

// Header
#include "Controller.h"
//
// Includes
#include "SysConfig.h"
#include "DataTable.h"
#include "SCCISlave.h"
#include "DeviceProfile.h"
#include "BCCITypes.h"
#include "Board.h"
#include "Global.h"
#include "LowLevel.h"
#include "Logic.h"
#include "Controller.h"
#include "Measurement.h"
#include "math.h"

// Types
//
typedef void (*FUNC_AsyncDelegate)();

// Variables
//
DeviceState CONTROL_State = DS_None;
static Boolean CycleActive = FALSE;
SubState SUB_State = SS_None;

volatile Int16U CONTROL_Values_Setpoint[VALUES_x_SIZE];
volatile Int16U CONTROL_Values_DUTVoltage[VALUES_x_SIZE];
volatile Int16U CONTROL_Values_DUTCurrent[VALUES_x_SIZE];
volatile Int16U CONTROL_Values_SetCounter = 0;
volatile Int16U CONTROL_Values_ADCCounter = 0;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U CONTROL_TimeCounterDelay = 0;

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_ResetToDefaults(bool StopPowerSupply);
void CONTROL_Idle();
void CONTROL_WatchDogUpdate();
void CONTROL_RegistersReset();


void CONTROL_SetDeviceState(DeviceState NewState);
void Delay_mS(uint32_t Delay);


/*
void CONTROL_SaveResultToEndpoints(ProcessResult Result);
void CONTROL_SaveResultToRegisters(ProcessResult Result);
uint16_t CONTROL_HandleWarningCondition(ProcessResult Result);
*/

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	/*
	Int16U EPIndexes[EP_COUNT] = { EP_DUT_V,  EP_DUT_I, EP_SETPOINT,
								   EP_DIAG_DUT_VBR, EP_DIAG_DUT_VRSM, EP_DIAG_DUT_IRSM, EP_DIAG_DUT_PRSM };
	Int16U EPSized[EP_COUNT] = { VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE,
								 PULSES_MAX, PULSES_MAX, PULSES_MAX, PULSES_MAX };
	pInt16U EPCounters[EP_COUNT] = { (pInt16U)&CONTROL_Values_ADCCounter, (pInt16U)&CONTROL_Values_ADCCounter, (pInt16U)&CONTROL_Values_SetCounter,
									 (pInt16U)&CONTROL_Values_DiagEPCounter, (pInt16U)&CONTROL_Values_DiagEPCounter,
									 (pInt16U)&CONTROL_Values_DiagEPCounter, (pInt16U)&CONTROL_Values_DiagEPCounter };
	pInt16U EPDatas[EP_COUNT] = { (pInt16U)CONTROL_Values_DUTVoltage, (pInt16U)CONTROL_Values_DUTCurrent, (pInt16U)CONTROL_Values_Setpoint,
								  (pInt16U)CONTROL_Values_DiagVbr, (pInt16U)CONTROL_Values_DiagVrsm,
								  (pInt16U)CONTROL_Values_DiagIrsm, (pInt16U)CONTROL_Values_DiagPrsm };
								  */
	pInt16U EPIndexes = NULL;
	pInt16U EPSized = NULL;
	pInt16U *EPCounters = NULL;
	pInt16U *EPDatas = NULL;

	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = { (FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT };
	// Инициализация data table
	DT_Init(EPROMService, FALSE);
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaults(TRUE);

	//Определение nid блоков, которые подключены
	//LSLPCFind(&MASTER_DEVICE_CAN_Interface);
}
//------------------------------------------------------------------------------

void CONTROL_ResetToDefaults(bool StopPowerSupply)
{
	//LOGIC_ResetHWToDefaults(StopPowerSupply);

	SUB_State = SS_None;
	CONTROL_SetDeviceState(DS_None);

	DataTable[REG_FAULT_REASON] = 0;
	CONTROL_RegistersReset();
}
//------------------------------------------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;

	switch (ActionID)
	{
		case ACT_CLR_FAULT:
			{
				if (CONTROL_State == DS_Fault)
					CONTROL_ResetToDefaults(TRUE);
			}
			break;

		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = 0;
			break;

		case ACT_ENABLE_POWER:
			{
				if (CONTROL_State == DS_None)
				{
					CONTROL_TimeCounterDelay = CONTROL_TimeCounter + BAT_CHARGE_TIMEOUT;
					CONTROL_SetDeviceState(DS_BatteryCharge);
					SUB_State = SS_Charge;
				}
				else if (CONTROL_State != DS_Ready)
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		case ACT_DISABLE_POWER:
			{
				if (CONTROL_State != DS_Disabled)
					CONTROL_ResetToDefaults(TRUE);
			}
			break;

		case ACT_START_PULSE:
			{
				if (CONTROL_State == DS_Ready)
				{
					CONTROL_RegistersReset();
					//
					//LOGIC_PrepareForPulse(DataTable[REG_DAC_I_Set], DataTable[REG_DAC_V_Set]);

					CONTROL_TimeCounterDelay = CONTROL_TimeCounter + TIME_Prepair1;
					CONTROL_SetDeviceState(DS_InProcess);
					SUB_State = SS_PulsePrepStep1;
				}
				else
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		case ACT_STOP_TEST:
			{
				if (CONTROL_State == DS_InProcess)
				{
					CONTROL_ResetToDefaults(FALSE);
					CONTROL_SetDeviceState(DS_Ready);
				}
			}
			break;

		case ACT_DBG_IFB_EN:
			if (CONTROL_State == DS_None)
			{
				LL_IFB_EN(TRUE);
				Delay_mS(1000);
				LL_IFB_EN(FALSE);
			}
			break;

		case ACT_DBG_RangeSelect:
			if (CONTROL_State == DS_None)
			{
				LL_RangeSelect(TRUE);
				Delay_mS(1000);
				LL_RangeSelect(FALSE);
			}
			break;

		case ACT_DBG_SYNC_SCPC:
			if (CONTROL_State == DS_None)
			{
				LL_SYNC_SCPC(TRUE);
				Delay_mS(1000);
				LL_SYNC_SCPC(FALSE);
			}
			break;

		case ACT_DBG_GATE_EN:
			if (CONTROL_State == DS_None)
			{
				LL_GATE_EN(TRUE);
				Delay_mS(1000);
				LL_GATE_EN(FALSE);
			}
			break;

		case ACT_DBG_SYNC_OSC:
			if (CONTROL_State == DS_None)
			{
				LL_SYNC_OSC(TRUE);
				Delay_mS(1000);
				LL_SYNC_OSC(FALSE);
			}
			break;

		case ACT_DBG_DAC_A_Write:
			if (CONTROL_State == DS_None)
			{
				//LL_DAC_Write((uint16_t) I_Set, DataTable[REG_DAC_I_Set]);
			}
			break;

		case ACT_DBG_DAC_B_Write:
			if (CONTROL_State == DS_None)
			{
				//LL_DAC_Write((uint16_t) V_Set, DataTable[REG_DAC_V_Set]);
			}
			break;


		default:
			return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------

void CONTROL_HandleBatteryCharge()
{
	/*
	switch (SUB_State)
	{
		// Запуск заряда на каждой ячейке
		case SS_Charge:
			{
				for (uint16_t Nid_Count = 0; Nid_Count < DataTable[REG_TOTAL_LSLPC]; Nid_Count++)
					LSLPC_Command(Interface, LSLPC_Data[Nid_Count].Nid, ACT_LSLPC_BAT_START_CHARGE);
				SUB_State = SS_ChargeWait;
			}
			break;

		case SS_ChargeWait:

			Nid_Count = 0;
			while (Nid_Count < DataTable[REG_TOTAL_LSLPC])
				{
				LSLPC_Read_Data(Interface, LSLPC_Data[Nid_Count].Nid, true);
				if(LSLPC_Data[Nid_Count].DevState = LSLPC_Ready)
					{
						Nid_Count++;
					}
						else if (CONTROL_TimeCounterDelay < CONTROL_TimeCounter)
						CONTROL_SwitchToFault(DF_BATTERY);
				}

			if((Nid_Count == DataTable[REG_TOTAL_LSLPC])&&(LSLPC_Data[Nid_Count].DevState = LSLPC_Ready))
			{
				SUB_State = SS_None;
				CONTROL_SetDeviceState(DS_Ready);
			}

		break;
		}
	*/
}
//-----------------------------------------------

void CONTROL_RegistersReset()
{
}
//-----------------------------------------------

void CONTROL_HandlePulse()
{
	/*
	switch (SUB_State)
	{
		case SS_PulsePrepStep1:
			if (CONTROL_TimeCounterDelay < CONTROL_TimeCounter)
			{
				CONTROL_TimeCounterDelay = CONTROL_TimeCounter + TIME_SET_LSLPC_TIME;
				//Всем блокам присваиваем значение  тока равным нулю
				for (uint16_t Nid_Count=0; Nid_Count<DataTable[REG_TOTAL_LSLPC]; Nid_Count++)
					{
						LSLPC_WriteData(Interface, LSLPC_Data[Nid_Count].Nid, REG_LSLPC_SC_PULSE_VALUE, 0);
					}
				// проверка
				Nid_Count = 0;
				while (Nid_Count < DataTable[REG_TOTAL_LSLPC])
					{
						LSLPC_Read_Data(Interface, LSLPC_Data[Nid_Count].Nid, true);
						if(LSLPC_Data[Nid_Count].SC_PulseValue == 0)
							{
								Nid_Count++;
							}
						else if (CONTROL_TimeCounterDelay < CONTROL_TimeCounter)
										CONTROL_SwitchToFault(DF_Z_SET);
					}

				if((Nid_Count == DataTable[REG_TOTAL_LSLPC])&&(LSLPC_Data[Nid_Count].SC_PulseValue == 0))
					{
					SUB_State = SS_PulsePrepStep2;
					}
			}
		break;

		case SS_PulsePrepStep2:

			//Распределяем значение  тока по блокам LSLPC
			Nid_Count = 0;
			float K2_Set = 0, K_Set = 1, B_Set = 0, CurrentSet = 0;

			CurrentSet = (float)DataTable[REG_CURRENT_VALUE];
			K2_Set = (float)((int16_t)DataTable[REG_P2_ISET]) / 1000000;
			K_Set  = (float)DataTable[REG_P1_ISET] / 1000;
			B_Set  = (float)((int16_t)DataTable[REG_P0_ISET]) / 10;
			CurrentSet = K2_Set * CurrentSet * CurrentSet + K_Set * CurrentSet + B_Set;

			if (CurrentSet > (float)LSL_CURRENT_MAX)
				CurrentSet = (float)LSL_CURRENT_MAX;
			else if (CurrentSet < (float)LSL_CURRENT_MIN)
			  	CurrentSet = (float)LSL_CURRENT_MIN;
			//Проверяем не было ли указания сконфигурировать только один блок
			if (DataTable[REG_NID_LSLPC_CONFIG] != 0)
				{
				//Проверяем есть ли требуемый блок в списке подключенных
				while (LSLPC_Data[Nid_Count].Nid != DataTable[REG_NID_LSLPC_CONFIG])
				{
					Nid_Count++;

					if (Nid_Count >= LSLPC_NUM_MAX)
					{
						DataTable[REG_WARNING] = WARNING_NID_NOT_FOUND;
						DataTable[REG_NID_LSLPC_CONFIG] = 0;
						return;
					}

				}

				if (CurrentSet > (float)LSLPC_CURRENT_MAX)
					CurrentSet = (float)LSLPC_CURRENT_MAX;

					LSLPC_WriteData(Interface, LSLPC_Data[Nid_Count].Nid, REG_LSLPC_SC_PULSE_VALUE, (uint16_t)CurrentSet);
					LSLPC_Command(Interface, LSLPC_Data[Nid_Count].Nid, ACT_LSLPC_SC_PULSE_CONFIG);

				DataTable[REG_NID_LSLPC_CONFIG] = 0;
				return;
				}

			//Всем учавствующим блокам, кроме 0-го прсваиваем максимальную амплитуду

			Nid_Count = (DataTable[REG_TOTAL_LSLPC] > 1) ? 1 : 0;
			CONTROL_TimeCounterDelay = CONTROL_TimeCounter + TIME_SET_LSLPC_TIME;
			while (CurrentSet > 0)
			{
				DEVPROFILE_ProcessRequests();
				if (CurrentSet > (float)LSLPC_CURRENT_MAX)
					{
						LSLPC_WriteData(Interface, LSLPC_Data[Nid_Count].Nid, REG_LSLPC_SC_PULSE_VALUE, LSLPC_CURRENT_MAX);
						LSLPC_Command(Interface, LSLPC_Data[Nid_Count].Nid, ACT_LSLPC_SC_PULSE_CONFIG);
						CurrentSet -= (float)LSLPC_CURRENT_MAX;
						Nid_Count++;
					}
				else //Остаток значения  тока присваеваем 0-му блоку
					{
						if (CurrentSet <= 0)
						CurrentSet = 10.0f;
						LSLPC_WriteData(Interface, LSLPC_Data[0].Nid, REG_LSLPC_SC_PULSE_VALUE, (uint16_t)CurrentSet);
						LSLPC_Command(Interface, LSLPC_Data[0].Nid, ACT_LSLPC_SC_PULSE_CONFIG);
					}
			}

			Nid_Count = 0;
			while (Nid_Count < DataTable[REG_TOTAL_LSLPC])
			{
				LSLPC_Read_Data(Interface, LSLPC_Data[Nid_Count].Nid, true);
				if(LSLPC_Data[Nid_Count].DevState == LSLPC_PulseConfigReady)
					{
						Nid_Count++;
					}
				else if (CONTROL_TimeCounterDelay < CONTROL_TimeCounter)
				CONTROL_SwitchToFault(DF_CONFIG_SET);
			}

			if((Nid_Count == DataTable[REG_TOTAL_LSLPC])&&(LSLPC_Data[Nid_Count].DevState == LSLPC_PulseConfigReady))
				{
					SUB_State = SS_PulsePrepCheckV;
					CONTROL_TimeCounterDelay = CONTROL_TimeCounter + TIME_SET_LSLPC_TIME;
				}

			break;

		case SS_PulsePrepCheckV:
				// Проверка уровня напряжения на батареях

				Nid_Count = 0;
				while (Nid_Count < DataTable[REG_TOTAL_LSLPC])
				{
					LSLPC_Read_Data(Interface, LSLPC_Data[Nid_Count].Nid, true);

					if(LSLPC_Data[Nid_Count].DevState = LSLPC_Ready)
						{
							Nid_Count++;
						}
					else if (CONTROL_TimeCounterDelay < CONTROL_TimeCounter)
							CONTROL_SwitchToFault(DF_BATTERY_P2P);
				}
				if((Nid_Count == DataTable[REG_TOTAL_LSLPC])&&(LSLPC_Data[Nid_Count].DevState = LSLPC_Ready))
				{
					SUB_State = SS_PulsePrepStep3;
					CONTROL_TimeCounterDelay = CONTROL_TimeCounter + TIME_SET_LSLPC_TIME;
				}

			break;



		case SS_PulsePrepStep3:
			// запуск формирования

			LOGIC_StartPulse();
			SUB_State = SS_PulseFinishWait;

			break;

		case SS_PulseFinishWait:
			// обработка результатов
			break;
	}
*/
}
//-----------------------------------------------

void CONTROL_Idle()
{
	// Process battery charge
	CONTROL_HandleBatteryCharge();

	// Handle pulse logic
	CONTROL_HandlePulse();

	// Process WD and interface
	CONTROL_WatchDogUpdate();
	DEVPROFILE_ProcessRequests();
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_SetDeviceState(DS_Fault);
	DataTable[REG_FAULT_REASON] = Reason;
}
//-----------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
}
//-----------------------------------------------

void Delay_mS(uint32_t Delay)
{
	uint64_t Counter = (uint64_t)CONTROL_TimeCounter + Delay;
	while (Counter != CONTROL_TimeCounter)
		CONTROL_WatchDogUpdate();
}
//-----------------------------------------------

void CONTROL_WatchDogUpdate()
{
	if (BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//-----------------------------------------------
