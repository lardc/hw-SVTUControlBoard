#ifndef __INITCONFIG_H
#define __INITCONFIG_H

#include "stdinc.h"

// Functions
//
void INITCFG_ConfigSystemClock();
void INITCFG_ConfigGPIO();
void INITCFG_ConfigCAN();
void INITCFG_ConfigUART();
void INITCFG_ConfigSPI();
void INITCFG_ConfigADC();
void INITCFG_ConfigSlaveADCChannel(ADC_TypeDef* ADCx, Int16U Channel);
void INITCFG_ConfigMasterADCChannel(ADC_TypeDef* ADCx, Int16U Channel, Int32U TrigSource);
void INITCFG_ConfigTimer3();
void INITCFG_ConfigTimer1();
void INITCFG_ConfigDMA();
void INITCFG_ConfigWatchDog();
void INITCFG_ConfigTimer2();
void INITCFG_ConfigTimer6();

#endif // __INITCONFIG_H
