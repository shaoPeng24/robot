#ifndef __REFEREE_USART_H
#define __REFEREE_USART_H

#include "main.h"


#define REFEREE_RX_BUF_NUM 512


void referee_usart_init(void);
void RE_IRQHandler(void);
void RE_usart_tx_dma_enable(uint8_t *data, uint16_t len);

#endif
