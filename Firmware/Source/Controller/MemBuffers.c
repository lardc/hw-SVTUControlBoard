// Header
#include "MemBuffers.h"

// Includes
#include "Global.h"

// Variables
//
Int16U MEMBUF_EP_Vd[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Id[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Vg[VALUES_x_SIZE] = {0};
Int16U MEMBUF_EP_Ig[VALUES_x_SIZE] = {0};

volatile Int16U MEMBUF_DMA_Vd[VALUES_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Id[VALUES_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Vg[VALUES_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Ig[VALUES_DMA_SIZE] = {0};
