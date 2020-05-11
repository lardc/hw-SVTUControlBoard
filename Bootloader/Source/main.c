// Include
//
#include "Global.h"
#include "Controller.h"
#include "Interrupts.h"
#include "SysConfig.h"
#include "BoardConfig.h"


// Forward functions
//
void SysClk_Config();
void IO_Config();
void CAN_Config();
void UART_Config();
void Timer2_Config();
void WatchDog_Config();


// Functions
//
int main()
{
	// Set request flag if firmware update is required
	if (*ProgramAddressStart == 0xFFFFFFFF || BOOT_LOADER_VARIABLE == BOOT_LOADER_REQUEST)
		WaitForFWUpload = TRUE;

	// Init peripherals
	SysClk_Config();
	IO_Config();
	CAN_Config();
	UART_Config();
	Timer2_Config();
	WatchDog_Config();

	// Init controller
	CONTROL_Init();

	// Infinite cycle
	while (true)
		CONTROL_Idle();

	return 0;
}
//--------------------------------------------

void SysClk_Config()
{
	RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//--------------------------------------------

void IO_Config()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);

	// Выходы
	GPIO_Config (GPIOA, Pin_1, Output, PushPull, HighSpeed, NoPull);				// PA1 - DAC_CS
	GPIO_Bit_Rst(GPIOA, Pin_1);
	GPIO_Config (GPIOA, Pin_2, Output, PushPull, HighSpeed, NoPull);				// PA2 - DAC_LDAC
	GPIO_Bit_Rst(GPIOA, Pin_2);
	GPIO_Config (GPIOA, Pin_3, Output, PushPull, HighSpeed, NoPull);				// PA1 - IFB_EN
	GPIO_Bit_Rst(GPIOA, Pin_3);
	GPIO_Config (GPIOA, Pin_8, Output, PushPull, HighSpeed, Pull_Up);				// PA8 - RangeSelect
	GPIO_Bit_Set(GPIOA, Pin_8);
	GPIO_Config (GPIOA, Pin_15, Output, PushPull, HighSpeed, NoPull);			 	// PA15 - SYNC_SCPC
	GPIO_Bit_Rst(GPIOA, Pin_15);
	GPIO_Config (GPIOB, Pin_0, Output, PushPull, HighSpeed, Pull_Up);				// PB0 - GATE_EN
	GPIO_Bit_Set(GPIOB, Pin_0);
	GPIO_Config (GPIOB, Pin_6, Output, PushPull, HighSpeed, NoPull);				// PB6 - SYNC_OSC
	GPIO_Bit_Rst(GPIOB, Pin_6);
	GPIO_Config (GPIOB, Pin_7, Output, PushPull, HighSpeed, NoPull);				// PB7 - LED
	GPIO_Bit_Rst(GPIOB, Pin_7);

	// Альтернативные функции портов
	GPIO_Config(GPIOA, Pin_9, AltFn, PushPull, HighSpeed, NoPull);		// PA9(USART1 TX)
	GPIO_AltFn(GPIOA, Pin_9, AltFn_7);

	GPIO_Config(GPIOA, Pin_10, AltFn, PushPull, HighSpeed, NoPull);		// PA10(USART1 RX)
	GPIO_AltFn(GPIOA, Pin_10, AltFn_7);

	GPIO_Config(GPIOA, Pin_11, AltFn, PushPull, HighSpeed, NoPull);		// PA11(CAN RX)
	GPIO_AltFn(GPIOA, Pin_11, AltFn_9);

	GPIO_Config(GPIOA, Pin_12, AltFn, PushPull, HighSpeed, NoPull);		// PA12(CAN TX)
	GPIO_AltFn(GPIOA, Pin_12, AltFn_9);
}
//--------------------------------------------

void CAN_Config()
{
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, FALSE);
	NCAN_FIFOInterrupt(TRUE);
	NCAN_FilterInit(0, 0, 0);		// Фильтр 0 пропускает все сообщения
}
//--------------------------------------------

void UART_Config()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//--------------------------------------------

void Timer2_Config()
{
	TIM_Clock_En(TIM_2);
	TIM_Config(TIM2, SYSCLK, TIMER2_uS);
	TIM_Interupt(TIM2, 0, true);
	TIM_Start(TIM2);
}
//--------------------------------------------

void WatchDog_Config()
{
	IWDG_Config();
	IWDG_ConfigureFastUpdate();
}
//--------------------------------------------
