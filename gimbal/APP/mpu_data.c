#include "mpu_data.h"
#include "bsp_tim.h"
#include "bsp_usart.h"
#include "caninfo_task.h"
#include "gimbal_task.h"
#include "vision.h"
#include "led.h"
MPUDataType mpuData ;
//更新mpu数据
uint8_t Gimbal_MpuUpdate(void)
{    
    float gx, gy, gz;
    float ax, ay, az;
    uint8_t result;
    
    result = MPU_getG(&gx, &gy, &gz);
    if(result)return 1;
    result = MPU_getA(&ax, &ay, &az);
    if(result)return 1;
    //中和mpu的个体差异，静止时对 0 校正！-- 2017.04.02
    mpuData.gx = -gx + GIMBAL_GYRO_X_OFFSET;
    mpuData.gy = -gy + GIMBAL_GYRO_Y_OFFSET;
    mpuData.gz = -gz + GIMBAL_GYRO_Z_OFFSET;
	//角速度零值修正，目的是建立死区，mpu波纹浮动，不如卡尔曼滤波好，但这种粗暴滤波效果极佳！-- 2017.04.02
    mpuData.gx = fabs(mpuData.gx)<GIMBAL_GYRO_X_ZERO_CORRECT ? 0.0f : mpuData.gx;
    mpuData.gy = fabs(mpuData.gy)<GIMBAL_GYRO_Y_ZERO_CORRECT ? 0.0f : mpuData.gy;
    mpuData.gz = fabs(mpuData.gz)<GIMBAL_GYRO_Z_ZERO_CORRECT ? 0.0f : mpuData.gz;
    mpuData.ax = ax;
    mpuData.ay = ay; 
    mpuData.az = az;
    return 0;
}

//校准mpu数据静态漂移
uint8_t MPU_offset(float gz,float gy)
{
	static uint8_t step=0;
	static float adjDataGz=0,adjDataGy=0;
	
	adjDataGz+=gz;
	adjDataGy+=gy;
	step++;
	if(step==250)
	{
		mpuData.status=1;//mpu 校准 完毕
		mpuData.gzOffset=adjDataGz/250;
		mpuData.gyOffset=adjDataGy/250;
		
		step=0;
		adjDataGz=0;
		adjDataGy=0;
		return 1;
	}
	return 0;
}
//定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)						
{
	if(htim==&htim2)//2ms
	{
		led_blin(50,&led[1]);
		Gimbal_MpuUpdate();
		mpuData.yaw -= mpuData.gz*0.002f;
		mpuData.status=1;			
	}
	else if(htim==&htim6){}
}
