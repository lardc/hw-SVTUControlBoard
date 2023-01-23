#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

// Definitions
//
#define YES						1
#define NO						0

// in uOhm
#define R_SHUNT_MIN				0
#define R_SHUNT_MAX				2000
#define R_SHUNT_DEF				750

// in A
#define LOW_RANGE_I_LIM_MIN		0
#define LOW_RANGE_I_LIM_MAX		6500
#define LOW_RANGE_I_LIM_DEF		1000

//
#define IDL_OFFSET_DEF			65530
#define IDL_K_DEF				349
//
#define ID_OFFSET_DEF			0
#define ID_K_DEF				2236

//
#define VD_OFFSET_DEF			65530
#define VD_K_DEF				1660

//
#define IG_OFFSET_DEF			1
#define IG_K_DEF				349

#define VG_OFFSET_DEF			65446
#define VG_K_DEF				3440

//
#define IG_SET_OFFSET_DEF		64140
#define IG_SET_K_DEF			1800
#define VG_SET_OFFSET_DEF		270
#define VG_SET_K_DEF			473

// in pcs
#define LSLPC_COUNT_MIN			1
#define LSLPC_COUNT_MAX			2
#define LSLPC_COUNT_DEF			2

// in A
#define LSLPC_MAX_I_MIN			100
#define LSLPC_MAX_I_MAX			1650
#define LSLPC_MAX_I_DEF			400

//
#define LSLPC_START_NID_MIN		1
#define LSLPC_START_NID_MAX		250
#define LSLPC_START_NID_DEF		150

// in ms
#define LONG_TIMEOUT_DEF		60000

// in %
#define CURRENT_OVERSHOOT_MIN	0
#define CURRENT_OVERSHOOT_MAX	10
#define CURRENT_OVERSHOOT_DEF	5

//
#define GAIN_COEFFICIENT_MIN	1
#define GAIN_COEFFICIENT_MAX	10000
#define GAIN_COEFFICIENT_DEF	1000

// in us
#define GATE_PULSE_MIN			10
#define GATE_PULSE_MAX			TIME_GATE_PULSE_MAX
#define GATE_PULSE_DEF			TIME_GATE_PULSE_MAX

// in mV
#define GATE_V_MIN				1000
#define GATE_V_MAX				12000
#define GATE_V_DEF				10000

// in mA
#define GATE_I_MIN				100
#define GATE_I_MAX				1000
#define GATE_I_DEF				1000

// in A
#define LSL_CURRENT_MAX			8000

// in uS
#define GATE_PULSE_DELAY_MIN	0
#define GATE_PULSE_DELAY_MAX	1000
#define GATE_PULSE_DELAY_DEF	0


// Types
typedef struct __TableItemConstraint
{
	float Min;
	float Max;
	float Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint Constraint[];

#endif // __CONSTRAINTS_H
