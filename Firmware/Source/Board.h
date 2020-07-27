#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f30x.h"

#include "ZwRCC.h"
#include "ZwGPIO.h"
#include "ZwNCAN.h"
#include "ZwTIM.h"
#include "ZwDAC.h"
#include "ZwDMA.h"
#include "ZwADC.h"
#include "ZwEXTI.h"
#include "ZwSCI.h"
#include "ZwSPI.h"
#include "ZwIWDG.h"
#include "ZwNFLASH.h"

// Определения для аналоговых портов
GPIO_PortPinSettingMacro GPIO_MEASURE_IG 		= {GPIOA, Pin_0};
GPIO_PortPinSettingMacro GPIO_MEASURE_VG 		= {GPIOA, Pin_4};
GPIO_PortPinSettingMacro GPIO_MEASURE_ID 		= {GPIOB, Pin_1};
GPIO_PortPinSettingMacro GPIO_MEASURE_VB 		= {GPIOB, Pin_12};

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_LED 				= {GPIOB, Pin_7};
GPIO_PortPinSettingMacro GPIO_IG_FEEDBACK 		= {GPIOA, Pin_3};
GPIO_PortPinSettingMacro GPIO_ID_LOW_RANGE	 	= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_SYNC_POWER_CELL 	= {GPIOA, Pin_15};
GPIO_PortPinSettingMacro GPIO_IG_PULSE			= {GPIOB, Pin_0};
GPIO_PortPinSettingMacro GPIO_SYNC_SCOPE		= {GPIOB, Pin_6};
GPIO_PortPinSettingMacro GPIO_DAC_CS			= {GPIOA, Pin_1};
GPIO_PortPinSettingMacro GPIO_DAC_LDAC			= {GPIOA, Pin_2};

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_CAN_RX 		= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_ALT_CAN_TX 		= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ALT_UART_RX 		= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_UART_TX 		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_CLK 		= {GPIOA, Pin_5};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_MOSI 		= {GPIOA, Pin_7};

#endif // __BOARD_H
