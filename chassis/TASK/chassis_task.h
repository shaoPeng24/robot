#ifndef __CHASSIS_TASK_H
#define __CHASSIS_TASK_H

#include "bsp_can.h"
#include "bsp_usart.h"

#define CHASSIS_LOKER 2
#define FALLOW 				1
#define CHASSIS_TOP		6
#define CHASSIS_SLOW	111
#define CHASSIS_PROTECT  0


#define WHEEL_SPEED_KP	17.0f
#define WHEEL_SPEED_KI	0.2f
#define WHEEL_SPEED_KD	0.0f
#define WHEEL_SPEED_MAX	10000.0f
#define WHEEL_SPEED_IMAX 5000.0f

#define WZSET_KP	25.0f
#define WZSET_KI	0.00f
#define WZSET_KD	200.0f
#define WZSET_MAX 4500.0f 
#define WZSET_IMAX 0.0f
typedef struct
{
	float vx;
	float vy;
	float wz;
	float vx_set;
	float vy_set;
	float wz_set;
	
	float	angle_set;

	uint16_t mode;
}Chassis_Move_t;

typedef struct
{
    int16_t Target;     //期望值
    int16_t Current;    //当前值
    int16_t Output;     //斜坡输出
    int16_t Dirt;       //方向，作用于斜坡系数
		int16_t Number;
}RampStruct;

void chassis_power_limit(void);

#endif
