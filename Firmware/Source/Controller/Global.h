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
#define EP_COUNT					0				// Количество массивов для чтения
#define FEP_COUNT					5				// Количество массивов для чтения типа float
#define ENABLE_LOCKING				FALSE			// Защита паролем
#define VALUES_x_SIZE				1000			// Размер выходных массивов

// Временные параметры
#define TIME_LED_BLINK				500				// Период моргания светодиода на плате (в мс)
#define TIME_LCSU_UPDATE			10				// Период опроса силовых ячеек (в мс)
#define TIME_VG_STAB				1000			// Время стабилизации Vg (в мкс)
#define TIMEOUT_LCSU_SHORT			1000			// Короткий таймаут по операциям с LCSU (в мс)
#define TIME_FAULT_LED_BLINK		250				//
#define TIME_EXT_LAMP_ON_STATE		500				//

// Параметры аналоговой подсистемы
#define ADC_REF_VOLTAGE				3000.0f			// Опорное напряжение (в мВ)
#define ADC_RESOLUTION				4095
#define DAC_REF_VOLTAGE				3000.0f			// Опорное напряжение (в мВ)
#define DAC_RESOLUTION				4095
#define ADC_EMA_FACTOR				0.5f			// Коэффициент экспоненциального скользящего среднего

// Предельный измеряемые значения
#define VD_MIN_VALUE				500				// Минимальное измеряемое напряжение
#define VD_MAX_VALUE				5000			// Максимальное измеряемое напряжение
#define ID_MIN_VALUE				30				// Минимальное измеряемое значение тока
#define ID_MAX_VALUE				1800			// Максимальное измеряемое значение тока

// Различные параметры
#define SELFTEST_ALLOWED_ERROR		10				// Допустимое отключение параметров режима самотестирования
#define LCSU_COUNT_MAX				1				// Максимальное количество блоков LCSU в установке

// Параметры DMA
#define VALUES_POWER_DMA_SIZE		700

#endif // __GLOBAL_H
