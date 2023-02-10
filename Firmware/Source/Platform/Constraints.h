#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

// Definitions
//
#define COEF_P2_MIN					-INT16S_MAX
#define COEF_P2_MAX					INT16S_MAX
#define COEF_P2_DEF					0
//
#define COEF_P1_MIN					-INT16S_MAX
#define COEF_P1_MAX					INT16S_MAX
#define COEF_P1_DEF					1
//
#define COEF_P0_MIN					-INT16S_MAX
#define COEF_P0_MAX					INT16S_MAX
#define COEF_P0_DEF					0
//
#define COEF_K_MIN					-INT16S_MAX
#define COEF_K_MAX					INT16S_MAX
//
#define COEF_B_MIN					-INT16S_MAX
#define COEF_B_MAX					INT16S_MAX
//
#define REGULATOR_QP_MIN			0
#define REGULATOR_QP_MAX			10
#define REGULATOR_QP_DEF			0
//
#define REGULATOR_QI_MIN			0
#define REGULATOR_QI_MAX			10
#define REGULATOR_QI_DEF			0
//
#define REGULATOR_QIMAX_MIN			0
#define REGULATOR_QIMAX_MAX			1000
#define REGULATOR_QIMAX_DEF			1000
//
#define ALLOWED_ERR_MIN				0
#define ALLOWED_ERR_MAX				5
#define ALLOWED_ERR_DEF				0
//
#define FOLLOWING_ERR_CNT_MIN		0
#define FOLLOWING_ERR_CNT_MAX		10
#define FOLLOWING_ERR_CNT_DEF		5
//
#define YES							1
#define NO							0
//
#define LCSU_CURRENT_MIN			1000
#define LCSU_CURRENT_MAX			6500
#define LCSU_CURRENT_DEF			1700
//
#define LCSU_START_NID_MIN			110
#define LCSU_START_NID_MAX			119
#define LCSU_START_NID_DEF			110
//
#define LONG_TIMEOUT_MIN			1000
#define LONG_TIMEOUT_MAX			10000
#define LONG_TIMEOUT_DEF			1000
//
#define VG_EDGE_TIME_MIN			500
#define VG_EDGE_TIME_MAX			2000
#define VG_EDGE_TIME_DEF			1000
//
#define R_SHUNT_MIN					75
#define R_SHUNT_MAX					1000
#define R_SHUNT_DEF					750
//
#define I_THRESHOLD_MIN				200
#define I_THRESHOLD_MAX				12000
#define I_THRESHOLD_DEF				300
//
#define ID_SETPOINT_MIN				50
#define ID_SETPOINT_MAX				1700
#define ID_SETPOINT_DEF				500
//
#define VG_SETPOINT_MIN				10
#define VG_SETPOINT_MAX				20
#define VG_SETPOINT_DEF				15
//
#define PULSE_DURATION_MIN			5
#define PULSE_DURATION_MAX			10
#define PULSE_DURATION_DEF			10
//
#define IG_THRESHOLD_MIN			10
#define IG_THRESHOLD_MAX			100
#define IG_THRESHOLD_DEF			30
//
#define COEF_ID_R0_K_DEF			1
#define COEF_ID_R1_K_DEF			1
#define COEF_VD_K_DEF				1
#define COEF_VG_K_DEF				1
#define COEF_VG_SET_K_DEF			1
#define COEF_IG_K_DEF				1
//
#define COEF_ID_R0_B_DEF			1
#define COEF_ID_R1_B_DEF			1
#define COEF_VD_B_DEF				1
#define COEF_VG_B_DEF				1
#define COEF_VG_SET_B_DEF			1
#define COEF_IG_B_DEF				1


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
