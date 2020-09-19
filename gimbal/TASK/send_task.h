#ifndef __SEND_TASK_H
#define __SEND_TASK_H

#include "main.h"
uint8_t canTX_gimbal(short pitch,short yaw);
uint8_t canTX_shoot(int16_t poker,int16_t shootleft,int16_t shootrigh);
uint8_t canTX_chassis(int16_t x,int16_t y,int8_t z,int8_t mode,int8_t superstate);
#endif
