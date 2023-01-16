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

// Variables
//
static volatile bool IgCompleted, VgCompleted, IdCompleted, VdCompleted;

// Functions
//
void TIM6_DAC_IRQHandler()
{
	Int16U ADC_RawValue = 0;
	static Int16U ItmMax = 0;
	Int16U ArrayIndex = 0;

	if(TIM_StatusCheck(TIM6))
	{
		if(LL_SyncScopeGetState())
		{
			ArrayIndex = DMA_ReadDataCount(DMA_ADC_ID_CHANNEL) - 1;
			ADC_RawValue = MEMBUF_DMA_Id[ArrayIndex];
			if(LL_IsIdLowRange())
				MEASURE_ConvertIdLow(&ADC_RawValue, 1);
			else
				MEASURE_ConvertId(&ADC_RawValue, 1);

			if(ItmMax < ADC_RawValue)
				ItmMax = ADC_RawValue;

			if((ADC_RawValue <= DataTable[REG_CURRENT_SETPOINT]) && (ADC_RawValue < ItmMax))
				LL_SyncScope(false);
		}
		else
		{
			ItmMax = 0;
			TIM_Stop(TIM6);
		}

		TIM_StatusClear(TIM6);
	}
}
//-----------------------------------------

bool IT_DMASampleCompleted()
{
	return IgCompleted && VgCompleted && IdCompleted && VdCompleted;
}
//-----------------------------------------

void IT_DMAFlagsReset()
{
	IgCompleted = VgCompleted = IdCompleted = VdCompleted = false;
}
//-----------------------------------------

void DMA1_Channel1_IRQHandler()
{
	// Ig
	if(DMA_IsTransferComplete(DMA1, DMA_ISR_TCIF1))
	{
		IgCompleted = true;
		DMA_TransferCompleteReset(DMA1, DMA_IFCR_CTCIF1);
	}
}
//-----------------------------------------

void DMA2_Channel1_IRQHandler()
{
	// Vg
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF1))
	{
		VgCompleted = true;
		DMA_TransferCompleteReset(DMA2, DMA_IFCR_CTCIF1);
	}
}
//-----------------------------------------

void DMA2_Channel5_IRQHandler()
{
	// Id
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF5))
	{
		IdCompleted = true;
		DMA_TransferCompleteReset(DMA2, DMA_IFCR_CTCIF5);
	}
}
//-----------------------------------------

void DMA2_Channel2_IRQHandler()
{
	// Vd
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF2))
	{
		VdCompleted = true;
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
	static uint16_t LED_BlinkTimeCounter = 0;
	
	if(TIM_StatusCheck(TIM3))
	{
		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}
		
		TIM_StatusClear(TIM3);
	}
}
//-----------------------------------------
