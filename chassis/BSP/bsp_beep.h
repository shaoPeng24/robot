#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "tim.h"

void buzzer_on(uint16_t psc, uint16_t pwm);
void buzzer_off(void);

#endif
