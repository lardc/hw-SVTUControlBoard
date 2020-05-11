// Header
//
#include "PowerCell.h"

// Includes
//
#include "Controller.h"
#include "DeviceProfile.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "BCCIMaster.h"


// Variables
//
PCError PC_Error = {ERR_NO_ERROR, 0, 0, 0};


// Forward functions
//
void PC_LoadError(Int16U Code, Int16U NodeID, Int16U Func, Int16U ExtData);


// Functions
//
bool PC_Call(uint16_t Nid, uint16_t Command)
{
	Int16U err;
	err = BCCIM_Call(&MASTER_DEVICE_CAN_Interface, Nid, Command);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		PC_LoadError(err, Node, FUNCTION_CALL, Command);
		return FALSE;
	}
}
// ----------------------------------------

bool PC_ReadRegister(uint16_t Nid, uint16_t Address, uint16_t* Data)
{
	Int16U err;
	err = BCCIM_Read16(&MASTER_DEVICE_CAN_Interface, Nid, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		PC_LoadError(err, Node, FUNCTION_READ, Address);
		return FALSE;
	}
}
// ----------------------------------------

bool PC_WriteRegister(uint16_t Nid, uint16_t Address, uint16_t Data)
{
	Int16U err;
	err = BCCIM_Write16(&DEVICE_CAN_Master_Interface, Nid, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		HLI_LoadError(err, Node, FUNCTION_WRITE, Address);
		return FALSE;
	}
}
// ----------------------------------------

PCError PC_GetError()
{
	return PC_Error;
}
// ----------------------------------------

void PC_LoadError(Int16U Code, Int16U NodeID, Int16U Func, Int16U ExtData)
{
	PC_Error.ErrorCode = Code;
	PC_Error.Device = NodeID;
	PC_Error.Func = Func;
	PC_Error.ExtData = ExtData;
}
// ----------------------------------------

void PC_ResetError()
{
	PC_Error.ErrorCode = ERR_NO_ERROR;
	PC_Error.Device = 0;
	PC_Error.Func = 0;
	PC_Error.ExtData = 0;
}
// ----------------------------------------
