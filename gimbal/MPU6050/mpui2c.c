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
  * MyBlog  ：liangzongnan.com
  * QQ		：494089986
  * 
  ******************************************************************************
***/



#include "mpui2c.h"
//#include "Bsp_Usart1.h"//测试文件时使用，正常使用时可删掉，否则可能会报错！



#define		MPU_I2C_Delay(x)	delay_us(x)	//软件模拟i2c的时钟延时
#define		MPU_ERR_TIME_MAX	200			//最大等待从机应答时间

/**
  * @brief	:初始化i2c
		     初始化i2c_sda为输入
			 初始化i2c_sda为输出
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_Init(void)
{					     
	GPIO_InitTypeDef  gpio;

	RCC_AHB1PeriphClockCmd(MPU_I2C_CLOCK_CMD, ENABLE);//使能GPIOB时钟

	gpio.GPIO_Pin   = MPU_I2C_PIN_SCL
					| MPU_I2C_PIN_SDA;
	gpio.GPIO_Mode  = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(MPU_I2C_PORT, &gpio);
	
	MPU_I2C_SCL_1();
	MPU_I2C_SDA_1();
}

void MPU_I2C_SDA_InMode(void)
{
	GPIO_InitTypeDef	gpio;
		
	gpio.GPIO_Pin  = MPU_I2C_PIN_SDA;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	 
	GPIO_Init(MPU_I2C_PORT, &gpio);
}

void MPU_I2C_SDA_OutMode(void)
{
	GPIO_InitTypeDef	gpio;
		
	gpio.GPIO_Pin   = MPU_I2C_PIN_SDA;
	gpio.GPIO_Mode  = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(MPU_I2C_PORT, &gpio);
	
	MPU_I2C_SDA_1();
}



/**
  * @brief	:产生i2c起始信号
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_Start(void)
{
	MPU_SDA_OUT();	//sda设置为输出	
	MPU_I2C_SDA_1();	  	  
	MPU_I2C_SCL_1();//scl为高电平时
	MPU_I2C_Delay(4);
 	MPU_I2C_SDA_0();//sda下降沿为起始信号
	MPU_I2C_Delay(4);
	MPU_I2C_SCL_0();//准备发送或接收数据 
}	  



/**
  * @brief	:产生i2c停止信号
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_Stop(void)
{
	MPU_SDA_OUT();	//sda设置为输出	
	MPU_I2C_SCL_0();//scl低电平时允许sda改变
	MPU_I2C_SDA_0();
 	MPU_I2C_Delay(4);
	MPU_I2C_SCL_1();//scl为高电平时 
	MPU_I2C_SDA_1();//sda上升沿为停止信号
	MPU_I2C_Delay(4);							   	
}



/**
  * @brief	:等待i2c应答信号
  * @note   :--
  * @param	:void
  * @return	:1, 接收应答失败
			 0, 接收应答成功
  *
  * @date   :2016/08/27
  * @design :
  **/
u8 MPU_I2C_Wait_Ack(void)
{
	u8 Err_Time=0;
	
	MPU_SDA_OUT();	//sda设置为输出 	
	MPU_I2C_SDA_1();
	MPU_I2C_Delay(1);	   
	MPU_I2C_SCL_1();
	MPU_I2C_Delay(1);	
	
	MPU_SDA_IN();	//sda设置为输入 		
	while(MPU_SDA_READ())//等待应答信号到来
	{
		Err_Time++;
		if(Err_Time>MPU_ERR_TIME_MAX)
		{
			MPU_I2C_Stop();
			return 1;
		}
	}
	
	MPU_I2C_SCL_0(); 	   
	return 0;  
} 



/**
  * @brief	:产生ack应答
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_Ack(void)
{
	MPU_I2C_SCL_0();
	
	MPU_SDA_OUT();	//sda设置为输出
	MPU_I2C_SDA_0();//sda低电平
	MPU_I2C_Delay(2);
	MPU_I2C_SCL_1();//scl下降沿为ack信号
	MPU_I2C_Delay(2);
	MPU_I2C_SCL_0();
}



/**
  * @brief	:产生nack应答
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_NAck(void)
{
	MPU_I2C_SCL_0();
	
	MPU_SDA_OUT();	//sda设置为输出
	MPU_I2C_SDA_1();//sda高电平
	MPU_I2C_Delay(2);
	MPU_I2C_SCL_1();//scl下降沿为nack信号
	MPU_I2C_Delay(2);
	MPU_I2C_SCL_0();
}	



/**
  * @brief	:i2c发送一个字节
  * @note   :--
  * @param	:txd, 待发送的数据字节
  * @return	:void
  *
  * @date   :2016/08/27
  * @design :
  **/
void MPU_I2C_Send_Byte(u8 txd)
{                        
    u8 i;   
	
	MPU_SDA_OUT();	//sda设置为输出 	    
    MPU_I2C_SCL_0();
	
    for(i=0;i<8;i++)//高位优先发送
    {              
		if(txd&0x80)
		{
			MPU_I2C_SDA_1();
		}
		else
		{
			MPU_I2C_SDA_0();
		} 		
        txd <<= 1; 
		
		MPU_I2C_Delay(2);//以下三个延时必须！ 
		MPU_I2C_SCL_1(); //scl下降沿发送数据
		MPU_I2C_Delay(2); 
		MPU_I2C_SCL_0();	
		MPU_I2C_Delay(2);
    }	 
} 	    



/**
  * @brief	:i2c读取一个字节
  * @note   :--
  * @param	:ack, (1,ack 0,nack)
  * @return	:u8,接收的数据
  *
  *
  * @date   :2016/08/27
  * @design :
  **/
u8 MPU_I2C_Read_Byte(u8 ack)
{
	u8 i,rec_dat=0;
	
	MPU_SDA_IN();//sda设置为输入
    for(i=0;i<8;i++)
	{
        MPU_I2C_SCL_0(); 
        MPU_I2C_Delay(2);
		MPU_I2C_SCL_1();
		
        rec_dat <<= 1;	
        if(MPU_SDA_READ())
			rec_dat++;   
		
		MPU_I2C_Delay(1); 
    }	
	
    if (!ack)
        MPU_I2C_NAck();//发送nack
    else
        MPU_I2C_Ack(); //发送ack  
	
    return rec_dat;
}




