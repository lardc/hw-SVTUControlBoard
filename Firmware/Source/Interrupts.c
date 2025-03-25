// Include
#include "Interrupts.h"
//
#include "LowLevel.h"
#include "Board.h"
#include "Controller.h"
#include "Logic.h"
#include "Global.h"
#include "DataTable.h"
#include "Measurement.h"
#include "MemBuffers.h"
#include "GateDriver.h"
#include "Constraints.h"

// Variables
//
static volatile bool ItCompleted, UTCompleted, UT2Completed;

// Functions
//
void ADC1_2_IRQHandler()
{
	// Расчеты производятся только для версии платы 1.0
	float GateVoltage, GateCurrent;

	GateVoltage = MEASURE_Ug_ADC_Direct(ADC_Read(ADC2));
	GateCurrent = MEASURE_Ig_ADC_Direct(ADC_Read(ADC1));

	GATE_RegulatorProcess(GateVoltage, GateCurrent);
}
//-----------------------------------------

bool IT_DMASampleCompleted()
{
	switch((Int16U)DataTable[REG_PCB_VERSION])
	{
		case PCB_VERSION_10:
			return ItCompleted && UTCompleted;

		case PCB_VERSION_20:
			if((Int16U)DataTable[REG_PCB_TIRIS_IGBT] == PCB_TIRIS)
			{
				return ItCompleted && UTCompleted;
				break;
			}
			if((Int16U)DataTable[REG_PCB_TIRIS_IGBT] == PCB_IGBT)
			{
				return ItCompleted && UTCompleted && UT2Completed;
			}

		default:
			return 0;
	}
}
//-----------------------------------------

void IT_DMAFlagsReset()
{
	ItCompleted = UTCompleted = UT2Completed = false;
}
//-----------------------------------------

void DMA1_Channel1_IRQHandler()
{
	// UT2
	if(DMA_IsTransferComplete(DMA1, DMA_ISR_TCIF1))
	{
		UT2Completed = true;
		DMA_TransferCompleteReset(DMA1, DMA_IFCR_CTCIF1);
	}

	// Расчеты производятся только для версии платы 2.0 с тиристором
	float GateVoltage, GateCurrent;

	DMA_ChannelReload(DMA_ADC_UT2_UGIG, 2);
	DMA_ChannelEnable(DMA_ADC_UT2_UGIG, true);
	GateVoltage = MEASURE_Ug_DMA(MEMBUF_DMA_Ut2_UgIg, 0);
	GateCurrent = MEASURE_Ig_DMA(MEMBUF_DMA_Ut2_UgIg, 1);

	GATE_RegulatorProcess(GateVoltage, GateCurrent);
}
//-----------------------------------------

void DMA1_Channel2_IRQHandler()
{
	// Расчеты производятся только для версии платы 2.0 с IGBT
	float GateVoltage, GateCurrent;

	DMA_ChannelReload(DMA_ADC_IGBT_UGIG, 2);
	DMA_ChannelEnable(DMA_ADC_IGBT_UGIG, true);
	GateVoltage = MEASURE_Ug_DMA(MEMBUF_DMA_IGBT_UgIg, 0);
	GateCurrent = MEASURE_Ig_DMA(MEMBUF_DMA_IGBT_UgIg, 1);

	GATE_RegulatorProcess(GateVoltage, GateCurrent);
}
//-----------------------------------------

void DMA2_Channel5_IRQHandler()
{
	// It
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF5))
	{
		ItCompleted = true;
		DMA_TransferCompleteReset(DMA2, DMA_IFCR_CTCIF5);
	}
}
//-----------------------------------------

void DMA2_Channel2_IRQHandler()
{
	// UT
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF2))
	{
		UTCompleted = true;
		DMA_TransferCompleteReset(DMA2, DMA_IFCR_CTCIF2);
	}
}
//-----------------------------------------

void USART1_IRQHandler()
{
	if(ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//-----------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if(NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------

void TIM3_IRQHandler()
{
	static Int16U LED_BlinkTimeCounter = 0;
	
	if(TIM_StatusCheck(TIM3))
	{
		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}
		
		CONTROL_HandleExternalLamp(IsImpulse);

		TIM_StatusClear(TIM3);
	}
}
//-----------------------------------------
