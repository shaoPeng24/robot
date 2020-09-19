#ifndef  __LED_H
#define __LED_H
#include "main.h"


typedef struct
{
	GPIO_TypeDef*  GPIO;
	uint32_t       PIN;
	uint8_t            tick;
}ledDataType;
void led_init(void);
void led_run(unsigned char tick);
void led_on(ledDataType* led);
void led_off(ledDataType* led);
void led_sw(ledDataType* led);
void led_blin(uint8_t tick,ledDataType * led);

extern ledDataType led[4];
#endif

