#include "Interrupts.h"
#include "InitConfig.h"
#include "SysConfig.h"

// Functions
//
void CONTROL_Idle();

int main()
{
	__disable_irq();
	SCB->VTOR = (uint32_t)BOOT_LOADER_MAIN_PR_ADDR;
	__enable_irq();

	// Настройка системной частоты тактирования
	SysClk_Config();

	// Настройка портов
	IO_Config();

	// Настройка CAN
	CAN_Config();

	// Настройка UART
	UART_Config();

	// Настройка SPI
	SPI_Config();

	// Настройка ЦАПа
	DAC_Config();

	// Настройка Timer3 общего назначения
	Timer3_Config();

	// Настройка Timer6 тактирования ЦАП и АЦП
	Timer6_Config();

	// Настройка Timer2 скоростной, вспомогательный
	Timer2_Config();

	// Начальная настройка АЦП
	ADC_Config();

	// Настройка DMA для ЦАП и АЦП
	DMA_Config();

	// Настройка сторожевого таймера
	WatchDog_Config();

	// Инициализация логики контроллера
	InitializeController(false);

	// Фоновый цикл
	while (TRUE)
		CONTROL_Idle();

	return 0;
}
