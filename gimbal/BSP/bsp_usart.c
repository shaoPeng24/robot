#include "bsp_usart.h"

#include "freertos.h"
#include "cmsis_os.h"
#include "led.h"
#include "caninfo_task.h"
#include "rc.h"
#include "keymouse.h"
#include "shoot_task.h"
#include "gimbal_task.h"

#include "mpu_data.h"
#include "vision.h"

#define PRINT UART4

float FloatWave[4];
/*print重定向*/
int fputc(int ch, FILE *f) 
{ 
	while((PRINT->SR&0X40)==0);
	PRINT->DR=(uint8_t)ch;
	return ch;
} 
//只被DATA_show函数调用
void UART_SendData(char *Data, uint8_t Size)
{
	uint8_t inc=0;
	for(inc=0;inc<Size;inc++)
	{
		while((PRINT->SR&0X40)==0);
		PRINT->DR=(uint8_t)*(Data+inc);
	}
}
void DATA_show(float *FloatData, uint8_t Length)//山外曲线显示数据 float类型
{
	uint8_t i=0;
	
		while((PRINT->SR&0X40)==0);
		PRINT->DR=(uint8_t)0x03;
		while((PRINT->SR&0X40)==0);
		PRINT->DR=(uint8_t)0xFC;
	
	for(i=0;i<Length;i++)
	{
		UART_SendData((char *)(FloatData+i), sizeof(*(FloatData+i)));
	}

		while((PRINT->SR&0X40)==0);
		PRINT->DR=(uint8_t)0xFC;
		while((PRINT->SR&0X40)==0);
		PRINT->DR=(uint8_t)0x03;
}





void Printf_Task(void const * argument)
{
	while(1)
	{

		FloatWave[0]=-shootMotor[0].speed;
		FloatWave[1]=shootMotor[1].speed;
		FloatWave[2]=visionData.id;
		FloatWave[3]=followData.yawOut;
		DATA_show(FloatWave,2);

        osDelay(4);
	}
}

