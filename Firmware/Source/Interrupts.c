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
static volatile bool ItCompleted, UTCompleted;

// Functions
//
void ADC1_2_IRQHandler()
{
	float GateVoltage, GateCurrent;
	switch((Int16U)DataTable[REG_PCB_VERSION])
	{
		case PCB_VERSION_10:
			GateVoltage = MEASURE_Ug_ADC_Direct(ADC_Read(ADC2));
			GateCurrent = MEASURE_Ig_ADC_Direct(ADC_Read(ADC1));
			break;

		/*case PCB_VERSION_20:
			if((Int16U)DataTable[REG_PCB_TIRIS_IGBT] == PCB_TIRIS)
			{
				DMA_ChannelReload(DMA_ADC_UT2_UGIG, 2);
    			DMA_ChannelEnable(DMA_ADC_UT2_UGIG, true);
				TIM_Start(TIM3);
				GateVoltage = MEASURE_Ug_DMA(MEMBUF_DMA_Ut2_UgIg, 0);
				GateCurrent = MEASURE_Ig_DMA(MEMBUF_DMA_Ut2_UgIg, 1);
				TIM_Stop(TIM3);
			}
			else if((Int16U)DataTable[REG_PCB_TIRIS_IGBT] == PCB_IGBT)
			{
				DMA_ChannelReload(DMA_ADC_IGBT_UGIG, 2);
    			DMA_ChannelEnable(DMA_ADC_IGBT_UGIG, true);
				TIM_Start(TIM3);
				GateVoltage = MEASURE_Ug_DMA(MEMBUF_DMA_IGBT_UgIg, 0);
				GateCurrent = MEASURE_Ig_DMA(MEMBUF_DMA_IGBT_UgIg, 1);
				TIM_Stop(TIM3);
			}
			break;*/
	}

	GATE_RegulatorProcess(GateVoltage, GateCurrent);
}
//-----------------------------------------

bool IT_DMASampleCompleted()
{
	return ItCompleted && UTCompleted;
}
//-----------------------------------------

void IT_DMAFlagsReset()
{
	ItCompleted = UTCompleted = false;
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
