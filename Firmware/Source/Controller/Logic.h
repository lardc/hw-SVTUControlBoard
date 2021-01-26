#ifndef __LOGIC_H
#define __LOGIC_H

// Includes
//
#include "stdinc.h"

//Functions
//
bool LOGIC_FindCells();
bool LOGIC_UpdateCellsState();

bool LOGIC_CallCommandForCells(uint16_t Command);
bool LOGIC_PowerEnableCells();
bool LOGIC_WriteCellsConfig();
bool LOGIC_AreCellsInStateX(uint16_t State);
bool LOGIC_IsCellInFaultOrDisabled(uint16_t Fault, uint16_t Disabled);

bool LOGIC_SetCurrentForCertainBlock(uint16_t Nid, float Current);
bool LOGIC_DistributeCurrent(float Current);

float LOGIC_GetCurrentSetpoint();
void LOGIC_SelectCurrentRange(float Current);

void LOGIC_ProcessPulse();

void LOGIC_SaveToEndpoint(volatile Int16U *InputArray, Int16U *OutputArray, uint16_t InputArraySize);
void LOGIC_SaveResults();

#endif //__LOGIC_H
