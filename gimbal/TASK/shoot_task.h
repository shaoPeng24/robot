#ifndef __SHOOT_TASK_H
#define __SHOOT_TASK_H

#include "freertos.h"
#include "FreeRTOSConfig.h"
#include "task.h"

typedef enum
{
	NOOVERHEAT =  1,
	OVERHEAT   = 0,
}_E_ShootHeat;

extern portTickType shoot_time1;
extern portTickType shoot_time2;
void Poker_PIDinit(void);
extern int time;

#endif
