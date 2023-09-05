#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Переход в состояние ожидания
#define ACT_DISABLE_POWER				2	// Отключение блока
#define ACT_FAULT_CLEAR					3	// Очистка fault
#define ACT_WARNING_CLEAR				4	// Очистка warning
//
#define ACT_DBG_PULSE_V_GATE			10	// Импульс напряжения управления IGBT
#define ACT_DBG_PULSE_SYNC_OSC			11	// Импульс синхронизации для осциллографа
#define ACT_DBG_VIN_ST					12	// Переключение канала напряжения в режим самотестирования
#define ACT_DBG_PULSE_SYNC_LCSU			13	// Импульс синхронизации LCSU
#define ACT_DBG_EXT_INDICATION			14	// Управление внешней индикацией
//
#define ACT_START_TEST					100	// Запуск процесса измерения
#define ACT_STOP_TEST					101	// Остановка процесса измерения
//
#define ACT_START_SELF_TEST				105	// Запуск самотестирования
//
#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию
//
#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_ID_R0_P2					0	// Диапазон 0 - Коэффициент точной корректировки Id P2
#define REG_ID_R0_P1					1	// Диапазон 0 - Коэффициент точной корректировки Id P1
#define REG_ID_R0_P0					2	// Диапазон 0 - Коэффициент точной корректировки Id P0
#define REG_ID_R0_K						3	// Диапазон 0 - Коэффициент пересчёта напряжения АЦП в Id
#define REG_ID_R0_B						4	// Диапазон 0 - Смещение оцифрованного Id
//
#define REG_ID_R1_P2					5	// Диапазон 1 - Коэффициент точной корректировки Id P2
#define REG_ID_R1_P1					6	// Диапазон 1 - Коэффициент точной корректировки Id P1
#define REG_ID_R1_P0					7	// Диапазон 1 - Коэффициент точной корректировки Id P0
#define REG_ID_R1_K						8	// Диапазон 1 - Коэффициент пересчёта напряжения АЦП в Id
#define REG_ID_R1_B						9	// Диапазон 1 - Смещение оцифрованного Id
//
#define REG_VD_P2						10	// Коэффициент точной корректировки Vd P2
#define REG_VD_P1						11	// Коэффициент точной корректировки Vd P1
#define REG_VD_P0						12	// Коэффициент точной корректировки Vd P0
#define REG_VD_K						13	// Коэффициент пересчёта напряжения АЦП в Vd
#define REG_VD_B						14	// Смещение оцифрованного Vd
//
#define REG_VG_P2						15	// Коэффициент точной корректировки Vg P2
#define REG_VG_P1						16	// Коэффициент точной корректировки Vg P1
#define REG_VG_P0						17	// Коэффициент точной корректировки Vg P0
#define REG_VG_K						18	// Коэффициент пересчёта напряжения АЦП в Vg
#define REG_VG_B						19	// Смещение оцифрованного Vg
//
#define REG_VG_SET_P2					20	// Коэффициент точной корректировки Vg P2
#define REG_VG_SET_P1					21	// Коэффициент точной корректировки Vg P1
#define REG_VG_SET_P0					22	// Коэффициент точной корректировки Vg P0
#define REG_VG_SET_K					23	// Коэффициент преобразованиия Vg в ЦАП
#define REG_VG_SET_B					24	// Смещение при преобразовании Vg в ЦАП
//
#define REG_IG_P2						25	// Коэффициент точной корректировки Ig P2
#define REG_IG_P1						26	// Коэффициент точной корректировки Ig P1
#define REG_IG_P0						27	// Коэффициент точной корректировки Ig P0
#define REG_IG_K						28	// Коэффициент пересчёта напряжения АЦП в Ig
#define REG_IG_B						29	// Смещение оцифрованного Ig
//
#define REG_ISET_P2						30	// Коэффициент точной корректировки задания прямого тока P2
#define REG_ISET_P1						31	// Коэффициент точной корректировки задания прямого тока P1
#define REG_ISET_P0						32	// Коэффициент точной корректировки задания прямого тока P0
//
#define REG_REGULATOR_QP				33	// Пропорциональный коэффициент регулятора
#define REG_REGULATOR_QI				34	// Интегральный коэффициент регулятора
#define REG_REGULATOR_QI_MAX			35	// Максимальное значение интегрального коэффициента регулятора
#define REG_REGULATOR_ALLOWED_ERR		36	// Максимально допустимая ошибка регулятора
#define REG_FOLLOWING_ERR_CNT			37	// Максимальное значение счетчика FollowingError
#define REG_FOLLOWING_ERR_MUTE			38	// Выключение слежения за FollowingError
//
#define REG_LCSU_MAX_CURRENT			39	// Максимальный ток с одного LCSU (в А)
#define REG_LCSU_START_NID				40	// Стартовое значение NodeID для первого LCSU
#define REG_LCSU_LONG_TIMEOUT			41	// Длинный таймаут по операциям с LCSU (в мс)
#define REG_VG_EDGE_TIME				42	// Длительность фронта импульса управления (мкс)
#define REG_LAMP_CTRL					43	// Запрет управление блоком внешнего индикатора
#define REG_R_SHUNT						44	// Сопротивление шунта (в мкОм)
#define REG_I_R0_THRESHOLD				45	// Граница нижнего диапазона тока (в А)
#define REG_IG_THRESHOLD				46	// Значчение тока Ig при выставлении PROBLEM_GATE_SHORT
#define REG_OSC_SYNC_TIME				47	// Длительность импульса синхронизации для осциллографа (в мкс)
#define REG_OSC_SYNC_DELAY				48	// Задержка формирования импульса синхронизации для осциллографа (мкс)
//

// Несохраняемы регистры чтения-записи
#define REG_ID_SETPOINT					128	// Уставка силового тока (в А)
#define REG_VG_SETPOINT					129	// Уставка напряжения управления (в В)
//
#define REG_DBG							150	// Отладочный регистр

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_SELF_TEST_OP_RESULT			198	// Регистр результата самотестирования
#define REG_SUB_STATE					199	// Регистр вспомогательного состояния
//
#define REG_RESULT_VD					200	// Измеренное значение прямого напряжения (в мВ)
#define REG_RESULT_ID					201	// Измеренное значение прямого тока (в А)
#define REG_RESULT_VG					202	// Измеренное значение напряжения цепи управления (в В)
//
#define REG_LCSU_DETECTED				205	// Обнаруженное количество силовых ячеек
#define REG_ID_MAX						206	// Максимальный ток, получаемый с установки (в А)
//
#define REG_BHL_ERROR_CODE				210	// Ошибка интерфейса ВУ: код ошибки
#define REG_BHL_DEVICE					211	// Ошибка интерфейса ВУ: NID устройства
#define REG_BHL_FUNCTION				212	// Ошибка интерфейса ВУ: код функции
#define REG_BHL_EXT_DATA				213	// Ошибка интерфейса ВУ: расширенная информация
//
#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record
// -----------------------------

// Команды LCSU
#define ACT_LCSU_ENABLE_POWER			1	// Включение блока
#define ACT_LCSU_DISABLE_POWER			2	// Выключение блока
#define ACT_LCSU_FAULT_CLEAR			3	// Очистка fault
#define ACT_LCSU_WARNING_CLEAR			4	// Очистка warning
//
#define ACT_LCSU_PULSE_CONFIG			100	// Команда на конфигурацию значения тока
#define ACT_LCSU_STOP_PROCESS			102	// Команда на на остановку процесса
// -----------------------------

// Регистры LCSU
#define REG_LCSU_PULSE_VALUE			128	// Значение амплитуды импульса тока (в А)

#define REG_LCSU_DEV_STATE				192	// Состояние блока
#define REG_LCSU_FAULT_REASON			193
#define REG_LCSU_DISABLE_REASON			194
#define REG_LCSU_WARNING				195
#define REG_LCSU_PROBLEM				196
// -----------------------------

// Endpoints
#define EP_ID							1	// Оцифрованные данные Id
#define EP_VD							2	// Оцифрованные данные Vd
#define EP_VG							3	// Оцифрованные данные Vg
#define EP_VG_ERR						4	// Ошибка регулятора Vg
#define EP_IG							5	// Оцифрованные данные Ig

// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

// Fault and disable codes
#define DF_NONE							0	// Ошибок нет
#define DF_INTERFACE					1	// Нет ответа от LCSU
#define DF_LCSU_UNEXPECTED_STATE		2	// Не корректное состояние LCSU
#define DF_LCSU_STATE_TIMEOUT			3	// Таймаут смены состояния LCSU
#define DF_LCSU_CURRENT_CONFIG			4	// Ошибка конфигурации LCSU
#define DF_SELFTEST_VD					5	// Ошибка измерения напряжения в режиме самотестирования
#define DF_SELFTEST_ID					6	// Ошибка измерения тока в режиме самотестирования
#define DF_GATE_VOLTAGE					7	// Проблема с формирователем напряжения управления

// Warning
#define WARNING_NONE					0	// Предупреждений нет
#define WARNING_VOLTAGE_OUT_OF_RANGE	1	// Измеренное напряжение вне рабочего диапазона
#define WARNING_CURRENT_OUT_OF_RANGE	2	// Измеренный ток вне рабочего диапазона

// Problem
#define PROBLEM_NONE					0
#define PROBLEM_FORCED_STOP				1	// Принудительная остановка процесса
#define PROBLEM_SAFETY					2	// Сработала система безопасности
#define PROBLEM_GATE_SHORT				3	// КЗ в цепи управления

// User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	// Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	// Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	// Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	// Неверный ключ безопасности

#endif // __DEV_OBJ_DIC_H
