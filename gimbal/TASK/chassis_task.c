#include "chassis_task.h"

#include "freertos.h"
#include "task.h"

#include "rc.h"
#include "keymouse.h"
#include "bsp_usart.h"

#include "gimbal_task.h"
#include "send_task.h"



void remote_control_chassis(void)
{
		if(rc_ctrl.rc.s[1] == 1)
		{
			canTX_chassis(rc_ctrl.rc.ch[0],rc_ctrl.rc.ch[1],3,1,0);
		}
		else if(rc_ctrl.rc.s[1] == 3)
		{
			canTX_chassis(rc_ctrl.rc.ch[0],rc_ctrl.rc.ch[1],3,6,0);
		}
		else 
		{
			canTX_chassis(rc_ctrl.rc.ch[0],rc_ctrl.rc.ch[1],3,0,0);
		}
}



void keymouse_control_chassis(void)
{
    GetKeyValue();
		
		if(rc_ctrl.rc.s[1] == 1)
				if(KeyMouseFlag.corgi == 1)
				{												
					canTX_chassis(KeyMouseFlag.movlr,	KeyMouseFlag.movfb,	1,	6,	KeyMouseFlag.superstate);
				}
				else if(KeyMouseFlag.servo == 1)
				{
					canTX_chassis(KeyMouseFlag.movlr,	KeyMouseFlag.movfb, 1, 111,	KeyMouseFlag.superstate);
				}
				else 
				{
					canTX_chassis(KeyMouseFlag.movlr,	KeyMouseFlag.movfb, 1,	1,	KeyMouseFlag.superstate);
				}
		else
					canTX_chassis(KeyMouseFlag.movlr,	KeyMouseFlag.movfb, 1,	2,	KeyMouseFlag.superstate);
		
		
		
		KeyStatusReset();
}

void Chassis_Task(void const * argument)
{
	portTickType currentTime;
	currentTime = xTaskGetTickCount();//当前系统时间
	
	while(1)
	{
		if(rc_ctrl.rc.s[0] == 1)
		{
				keymouse_control_chassis();
		}				
		else if(rc_ctrl.rc.s[0] == 3)
		{
				remote_control_chassis();
		}
		else
		{
				canTX_chassis(rc_ctrl.rc.ch[0],rc_ctrl.rc.ch[1],2,0,0);
		}
		vTaskDelayUntil(&currentTime, 2);
	}
}


