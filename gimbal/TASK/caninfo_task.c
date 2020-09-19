#include "caninfo_task.h"

#include "string.h"
#include "math.h"
#include "led.h"
#include "vision.h"


Motor_HandleTypeDef	yawMotor={0},pitchMotor={0},pokerMotor={0},shootMotor[2]={0};
Refeere_HandleTypeDef		Refeere={0};

int16_t euler_angle[3];

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rxFrame;
	uint8_t rxData[8]={0};

	
	if(hcan==&hcan1)
	{
		led_blin(50,&led[2]);
		HAL_CAN_GetRxMessage(&hcan1,  CAN_RX_FIFO0, &rxFrame,  rxData);
		switch (rxFrame.StdId)
		{
			
			case YAW_MOTOR_ID:
												{
													yawMotor.lastCoder = yawMotor.coder;
													
													yawMotor.coder = (int16_t) rxData[0]<<8|rxData[1];
													yawMotor.speed = (int16_t) rxData[2]<<8|rxData[3];
													
													yawMotor.turns=(yawMotor.coder-yawMotor.lastCoder)<-4096?yawMotor.turns+1:yawMotor.turns;
													yawMotor.turns=(yawMotor.coder-yawMotor.lastCoder)>4096? yawMotor.turns-1:yawMotor.turns;	
													break;
												}
			case PITCH_MOTOR_ID:
												{
													pitchMotor.coder = (int16_t) rxData[0]<<8|rxData[1];
													pitchMotor.speed = (int16_t) rxData[2]<<8|rxData[3];
													
													break;
												}
			case	REFEERE_DATA_ID	:		
												{
													Refeere.robot_id		=	(int8_t)	rxData[0]; 
													Refeere.heat0 			=	(int16_t) (rxData[1]<<8|rxData[2]);
													Refeere.heat0_limit = (int16_t)	(rxData[3]<<8|rxData[4]);
													Refeere.bullet_speed = (float)	(rxData[5]<<8|rxData[6]);
													Refeere.bullet_speedlimit = (int8_t)	rxData[7]; 
												}				
		}
	}
	else if(hcan==&hcan2)
	{
		led_blin(50,&led[3]);
		HAL_CAN_GetRxMessage(&hcan2,  CAN_RX_FIFO0, &rxFrame,  rxData);
		switch (rxFrame.StdId)
		{
			
			case POKER_MOTOR_ID:
												{
													pokerMotor.lastCoder = pokerMotor.coder;
													
													pokerMotor.coder = (int16_t) rxData[0]<<8|rxData[1];
													pokerMotor.speed = (int16_t) rxData[2]<<8|rxData[3];
													
													pokerMotor.turns=(pokerMotor.coder-pokerMotor.lastCoder)<-4096?pokerMotor.turns+1:pokerMotor.turns;
													pokerMotor.turns=(pokerMotor.coder-pokerMotor.lastCoder)>4096? pokerMotor.turns-1:pokerMotor.turns;	
													break;
												}
			case SHOOT_LEFT_MOTOR_ID:
												{
													shootMotor[0].coder = (int16_t) rxData[0]<<8|rxData[1];	
													shootMotor[0].speed = (int16_t) rxData[2]<<8|rxData[3];
													break;
												}	
			case SHOOT_RIGH_MOTOR_ID:
												{
													shootMotor[1].coder = (int16_t) rxData[0]<<8|rxData[1];	
													shootMotor[1].speed = (int16_t) rxData[2]<<8|rxData[3];
													break;
												}																		
		}		
	}
}




