#include "vision.h"
#include <stdio.h>
#include <string.h>

#include "dma.h"
#include "usart.h"
#include "gimbal_task.h"
#include "caninfo_task.h"
#include "rc.h"

#define VS_huart	huart1
#define VS_UART		USART1
#define VS_rx_dma	hdma_usart1_rx
#define VS_tx_dma	hdma_usart1_tx

#define VS_FRAME_LENGTH 16
#define VS_BUF_NUM      64

portTickType visionTime;
/*
为视觉数据开辟空间
*/
visionStruct visionData={0};
visionStruct visionTempData={0};
RootAngleType rootAngle={0};
followStruct followData={0};

uint8_t vision_rx_buf[2][VS_FRAME_LENGTH];
uint8_t errcnt;

void Vision_Task(void const * argument)
{
	while(1)
	{
		static uint8_t tx_buff[1];
		tx_buff[0] = rc_ctrl.rc.s[1];
		/********与视觉通信*******/
		if(visionTempData.id == 255)
		{
				
			tx_buff[0] = (Refeere.robot_id > 100) ?  11:12;
			HAL_UART_Transmit(&huart1, tx_buff, 1, 20);

		}
		else
		{
			if(rc_ctrl.rc.s[0] == 3)
			{
				tx_buff[0] = 1;
				HAL_UART_Transmit(&huart1, tx_buff, 1, 20);
			}
			else
				HAL_UART_Transmit(&huart1, tx_buff, 1, 20);
		}
		/************************/
		osDelay(10);
	}
}


void vision_usart_init(void)
{
	vision_init(vision_rx_buf[0],vision_rx_buf[1],VS_BUF_NUM );
}

void vision_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{

	SET_BIT(VS_huart.Instance->CR3, USART_CR3_DMAR);

	__HAL_UART_ENABLE_IT(&VS_huart, UART_IT_IDLE);

	__HAL_DMA_DISABLE(&VS_rx_dma);

	while(VS_rx_dma.Instance->CR & DMA_SxCR_EN)
	{
			__HAL_DMA_DISABLE(&VS_rx_dma);
	}

	VS_rx_dma.Instance->PAR = (uint32_t) & (VS_UART->DR);

	VS_rx_dma.Instance->M0AR = (uint32_t)(rx1_buf);

	VS_rx_dma.Instance->M1AR = (uint32_t)(rx2_buf);

	VS_rx_dma.Instance->NDTR = dma_buf_num;

	SET_BIT(VS_rx_dma.Instance->CR, DMA_SxCR_DBM);

	__HAL_DMA_ENABLE(&VS_rx_dma);
		

}

//写入stm32f4xx_it.c串口中断
void USART1_IRQHandler(void)
{
	visionTime = xTaskGetTickCount();
		if(VS_huart.Instance->SR & UART_FLAG_RXNE)//接收到数据
		{
			__HAL_UART_CLEAR_PEFLAG(&VS_huart);
		}
		else if(VS_UART->SR & UART_FLAG_IDLE)
		{
			static uint16_t this_time_rx_len = 0;
			__HAL_UART_CLEAR_PEFLAG(&VS_huart);
			
			if ((VS_rx_dma.Instance->CR & DMA_SxCR_CT) == RESET)
			{
				/* Current memory buffer used is Memory 0 */
				//失效 DMA
				__HAL_DMA_DISABLE(&VS_rx_dma);
				
				this_time_rx_len = VS_BUF_NUM - VS_rx_dma.Instance->NDTR;
				
				VS_rx_dma.Instance->NDTR = VS_BUF_NUM;
				
				VS_rx_dma.Instance->CR |= DMA_SxCR_CT;
				
				__HAL_DMA_ENABLE(&VS_rx_dma);
			

				memcpy(&visionTempData,vision_rx_buf[0],VS_FRAME_LENGTH);
				memset(vision_rx_buf[0],0,VS_FRAME_LENGTH);	
			}
			else
			{
				/* Current memory buffer used is Memory 1 */
				//失效 DMA
				__HAL_DMA_DISABLE(&VS_rx_dma);
				
				this_time_rx_len = VS_BUF_NUM - VS_rx_dma.Instance->NDTR;
				
				VS_rx_dma.Instance->NDTR = VS_BUF_NUM;
				
				DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
				
				__HAL_DMA_ENABLE(&VS_rx_dma);
			
						
				memcpy(&visionTempData,vision_rx_buf[1],VS_FRAME_LENGTH);
				memset(vision_rx_buf[1],0,VS_FRAME_LENGTH);						
			}
			}
			if(visionTempData.id == 1)
			{
				errcnt = 0;
				visionData.id = 1;
				visionData.yaw = visionTempData.yaw;
				visionData.pitch = visionTempData.pitch;
				visionData.distance = visionTempData.distance;
			}
			else
			{
				errcnt++;
				if(errcnt == 10)
				{
					errcnt = 0;
					visionData.id = 0;
					visionData.yaw = 0;
					visionData.pitch = 0;
					visionData.distance = 0;
				}
			}
}



