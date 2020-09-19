#include "chassis_task.h"

#include "freertos.h"
#include "task.h"

#include "bsp_usart.h"
#include "math.h"
#include "pid.h"
#include "caninfo_task.h"
#include "INA230.h"
#include "referee.h"
#include "super_control.h"
#include "bsp_beep.h"

Chassis_Move_t chassis_move={0};

static void chassis_mode_set(uint8_t mode);
static void chassis_speed_set(void);
static void chassis_set_control(Chassis_Move_t *chassismove);
static void chassis_vector_to_mecanum_wheel_speed(void);
static void chassis_Pid_Init(void);
static void chassis_speed_loop(void);
static int16_t Rampcalc(RampStruct *Ramp);

static uint8_t canTX_chassis(short wheel1,short wheel2,short wheel3,short wheel4);

PidTypeDef Chassis_Control_Pid[4];
PidTypeDef Wzset_Control_Pid;

RampStruct LFRamp = {0, 0, 0, 0, 0};
RampStruct LBRamp = {0, 0, 0, 0, 1};
RampStruct RFRamp = {0, 0, 0, 0, 2};
RampStruct RBRamp = {0, 0, 0, 0, 3};


float ChassisPid[3] = {WHEEL_SPEED_KP,WHEEL_SPEED_KI,WHEEL_SPEED_KD};
float WzsetPid[3]   = {WZSET_KP,WZSET_KI,WZSET_KD};

void Chassis_Task(void const * argument)
{
	
	chassis_Pid_Init();
	INA_init();
	
	portTickType currentTime;
	currentTime = xTaskGetTickCount();//当前系统时间
	while(1)
	{
		INA_getPow();
		super_voltage();
		
		chassis_mode_set(chassisCan.mode);
		
		chassis_speed_set();
		
		chassis_set_control(&chassis_move);
		
		chassis_vector_to_mecanum_wheel_speed();
		
		chassis_speed_loop();

		super_control();
	
		if(chassisCan.mode == 0)
			for(int i=0;i < 4;i++)
				Chassis_Control_Pid[i].out=0;

		
		canTX_chassis(Chassis_Control_Pid[0].out,Chassis_Control_Pid[1].out,Chassis_Control_Pid[2].out,Chassis_Control_Pid[3].out);


		
		vTaskDelayUntil(&currentTime,4);
	}
}

//底盘模式选择
static void chassis_mode_set(uint8_t mode)
{
	if(mode == CHASSIS_LOKER)
		chassis_move.mode =	CHASSIS_LOKER;
	else if(mode == FALLOW)
		chassis_move.mode =	FALLOW;
	else if(mode == CHASSIS_TOP)
		chassis_move.mode = CHASSIS_TOP;
	else if(mode == CHASSIS_SLOW)
		chassis_move.mode = CHASSIS_SLOW;
	else
		chassis_move.mode = CHASSIS_PROTECT;
}

float vxset,vyset;
static void chassis_speed_set(void)
{
		
		
		if(chassisCan.s0 == 1)
		{
			vxset = (float)chassisCan.x*600*12;
			vyset =	(float)chassisCan.y*600*12;
		}
		else if(chassisCan.s0 == 3)
		{
			vxset = (float)chassisCan.x*12;
			vyset =	(float)chassisCan.y*12;
		}
		else
		{
			vxset = 0;
			vyset =	0;
		}	
		
		if(chassis_move.mode == CHASSIS_LOKER)
		{
			
			chassis_move.vx =  0;
			chassis_move.vy =  0;
			chassis_move.angle_set = 0;
			
		}
		else if(chassis_move.mode == FALLOW)
		{
			

			chassis_move.vx =	vxset;
			chassis_move.vy =	vyset;
			chassis_move.angle_set =  (yawMotor.coder-4100) ;	
						
	
		}
		else if(chassis_move.mode == CHASSIS_TOP)
		{
			
			chassis_move.vx =	vxset;
			chassis_move.vy =	vyset;
			chassis_move.angle_set = (600);
			
		}
		else if(chassis_move.mode == CHASSIS_SLOW) 
		{
			
			chassis_move.vx =	vxset*0.5f;
			chassis_move.vy =	vyset*0.5f;
			chassis_move.angle_set =  (yawMotor.coder-4100) ;
		}
		else
		{
			for(int i=0;i < 4;i++)
				Chassis_Control_Pid[i].out=0;
		}
}

static void chassis_set_control(Chassis_Move_t *chassismove)
{
	float cos_yaw = 0.0f,	sin_yaw = 0.0f;
	
	cos_yaw = cos((yawMotor.coder-4100)*0.000767);//4100为中心值
	sin_yaw = sin((yawMotor.coder-4100)*0.000767);
	
	chassismove->vx_set = chassismove->vx*sin_yaw + chassismove->vy*cos_yaw;
	chassismove->vy_set = chassismove->vx*cos_yaw - chassismove->vy*sin_yaw;

	chassismove->wz_set = chassis_move.angle_set*6.0f;
	
}
//麦轮解算
static void chassis_vector_to_mecanum_wheel_speed(void)
{
    //because the gimbal is in front of chassis, when chassis rotates, wheel 0 and wheel 1 should be slower and wheel 2 and wheel 3 should be faster
    //旋转的时候， 由于云台靠前，所以是前面两轮 0 ，1 旋转的速度变慢， 后面两轮 2,3 旋转的速度变快
    chassisMotor[0].speedTarget =  chassis_move.vx_set + chassis_move.vy_set - chassis_move.wz_set;
    chassisMotor[1].speedTarget =  chassis_move.vx_set - chassis_move.vy_set - chassis_move.wz_set;
    chassisMotor[2].speedTarget = -chassis_move.vx_set + chassis_move.vy_set - chassis_move.wz_set;
    chassisMotor[3].speedTarget = -chassis_move.vx_set - chassis_move.vy_set - chassis_move.wz_set;
	
		chassisMotor[0].speedTarget = Rampcalc(&LFRamp);	
		chassisMotor[1].speedTarget = Rampcalc(&LBRamp);
		chassisMotor[2].speedTarget = Rampcalc(&RFRamp);
		chassisMotor[3].speedTarget = Rampcalc(&RBRamp);
}

//功率限制
void chassis_power_limit(void)
{
	static uint16_t  Power_Max;
	static float  Power_Maybe = 0.0f;
	static float Power_Bufferlimit = 0.0f;
	static float Power,Power_Buffer;
	
	get_chassis_power_and_buffer(&Power, &Power_Buffer, &Power_Max);
	

	
 	Power_Bufferlimit = Power_Buffer-10;
	//计算原本发送目标电流值大小
	Power_Maybe = (uint16_t)((fabs(Chassis_Control_Pid[0].out)+fabs(Chassis_Control_Pid[1].out)+fabs(Chassis_Control_Pid[2].out)+fabs(Chassis_Control_Pid[3].out))*24/1000);
	Power_Max = Power_Bufferlimit > 0 ?(uint16_t)Power_Bufferlimit*15:Power_Max;//计算当前可用功率上限

		if(Power_Maybe>Power_Max){
			for(uint8_t i = 0; i <= 3; i++){
				 Chassis_Control_Pid[i].out = Chassis_Control_Pid[i].out*(float)Power_Max/Power_Maybe;//同比例放缩电流值
				}
		}
}

//运动斜坡
static int16_t Rampcalc(RampStruct *Ramp)
{
		Ramp->Target = chassisMotor[Ramp->Number].speedTarget;
		Ramp->Dirt = ((Ramp->Target - Ramp->Current) >= 0) ? 1 : -1;         //方向标记
		
		Ramp->Output = Ramp->Current + Ramp->Dirt*120;	
	
    Ramp->Output = (fabs(Ramp->Output) >= fabs(Ramp->Target)) ? Ramp->Target : Ramp->Output;  //限幅！
    Ramp->Current = Ramp->Output;
    return Ramp->Output;
}



static void chassis_Pid_Init(void)
{
	for(int i = 0;i < 4; i++)
	{
		PID_Init(&Chassis_Control_Pid[i],PID_POSITION,ChassisPid,WHEEL_SPEED_MAX,WHEEL_SPEED_IMAX);
	}
		PID_Init(&Wzset_Control_Pid,PID_POSITION,WzsetPid,WZSET_MAX,WZSET_IMAX);
}
//底盘速度环
static void chassis_speed_loop(void)
{
	for(int i = 0;i < 4; i++)
	{
		if(fabs(chassisMotor[i].speed) < 20)
			chassisMotor[i].speed = 0;
		PID_Calc(&Chassis_Control_Pid[i],chassisMotor[i].speed,chassisMotor[i].speedTarget);
		
	}
}

static uint8_t canTX_chassis(short wheel1,short wheel2,short wheel3,short wheel4)
{
	CAN_TxHeaderTypeDef canFrame;
	uint8_t data[8]={0};
	uint32_t temp=0;
	
	canFrame.IDE=CAN_ID_STD;
	canFrame.StdId=0x200;
	canFrame.RTR=CAN_RTR_DATA;
	canFrame.DLC=8;
	canFrame.TransmitGlobalTime=DISABLE;
	data[0]=wheel1>>8;
	data[1]=wheel1&0xff;
	data[2]=wheel2>>8;
	data[3]=wheel2&0xff;
	data[4]=wheel3>>8;
	data[5]=wheel3&0xff;
	data[6]=wheel4>>8;
	data[7]=wheel4&0xff;
	HAL_CAN_AddTxMessage(&hcan1, &canFrame, data,&temp);
	
	return temp;	
}
