#ifndef _VISION_H
#define _VISION_H

#include "main.h"

#include "freertos.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "cmsis_os.h"

typedef  struct
{
	float	yawRoot;
	float pitchRoot;
	
	float yawToRoot;				//相对于root的角度
	float pitchToRoot;			//相对于root的角度
	
}RootAngleType;


typedef struct
{
	unsigned char dataReset;
	float yaw[2];         //相对于root的yaw，pitch角度
	float pitch[2];       //0：为初始数据    1：状态更新方程之后的数据
	float yawRate[2];     //yaw，pitch的角速度
	float pitchRate[2];   //0：为初始数据    1：状态更新方程之后的数据
	float yawAcc[2];     //yaw，pitch的加速度
	float pitchAcc[2];   //0：为初始数据    1：状态更新方程之后的数据
	float yawX;            
	float pitchX;           //状态外推数据
	float yawRateX;                            
	float pitchRateX;
	float yawAccX;
	float pitchAccX;
	float yawOut;        //最终预判位置
	float pitchOut;
	/*new*/
	float distanceX;
	float distanceRateX;       
	float distanceAccX;
	float distance[2];
	float distanceRate[2];
	float distanceAcc[2];
	
	float xX;
	float xRateX;
	float xAccX;
	float yX;
	float yRateX;
	float yAccX;
	
	float x[2];
	float y[2];
	float xRate[2];
	float yRate[2];
	float xAcc[2];
	float yAcc[2];
	float xOut;
	float yOut;
	
	float angle;
	float angleOut;
	float carRate;
	float flyTime;
}followStruct;					//自瞄运算数据

typedef struct 
{
  int32_t id;
	float yaw;
	float pitch;
	float distance;
	
}visionStruct;

extern visionStruct visionData;
extern followStruct followData;
extern RootAngleType rootAngle;

extern portTickType visionTime;

void vision_usart_init(void);
void vision_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
void VS_tx_dma_enable(uint8_t *data, uint16_t len);

#endif
