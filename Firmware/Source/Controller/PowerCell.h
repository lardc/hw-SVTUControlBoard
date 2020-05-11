#ifndef __POWERCELL_H
#define __POWERCELL_H

// Includes
//
#include "stdinc.h"

// Types
//
typedef struct __PCError
{
	Int16U ErrorCode;
	Int16U Device;
	Int16U Func;
	Int16U ExtData;
} PCError, *pPCError;

// Functions
//
uint16_t PC_Find();
bool PC_Call(uint16_t Nid, uint16_t Command);
bool PC_ReadRegister(uint16_t Nid, uint16_t Address, uint16_t* Data);
bool PC_WriteRegister(uint16_t Nid, uint16_t Address, uint16_t Data);
//
PCError PC_GetError();
void PC_ResetError();

#endif // __POWERCELL_H
