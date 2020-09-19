#include "bsp_tim.h"
#include "cmsis_os.h"

void DELAY_init(void)
{
	MX_TIM6_Init();
	HAL_TIM_Base_Start_IT(&htim6);
}

void Bsp_tim2Init(void)
{
	MX_TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);	
}

void Bsp_tim1Init(void)
{
	MX_TIM1_Init();
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	
}

/*为软件iic提供延时*/
void DELAY_us(uint16_t us)  //最大780us
{
	htim6.Instance->CNT=0;
	while(htim6.Instance->CNT<us*84);
}

