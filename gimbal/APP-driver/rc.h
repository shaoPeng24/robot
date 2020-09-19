#ifndef __RC_H
#define __RC_H

#include "dma.h"
#include "usart.h"

#define SBUS_RX_BUF_NUM 36u
#define RC_FRAME_LENGTH 18u

//遥控器数据结构体
typedef __packed struct
{
	__packed struct
	{
			int16_t ch[5];
			char s[2];
	} rc;
	__packed struct
	{
			int16_t x;
			int16_t y;
			int16_t z;
			uint8_t press_l;
			uint8_t press_r;
	} mouse;
	__packed struct
	{
			uint16_t v;
	} key;

} RC_ctrl_t;

extern RC_ctrl_t rc_ctrl;
void remote_control_init(void);
void RC_IRQHandler(void);

#endif
