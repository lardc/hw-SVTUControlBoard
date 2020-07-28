// -----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __GLOBAL_H
#define __GLOBAL_H

// Include
#include "stdinc.h"
#include "SysConfig.h"
#include "Logic.h"

// Definitions
// 
// Global miscellaneous parameters
#define	SCCI_TIMEOUT_TICKS				1000						// (в мс)
// Password to unlock non-volatile area for write
#define ENABLE_LOCKING					FALSE
// -----------------------------------------------
#define EP_WRITE_COUNT					0							// Количество массивов для записи
#define EP_COUNT						7							// Количество массивов для чтения
#define VALUES_x_SIZE					1000						// Размер выходных массивов
#define ENABLE_LOCKING					FALSE
// -----------------------------------------------



#define PC_ANS_TIMEOUT                       5                              //Масимальное время ожидания прихода ответа от LSLPC, мС
#define PC_START_ADDR                              150                             //Адрес первого блока LSLPC
#define PC_MAX_CELLS                        2                               //Максимально возможно число блоков LSLPC

#define PC_POWER_ENABLE_PAUSE				200
#define PC_CONFIG_PAUSE						20
#define PC_CHARGE_TIMEOUT				60000
#define PC_MAX_CURRENT                       6500                           //Максимальное значение тока для блока LSLPC


// Временные параметры
#define TIME_LED_BLINK					500							// Период моргания светодиода на плате (в мс)
#define	TIME_Prepair1					10							// Время размагничивания сердечника и включения контактора (в мс)
#define TIME_Prepair2					200
#define TIME_SET_LSLPC_TIME				500

// -----------------------------------------------

// Параметры формирования выходного импульса
#define PI_VALUE						3.1416f						// Значение числа пи
#define DAC_MAIN_PULSE_STOP				20.0f						// Время после основного импульса для удержания 0 по уставке (в мкс)
#define DAC_TIME_STEP					TIMER6_uS					// Дискрета времени ЦАП (в мкс)
#define MEASURE_AVG						10							// Количество сэмплов для усреднения результатов
#define LOAD_R_STDEV					1							// Среднеквадратичное отклонение сопротивления для резистора
#define ANALOG_OPTO_UNLOCK				500							// Задержка отключения оптопары, блокирующей ОС ОУ (в мкс)
//
#define LSL_CURRENT_MIN                         150                             //Минимальное значение тока, A
#define LSL_CURRENT_MAX                         3000                           //Максимальное значение тока, А
#define LSLPC_CURRENT_MAX                       6500                           //Максимальное значение тока для блока LSLPC
// -----------------------------------------------

// Параметры заряда батареи
#define	BAT_CHARGE_TIMEOUT				60000						// Время ожидания заряда после подачи питания (в мс)
#define	BAT_CHARGE_SHORT_TIMEOUT		5000						// Время ожидания заряда между импульсами (в мс)
#define BAT1_ADC1_CH					3							// Номер канала АЦП батареи 1
#define BAT2_ADC1_CH					4							// Номер канала АЦП батареи 2
#define BAT_VOLTAGE_DELTA				5.0f						// Гистерезис поддержания напряжения (в В)
// -----------------------------------------------

// Параметры оцифровки
#define ADC_REF_VOLTAGE					3300.0f			// Опорное напряжение (в мВ)
#define ADC_RESOLUTION					4095

// Параметры ЦАП
#define DAC_REF_VOLTAGE					5000.0f			// Опорное напряжение (в мВ)
#define DAC_RESOLUTION					4095
// -----------------------------------------------

#endif //  __GLOBAL_H
