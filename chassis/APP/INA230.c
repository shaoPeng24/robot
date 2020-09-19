#include "INA230.h"

IIC_HandleTypeDef inaIic; 
IIC_HandleTypeDef dacIic; 

INADataType inaData;

void INA_init(void)  //ina230 和 DAC5571
{
	
	__HAL_RCC_GPIOB_CLK_ENABLE();

	inaIic.SCL_GPIO=GPIOB;
	inaIic.SCL_PIN=GPIO_PIN_10;
	inaIic.SDA_GPIO=GPIOB;
	inaIic.SDA_PIN=GPIO_PIN_11;
	IIC_Init(&inaIic);
	
	dacIic.SCL_GPIO=GPIOB;
	dacIic.SCL_PIN=GPIO_PIN_6;
	dacIic.SDA_GPIO=GPIOB;
	dacIic.SDA_PIN=GPIO_PIN_7;
	IIC_Init(&dacIic);
	
	INA_writeData(INA226_ADDR1,0x00,0x42ef);
	INA_writeData(INA226_ADDR1,0x05,1707);
}


uint8_t DAC_set(uint8_t data)
{
  IIC_Start(&dacIic);
	
	IIC_writeByte(&dacIic,0x98);
	if(IIC_waitAck(&dacIic)==1){return 1;}
	
	IIC_writeByte(&dacIic,data>>4);
	if(IIC_waitAck(&dacIic)==1){return 2;}
	
	IIC_writeByte(&dacIic,data<<4);
	if(IIC_waitAck(&dacIic)==1){return 3;}
	
	IIC_Stop(&dacIic);
	return 0;
}
void INA_getCur(void)
{
	uint8_t buff[2]={0};
	INA_readLen(INA226_ADDR1,0x04,2,buff);
	inaData.cur=(short)(buff[0]<<8)|buff[1]; 
//	if(inaData.cur&0x8000)	inaData.cur = ~(inaData.cur - 1);
}
void INA_getPow(void)
{
	uint8_t buff[2]={0};
	INA_readLen(INA226_ADDR1,0x03,2,buff);
	inaData.pow=(short)(buff[0]<<8)|buff[1]; 
}
void INA_getVol(void)
{
	uint8_t buff[2]={0};
	INA_readLen(INA226_ADDR1,0x01,2,buff);
	inaData.vol=(short)(buff[0]<<8)|buff[1]; 
}



uint8_t INA_writeData(uint8_t dev, uint8_t reg,uint16_t data)
{
	
    IIC_Start(&inaIic);
	
		IIC_writeByte(&inaIic,(dev<<1)|0);
		if(IIC_waitAck(&inaIic)==1){return 1;}
		
		IIC_writeByte(&inaIic,reg);
		if(IIC_waitAck(&inaIic)==1){return 2;}
		

		IIC_writeByte(&inaIic,data>>8);
		if(IIC_waitAck(&inaIic)==1){return 3;} 
		
		IIC_writeByte(&inaIic,data&0xff);
		if(IIC_waitAck(&inaIic)==1){return 4;} 
			
		IIC_Stop(&inaIic);
		return 0;

}
uint8_t INA_readLen(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *buf)
{


	IIC_Start(&inaIic);

	IIC_writeByte(&inaIic,dev<<1);
	if(IIC_waitAck(&inaIic)==1){return 1;}

	IIC_writeByte(&inaIic,reg);
	if(IIC_waitAck(&inaIic)==1){return 2;}

	IIC_Start(&inaIic);
	IIC_writeByte(&inaIic,(dev<<1)+1);
	if(IIC_waitAck(&inaIic)==1){return 3;}

	while(len)
	{
		if(len==1)
		{
			*buf = IIC_readByte(&inaIic);//读数据,发送nACK 
			IIC_ack(&inaIic,1);
		}
		else 
		{
			*buf = IIC_readByte(&inaIic);//读数据,发送ACK  
			IIC_ack(&inaIic,0);
		}
		
		len--;
		buf++; 
	}    
	
	IIC_Stop(&inaIic);
return 0;

}
