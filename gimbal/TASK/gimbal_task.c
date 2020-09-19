#include "gimbal_task.h"

#include "mpu_data.h"
#include "bsp_usart.h"
#include "pid.h"
#include "rc.h"
#include "keymouse.h"
#include "caninfo_task.h"
#include "send_task.h"
#include "vision.h"
#include "math.h"





PidTypeDef YAW_SPEED_PID; 
PidTypeDef YAW_POSITION_PID;
PidTypeDef PITCH_SPEED_PID; 
PidTypeDef PITCH_POSITION_PID;

float YawPositionPid[5] 	= {15.0f,			0.0f,			0.0f,		600.0f,			3000.0f};
float YawSpeedPid[5] 			= {300.0f,		50.0f,		0.0f,		21000.0f,		1000.0f};

float PitchPositionPid[5] = {30.0f,			0.0f,			2.5f,		300.0f,			150.0f};
float PitchSpeedPid[5] 		= {350.0f,		50.0f,		0.0f,		21000.0f,		5000.0f};

static void YawPitch_PIDinit(void);
static void AngelUpdate(void);
static void Gimbal_RemotePid(float yaw,float pitch);
static void Gimbal_resetRoot(float yaw,float pitch);

void Gimbal_Remotemode(void);
void Gimbal_Keymousemode(void);
void Gimbal_Automode(void);

float yawset,pitchset,auto_change;

portTickType gimbalTime;

void Gimbal_Task(void const * argument)
{
		portTickType currentTime;
		
		YawPitch_PIDinit();
		vTaskDelay(20);                                  //等待imu稳定
		
		while(mpuData.status==0)printf("Gimbal err\r\n");	//获取imu第上电角度
	
		rootAngle.yawRoot=mpuData.yaw;
		rootAngle.pitchRoot=pitchMotor.coder*ENCODER_TO_EULER;
	
	
		currentTime = xTaskGetTickCount();//当前系统时间
		
		while(1)
		{
			/*************检测视觉掉线**************/
			gimbalTime	=	xTaskGetTickCount();
			if(fabs(gimbalTime-visionTime) > 20)
				visionData.id = 0;
			/**************************************/
			if(rc_ctrl.rc.s[0] == 1)
			{
					GetKeyValue();
					Gimbal_Keymousemode();	
					
			}
			else if(rc_ctrl.rc.s[0] == 3)
			{
	
					Gimbal_Remotemode();
				
			}
			else
			{		
				servoClose();
				AngelUpdate();
				canTX_gimbal(0,0);

			}

			vTaskDelayUntil(&currentTime,5);
		}
}



void Gimbal_Remotemode(void)
{
	
			yawset 	 = (float)rc_ctrl.rc.ch[2]/440;
			pitchset = (float)rc_ctrl.rc.ch[3]/440;
			
		if(rc_ctrl.rc.s[1] == 1)
		{
				if(auto_change == 1) 
				{
					auto_change =0;
					rootAngle.yawRoot=mpuData.yaw;
					rootAngle.pitchRoot=pitchMotor.coder*ENCODER_TO_EULER; 
				}
				Gimbal_RemotePid(yawset,pitchset);
				servoClose();
			

		}
		else
		{
				auto_change =1;
				
				Gimbal_Automode();
			if(rc_ctrl.rc.s[1] == 3)	servoClose();
			else											servoOpen();
				
		}		
}


void Gimbal_Keymousemode(void)
{
	

	yawset   = (float)rc_ctrl.mouse.x *0.0439453f*0.5f;
	pitchset = (float)-rc_ctrl.mouse.y *0.0439453f*0.5f;

	if(rc_ctrl.rc.s[1]==1)
	{
		if(rc_ctrl.mouse.press_r == 1)
		{
				auto_change =1;
				Gimbal_Automode();
		}
		else		
		{
				if(auto_change == 1) 
				{
					auto_change =0;
					rootAngle.yawRoot=mpuData.yaw;
					rootAngle.pitchRoot=pitchMotor.coder*ENCODER_TO_EULER; 
				}
				Gimbal_RemotePid(yawset,pitchset);
		}
	}
	else 
	{
			auto_change =1;
			Gimbal_Automode();
	}
	
	if(KeyMouseFlag.servo == 1)
		servoOpen();
	else
		servoClose();
}

void Gimbal_Automode(void)
{
		static uint8_t  aiChange=1;
		
	
		if(visionData.id==1)
		{								
				rootAngle.yawToRoot 	= mpuData.yaw;   //yaw轴采用相对角度控制
				rootAngle.pitchToRoot = pitchMotor.coder*0.0439453f;
							
				followMate();
				aiChange=1;
			
		}
		else
		{
				if(aiChange==1)
				{
					rootAngle.yawRoot 	= mpuData.yaw;   //yaw轴采用相对角度控制
					rootAngle.pitchRoot = pitchMotor.coder*0.0439453f;
									
					
					rootAngle.pitchToRoot = 0;
					rootAngle.yawToRoot   = 0;
					aiChange=0;
					aiDataReset();
					
				}
				
				Gimbal_RemotePid(yawset,pitchset);
		}
		
}

static void Gimbal_resetRoot(float yaw,float pitch)
{
		

		rootAngle.yawRoot 	-= yaw ;//yaw轴采用相对角度控制
		rootAngle.pitchRoot += pitch ;

		rootAngle.pitchRoot =((rootAngle.pitchRoot >= PITCH_DOWN_LIMIT) ? rootAngle.pitchRoot : PITCH_DOWN_LIMIT);
		rootAngle.pitchRoot =((rootAngle.pitchRoot <= PITCH_UP_LIMIT) ? rootAngle.pitchRoot : PITCH_UP_LIMIT);
		

}





//遥控控制

static void Gimbal_RemotePid(float yaw,float pitch)
{
		Gimbal_resetRoot(yaw,pitch);
			
		//YAW

		PID_Calc(&YAW_POSITION_PID,mpuData.yaw,rootAngle.yawRoot);
		PID_Calc(&YAW_SPEED_PID,-mpuData.gz, YAW_POSITION_PID.out);

  	//PITCH
		rootAngle.pitchRoot =((rootAngle.pitchRoot >= PITCH_DOWN_LIMIT) ? rootAngle.pitchRoot : PITCH_DOWN_LIMIT);
		rootAngle.pitchRoot =((rootAngle.pitchRoot <= PITCH_UP_LIMIT) ? rootAngle.pitchRoot : PITCH_UP_LIMIT);
		
		PID_Calc(&PITCH_POSITION_PID,pitchMotor.coder*ENCODER_TO_EULER, rootAngle.pitchRoot);
		PID_Calc(&PITCH_SPEED_PID,-mpuData.gx,PITCH_POSITION_PID.out);	
		

		canTX_gimbal(PITCH_SPEED_PID.out,YAW_SPEED_PID.out);
		


}
//自瞄控制
static void Gimbal_AutoPid(float yaw,float pitch)
{
		//YAW
		PID_Calc(&YAW_POSITION_PID,mpuData.yaw,yaw);
		PID_Calc(&YAW_SPEED_PID,-mpuData.gz, YAW_POSITION_PID.out);

	
		//PITCH

		pitch =((pitch >= PITCH_DOWN_LIMIT) ? pitch : PITCH_DOWN_LIMIT);
		pitch =((pitch <= PITCH_UP_LIMIT) ? pitch : PITCH_UP_LIMIT);;
	
		PID_Calc(&PITCH_POSITION_PID,pitchMotor.coder*ENCODER_TO_EULER, pitch);
		PID_Calc(&PITCH_SPEED_PID,-mpuData.gx,PITCH_POSITION_PID.out);	
	
	
		canTX_gimbal(PITCH_SPEED_PID.out,YAW_SPEED_PID.out);
}	

//弹道下坠
float dropoffset;
float dropOffset(float distance,float shootSpd)  //下坠补偿。
{

	if(distance>=shootSpd*shootSpd/5.0f)return 0.0f;
	else if(distance<=0)return 0.0f;
	else
	{
		return asinf(distance*9.8f/(shootSpd*shootSpd))*28.65f-7.568f*exp(-0.5633f*distance);
	}

}

float pTime,yTime;
//自瞄跟随
uint8_t followMate(void)
{
		if(rc_ctrl.rc.s[0]==1)
			if(rc_ctrl.rc.s[1]==1)  {	if(KeyMouseFlag.prediction ==0) pTime = 5 , yTime = (visionData.distance/17.0f*0.2f) ;
																else                            pTime = 5 , yTime = 5;  }
			else 										 pTime = 2  , yTime = 2;
		else											 pTime = 5 , yTime = (visionData.distance/17.0f*0.2f);
				
	
			followData.yaw[0]  = rootAngle.yawToRoot-visionData.yaw-1;
			followData.pitch[0]= rootAngle.pitchToRoot+visionData.pitch;   
			

			if(followData.dataReset < 100)
			{
				yTime = 0;
				followData.dataReset++;
			}
			else followData.dataReset = 100;

		  followData.yawX=followData.yaw[1]+0.15f*(followData.yaw[0]-followData.yaw[1]);
			followData.yawRateX=followData.yawRate[1] +0.1f*(followData.yawX-followData.yaw[1]);
			followData.yawAccX=0.707f*(followData.yawRateX-followData.yawRate[1]);
			followData.yawAcc[0]=followData.yawAcc[0]+0.01f*(followData.yawAccX-followData.yawAcc[0]);
	
			followData.pitchX=followData.pitch[1]+0.15f*(followData.pitch[0]-followData.pitch[1]);
			followData.pitchRateX=followData.pitchRate[1]+0.1f*(followData.pitchX-followData.pitch[1]);
			followData.pitchAccX=0.707f*(followData.pitchRateX-followData.pitchRate[1]);
			followData.pitchAcc[0]=followData.pitchAcc[0]+0.01f*(followData.pitchAccX-followData.pitchAcc[0]);

			followData.yawRateX=followData.yawRateX>0.12f?0.12f:followData.yawRateX;
			followData.yawRateX=followData.yawRateX<-0.12f?-0.12f:followData.yawRateX;
			
			followData.pitchRateX=followData.pitchRateX>0.12f?0.12f:followData.pitchRateX;
			followData.pitchRateX=followData.pitchRateX<-0.12f?-0.12f:followData.pitchRateX;

			followData.yaw[1]=followData.yawX+followData.yawRateX+0.5f*followData.yawAcc[0];
			followData.yawRate[1]=followData.yawRateX-followData.yawAccX+followData.yawAcc[0];
			
			followData.pitch[1]=followData.pitchX+followData.pitchRateX+0.5f*followData.pitchAcc[0];
			followData.pitchRate[1]=followData.pitchRateX-followData.pitchAccX+followData.yawAcc[0];
			

			

		  followData.yawOut=followData.yawX+yTime*followData.yawRateX+0.5f*yTime*yTime*followData.yawAcc[0];
		  followData.pitchOut=followData.pitchX+pTime*followData.pitchRateX+0.5f*pTime*pTime*followData.pitchAcc[0];
			

		
			if(rc_ctrl.rc.s[0]==1)
					if(rc_ctrl.rc.s[1]==1)   Gimbal_AutoPid(followData.yawOut,followData.pitchOut+dropOffset(visionData.distance/1000,16.8f));
					else 										 Gimbal_AutoPid(followData.yawOut,followData.pitchOut);
				else											 Gimbal_AutoPid(followData.yawOut,followData.pitchOut+dropOffset(visionData.distance/1000,16.8f));
				


			return 0;
			
}
void aiDataReset(void)
{
	if(followData.dataReset<=100)
	{
		followData.yaw[1]=0;
		followData.yawRate[1]=0;
		followData.yawAcc[0]=0;
		followData.pitch[1]=0;
		followData.pitchRate[1]=0;
		followData.pitchAcc[0]=0;
		followData.distance[1]=0;
		followData.distanceRate[1]=0;
		followData.distanceAcc[0]=0;
		followData.x[1]=0;
		followData.xRate[1]=0;
		followData.xAcc[0]=0;
		followData.y[1]=0;
		followData.yRate[1]=0;
		followData.yAcc[0]=0;
		followData.dataReset=1;
	}
}


static void YawPitch_PIDinit(void)
{
		PID_Init(&YAW_POSITION_PID,  YawPositionPid);
		PID_Init(&YAW_SPEED_PID,  YawSpeedPid);
		PID_Init(&PITCH_POSITION_PID,  PitchPositionPid);
		PID_Init(&PITCH_SPEED_PID,  PitchSpeedPid);

}

static void AngelUpdate(void)
{
	 rootAngle.pitchRoot =pitchMotor.coder*ENCODER_TO_EULER;
   rootAngle.yawRoot =mpuData.yaw;

}
