#ifndef __MEM_BUFFERS_H
#define __MEM_BUFFERS_H

#include "stdinc.h"
#include "Global.h"

// Variables
//
extern float MEMBUF_EP_Vd[VALUES_x_SIZE];
extern float MEMBUF_EP_Id[VALUES_x_SIZE];
extern float MEMBUF_EP_Vg[VALUES_x_SIZE];
extern float MEMBUF_EP_VgErr[VALUES_x_SIZE];
extern float MEMBUF_EP_Ig[VALUES_x_SIZE];

extern float MEMBUF_DMA_Vd[VALUES_POWER_DMA_SIZE];
extern float MEMBUF_DMA_Id[VALUES_POWER_DMA_SIZE];

#endif // __MEM_BUFFERS_H
