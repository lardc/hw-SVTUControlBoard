#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Переход в состояние ожидания
#define ACT_DISABLE_POWER				2	// Отключение блока
#define ACT_FAULT_CLEAR					3	// Очистка fault
#define ACT_WARNING_CLEAR				4	// Очистка warning

#define ACT_DBG_PULSE_IG_FEEDBACK		10	// Импульс включения обратной связи Ig
#define ACT_DBG_PULSE_ID_LOW_RANGE		11	// Импульс включения нижнего диапазона измерения Id
#define ACT_DBG_PULSE_SYNC_SCPC			12	// Импульс синхронизации ячеек SCPC
#define ACT_DBG_PULSE_IG				13	// Импульс включения подачи сигнала управления
#define ACT_DBG_PULSE_SYNC_SCOPE		14	// Импульс синхронизации осциллографа
#define ACT_DBG_SET_GATE_VG				15	// Задание напряжения в цепи управления
#define ACT_DBG_SET_GATE_IG				16	// Задание тока в цепи управления

#define ACT_START_TEST					100	// Запуск процесса измерения
#define ACT_STOP_TEST					101	// Остановка процесса измерения

#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_R_SHUNT						0	// Сопротивление шунта (в мкОм)
#define REG_I_LOW_RANGE_LIMIT			1	// Граница нижнего диапазона тока (в А)

#define REG_IDL_OFFSET					2	// Смещение оцифрованного Id (нижний диапазон) (в тиках)
#define REG_IDL_K						3	// Коэффициент пересчёта напряжения АЦП (в мВ) в Id (нижний диапазон) (в А) х1000
#define REG_IDL_P0						4	// Полином точной корректировки Id (нижний диапазон) P0 (в А)
#define REG_IDL_P1						5	// Полином точной корректировки Id (нижний диапазон) P1 x1000
#define REG_IDL_P2						6	// Полином точной корректировки Id (нижний диапазон) P2 x1e6

#define REG_ID_OFFSET					7	// Смещение оцифрованного Id (в тиках)
#define REG_ID_K						8	// Коэффициент пересчёта напряжения АЦП (в мВ) в Id (в А) х1000
#define REG_ID_P0						9	// Полином точной корректировки Id P0 (в А)
#define REG_ID_P1						10	// Полином точной корректировки Id P1 x1000
#define REG_ID_P2						11	// Полином точной корректировки Id P2 x1e6

#define REG_VD_OFFSET					12	// Смещение оцифрованного Vd (в тиках)
#define REG_VD_K						13	// Коэффициент пересчёта напряжения АЦП (в мВ) в Vd (в мВ) х1000
#define REG_VD_P0						14	// Полином точной корректировки Vd P0 (в мВ)
#define REG_VD_P1						15	// Полином точной корректировки Vd P1 x1000
#define REG_VD_P2						16	// Полином точной корректировки Vd P2 x1e6

#define REG_IG_OFFSET					17	// Смещение оцифрованного Ig (в тиках)
#define REG_IG_K						18	// Коэффициент пересчёта напряжения АЦП (в мВ) в Ig (в мА) х1000
#define REG_IG_P0						19	// Полином точной корректировки Ig P0 (в мА)
#define REG_IG_P1						20	// Полином точной корректировки Ig P1 x1000
#define REG_IG_P2						21	// Полином точной корректировки Ig P2 x1e6

#define REG_VG_OFFSET					22	// Смещение оцифрованного Vg (в тиках)
#define REG_VG_K						23	// Коэффициент пересчёта напряжения АЦП (в мВ) в Vg (в мВ) х1000
#define REG_VG_P0						24	// Полином точной корректировки Vg P0 (в мВ)
#define REG_VG_P1						25	// Полином точной корректировки Vg P1 x1000
#define REG_VG_P2						26	// Полином точной корректировки Vg P2 x1e6

// Несохраняемы регистры чтения-записи
#define REG_CURRENT_SETPOINT			140	// Уставка тока (в А)

#define REG_DBG_GATE_PULSE_TIME			150	// Отладочный регистр длительности импульса управления (в мкс)
#define REG_DBG_VG_VALUE				151	// Отладочный регистр напряжения цепи управления (в мВ)
#define REG_DBG_IG_VALUE				152	// Отладочный регистр тока цепи управления (в мА)

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции

#define REG_QUADRATIC_CORR				254	// Use quadratic correction for block
// -----------------------------

// Endpoints
#define EP16_DATA_ID					1	// Оцифрованные данные Id
#define EP16_DATA_UD					2	// Оцифрованные данные Vd

// Fault and disable codes
#define FAULT_NONE						0
#define FAULT_PROTOCOL					0
#define FAULT_PC_UNEXPECTED_STATE		0
#define FAULT_PC_CHARGE_TIMEOUT			0

// Warning
#define WARNING_NONE					0

// User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	// Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	// Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	// Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	// Неверный ключ безопасности

#endif // __DEV_OBJ_DIC_H
