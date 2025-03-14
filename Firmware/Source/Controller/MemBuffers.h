#ifndef __MEM_BUFFERS_H
#define __MEM_BUFFERS_H

#include "stdinc.h"
#include "Global.h"

// Variables
//
extern float MEMBUF_EP_Ut[VALUES_x_SIZE];
extern float MEMBUF_EP_Ut_Ch2[VALUES_x_SIZE];
extern float MEMBUF_EP_It[VALUES_x_SIZE];
extern float MEMBUF_EP_Vg[VALUES_x_SIZE];
extern float MEMBUF_EP_VgErr[VALUES_x_SIZE];
extern float MEMBUF_EP_Ig[VALUES_x_SIZE];

extern volatile float CONTROL_ExtInfoData[VALUES_EXT_INFO_SIZE];

extern float MEMBUF_DMA_Ut[VALUES_POWER_DMA_SIZE];
extern float MEMBUF_DMA_It[VALUES_POWER_DMA_SIZE];
extern float MEMBUF_DMA_Ut2[VALUES_POWER_DMA_SIZE];

#endif // __MEM_BUFFERS_H
