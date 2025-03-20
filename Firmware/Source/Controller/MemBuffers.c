// Header
#include "MemBuffers.h"

// Includes


// Variables
//
float MEMBUF_EP_Ut[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Ut_Ch2[VALUES_x_SIZE] = {0};
float MEMBUF_EP_It[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Ug[VALUES_x_SIZE] = {0};
float MEMBUF_EP_UgErr[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Ig[VALUES_x_SIZE] = {0};

volatile float CONTROL_ExtInfoData[VALUES_EXT_INFO_SIZE] = {0};

float MEMBUF_DMA_Ut[VALUES_POWER_DMA_SIZE] = {0};
float MEMBUF_DMA_It[VALUES_POWER_DMA_SIZE] = {0};
float MEMBUF_DMA_Ut2_UgIg[VALUES_POWER_DMA_SIZE] = {0};
float MEMBUF_DMA_IGBT_UgIg[2] = {0};
