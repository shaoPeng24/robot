#ifndef  __INA230_H
#define __INA230_H
#include "iic.h"

#define 	CFG_REG	 		0x00
#define 	SV_REG 			0x01
#define 	BV_REG 			0x02
#define 	PWR_REG 		0x03
#define 	CUR_REG 		0x04
#define 	CAL_REG 		0x05
#define 	ONFF_REG 		0x06
#define 	AL_REG 			0x07
#define   	INA226_ADDR1	0x40  //超级电容入口INA
#define   	INA226_ADDR2	0x41

typedef struct
{
	int16_t cur;  //电流值
	float pow;  //功率
	float vol;
	uint16_t cal;
}INADataType;

extern INADataType inaData;

uint8_t DAC_set(uint8_t data);
void INA_init(void);
void INA_getCur(void);
void INA_getPow(void);
void INA_getVol(void);
uint8_t INA_writeData(uint8_t dev, uint8_t reg,uint16_t data);
uint8_t INA_readLen(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *buf);

#endif

