// Header
#include "MemBuffers.h"

// Includes


// Variables
//
Int16U MEMBUF_EP_Vd[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Id[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Vg[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Ig[VALUES_x_SIZE] = {0};

volatile Int16U MEMBUF_DMA_Vd[VALUES_VD_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Id[VALUES_ID_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Vg[VALUES_VG_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Ig[VALUES_IG_DMA_SIZE] = {0};
