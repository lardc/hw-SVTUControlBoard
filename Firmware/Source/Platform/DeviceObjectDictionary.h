#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

//Команды
//
#define ACT_ENABLE_POWER                        1                               //Переход в состояние ожидания
#define ACT_DISABLE_POWER                       2                               //Команда блокам LSLPC на заряд батареи конденсаторов
#define ACT_FAULT_CLEAR                         3                               //Очистка fault
#define ACT_WARNING_CLEAR                       4                               //Очистка warning
#define ACT_RESET_DEVICE                        5                               //Перезапуск процессора
//-----------


#define ACT_ENABLE_POWER						1		// Включить питание блока
#define ACT_DISABLE_POWER						2		// Выключить питание блока
#define ACT_CLR_FAULT							3		// Очистка ошибки
#define ACT_CLR_WARNING							4		// Очистка предупреждения#define ACT_START_PULSE							100		// Запуск процесса измерения


#define ACT_DBG_PULSE_IG_FEEDBACK				10		// Запуск размагничивающего тока на 1сек.
#define ACT_DBG_PULSE_ID_LOW_RANGE				11		// Синхросигнал на 1сек.
#define ACT_DBG_PULSE_SYNC_SCPC					12		// Включение реле разряда на 1сек.
#define ACT_DBG_PULSE_IG						13		// Подача питания на зарядные устройства на 1сек.
#define ACT_DBG_PULSE_SYNC_SCOPE				14		// Включение остановки зарядных устройств на 1сек.


#define ACT_START_PULSE							100		// Запуск процесса измерения
#define ACT_STOP_TEST							101		// Останов процесса измерения



#define ACT_LSLPC_GET_INFO                      102                             //Получить информацию о конкретном блоке LSLPC
#define ACT_LSLPC_FIND                          103                             //Команда на поиск блоков LSLPC
//
#define ACT_LSLPC_REG_WRITE                105                             //Команда для записи в регистр блока LSLPC
#define ACT_LSLPC_CALL                          106                             //Команда для вызова определенной команды в блоке LSLPC
#define ACT_LSLPC_REG_READ                      107                             //Команда для чтения регистра LSLPC
//
#define ACT_LSLPC_SYNC_TEST                     110
#define ACT_OSC_SYNC_TEST                       111
#define ACT_DUT_TEST                            112


#define ACT_SAVE_TO_ROM							200		// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM					201		// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT					202		// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST					320		// Перезапуск процессора с целью перепрограммирования

#define ACT_RESET_FOR_PROGRAMMING               320                             //Перезапуск процессора с целью перепрограммирования


//Регистры

#define REG_V_BAT1_OFFSET						2		// Смещение оцифрованного напряжения батареи 1 (в тиках)
#define REG_V_BAT1_K							3		// Коэффициент пересчёта напряжения АЦП (в мВ) в напряжение батареи 1 (в В) х1000
#define REG_V_BAT2_OFFSET						4		// Смещение оцифрованного напряжения батареи 2 (в тиках)
#define REG_V_BAT2_K							5		// Коэффициент пересчёта напряжения АЦП (в мВ) в напряжение батареи 1 (в В) х1000
#define REG_VBAT_THRESHOLD						6		// Порог заряда конденсаторной батареи (в В)
//
#define REG_V_DUT_OFFSET						7		// Смещение оцифрованного напряжения DUT (в тиках)
#define REG_V_DUT_K								8		// Коэффициент пересчёта напряжения АЦП (в мВ) в напряжение DUT (в В) х1000
#define REG_V_DUT_P0							9		// Полином точной корректировки напряжения DUT P0 (в вольтах)
#define REG_V_DUT_P1							10		// Полином точной корректировки напряжения DUT P1 x1000
#define REG_V_DUT_P2							11		// Полином точной корректировки напряжения DUT P2 x1e6

//
#define REG_VOLT_GAIN                             0                               //Пропорциональный коэффициент по напряжению
#define REG_CURR_GAIN								1
// 2 - 3
#define REG_R_SHUNT                             4                              //Сопротивление шунта в мкОм
#define REG_CURR_GAIN_R0                        5                              //Коэффициент усиления ShuntAmplifier, диапазон 0
#define REG_CURR_GAIN_R1                        6                              //Коэффициент усиления ShuntAmplifier, диапазон 1
#define REG_CURR_GAIN_R2                        7                              //Коэффициент усиления ShuntAmplifier, диапазон 2
#define REG_CURRENT_I_LIM_R0					8
#define REG_CURRENT_I_LIM_R1					9
// 10
#define REG_P2_ISET                      		11                               //Множитель 2 калибровочного уравнения значения задания тока
#define REG_P1_ISET                       		12                               //Множитель калибровочного уравнения значения задания тока
#define REG_P0_ISET                       		13                               //Смещение калибровочного уравнения значения задания тока
//
#define REG_P2_UCAL                            14                              //Множитель 2 калибровочного уравнения измерения напряжения
#define REG_P1_UCAL                             15                              //Множитель калибровочного уравнения измерения напряжения
#define REG_P0_UCAL                             16                               //Смещение калибровочного уравнения измерения напряжения
//
#define REG_P2_ICAL_R0                           17                               //Множитель 2 калибровочного уравнения измерения тока, диапазон 1
#define REG_P1_ICAL_R0                            18                               //Множитель 2 калибровочного уравнения измерения тока, диапазон 1
#define REG_P0_ICAL_R0                            19                              //Смещение калибровочного уравнения измерения тока, диапазон 1
//
#define REG_P2_ICAL_R1                           20                               //Множитель 2 калибровочного уравнения измерения тока, диапазон 1
#define REG_P1_ICAL_R1                            21                              //Множитель калибровочного уравнения измерения тока, диапазон 2
#define REG_P0_ICAL_R1                            22                              //Смещение калибровочного уравнения измерения тока, диапазон 2
//
#define REG_P2_ICAL_R2                           23                               //Множитель 2 калибровочного уравнения измерения тока, диапазон 1
#define REG_P1_ICAL_R2                            24                              //Множитель калибровочного уравнения измерения тока, диапазон 3
#define REG_P0_ICAL_R2                            25                              //Смещение калибровочного уравнения измерения тока, диапазон 3


#define REG_CURRENT_VALUE                       140                             //Значение тока, А
//
#define REG_LSLPC_DATA_REG                      171                             //Регистр данных, которые надо записать в регистр определенного блока LSLPC
#define REG_LSLPC_ADDR_REG                      172                             //Регистр адрерас регистра блока LSLPC, в который надо записать данные
#define REG_LSLPC_CALL                          173                             //Регистр команды, которую надо вызвать в блоке LSLPC
#define REG_LSLPC_NID                           174                             //Регистр NID блока LSLPC
#define REG_NID_LSLPC_CONFIG                    175                             //Nid блока, который нужно сконфигурировать отдельно от остальных
#define REG_NID_LSLPC_INFO                      176                             //Nid блока SCPC, информацию о котором необходимо получить
//
#define REG_K_SHUNT_AMP                         177                             //Задаваемое значение усиления SCShuntAmplifier


#define REG_DEV_STATE                           192                             //Регистр состояния установки SCTU
#define REG_FAULT_REASON                        193                             //Регистр Fault
#define REG_FAULT_DESABLE                       194                             //Регистр Desable
#define REG_WARNING                             195                             //Регистр Warning
#define REG_PROBLEM                             196                             //Регистр Problem
#define REG_TEST_FINISHED						197
//
#define REG_DUT_U                               198                              //Измеренное значение прямого напряжения, мВ
// 199 - 205
#define REG_DUT_I                               206                              //Измеренное значение ударного тока, А (младшая часть)
#define REG_TOTAL_LSLPC                         207                             //Количество блоков SCPC в установке
// 208 - 222
#define REG_FAILED_LSLPC						223								// Номер проблемной ячейки
#define REG_FAILED_OPERATION					224								// Номер проблемной операции
#define REG_CURRENT_MAX                         225                             //Максимальное возможное значение ударного тока при текущей конфигурации (младшая часть)
// 226
#define LSLPC_REG_PULSE_VALUE              227                             //Значение ударного тока, интересующего блока
#define REG_INFO_LSLPC_BAT_VOLTAGE              228                             //Напряжение батареи, интересующего блока
#define LSLPC_REG_DEV_STATE                229                             //Состояние, интересующего блока
#define REG_INFO_LSLPC_DEV_FAULT                230                             //Значение Fault, интересующего блока
#define REG_INFO_LSLPC_DEV_DISABLE              231                             //Значение Disable, интересующего блока
#define REG_INFO_LSLPC_DEV_WARNING              232                             //Значение Warning, интересующего блока
#define REG_INFO_LSLPC_DEV_PROBLEM              233                             //Значение Problem, интересующего блока



#define LSLPC_ACT_POWER_ENABLE					1
#define LSLPC_ACT_POWER_DISABLE					2

#define LSLPC_ACT_PULSE_CONFIG					100
//
//
#define REG_QUADRATIC_CORR			254	// Use quadratic correction for block

// ENDPOINTS
//
#define EP16_Data_I                		1	                        // Оцифрованные данные ударного тока
#define EP16_Data_U                		2	                        // Оцифрованные данные прямого напряжения
#define EP16_Dummy3                		3
#define EP16_Dummy4                		4
#define EP16_Dummy5                		5
#define EP16_Sorted_I					6
#define EP16_Sorted_U					7


//Fault
//
#define FAULT_NONE									0
#define FAULT_PROTOCOL								0
#define FAULT_PC_UNEXPECTED_STATE					0
#define FAULT_PC_CHARGE_TIMEOUT						0

#define DF_LSLPC_OPERATION_FAILED                  1                               //Блок SCPC не отвечает
#define DF_SECURITY                            2                               //Открыта шторка безопасности
#define DF_DUT_NOT_OPENED                     3                               //Прибор не открылся
#define DF_LSLPC_NO_PULSE                         4                               //Один из блоков не сформировал импульс ударного тока
//

// Cell fault
#define CF_NONE									0
#define CF_CELL_CALL								1
#define CF_CELL_READ_REGS								2
#define CF_CELL_WRITE_REGS								3
#define CF_CELL_CONFIG								4
#define CF_CELL_POST_PULSE								5


//Warning
//
// WARNING CODES
//
#define WARNING_NONE							0								// No warning
#define WARNING_I_OUT_OF_RANGE                  1                             	//Измеренное значение тока вне диапазона
#define WARNING_U_OUT_OF_RANGE                  2                             	//Измеренное значение напряжения вне диапазона
#define WARNING_LSLPC_NO_CELLS                  3                             	//Один или несколько блоков не определились при запуске
#define WARNING_NID_NOT_FOUND                   4                             	//Заданный Nid не найден в списке
#define WARNING_CURRENT_TOO_HIGH                  5                             	//Заданный Nid не найден в списке
//

//User Errors
//
#define ERR_NONE                                0 				// Ошибок нет
#define ERR_CONFIGURATION_LOCKED                1 				// Устройство защищено от записи
#define ERR_OPERATION_BLOCKED                   2 				// Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY                    3 				// Устройство не готово для смены состояния
#define ERR_WRONG_PWD                           4 				// Неправильный ключ
//

#endif // __DEV_OBJ_DIC_H
