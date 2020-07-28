#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

// Definitions
#define GAIN_COEFFICIENT_MIN	1
#define GAIN_COEFFICIENT_MAX	10000
#define GAIN_COEFFICIENT_DEF	1000

#define GATE_PULSE_MIN			10
#define GATE_PULSE_MAX			1000
#define GATE_PULSE_DEF			100

#define GATE_V_MIN				2000
#define GATE_V_MAX				12000
#define GATE_V_DEF				5000

#define GATE_I_MIN				100
#define GATE_I_MAX				1000
#define GATE_I_DEF				500

// Types
typedef struct __TableItemConstraint
{
	Int16U Min;
	Int16U Max;
	Int16U Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE];
extern const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START];

#endif // __CONSTRAINTS_H
