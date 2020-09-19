#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "gpio.h"
#include "tim.h"



void DELAY_init(void);

void DELAY_us(uint16_t us);
void Bsp_tim2Init(void);
void Bsp_tim1Init(void);

#endif
