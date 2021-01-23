#ifndef __MEM_BUFFERS_H
#define __MEM_BUFFERS_H

#include "stdinc.h"
#include "Global.h"

// Variables
//
extern Int16U MEMBUF_EP_Vd[VALUES_x_SIZE];
extern Int16U MEMBUF_EP_Id[VALUES_x_SIZE];
extern Int16U MEMBUF_EP_Vg[VALUES_x_SIZE];
extern Int16U MEMBUF_EP_Ig[VALUES_x_SIZE];

extern volatile Int16U MEMBUF_DMA_Vd[VALUES_VD_DMA_SIZE];
extern volatile Int16U MEMBUF_DMA_Id[VALUES_ID_DMA_SIZE];
extern volatile Int16U MEMBUF_DMA_Vg[VALUES_VG_DMA_SIZE];
extern volatile Int16U MEMBUF_DMA_Ig[VALUES_IG_DMA_SIZE];

#endif // __MEM_BUFFERS_H
