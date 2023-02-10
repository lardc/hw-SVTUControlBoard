#include "InitConfig.h"
#include "Controller.h"
#include "SysConfig.h"

int main()
{
	__disable_irq();
	SCB->VTOR = (Int32U)BOOT_LOADER_MAIN_PR_ADDR;
	__enable_irq();
	
	// Настройка аппаратной периферии
	INITCFG_ConfigSystemClock();
	INITCFG_ConfigGPIO();
	INITCFG_ConfigCAN();
	INITCFG_ConfigUART();
	INITCFG_ConfigADC();
	INITCFG_ConfigTimer3();
	INITCFG_ConfigTimer1();
	INITCFG_ConfigTimer2();
	INITCFG_ConfigTimer15();
	INITCFG_ConfigDAC();
	INITCFG_ConfigDMA();
	INITCFG_ConfigWatchDog();
	
	// Инициализация логики контроллера
	CONTROL_Init();
	
	while(true)
		CONTROL_Idle();
	
	return 0;
}
//------------------------------------
