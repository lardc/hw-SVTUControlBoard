#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Переход в состояние ожидания
#define ACT_DISABLE_POWER				2	// Отключение блока
#define ACT_FAULT_CLEAR					3	// Очистка fault
#define ACT_WARNING_CLEAR				4	// Очистка warning

#define ACT_DBG_PULSE_ID_LOW_RANGE		10	// Импульс включения нижнего диапазона измерения Id
#define ACT_DBG_PULSE_SYNC_SCPC			11	// Импульс синхронизации ячеек SCPC
#define ACT_DBG_PULSE_IG				12	// Импульс включения подачи сигнала управления
#define ACT_DBG_PULSE_SYNC_SCOPE		13	// Импульс синхронизации осциллографа
#define ACT_DBG_SET_GATE_VG				14	// Задание напряжения в цепи управления
#define ACT_DBG_SET_GATE_IG				15	// Задание тока в цепи управления

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

#define REG_PC_MAX_CURRENT				31	// Максимальный ток с одной силовой ячейки (в А)
#define REG_PC_START_NID				32	// Стартовое значение NodeID для первой силовой ячейки
#define REG_PC_LONG_TIMEOUT				33	// Длинный таймаут по операциям с ячейками (в мс)

#define REG_CURRENT_OVERSHOOT			40	// Перерегулирование амплитуды тока (в %)
#define REG_ISET_P0						41	// Полином точной корректировки задания прямого тока P0 (в А)
#define REG_ISET_P1						42	// Полином точной корректировки задания прямого тока P1 х1000
#define REG_ISET_P2						43	// Полином точной корректировки задания прямого тока P2 х1е6

// Несохраняемы регистры чтения-записи
#define REG_CURRENT_SETPOINT			140	// Уставка тока (в А)

#define REG_GATE_PULSE_TIME				150	// Длительность импульса управления (в мкс)
#define REG_VG_VALUE					151	// Напряжение цепи управления (в мВ)
#define REG_IG_VALUE					152	// Ток цепи управления (в мА)
#define REG_GATE_PULSE_DELAY			153	// Задержка управляющего импульса (в мкс)

#define REG_DBG							160	// Отладочный регистр

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции

#define REG_DUT_VOLTAGE					198	// Измеренное значение прямого напряжения (в мВ)
#define REG_DUT_CURRENT					206	// Измеренное значение прямого тока (в А)

#define REG_DEV_SUB_STATE				209	// Регистр вспомогательного состояния
#define REG_BHL_ERROR_CODE				210	// Ошибка интерфейса ВУ: код ошибки
#define REG_BHL_DEVICE					211	// Ошибка интерфейса ВУ: NID устройства
#define REG_BHL_FUNCTION				212	// Ошибка интерфейса ВУ: код функции
#define REG_BHL_EXT_DATA				213	// Ошибка интерфейса ВУ: расширенная информация

#define REG_TOTAL_LSLPC					207	// Обнаруженное количество силовых ячеек
#define REG_CURRENT_MAX					225	// Максимальный ток, получаемый с установки (в А)

#define REG_QUADRATIC_CORR				254	// Use quadratic correction for block
// -----------------------------

// Команды LSLPC
#define ACT_LSLPC_ENABLE_POWER			1	// Включение блока
#define ACT_LSLPC_DISABLE_POWER			2	// Выключение блока
#define ACT_LSLPC_FAULT_CLEAR			3	// Очистка fault
#define ACT_LSLPC_WARNING_CLEAR			4	// Очистка warning
//
#define ACT_LSLPC_PULSE_CONFIG			100	// Команда на конфигурацию значения тока
// -----------------------------

// Регистры LSLPC
#define REG_LSLPC_PULSE_VALUE			128	// Значение амплитуды импульса тока (в А)

#define REG_LSLPC_DEV_STATE				192	// Состояние блока
#define REG_LSLPC_FAULT_REASON			193
#define REG_LSLPC_DISABLE_REASON		194
#define REG_LSLPC_WARNING				195
#define REG_LSLPC_PROBLEM				196
// -----------------------------

// Endpoints
#define EP16_DATA_ID					1	// Оцифрованные данные Id
#define EP16_DATA_VD					2	// Оцифрованные данные Vd
#define EP16_DATA_IG					3	// Оцифрованные данные Ig
#define EP16_DATA_VG					4	// Оцифрованные данные Vg

// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

// Fault and disable codes
#define DF_NONE							0
#define DF_INTERFACE					1
#define DF_PC_UNEXPECTED_STATE			2
#define DF_PC_STATE_TIMEOUT				3
#define DF_PC_CURRENT_CONFIG			4

// Warning
#define WARNING_NONE					0

// Problem
#define PROBLEM_NONE					0

// User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	// Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	// Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	// Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	// Неверный ключ безопасности

#endif // __DEV_OBJ_DIC_H
