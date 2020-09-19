#ifndef __BSP_UASRT_H
#define __BSP_UASRT_H

#include "usart.h"
#include <stdio.h>

extern float showData[4];
int fputc(int ch, FILE *f);
void DATA_show(float *FloatData, uint8_t Length);//山外曲线显示数据 float类型

#endif
