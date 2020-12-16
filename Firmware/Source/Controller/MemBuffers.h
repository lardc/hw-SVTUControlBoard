#ifndef __MEM_BUFFERS_H
#define __MEM_BUFFERS_H

#include "stdinc.h"

// Variables
//
extern Int16U MEMBUF_EP_Vd[];
extern Int16U MEMBUF_EP_Id[];
extern Int16U MEMBUF_EP_Vg[];
extern Int16U MEMBUF_EP_Ig[];

extern volatile Int16U MEMBUF_DMA_Vd[];
extern volatile Int16U MEMBUF_DMA_Id[];
extern volatile Int16U MEMBUF_DMA_Vg[];
extern volatile Int16U MEMBUF_DMA_Ig[];

#endif // __MEM_BUFFERS_H
