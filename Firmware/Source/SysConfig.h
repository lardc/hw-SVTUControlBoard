#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stdinc.h"

// Flash loader options
#define BOOT_LOADER_VARIABLE			(*((volatile uint32_t *)0x20000000))
#define BOOT_LOADER_REQUEST				0x12345678
#define BOOT_LOADER_MAIN_PR_ADDR		0x08008000
//-----------------------------------------------

// System clock
#define SYSCLK							70000000	// Тактовая частота системной шины процессора
#define QUARTZ_FREQUENCY				20000000	// Частота кварца
// ----------------------------------------------

// USART
#define USART_BAUDRATE					115200		// Скорость USART
#define USART_FIFOlen					32			// Длина FIFO USART
// ----------------------------------------------

// Timers
#define TIMER6_uS                		0.5f		// Таймер тактирования ЦАП и АЦП
#define TIMER3_uS                  		1000		// Таймер общего назначения
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// DMA
#define DMA_ADC_Igt_CHANNEL				DMA1_Channel1	// Канал DMA для АЦП Igt
#define DMA_ADC_Ugt_CHANNEL				DMA2_Channel1	// Канал DMA для АЦП Ugt
#define DMA_ADC_Itm_CHANNEL				DMA2_Channel5	// Канал DMA для АЦП Itm
#define DMA_ADC_Utm_CHANNEL				DMA2_Channel4	// Канал DMA для АЦП Utm
// ----------------------------------------------

// ADC
#define ADC1_VOLTAGE_CHANNEL			1			// АЦП1 номер канала напряжения
#define ADC2_CURRENT_CHANNEL			4			// АЦП2 номер канала тока
// ----------------------------------------------

#endif // __SYSCONFIG_H
