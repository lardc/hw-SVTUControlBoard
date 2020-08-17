// Include
#include "Interrupts.h"
//
#include "LowLevel.h"
#include "Board.h"
#include "Controller.h"
#include "Logic.h"
#include "Global.h"

// Variables
//
static volatile bool IgCompleted, VgCompleted, IdCompleted, VdCompleted;

// Functions
//
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

void DMA2_Channel4_IRQHandler()
{
	// Vd
	if(DMA_IsTransferComplete(DMA2, DMA_ISR_TCIF4))
	{
		VdCompleted = true;
		DMA_TransferCompleteReset(DMA2, DMA_IFCR_CTCIF4);
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
