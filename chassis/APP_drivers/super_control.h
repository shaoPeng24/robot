#ifndef __SUPER_GPIO_H
#define __SUPER_GPIO_H

#include "gpio.h"
#include "adc.h"

typedef struct
{
	float kp; 
	float ki;	
	float kd;
    
	float componentKp;
	float componentKi;
	float integral;
	float componentKd;
	
	float currentErr;
	float lastErr;
	
	float ref;
	float fdb;
	float outPut;
}_CurrentPIDStruct;

#define CURRENT_CIRCLE     {                           \
                                    0.4f, 0.06f, 0.15f,      \
                                    0.0f, 0.0f, 0.0f, 0.0f, \
                                    0.0f, 0.0f,             \
                                    0.0f, 0.0f, 0.0f,       \
                                }

void super_out(void);
void chassis_out(void);
void choose_out(void);
void super_voltage(void);
void super_control(void);
#endif
