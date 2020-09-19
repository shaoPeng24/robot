
#include "send_task.h"

#include "can.h"
#include "bsp_can.h"


uint8_t canTX_gimbal(short pitch,short yaw)
{
	CAN_TxHeaderTypeDef canFrame;
	uint8_t data[8]={0};
	uint32_t temp=0;
	
	canFrame.IDE=CAN_ID_STD;
	canFrame.StdId=0x1ff;
	canFrame.RTR=CAN_RTR_DATA;
	canFrame.DLC=8;
	canFrame.TransmitGlobalTime=DISABLE;
	data[0]=0;
	data[1]=0;
	data[2]=0;
	data[3]=0;
	data[4]=yaw>>8;
	data[5]=yaw&0xff;
	data[6]=pitch>>8;
	data[7]=pitch&0xff;
	HAL_CAN_AddTxMessage(&hcan1, &canFrame, data,&temp);
	
	return temp;
}

uint8_t canTX_shoot(int16_t poker,int16_t shootleft,int16_t shootrigh)
{
	CAN_TxHeaderTypeDef canFrame;
	uint8_t data[8]={0};
	uint32_t temp=0;
	
	canFrame.IDE=CAN_ID_STD;
	canFrame.StdId=0x200;
	canFrame.RTR=CAN_RTR_DATA;
	canFrame.DLC=8;
	canFrame.TransmitGlobalTime=DISABLE;
	data[0]=poker>>8;
	data[1]=poker&0xff;
	data[2]=shootrigh>>8;
	data[3]=shootrigh&0xff;
	data[4]=0;
	data[5]=0;
	data[6]=shootleft>>8;
	data[7]=shootleft&0xff;
	HAL_CAN_AddTxMessage(&hcan2, &canFrame, data,&temp);
	
	return temp;	
}

uint8_t canTX_chassis(int16_t x,int16_t y,int8_t z,int8_t mode,int8_t superstate)
{
	CAN_TxHeaderTypeDef canFrame;
	uint8_t data[8]={0};
	uint32_t temp=0;
	
	canFrame.IDE=CAN_ID_STD;
	canFrame.StdId=0x007;
	canFrame.RTR=CAN_RTR_DATA;
	canFrame.DLC=8;
	canFrame.TransmitGlobalTime=DISABLE;
	data[0]=x>>8;
	data[1]=x&0xff;
	data[2]=y>>8;
	data[3]=y&0xff;
	data[4]=z;
	data[5]=mode;
	data[6]=superstate;
	data[7]=0;
	HAL_CAN_AddTxMessage(&hcan1, &canFrame, data, &temp);
	
	return temp;	
}
