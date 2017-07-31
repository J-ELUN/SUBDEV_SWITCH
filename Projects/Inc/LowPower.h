#ifndef __LOWPOWER_H__
#define __LOWPOWER_H__
#include "stm32l0xx_hal.h"

void LowPower_Init(void);
void Enter_LowPower_Mode(void);
void SystemClockConfig_STOP(void);

#endif
