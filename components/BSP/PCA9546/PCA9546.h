#ifndef __PCA9546_H__
#define __PCA9546_H__

#include "I2C.h"

#define PCA9546_I2C_ADDR 0x70

void PCA9546_Init();
void PCA9546_SelectChannel(uint8_t channel);
void PCA9546_DeselectChannel(uint8_t channel);
void PCA9546_EnableAllChannels();

#endif