#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stdinc.h"

// Flash loader options
#define BOOT_LOADER_VARIABLE			(*((volatile Int32U *)0x20000000))
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
#define TIMER2_uS                		50			// Таймер тактирования АЦП 1,2
#define TIMER1_uS                		0.5f		// Таймер тактирования АЦП 3,4
#define TIMER3_uS                  		1000		// Таймер общего назначения
#define TIMER15_uS                 		1			// Таймер тактирования ЦАП
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// DMA
#define DMA_ADC_THYRISTOR_GATE_CH		DMA1_Channel1	// Канал DMA для АЦП Vg, Ig тиристора
#define DMA_ADC_IGBT_GATE_CH			DMA2_Channel1	// Канал DMA для АЦП Vg IGBT
#define DMA_ADC_ID_CH					DMA2_Channel2	// Канал DMA для АЦП Id
#define DMA_ADC_VD_CH					DMA2_Channel5	// Канал DMA для АЦП Vd
// ----------------------------------------------

// ADC
#define ADC1_THYRISTOR_UG_CH			2			// АЦП1 номер канала измерения Ug тиристора
#define ADC1_THYRISTOR_IG_CH			3			// АЦП1 номер канала измерения Ig тиристора
#define ADC2_IGBT_UG_CH					4			// АЦП2 номер канала измерения Ug IGBT
#define ADC3_UD_CH						5			// АЦП3 номер канала измерения Ud
#define ADC4_ID_CH						3			// АЦП4 номер канала измерения Id
// ----------------------------------------------

#endif // __SYSCONFIG_H
