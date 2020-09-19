#ifndef __CANINFO_TASK_H
#define __CANINFO_TASK_H

#include "can.h"

#define CHASSIS_LF_MOTOR_CAN_ID         0x201
#define CHASSIS_LB_MOTOR_CAN_ID         0x202
#define CHASSIS_RF_MOTOR_CAN_ID         0x203
#define CHASSIS_RB_MOTOR_CAN_ID         0x204
#define CHASSIS_YAW_MOTOR_CAN_ID        0x207
#define CHASSIS_CONTROL_MAIL_ID         0x007
typedef struct
{
	uint16_t    lastCoder; /*上次电机转子编码器值*/
	
	float 			sumCoder;/*电机转子编码器值累计和*/
	
	uint16_t 		coder;/*电机实时转子编码器值*/
	
	float				coderTarget;/*位置环的期望*/
	
	int16_t		  speed;/*电机实时速度*/
	
	float       speedTarget;/*速度环的期望*/
	
	uint8_t     temp; /*电机温度*/
	
	int32_t 		turns;/*电机从上电开始转过的圈数*/
	
	int16_t       current;/*电机电流*/
}Motor_HandleTypeDef;	

typedef struct 
{
	int16_t x;
	int16_t y;
	int8_t  s0;
	int8_t  mode;
	int8_t  superstate;
}Control_HandleTypeDef;

extern Motor_HandleTypeDef chassisMotor[4],yawMotor;
extern Control_HandleTypeDef	chassisCan;
#endif
