#ifndef __GATE_DRIVER_H
#define __GATE_DRIVER_H

#include "stdinc.h"

// Functions
//
void GATE_PulseOutput(bool State);
void GATE_SetVg(uint16_t Value);
void GATE_SetIg(uint16_t Value);

#endif // __GATE_DRIVER_H
