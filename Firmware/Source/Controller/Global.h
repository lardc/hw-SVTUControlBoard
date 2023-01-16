﻿// ----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "SysConfig.h"

// Параметры интерфейсов
#define	SCCI_TIMEOUT_TICKS			1000			// Таймаут протоколоа SCCI (в мс)
#define	BCCIM_TIMEOUT_TICKS			50				// Таймаут протоколоа мастер BCCI (в мс)
#define EP_WRITE_COUNT				0				// Количество массивов для записи
#define EP_COUNT					4				// Количество массивов для чтения
#define ENABLE_LOCKING				FALSE			// Защита паролем
#define VALUES_x_SIZE				500				// Размер выходных массивов

// Временные параметры
#define TIME_LED_BLINK				500				// Период моргания светодиода на плате (в мс)
#define TIME_PC_UPDATE				500				// Период опроса силовых ячеек (в мс)
#define TIME_VG_STAB				100				// Время стабилизации Vg (в мкс)
#define TIME_PULSE_WIDTH			10000			// Длительность импульса (в мкс)
#define TIMEOUT_PC_SHORT			1000			// Короткий таймаут по операциям с силовыми ячейками (в мс)
#define TIME_GATE_PULSE_MAX			1000			// Максимальная длительность импульса тока управления

// Параметры аналоговой подсистемы
#define ADC_REF_VOLTAGE				3300.0f			// Опорное напряжение (в мВ)
#define ADC_RESOLUTION				4095
#define DAC_REF_VOLTAGE				5000.0f			// Опорное напряжение (в мВ)
#define DAC_RESOLUTION				4095
#define ADC_EMA_FACTOR				0.5f			// Коэффициент экспоненциального скользящего среднего

// Предельный измеряемые значения
#define VOLTAGE_MIN_VALUE			500				// Минимальное измеряемое напряжение
#define VOLTAGE_MAX_VALUE			5000				// Максимальное измеряемое напряжение

// Параметры DMA
#define VALUES_POWER_DMA_SIZE		2000
#define VALUES_GATE_DMA_SIZE		500

#endif // __GLOBAL_H
