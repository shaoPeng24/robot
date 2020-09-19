#ifndef __MPU_DATA_H
#define __MPU_DATA_H

#include "main.h"
#include "mpu6050.h"
#include "usart.h"
#include <math.h>

#define GIMBAL_GYRO_X_OFFSET            -1.8561f//-3.2927f                      //俯仰陀螺仪x轴方向偏移量(云台)
#define GIMBAL_GYRO_Y_OFFSET	    	      1.660460f//0.7927f                     //俯仰陀螺仪y轴方向偏移量(云台)
#define GIMBAL_GYRO_Z_OFFSET            -1.3146f//-3.3902f                      //俯仰陀螺仪z轴方向偏移量(云台)

#define GIMBAL_ANGLE_PITCH_INIT         -24.20f                      //俯仰陀螺仪pitch方向角度初始值(云台)
#define GIMBAL_ANGLE_ROLL_INIT           0.0f                        //俯仰陀螺仪roll方向角度初始值(云台)
#define GIMBAL_ANGLE_YAW_INIT            0.0f                        //俯仰陀螺仪yaw方向角度初始值(云台)

#define GIMBAL_GYRO_X_ZERO_CORRECT       0.5f                        //俯仰陀螺仪x轴方向死区修正(云台)
#define GIMBAL_GYRO_Y_ZERO_CORRECT       0.3f                        //俯仰陀螺仪y轴方向死区修正(云台)
#define GIMBAL_GYRO_Z_ZERO_CORRECT       0.4f                        //俯仰陀螺仪z轴方向死区修正(云台)

#define GIMBAL_ANGLE_PITCH_ZERO_CORRECT       0.8f                        //俯仰陀螺仪PITCH轴方向死区修正(云台)
#define GIMBAL_ANGLE_YAW_ZERO_CORRECT         1.5f                        //俯仰陀螺仪YAW轴方向死区修正(云台)
#define GIMBAL_ANGLE_ROLL_ZERO_CORRECT        1.5f                        //俯仰陀螺仪ROLL方向死区修正(云台)

typedef struct
{
	float gx; 
	float gy;
	float gz;
	
	unsigned char status;
	float gzOffset;
	float gyOffset;
	
	float ax; 
	float ay;
	float az;
	
	float yaw;


}MPUDataType;

extern MPUDataType mpuData;
uint8_t Gimbal_MpuUpdate(void);
float Updata_MPU6050_AngleContinuation(float MpuDataGimbalAngle);


#endif
