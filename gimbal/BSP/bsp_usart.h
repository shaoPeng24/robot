#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "usart.h"
#include "stdio.h"
#include <string.h>
extern float FloatWave[4];
int fputc(int ch, FILE *f);
void DATA_show(float *FloatData, uint8_t Length);//山外曲线显示数据 float类型



#endif
