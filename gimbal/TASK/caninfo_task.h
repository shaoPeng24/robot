#ifndef __CANINFO_TASK_H
#define __CANINFO_TASK_H

#include "bsp_can.h"
#define YAW_MOTOR_ID			0x207
#define PITCH_MOTOR_ID		0x208
#define POKER_MOTOR_ID		0x201
#define SHOOT_LEFT_MOTOR_ID   0x204
#define SHOOT_RIGH_MOTOR_ID   0x202
#define REFEERE_DATA_ID		0x006

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
}Motor_HandleTypeDef;		

typedef struct
{
	int8_t robot_id;
	int16_t	heat0;
	int16_t	heat0_limit;	
	float   bullet_speed;
	uint8_t bullet_speedlimit;
	
}Refeere_HandleTypeDef;

extern Motor_HandleTypeDef yawMotor,pitchMotor,pokerMotor,shootMotor[2];

extern Refeere_HandleTypeDef		Refeere;
#endif
