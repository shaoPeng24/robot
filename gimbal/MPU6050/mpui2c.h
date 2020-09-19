/**
  ******************************************************************************
  * 
  * Mpu6050 i2c驱动，本部分适用于stm32f4xx系列单片机
  * 如需修改，请修改mpui2c.h下标记修改部分
  * 
  * 
  *	创建时间：2016年08月27日
  * 最后更新：2016年10月05日
  * 创 建 人：梁宗南(LJI)
  * E_Mail	：liangzongnan0214@163.com
  * QQ		：494089986
  * 
  ******************************************************************************
***/



#ifndef __MPUI2C_H__
#define __MPUI2C_H__



/*Mpu_i2c Needed Library Define*/
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "delay.h"
   	   		   


/********************************* i2c 供用户更改部分 **********************************/

#define		MPU_I2C_PORT			GPIOB
#define		MPU_I2C_PIN_SCL			GPIO_Pin_9
#define		MPU_I2C_PIN_SDA			GPIO_Pin_8
#define		MPU_I2C_CLOCK_CMD		RCC_AHB1Periph_GPIOB

/**************************************************************************************/

#define		MPU_I2C_SCL_0()			GPIO_ResetBits(MPU_I2C_PORT, MPU_I2C_PIN_SCL)
#define		MPU_I2C_SCL_1()			GPIO_SetBits(MPU_I2C_PORT, MPU_I2C_PIN_SCL)
#define		MPU_I2C_SDA_0()			GPIO_ResetBits(MPU_I2C_PORT, MPU_I2C_PIN_SDA)
#define		MPU_I2C_SDA_1()			GPIO_SetBits(MPU_I2C_PORT, MPU_I2C_PIN_SDA)

#define		MPU_SDA_READ()			GPIO_ReadInputDataBit(MPU_I2C_PORT, MPU_I2C_PIN_SDA)

#define		MPU_SDA_IN()			MPU_I2C_SDA_InMode()
#define		MPU_SDA_OUT()			MPU_I2C_SDA_OutMode()



/*Mpu_i2c Function Define*/
void MPU_I2C_Init(void);	   //初始化i2c
void MPU_I2C_SDA_InMode(void); //初始化i2c_sda为输入
void MPU_I2C_SDA_OutMode(void);//初始化i2c_sda为输出

void MPU_I2C_SDA_InMode(void);
void MPU_I2C_SDA_OutMode(void);

void MPU_I2C_Start(void);//产生i2c起始信号
void MPU_I2C_Stop(void); //产生i2c停止信号

u8 MPU_I2C_Wait_Ack(void);//等待应答

void MPU_I2C_Ack(void); //产生ack 应答
void MPU_I2C_NAck(void);//产生nack应答

void MPU_I2C_Send_Byte(u8 txd);//i2c发送一个字节
u8 MPU_I2C_Read_Byte(u8 ack);  //i2c接收一个字节



#endif


