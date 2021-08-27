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
#define TIMER2_uS                		0.5f		// Таймер тактирования АЦП 1,2
#define TIMER1_uS                		5			// Таймер тактирования АЦП 3,4
#define TIMER3_uS                  		1000		// Таймер общего назначения
#define TIMER6_uS                  		20			// Таймер определения момента синхронизации
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// DMA
#define DMA_ADC_IG_CHANNEL				DMA1_Channel1	// Канал DMA для АЦП Ig
#define DMA_ADC_VG_CHANNEL				DMA2_Channel1	// Канал DMA для АЦП Vg
#define DMA_ADC_ID_CHANNEL				DMA2_Channel5	// Канал DMA для АЦП Id
#define DMA_ADC_VD_CHANNEL				DMA2_Channel2	// Канал DMA для АЦП Vd
// ----------------------------------------------

// ADC
#define ADC1_IG_CHANNEL					1			// АЦП1 номер канала измерения Ig
#define ADC2_VG_CHANNEL					1			// АЦП2 номер канала измерения Vg
#define ADC3_ID_CHANNEL					1			// АЦП3 номер канала измерения Id
#define ADC4_VD_CHANNEL					3			// АЦП4 номер канала измерения Vd
// ----------------------------------------------

#endif // __SYSCONFIG_H
