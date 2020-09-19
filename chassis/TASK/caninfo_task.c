#include "caninfo_task.h"

#include "string.h"
#include "bsp_usart.h"
#include "led.h"


Motor_HandleTypeDef chassisMotor[4]={0},yawMotor={0};
Control_HandleTypeDef	chassisCan={0};

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rxFrame;
	uint8_t rxData[8]={0};
	
	if(hcan==&hcan1)
	{
		led_blin(100,&led[0]);
		HAL_CAN_GetRxMessage(&hcan1,  CAN_RX_FIFO0, &rxFrame,  rxData);
		switch (rxFrame.StdId)
		{
			case CHASSIS_LF_MOTOR_CAN_ID:
																	{
																		chassisMotor[0].coder = (int16_t) rxData[0]<<8|rxData[1];												
																		chassisMotor[0].speed = (int16_t) rxData[2]<<8|rxData[3];
																		chassisMotor[0].current = (int16_t) rxData[4]<<8|rxData[5];
																		break;
																	}
																		
			case CHASSIS_LB_MOTOR_CAN_ID:
																	{
																		chassisMotor[1].coder = (int16_t) rxData[0]<<8|rxData[1];												
																		chassisMotor[1].speed = (int16_t) rxData[2]<<8|rxData[3];
																		chassisMotor[1].current = (int16_t) rxData[4]<<8|rxData[5];
																		break;
																	}
																	
			case CHASSIS_RF_MOTOR_CAN_ID:
																	{
																		chassisMotor[2].coder = (int16_t) rxData[0]<<8|rxData[1];												
																		chassisMotor[2].speed = (int16_t) rxData[2]<<8|rxData[3];	
																		chassisMotor[2].current = (int16_t) rxData[4]<<8|rxData[5];																		
																		break;
																	}	
																	
			case CHASSIS_RB_MOTOR_CAN_ID:
																	{
																		chassisMotor[3].coder = (int16_t) rxData[0]<<8|rxData[1];												
																		chassisMotor[3].speed = (int16_t) rxData[2]<<8|rxData[3];	
																		chassisMotor[3].current = (int16_t) rxData[4]<<8|rxData[5];
																		break;
																	}				
		}
	}
	else if(hcan==&hcan2)
	{
		led_blin(100,&led[1]);
		HAL_CAN_GetRxMessage(&hcan2,  CAN_RX_FIFO0, &rxFrame,  rxData);
		switch (rxFrame.StdId)
		{
			case CHASSIS_YAW_MOTOR_CAN_ID:
																	 {
																		
																		 
																		yawMotor.coder = (int16_t) rxData[0]<<8|rxData[1];												
																		yawMotor.speed = (int16_t) rxData[2]<<8|rxData[3];	
																		
																		 
																		
																		break;
																	 }
			case CHASSIS_CONTROL_MAIL_ID:
																	{
																		chassisCan.x	= (int16_t) rxData[0]<<8|rxData[1];
																		chassisCan.y	= (int16_t) rxData[2]<<8|rxData[3];
																		chassisCan.s0 = rxData[4];
																		chassisCan.mode = rxData[5];
																		chassisCan.superstate = rxData[6];
																		break;
																	}
														
		}
	}
}
