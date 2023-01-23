#ifndef __LOGIC_H
#define __LOGIC_H

// Includes
//
#include "stdinc.h"

//Functions
//
bool LOGIC_FindLCSU();
bool LOGIC_UpdateLCSUState();

bool LOGIC_CallCommandForLCSU(uint16_t Command);
bool LOGIC_PowerEnableLCSU();
bool LOGIC_WriteLCSUConfig();
bool LOGIC_AreLCSUInStateX(uint16_t State);
bool LOGIC_IsLCSUInFaultOrDisabled(uint16_t Fault, uint16_t Disabled);

bool LOGIC_SetCurrentForCertainLCSU(uint16_t Nid, float Current);
bool LOGIC_DistributeCurrent(float Current);

float LOGIC_GetCurrentSetpoint();
void LOGIC_SelectCurrentRange(float Current);

void LOGIC_ProcessPulse();

void LOGIC_SaveToEndpoint(volatile pFloat32 InputArray, pFloat32 OutputArray, uint16_t InputArraySize);
void LOGIC_SaveResults();

#endif //__LOGIC_H
