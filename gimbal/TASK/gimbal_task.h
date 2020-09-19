#ifndef __GIMBAL_TASK_H
#define __GIMBAL_TASK_H

#include "math.h"
#include "stdint.h"

#include "freertos.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "cmsis_os.h"

#define PITCH_INITIAL_VALUE 		166	//3800*0.0439453f 
#define PITCH_UP_LIMIT 					300	//6700*0.0439453f 
#define PITCH_DOWN_LIMIT 				242	//5700*0.0439453f 
#define ENCODER_TO_EULER                0.0439453f                  //编码器查分速度转换至角速度转换系数


#define YAW_REMOTE_STEPPER_ACTIVE				0.05f
#define YAW_MOUSE_STEPPER_ACTIVE        0.1f  
#define YAW_KEY_STEPPER_ACTIVE					180.0f	
#define PITCH_REMOTE_STEPPER_ACTIVE 		0.03f
#define PITCH_MOUSE_STEPPER_ACTIVE      0.04f   

#define servoOpen()			  __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 2000)
#define servoClose()			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 500)


extern portTickType gimbalTime;

uint8_t followMate(void);
void aiDataReset(void);

#endif
