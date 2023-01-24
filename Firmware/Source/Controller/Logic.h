#ifndef __LOGIC_H
#define __LOGIC_H

// Includes
//
#include "stdinc.h"

//Functions
//
bool LOGIC_FindLCSU();
bool LOGIC_UpdateLCSUState();

bool LOGIC_CallCommandForLCSU(Int16U Command);
bool LOGIC_PowerEnableLCSU();
bool LOGIC_WriteLCSUConfig();
bool LOGIC_AreLCSUInStateX(Int16U State);
bool LOGIC_IsLCSUInFaultOrDisabled(Int16U Fault, Int16U Disabled);

bool LOGIC_SetCurrentForCertainLCSU(Int16U Nid, float Current);
bool LOGIC_DistributeCurrent(float Current);

float LOGIC_GetCurrentSetpoint();
void LOGIC_SelectCurrentRange(float Current);

void LOGIC_ProcessPulse();

void LOGIC_SaveToEndpoint(volatile pFloat32 InputArray, pFloat32 OutputArray, Int16U InputArraySize);
void LOGIC_SaveResults();

#endif //__LOGIC_H
