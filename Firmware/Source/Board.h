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
GPIO_PortPinSettingMacro GPIO_MSR_THYRISTOR_UG 		= {GPIOA, Pin_1};
GPIO_PortPinSettingMacro GPIO_MSR_THYRISTOR_IG 		= {GPIOA, Pin_2};
GPIO_PortPinSettingMacro GPIO_MSR_IGBT_UG 			= {GPIOA, Pin_7};
GPIO_PortPinSettingMacro GPIO_MSR_UD 				= {GPIOB, Pin_13};
GPIO_PortPinSettingMacro GPIO_MSR_ID				= {GPIOB, Pin_12};
GPIO_PortPinSettingMacro GPIO_ID_RANGE				= {GPIOB, Pin_14};

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_LED 				= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_GATE_EN			= {GPIOA, Pin_0};
GPIO_PortPinSettingMacro GPIO_TGATE_ST			= {GPIOA, Pin_6};
GPIO_PortPinSettingMacro GPIO_SYNC_OSC			= {GPIOA, Pin_15};
GPIO_PortPinSettingMacro GPIO_AIN_ST			= {GPIOB, Pin_1};
GPIO_PortPinSettingMacro GPIO_SYNC_LCSU			= {GPIOB, Pin_5};
GPIO_PortPinSettingMacro GPIO_IND_CTRL			= {GPIOB, Pin_6};

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_SAFETY			= {GPIOB, Pin_4};

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_CAN_RX 		= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_ALT_CAN_TX 		= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ALT_UART_RX 		= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_UART_TX 		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_SPI1_CLK 		= {GPIOA, Pin_5};
GPIO_PortPinSettingMacro GPIO_ALT_SPI1_MOSI 	= {GPIOA, Pin_7};

#endif // __BOARD_H
