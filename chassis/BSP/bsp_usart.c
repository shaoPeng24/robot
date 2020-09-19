#include "bsp_usart.h"

#include "freertos.h"
#include "cmsis_os.h"
#include "stdio.h"
#include <string.h>
#include "caninfo_task.h"
#include "INA230.h"
#include "referee.h"
#include "string.h"
#include "math.h"
#include "can.h"
#define PRINT USART3


float showData[4];
int fputc(int ch, FILE *f) 
{ 
	while((PRINT->SR&0X40)==0);
	PRINT->DR=(uint8_t)ch;
	return ch;
} 


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
//测试打印
void Printf_Task(void const * argument)
{
	while(1)
	{
			static uint8_t m;
			static float powermaybe,powermaybesum,inapow;
			static uint16_t  powermax;
			static float power,powerbuff;
			float powerm[4];
		
		for(m=0;m<4;m++ )
		{
			powerm[m] = chassisMotor[m].current*chassisMotor[m].speed;
			if(powerm[m]<0) powerm[m] = 0;
				
		}
		
		powermaybe = (powerm[0]+powerm[1]+powerm[2]+powerm[3])*0.000003f;
		INA_getPow();
		
		get_chassis_power_and_buffer(&power, &powerbuff, &powermax);

		powermaybe = 0.95f*powermaybesum + 0.05f*powermaybe;
		inaData.pow = 0.9f*inapow + 0.1f*inaData.pow;
		powermaybesum = powermaybe;
		inapow = inaData.pow;
		
		showData[0]	= powermax;
		showData[1] = power;
		showData[2] = inaData.pow/40.0f;
		showData[3] = 0;
		DATA_show(showData,3);
		
		osDelay(2);
	}
}

