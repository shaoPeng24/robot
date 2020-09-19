#include "bsp_can.h"
/*
can通信滤波器初始化
*/

uint8_t bsp_canInit(void)    
{
	uint8_t status=0;
	CAN_FilterTypeDef canFilter;
	
	/*can1初始化*/
	MX_CAN1_Init();             								//MX生成的代码
	
	canFilter.FilterBank=1;    																//筛选器组1
	canFilter.FilterIdHigh=0;
	canFilter.FilterIdLow=0;
	canFilter.FilterMaskIdHigh=0;
	canFilter.FilterMaskIdLow=0;
	canFilter.FilterMode=CAN_FILTERMODE_IDMASK;  							//掩码模式
	canFilter.FilterActivation=CAN_FILTER_ENABLE;							//开启
	canFilter.FilterScale=CAN_FILTERSCALE_32BIT; 							//32位模式
	canFilter.FilterFIFOAssignment=CAN_FILTER_FIFO0; 					//链接到fifo0
	canFilter.SlaveStartFilterBank=14;												//can2筛选组起始编号
	
	status|=HAL_CAN_ConfigFilter(&hcan1,&canFilter);					//配置过滤器
	
	/*can2初始化*/
	MX_CAN2_Init();             								//MX生成的代码
	canFilter.FilterBank=15;    															//筛选器组15
	status|=HAL_CAN_ConfigFilter(&hcan2,&canFilter);					//配置过滤器
	
	/*离开初始模式*/
	HAL_CAN_Start(&hcan1);				
	HAL_CAN_Start(&hcan2);
	
	
	/*开中断*/
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);       //can1 接收fifo 0不为空中断
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);       //can2 接收fifo 0不为空中断
	return status;

}



