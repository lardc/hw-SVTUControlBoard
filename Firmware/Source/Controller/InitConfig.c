// Header
#include "InitConfig.h"

// Includes
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"
#include "MemBuffers.h"
#include "Global.h"
#include "LowLevel.h"
// Functions
//
void INITCFG_ConfigSystemClock()
{
	RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
	RCC_SysCfg_Clk_EN();
}
//------------------------------------

void INITCFG_ConfigGPIO()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	
	// Аналоговые входы
	GPIO_InitAnalog(GPIO_MSR_IGBT_UG);
	GPIO_InitAnalog(GPIO_MSR_UD);
	GPIO_InitAnalog(GPIO_MSR_ID);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_SYNC_OSC);
	GPIO_InitPushPullOutput(GPIO_AIN_ST);
	GPIO_InitPushPullOutput(GPIO_SYNC_LCSU);
	GPIO_InitPushPullOutput(GPIO_IND_CTRL);
	GPIO_InitPushPullOutput(GPIO_ID_RANGE);

	// Начальная установка состояний
	GPIO_SetState(GPIO_LED, false);
	GPIO_SetState(GPIO_SYNC_OSC, false);
	GPIO_SetState(GPIO_AIN_ST, false);
	GPIO_SetState(GPIO_SYNC_LCSU, false);
	GPIO_SetState(GPIO_IND_CTRL, false);
	GPIO_SetState(GPIO_ID_RANGE, false);
	
	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_CAN_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_UART_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART_TX, AltFn_7);
}
//------------------------------------

void INITCFG_ConfigCAN()
{
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, false);
	NCAN_FIFOInterrupt(true);
	NCAN_FilterInit(0, CAN_SLAVE_FILTER_ID, CAN_SLAVE_NID_MASK);
	NCAN_FilterInit(1, CAN_MASTER_FILTER_ID, CAN_MASTER_NID_MASK);
}
//------------------------------------

void INITCFG_ConfigUART()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//------------------------------------

void INITCFG_ConfigADC()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	RCC_ADC_Clk_EN(ADC_34_ClkEN);

	ADC1_2_SetDualMode(true);
	ADC3_4_SetDualMode(true);

	// ADC1
	ADC_Calibration(ADC1);
	ADC_TrigConfig(ADC1, ADC12_TIM2_TRGO, RISE);
	ADC_ChannelSeqReset(ADC1);
	ADC_ChannelSet_Sequence(ADC1, ADC1_IGBT_IG_CH, 1);
	ADC_ChannelSeqLen(ADC1, 1);
	ADC_Interrupt(ADC1, EOCIE, 0, true);
	ADC_Enable(ADC1);

	// ADC2
	ADC_Calibration(ADC2);
	ADC_ChannelSeqReset(ADC2);
	ADC_ChannelSet_Sequence(ADC2, ADC2_IGBT_UG_CH, 1);
	ADC_ChannelSeqLen(ADC2, 1);
	ADC_Enable(ADC2);

	// ADC3
	ADC_Calibration(ADC3);
	ADC_TrigConfig(ADC3, ADC34_TIM1_TRGO, RISE);
	ADC_ChannelSeqReset(ADC3);
	ADC_ChannelSet_Sequence(ADC3, ADC3_UD_CH, 1);
	ADC_ChannelSeqLen(ADC3, 1);
	ADC_DMAEnable(ADC3, true);
	ADC_Enable(ADC3);

	// ADC4
	ADC_Calibration(ADC4);
	ADC_ChannelSeqReset(ADC4);
	ADC_ChannelSet_Sequence(ADC4, ADC4_ID_CH, 1);
	ADC_ChannelSeqLen(ADC4, 1);
	ADC_DMAEnable(ADC4, true);
	ADC_Enable(ADC4);

	ADC_SamplingStart(ADC1);
	ADC_SamplingStart(ADC3);
}
//------------------------------------

void INITCFG_ConfigTimer3()
{
	TIM_Clock_En(TIM_3);
	TIM_Config(TIM3, SYSCLK, TIMER3_uS);
	TIM_Interupt(TIM3, 0, true);
	TIM_Start(TIM3);
}
//------------------------------------

void INITCFG_ConfigTimer1()
{
	TIM_Clock_En(TIM_1);
	TIM_Config(TIM1, SYSCLK, TIMER1_uS);
	TIM_DMA(TIM1, DMAEN);
	TIM_MasterMode(TIM1, MMS_UPDATE);
}
//------------------------------------

void INITCFG_ConfigTimer2()
{
	TIM_Clock_En(TIM_2);
	TIM_Config(TIM2, SYSCLK, TIMER2_uS);
	TIM_DMA(TIM2, DMAEN);
	TIM_MasterMode(TIM2, MMS_UPDATE);
}
//------------------------------------

void INITCFG_ConfigDMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);
	DMA_Clk_Enable(DMA2_ClkEN);
	
	DMA_Reset(DMA_ADC_ID_CH);
	DMA_Interrupt(DMA_ADC_ID_CH, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_ID_CH, (Int32U)(&MEMBUF_DMA_Id[0]), (Int32U)(&ADC4->DR), VALUES_POWER_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_ID_CH, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_32BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
	
	DMA_Reset(DMA_ADC_VD_CH);
	DMA_Interrupt(DMA_ADC_VD_CH, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_VD_CH, (Int32U)(&MEMBUF_DMA_Vd[0]), (Int32U)(&ADC3->DR), VALUES_POWER_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_VD_CH, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_32BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
}
//------------------------------------

void INITCFG_ConfigDAC()
{
	DACx_Clk_Enable(DAC_1_ClkEN);
	DACx_Reset();
	DAC_TriggerConfigCh1(DAC1, TRIG1_TIMER15, TRIG1_ENABLE);
	DAC_BufferCh2(DAC1, false);
	DAC_EnableCh2(DAC1);
}
//------------------------------------------------

void INITCFG_ConfigTimer15()
{
	TIM_Clock_En(TIM_15);
	TIM_Config(TIM15, SYSCLK, TIMER15_uS);
	TIM_MasterMode(TIM15, MMS_UPDATE);
	TIM_Start(TIM15);
}
//------------------------------------------------

void INITCFG_ConfigTimer6(Int16U Period_us)
{
	TIM_Clock_En(TIM_6);
	TIM_Config(TIM6, SYSCLK, Period_us);
	TIM_Interupt(TIM6, 0, true);
}
//------------------------------------------------

void INITCFG_ConfigTimer7(Int16U Period_us)
{
	TIM_Clock_En(TIM_7);
	TIM_Config(TIM7, SYSCLK, Period_us);
	TIM_Interupt(TIM7, 0, true);
}
//------------------------------------------------

void INITCFG_ConfigWatchDog()
{
	IWDG_Config();
	IWDG_ConfigureFastUpdate();
}
//------------------------------------
