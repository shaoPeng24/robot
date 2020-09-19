#include "mpu6050.h"
IIC_HandleTypeDef mpuIic;

//mpu 初始化
uint8_t MPU_init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	mpuIic.SCL_GPIO=GPIOB;
	mpuIic.SCL_PIN=GPIO_PIN_8;
	mpuIic.SDA_GPIO=GPIOB;
	mpuIic.SDA_PIN=GPIO_PIN_9;
	IIC_Init(&mpuIic);
	
	while(MPU_readData(0x68,0x75)!=104)return 1;
	
	MPU_writeData(0X68,0X6B,0X80);
	HAL_Delay(100);
	MPU_writeData(0X68,0X6B,1);
	MPU_writeData(0x68,0X1B,0X18);//+-2000°/s
	MPU_writeData(0x68,0X1C,0x01);//+-2 g
	MPU_writeData(0x68,0X1A,0X01);//  带宽21
	HAL_Delay(10);
	return 0;
	
}
//获得mpu的角速度
uint8_t MPU_getG(float *gx, float *gy, float *gz)
{
    uint8_t buf[6],res;  
	
	res = MPU_readLen(0x68, MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx = (float)((short)(buf[0]<<8)|buf[1])/16.3840f;  
		*gy = (float)((short)(buf[2]<<8)|buf[3])/16.3840f;  
		*gz = (float)((short)(buf[4]<<8)|buf[5])/16.3840f;
	} 	
	
    return res;
}
//获得mpu的加速度
uint8_t MPU_getA(float *ax, float *ay, float *az)
{
    uint8_t buf[6],res;  
	
	res = MPU_readLen(0x68, 0x3b,6,buf);
	if(res==0)
	{
		*ax = (float)((short)(buf[0]<<8)|buf[1])/16384.0f;  
		*ay = (float)((short)(buf[2]<<8)|buf[3])/16384.0f;  
		*az = (float)((short)(buf[4]<<8)|buf[5])/16384.0f;
	} 	
	
    return res;
}

//内部驱动
uint8_t MPU_writeData(uint8_t dev, uint8_t reg, uint8_t data)
{
    IIC_Start(&mpuIic);
	
	IIC_writeByte(&mpuIic,(dev<<1)|0);
	if(IIC_waitAck(&mpuIic)==1){return 1;}
	
	IIC_writeByte(&mpuIic,reg);
	if(IIC_waitAck(&mpuIic)==1){return 2;}
	

	IIC_writeByte(&mpuIic,data);
	if(IIC_waitAck(&mpuIic)==1){return 3;} 
		
	IIC_Stop(&mpuIic);
	return 0;

}
uint8_t MPU_readData(uint8_t dev, uint8_t reg)
{
	uint8_t data=0;
	IIC_Start(&mpuIic);

	IIC_writeByte(&mpuIic,dev<<1);
	if(IIC_waitAck(&mpuIic)==1){return 1;}

	IIC_writeByte(&mpuIic,reg);
	if(IIC_waitAck(&mpuIic)==1){return 2;}

	IIC_Start(&mpuIic);
	IIC_writeByte(&mpuIic,(dev<<1)+1);
	if(IIC_waitAck(&mpuIic)==1){return 3;}


	data= IIC_readByte(&mpuIic);//读数据
	IIC_ack(&mpuIic,1);

	
	IIC_Stop(&mpuIic);
	return data;
}
uint8_t MPU_writeLen(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i; 
    IIC_Start(&mpuIic);
	
	IIC_writeByte(&mpuIic,(dev<<1)|0);
	if(IIC_waitAck(&mpuIic)==1){return 1;}
	
	IIC_writeByte(&mpuIic,reg);
	if(IIC_waitAck(&mpuIic)==1){return 2;}
		
	for(i=0;i<len;i++)
	{
		IIC_writeByte(&mpuIic,buf[i]);
		if(IIC_waitAck(&mpuIic)==1){return 2+i;} 			
	}  
	IIC_Stop(&mpuIic);
	return 0;	
} 

uint8_t MPU_readLen(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start(&mpuIic);

	IIC_writeByte(&mpuIic,dev<<1);
	if(IIC_waitAck(&mpuIic)==1){return 1;}

	IIC_writeByte(&mpuIic,reg);
	if(IIC_waitAck(&mpuIic)==1){return 2;}

	IIC_Start(&mpuIic);
	IIC_writeByte(&mpuIic,(dev<<1)+1);
	if(IIC_waitAck(&mpuIic)==1){return 3;}

	while(len)
	{
		if(len==1)
		{
			*buf = IIC_readByte(&mpuIic);//读数据,发送nACK 
			IIC_ack(&mpuIic,1);
		}
		else 
		{
			*buf = IIC_readByte(&mpuIic);//读数据,发送ACK  
			IIC_ack(&mpuIic,0);
		}
		len--;
		buf++; 
	}    
	IIC_Stop(&mpuIic);
	return 0;
}
