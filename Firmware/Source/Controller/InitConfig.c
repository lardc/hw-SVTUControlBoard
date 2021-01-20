// Header
#include "InitConfig.h"

// Includes
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"
#include "MemBuffers.h"
#include "Global.h"

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
	GPIO_InitAnalog(GPIO_MEASURE_IG);
	GPIO_InitAnalog(GPIO_MEASURE_VG);
	GPIO_InitAnalog(GPIO_MEASURE_ID);
	GPIO_InitAnalog(GPIO_MEASURE_VD);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_SYNC_POWER_CELL);
	GPIO_InitPushPullOutput(GPIO_SYNC_SCOPE);
	GPIO_InitPushPullOutput(GPIO_DAC_CS);
	GPIO_InitPushPullOutput(GPIO_DAC_LDAC);

	// Выходы с OpenDrain
	GPIO_InitOpenDrainOutput(GPIO_IG_PULSE, NoPull);
	GPIO_InitOpenDrainOutput(GPIO_ID_LOW_RANGE, NoPull);

	// Начальная установка состояний
	GPIO_SetState(GPIO_DAC_CS, true);
	GPIO_SetState(GPIO_DAC_LDAC, true);
	GPIO_SetState(GPIO_IG_PULSE, true);
	
	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_CAN_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_UART_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_MOSI, AltFn_5);
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

void INITCFG_ConfigSPI()
{
	SPI_Init(SPI1, 3, false);
	SPI_InvertClockPolarity(SPI1, true);
}
//------------------------------------

void INITCFG_ConfigADC()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	RCC_ADC_Clk_EN(ADC_34_ClkEN);
	
	ADC_Calibration(ADC1);
	ADC_Calibration(ADC2);
	ADC_Calibration(ADC3);
	ADC_Calibration(ADC4);
	
	ADC1_2_SetDualMode(true);
	ADC3_4_SetDualMode(true);
	
	ADC_Enable(ADC1);
	ADC_Enable(ADC2);
	ADC_Enable(ADC3);
	ADC_Enable(ADC4);
	
	ADC_TrigConfig(ADC1, ADC12_TIM1_TRGO, RISE);
	ADC_TrigConfig(ADC3, ADC34_TIM1_TRGO, RISE);
	
	ADC_ChannelSeqReset(ADC1);
	ADC_ChannelSeqReset(ADC2);
	ADC_ChannelSeqReset(ADC3);
	ADC_ChannelSeqReset(ADC4);
	
	ADC_ChannelSet_Sequence(ADC1, ADC1_IG_CHANNEL, 1);
	ADC_ChannelSet_Sequence(ADC2, ADC2_VG_CHANNEL, 1);
	ADC_ChannelSet_Sequence(ADC3, ADC3_ID_CHANNEL, 1);
	ADC_ChannelSet_Sequence(ADC4, ADC4_VD_CHANNEL, 1);
	
	ADC_ChannelSeqLen(ADC1, 1);
	ADC_ChannelSeqLen(ADC2, 1);
	ADC_ChannelSeqLen(ADC3, 1);
	ADC_ChannelSeqLen(ADC4, 1);
	
	ADC_DMAConfig(ADC1);
	ADC_DMAConfig(ADC2);
	ADC_DMAConfig(ADC3);
	ADC_DMAConfig(ADC4);

	ADC_DMAEnable(ADC1, true);
	ADC_DMAEnable(ADC2, true);
	ADC_DMAEnable(ADC3, true);
	ADC_DMAEnable(ADC4, true);
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

void INITCFG_ConfigDMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);
	DMA_Clk_Enable(DMA2_ClkEN);
	
	DMA_Reset(DMA_ADC_IG_CHANNEL);
	DMA_Interrupt(DMA_ADC_IG_CHANNEL, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_IG_CHANNEL, (uint32_t)(&MEMBUF_DMA_Ig[0]), (uint32_t)(&ADC1->DR), VALUES_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_IG_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
	
	DMA_Reset(DMA_ADC_VG_CHANNEL);
	DMA_Interrupt(DMA_ADC_VG_CHANNEL, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_VG_CHANNEL, (uint32_t)(&MEMBUF_DMA_Vg[0]), (uint32_t)(&ADC2->DR), VALUES_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_VG_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
	
	DMA_Reset(DMA_ADC_ID_CHANNEL);
	DMA_Interrupt(DMA_ADC_ID_CHANNEL, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_ID_CHANNEL, (uint32_t)(&MEMBUF_DMA_Id[0]), (uint32_t)(&ADC3->DR), VALUES_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_ID_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
	
	DMA_Reset(DMA_ADC_VD_CHANNEL);
	DMA_Interrupt(DMA_ADC_VD_CHANNEL, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_VD_CHANNEL, (uint32_t)(&MEMBUF_DMA_Vd[0]), (uint32_t)(&ADC4->DR), VALUES_DMA_SIZE);
	DMAChannelX_Config(DMA_ADC_VD_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH);
}
//------------------------------------

void INITCFG_ConfigWatchDog()
{
	IWDG_Config();
	IWDG_ConfigureFastUpdate();
}
//------------------------------------
