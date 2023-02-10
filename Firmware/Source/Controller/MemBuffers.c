// Header
#include "MemBuffers.h"

// Includes


// Variables
//
float MEMBUF_EP_Vd[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Id[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Vg[VALUES_x_SIZE] = {0};
float MEMBUF_EP_VgErr[VALUES_x_SIZE] = {0};
float MEMBUF_EP_Ig[VALUES_x_SIZE] = {0};

volatile Int16U MEMBUF_DMA_Vd[VALUES_POWER_DMA_SIZE] = {0};
volatile Int16U MEMBUF_DMA_Id[VALUES_POWER_DMA_SIZE] = {0};
