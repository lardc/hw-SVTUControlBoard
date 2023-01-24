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

// Variables
//
static volatile bool IdCompleted, VdCompleted;

// Functions
//
void ADC1_2_IRQHandler()
{
	GATE_RegulatorProcess(MEASURE_Vg(ADC2->DR));
}
//-----------------------------------------

bool IT_DMASampleCompleted()
{
	return IdCompleted && VdCompleted;
}
//-----------------------------------------

void IT_DMAFlagsReset()
{
	IdCompleted = VdCompleted = false;
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
