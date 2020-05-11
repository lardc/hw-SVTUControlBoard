#ifndef __LOGIC_H
#define __LOGIC_H

// Includes
//
#include "stdinc.h"

//Functions
//
void LOGIC_FindCells();
//
void LOGIC_PowerEnablePrepare();
void LOGIC_PowerEnable();
void LOGIC_PowerDisable();
//
bool LOGIC_SetCurrentForCertainBlock(uint16_t Nid, float Current);
bool LOGIC_DistributeCurrent(float Current);

#endif //__LOGIC_H
