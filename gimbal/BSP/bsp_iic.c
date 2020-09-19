#include "bsp_iic.h"
/*
* @author  路少鹏
* @data    2020.1.13
* @brief   兼容hal库的软件IIC 需要提供DELAY_us()函数
					 实测396kHZ,修复上次通信bug
*/
/*IIC驱动代码，无需更改*/
void IIC_Init(IIC_HandleTypeDef* iic)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = iic->SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(iic->SCL_GPIO, &GPIO_InitStruct);
}
void SDA_Out(IIC_HandleTypeDef* iic)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = iic->SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD ;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(iic->SDA_GPIO, &GPIO_InitStruct);
}
void SDA_In(IIC_HandleTypeDef* iic)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = iic->SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(iic->SDA_GPIO, &GPIO_InitStruct);
}
void IIC_Start(IIC_HandleTypeDef* iic)
{
	SDA_Out(iic);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_SET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_RESET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
}
void IIC_Stop(IIC_HandleTypeDef* iic)
{
	SDA_Out(iic);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
	DELAY_us(1);
		HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_RESET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_SET);
	DELAY_us(1);
}
void IIC_writeByte(IIC_HandleTypeDef* iic,uint8_t data)
{
	uint8_t i;
	SDA_Out(iic);
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
			(data&0x80)?HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_RESET);
			data <<= 1; 
		DELAY_us(1);
		HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
			DELAY_us(1);


	}
HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
}
uint8_t IIC_readByte(IIC_HandleTypeDef* iic)
{
	uint8_t i=0,data=0;
	SDA_In(iic);
	data=0;
	for(i=0;i<8;i++)
	{	
		
		HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
		DELAY_us(1);
		HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
		data<<=1;
		data|=HAL_GPIO_ReadPin(iic->SDA_GPIO,iic->SDA_PIN);
		DELAY_us(1);
	}
	return data;
}
uint8_t IIC_waitAck(IIC_HandleTypeDef* iic)	    //0有应答  
{
	uint8_t ack=1;
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
	SDA_In(iic);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
	DELAY_us(1);
	ack=HAL_GPIO_ReadPin(iic->SDA_GPIO,iic->SDA_PIN);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
	return ack;
}
void IIC_ack(IIC_HandleTypeDef* iic,uint8_t ack)//0应答
{
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
	
	SDA_Out(iic);
	if(ack==0)HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(iic->SDA_GPIO,iic->SDA_PIN,GPIO_PIN_SET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_SET);
	DELAY_us(1);
	HAL_GPIO_WritePin(iic->SCL_GPIO,iic->SCL_PIN,GPIO_PIN_RESET);
}

