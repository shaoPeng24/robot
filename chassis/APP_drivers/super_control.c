#include "super_control.h"

#include "math.h"

#include "INA230.h"
#include "bsp_adc.h"
#include "chassis_task.h"
#include "referee.h"
#include "caninfo_task.h"

/*
IN1			PA2				电池输出
IN2     PA1				超电输出
Diode   PE5				电压高输出
*/


void chassis_out(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);					//IN1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);				//IN2
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);					//Diode
}


void super_out(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);				//IN1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);					//IN2
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);					//Diode
}

void choose_out(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);				//IN1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);				//IN2
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);				//Diode
}


/*获取超电电压*/
extern ADC_HandleTypeDef hadc1;

void super_voltage(void)
{
	inaData.vol = adcx_get_chx_value(&hadc1,ADC_CHANNEL_0)/123.7f;
}



/*控制充电电流*/
static _CurrentPIDStruct Current_CirClePid = CURRENT_CIRCLE;

uint8_t super_flag=0;

void get_motor_power(float *motorPower , float *inaPower)
{

	static float powermaybe ,powermaybelast , inapower , inapowerlast ;
	float powerm[4];
	for(int m=0; m < 4; m++)
	{
			powerm[m] = chassisMotor[m].current*chassisMotor[m].speed;
			if(powerm[m]<0) powerm[m] = 0;
			else;
	}
	powermaybe = (powerm[0]+powerm[1]+powerm[2]+powerm[3])*0.000003f;
	inapower   = inaData.pow/40.0f;
	
	*motorPower = 0.95f*powermaybelast + 0.05f*powermaybe;
	*inaPower = 0.95f*inapowerlast + 0.05f*inapower;
	
	powermaybelast = powermaybe;
	inapowerlast = inapower;
}	

void super_current_control(void)
{
	static uint16_t  powermax;
	static float power , powerbuff;
	static float motorpower , inapower;
	get_chassis_power_and_buffer(&power, &powerbuff, &powermax);
	get_motor_power(&motorpower , &inapower);
	

	
	Current_CirClePid.lastErr = Current_CirClePid.currentErr;            //保存历史误差
	
	Current_CirClePid.componentKp  = Current_CirClePid.kp *  Current_CirClePid.currentErr;
	Current_CirClePid.componentKi += Current_CirClePid.ki *  Current_CirClePid.currentErr;
	Current_CirClePid.componentKd  = Current_CirClePid.kd * (Current_CirClePid.currentErr - Current_CirClePid.lastErr);
	
	if(Current_CirClePid.componentKi>powermax)Current_CirClePid.componentKi=powermax;

	Current_CirClePid.outPut = Current_CirClePid.componentKp + Current_CirClePid.componentKi+Current_CirClePid.componentKd ;

		if(inaData.vol >= 12)			
	{
		Current_CirClePid.currentErr = powermax - power - 10; 
	}
	else 
	{
		if(fabs(chassisMotor[0].speed)+fabs(chassisMotor[1].speed)+fabs(chassisMotor[2].speed)+fabs(chassisMotor[3].speed) >= 100)
		{
			Current_CirClePid.currentErr = 0;
			Current_CirClePid.outPut = 0;
		}
		else
		{
			Current_CirClePid.currentErr = powermax - power - 10;
		}
	}
	
	if(powermax-power<10 || powerbuff < 30)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
		DAC_set(0);
	}
	else
	{
		if(Current_CirClePid.outPut <= 0)
			Current_CirClePid.outPut = 0;
		else if(Current_CirClePid.outPut >= 150)
			Current_CirClePid.outPut = 150;
		else 
			Current_CirClePid.outPut = Current_CirClePid.outPut;
		
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);
		DAC_set(Current_CirClePid.outPut);
	}
}


/*超电工作状态控制*/
void super_control(void)
{
	static int8_t surper_state;
	
	if(inaData.vol <= 12)  						super_flag=0;
	else if(inaData.vol >= 19)				super_flag=1;
		
	super_current_control();
			
	if(chassisCan.superstate == 1)
	{
			if(inaData.vol >12 && super_flag == 1)
			{
				surper_state = 1;
			}
			else
			{
				surper_state = 0;
			}
	}
	else
	{
			surper_state =0;
	}
	
	if(surper_state == 1)
	{
		
			super_out();
	}
	else
	{
		
			chassis_out();
			chassis_power_limit();
		
	}

		
}
