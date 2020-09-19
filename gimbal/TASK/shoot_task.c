#include "shoot_task.h"

#include "caninfo_task.h"

#include "rc.h"
#include "pid.h"
#include "math.h"
#include "bsp_usart.h"
#include "send_task.h"
#include "vision.h."
#include "keymouse.h"

portTickType shoot_time1;
portTickType shoot_time2;
int time;

PidTypeDef POKER_SPEED_PID;
PidTypeDef POKER_POSITION_PID;
PidTypeDef SHOOT_SPEED_PID[2];

_E_ShootHeat ShootHeat;

float PokerPositionPid[5] = {0.25f,		0.0f,		0.0f,		4500.0f,		0.0f};
float PokerSpeedPid[5]		= {7.0f,		0.5f,		0.1f,		7000.0f,		3000.0f};
float ShootSpeedPid[5] 		= {30.0f,		1.2f,		0.0f,		12000.0f,		5000.0f};

static void PokerSpeedLoop(void);
static void PokerpPositionLoop(void);
static void Shoot_Single_Ctrl(int32_t shoot_num);
static void Shoot_PIDinit(void);
static void ShootSpeedLoop(void);
static void Shoot_Triple_Ctrl(void);
static void Shoot_Heat_control(void);
static void Shoot_Speed_control(void);

void Shoot_Remotemode(void);
void Shoot_Keymousemode(void);
void Shoot_Protectmode(void);

int32_t shoot_motor_speed;

void Shoot_Task(void const * argument)
{
	portTickType currentTime;
	
	Poker_PIDinit();
	Shoot_PIDinit();
	currentTime = xTaskGetTickCount();//当前系统时间
	
	while(1)
	{
		Shoot_Heat_control();
		Shoot_Speed_control();
		
		if(rc_ctrl.rc.s[0] ==1)
		{
			 Shoot_Keymousemode();

		}
		else if(rc_ctrl.rc.s[0] ==3)
		{
			Shoot_Remotemode();
		}
		else
		{
		
			Shoot_Protectmode();
		}
		
		canTX_shoot(POKER_SPEED_PID.out,SHOOT_SPEED_PID[0].out,SHOOT_SPEED_PID[1].out);
		
		vTaskDelayUntil(&currentTime, 2);
	}
	
}


//发射遥控模式
void Shoot_Remotemode(void)
{
			if(rc_ctrl.rc.s[1] ==3)
			{
				shootMotor[0].speedTarget=0;
				shootMotor[1].speedTarget=0;
				
				
			}
			else if(rc_ctrl.rc.s[1] ==2)
			{
				shootMotor[0].speedTarget=-shoot_motor_speed;
				shootMotor[1].speedTarget=shoot_motor_speed;
				
				
			}
			else
			{
				shootMotor[0].speedTarget=0;
				shootMotor[1].speedTarget=0;
			
		
			}
			if(ShootHeat == NOOVERHEAT)
				pokerMotor.speedTarget = fabs(rc_ctrl.rc.ch[4]*8);
			else
				pokerMotor.speedTarget = 0;
			
			ShootSpeedLoop();
			PokerSpeedLoop();

}
//发射键鼠模式
void Shoot_Keymousemode(void)
{
		GetKeyValue();
	
		if(rc_ctrl.rc.s[1] == 1)
		{
				if(KeyMouseFlag.LspeedHfrq == 1)
				{
						shootMotor[0].speedTarget=-shoot_motor_speed;
						shootMotor[1].speedTarget=shoot_motor_speed;
				}
				else
				{
						if(KeyMouseFlag.shootspeed == 0)
						{
							shootMotor[0].speedTarget=-shoot_motor_speed;
							shootMotor[1].speedTarget=shoot_motor_speed;
						}
						else
						{
							shootMotor[0].speedTarget=-shoot_motor_speed;
							shootMotor[1].speedTarget=shoot_motor_speed;
						}
				}
		}
		else 
		{
				shootMotor[0].speedTarget=-5200;
				shootMotor[1].speedTarget=5200;
		}
		ShootSpeedLoop();
		
		if(ShootHeat == NOOVERHEAT)
		{
			if(KeyMouseFlag.shootfrq == 0)
				Shoot_Single_Ctrl(1);
			else
				Shoot_Triple_Ctrl();
		}
		else
		{
			pokerMotor.speedTarget = 0;
			PokerSpeedLoop();
		}
		
}



void Shoot_Protectmode(void)
{
		shootMotor[0].speedTarget=0;
		shootMotor[1].speedTarget=0;
		ShootSpeedLoop();
		pokerMotor.speedTarget =0; 
		PokerSpeedLoop();
}


#define 	POKER_SPEED_RATIO   2160       	//电机轴一秒转一圈,2160转子转速,60*36,乘射频再除以拨盘格数就可得相应射频下的转速
#define 	POKER_SPEED_GRID    9						//拨盘格数
#define   AN_BULLET         (36864.0f)		//单个子弹电机位置增加值


 
 uint8_t press_l_pre;
 uint16_t	press_l_count;
 uint16_t cout,flag,couttime,last_first;
//自动打符
void Buff_control(void)
{
	static float lastyaw,lastpitch;
	
	if(rc_ctrl.mouse.press_l-press_l_pre == 1) 
	{
		press_l_count++;
	}
	press_l_pre = rc_ctrl.mouse.press_l;

		if(rc_ctrl.rc.s[1] == 2)
		{
			if(visionData.distance -last_first == 1)
			{
				flag = 1;
				couttime = 100;
			}
			else if(visionData.distance == 1)
			{
					if((fabs(visionData.yaw-lastyaw)>1.5f||fabs(visionData.pitch-lastpitch)>1.5f)&&flag==0)
					{
						flag = 1;
						
							if(fabs(visionData.yaw-lastyaw)+fabs(visionData.pitch-lastpitch)>10)
								couttime=320;
							else 
								couttime=220;
					}
			}
			else 
				flag = 0;
		}
		else
		{
			if((fabs(visionData.yaw-lastyaw)>1.5f||fabs(visionData.pitch-lastpitch)>1.5f)&&flag==0)
				{
					flag = 1;
					
						if(fabs(visionData.yaw-lastyaw)+fabs(visionData.pitch-lastpitch)>10)
							couttime=300;
						else 
							couttime=200;
				}
		}
	
		if(flag==1)
		{
				
			cout = cout+1;
			if(cout==couttime)
			{
				flag=0;
				cout=0;
				press_l_count = press_l_count+1;
			}
		}
		else
		{
			flag=0;
			cout = 0;
		
		}

		lastyaw = visionData.yaw,lastpitch = visionData.pitch;
		last_first = visionData.distance;
}

//单发控制
static void Shoot_Single_Ctrl(int32_t shoot_num)
{
	static uint8_t status = 0;
	
	if(rc_ctrl.rc.s[1] == 1)
	{
		if(rc_ctrl.mouse.press_l-press_l_pre == 1) 
		{
			press_l_count++;
		}
		press_l_pre = rc_ctrl.mouse.press_l;
	}
	else
	{
		Buff_control();
	}
	
	if(press_l_count > 0)
	{
		if(status == 0)
		{
			pokerMotor.turns = 0; 
			pokerMotor.coderTarget = 4096+AN_BULLET*shoot_num;
			status = 1;
		}
			pokerMotor.sumCoder = pokerMotor.coder+pokerMotor.turns*8192;
			PokerpPositionLoop();
		if((fabs(pokerMotor.coderTarget-pokerMotor.sumCoder)<3000)&&(fabs(pokerMotor.speed)<1300))
		{
			status = 0;
			press_l_count--;
		}
	}
	else 
	{
		pokerMotor.speedTarget = 0;
		PokerSpeedLoop();
	}
	
	
}


//连发控制
#define  SHOOT_LEFT_TIME_MAX  150
int16_t poker_freq;

static void Shoot_Triple_Ctrl(void)
{
	
	 if(rc_ctrl.rc.s[0] == 3)
	 {
		 if(rc_ctrl.rc.s[1] == 3)  			poker_freq = 0;//射频选择
		 else if(rc_ctrl.rc.s[1] == 2)  poker_freq = 9;
		 else 															 				poker_freq = 0;
	 }
	 else if(rc_ctrl.rc.s[0] == 1)
	 {
			if(KeyMouseFlag.LspeedHfrq == 0)
			{
				if(rc_ctrl.mouse.press_l == 1)		poker_freq = 12;
				else															poker_freq = 0;
			}
			else
			{
				if(rc_ctrl.mouse.press_l == 1)		poker_freq = 12;
				else															poker_freq = 0;
			}
	}
	 else
	 {
		 		poker_freq = 0;
	 }
	 pokerMotor.speedTarget = POKER_SPEED_RATIO/POKER_SPEED_GRID*poker_freq;
	 PokerSpeedLoop();
	
}




//热量控制
static void Shoot_Heat_control(void)
{
	ShootHeat = (Refeere.heat0_limit - Refeere.heat0 > Refeere.bullet_speed+10) ? NOOVERHEAT : OVERHEAT;//OVERHEAT ;		
}

//射速选择
static void Shoot_Speed_control(void)
{
	if(Refeere.bullet_speedlimit == 15)
		shoot_motor_speed = 3900;
	else if(Refeere.bullet_speedlimit == 18)
		shoot_motor_speed = 4300;
	else
		shoot_motor_speed = 5200;
}

void Poker_PIDinit(void)
{
		PID_Init(&POKER_POSITION_PID,  PokerPositionPid);	
		PID_Init(&POKER_SPEED_PID,  PokerSpeedPid);
		
}

static void PokerSpeedLoop(void)
{
		PID_Calc(&POKER_SPEED_PID,pokerMotor.speed,pokerMotor.speedTarget);
	
}

static void PokerpPositionLoop(void)
{
		PID_Calc(&POKER_POSITION_PID,pokerMotor.sumCoder,pokerMotor.coderTarget);
		PID_Calc(&POKER_SPEED_PID,pokerMotor.speed,POKER_POSITION_PID.out);

}



static void Shoot_PIDinit(void)
{ 
		for(int i = 0;i < 2;i++)
		{
			PID_Init(&SHOOT_SPEED_PID[i],  ShootSpeedPid);	
		}
}

static void ShootSpeedLoop(void)
{

		for(int i = 0;i < 2; i++)
		{
			PID_Calc(&SHOOT_SPEED_PID[i],shootMotor[i].speed,shootMotor[i].speedTarget);			
		}

}
