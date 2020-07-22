// Include
#include "Interrupts.h"
//
#include "LowLevel.h"
#include "Board.h"
#include "Controller.h"
#include "Logic.h"
#include "Global.h"

// Functions
//
void DMA1_Channel3_IRQHandler()
{
	if (DMA_IsTransferComplete(DMA1, DMA_ISR_TCIF3))
	{
		/*
		TIM_Stop(TIM6);
		TIM_Reset(TIM6);

		ADC_SamplingStop(ADC1);
		HS_State = HSS_None;
		*/

		DMA_TransferCompleteReset(DMA1, DMA_IFCR_CTCIF3);
	}
}
//-----------------------------------------

void USART1_IRQHandler()
{
	if (ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//-----------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if (NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------

void TIM3_IRQHandler()
{
	static uint16_t LED_BlinkTimeCounter = 0;

	if (TIM_StatusCheck(TIM3))
	{
		CONTROL_TimeCounter++;
		if (++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}

		TIM_StatusClear(TIM3);
	}
}
//-----------------------------------------
