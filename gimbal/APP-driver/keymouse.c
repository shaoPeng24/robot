#include "keymouse.h"

_KeyMouseFlagStruct KeyMouseFlag;
uint8_t KeyValue[16];

static void ServoOpenOrNot(uint8_t Key_B_State);
static void TopStateGet(uint8_t Key_C_State);
static void FireShootSpeed(uint8_t Key_F_State);
static void GetPokerSpeed(uint8_t Key_R_State);
static void GetLspeedHfrq(uint8_t Key_X_State);
static void GetCutprediction(uint8_t Key_E_State);


void GetKeyValue(void)
{
    
	uint8_t i=0;
	for(i=0;i<16;i++){
		KeyValue[i] = (rc_ctrl.key.v >> i) & 0x0001;//判断键盘是否按下！
	}
		   
	if(KEY_VALUE_W){
		KeyMouseFlag.movfb = 1;//底盘前进标记
	}        

	if(KEY_VALUE_S){
		KeyMouseFlag.movfb =  -1;//底盘后退标记
	}        

	if(KEY_VALUE_A){
		KeyMouseFlag.movlr = -1;//底盘左移标记
	}        

	if(KEY_VALUE_D){
		KeyMouseFlag.movlr =  1;//底盘前进标记
	}

	if(KEY_VALUE_Q){
		KeyMouseFlag.movrt = -1;//云台左旋
	}        

	if(KEY_VALUE_E){
		KeyMouseFlag.movrt =  1;//云台右旋
	} 

	if(KEY_VALUE_SHIFT){
		KeyMouseFlag.superstate = 1;
	}
		
    TopStateGet(KEY_VALUE_C);//按键Shift+C/C的动作，控制底盘小陀螺	    
    ServoOpenOrNot(KEY_VALUE_B);//按键B的动作，控制弹仓盖开闭
	FireShootSpeed(KEY_VALUE_F);//控制摩擦轮转速
	GetPokerSpeed(KEY_VALUE_R);//控制拨盘转速
	GetLspeedHfrq(KEY_VALUE_X);
	GetCutprediction(KEY_VALUE_E);//取消预判
}

void KeyStatusReset(void)
{
    uint8_t i=0;
    for(i=0;i<16;i++){
        KeyValue[i] = 0;//恢复按键状态为未按下
    }
    //注：仅对键盘有关的标志清零复位，对鼠标的无效！！！
    KeyMouseFlag.movfb = 0;
    KeyMouseFlag.movlr = 0; 
	KeyMouseFlag.movrt = 0;
	KeyMouseFlag.superstate = 0;
}
static void ServoOpenOrNot(uint8_t Key_B_State)
{    
    static uint8_t Last_B_State=0;
    if( (Key_B_State == 1) && (Last_B_State == 0) ){//提取按键下降沿
        KeyMouseFlag.servo = (KeyMouseFlag.servo == 1) ? 0 : 1;
    }
    Last_B_State = Key_B_State;
}
static void TopStateGet(uint8_t Key_C_State)
{
    static uint8_t Last_C_State=0;
    if( (Key_C_State == 0) && (Last_C_State == 1) ){//提取按键下降沿
        KeyMouseFlag.corgi = (KeyMouseFlag.corgi == 1) ? 0 : 1;
    }
    Last_C_State = Key_C_State;
}


static void FireShootSpeed(uint8_t Key_F_State)
{     
	static uint8_t Last_F_State=0;
    if( (Key_F_State == 0) && (Last_F_State == 1) ){//提取按键下降沿
        KeyMouseFlag.shootspeed = (KeyMouseFlag.shootspeed == 1) ? 0 : 1;
    }
    Last_F_State = Key_F_State;
}

static void GetPokerSpeed(uint8_t Key_R_State)
{    
	static uint8_t Last_R_State=0;
    if( (Key_R_State == 0) && (Last_R_State == 1) ){//提取按键下降沿
        KeyMouseFlag.shootfrq = (KeyMouseFlag.shootfrq == 1) ? 0 : 1;
    }
    Last_R_State = Key_R_State;
}

static void GetLspeedHfrq(uint8_t Key_X_State)
{    
    static uint8_t Last_X_State=0;
    
    if( (Key_X_State == 0) && (Last_X_State == 1) ){//提取按键下降沿
        KeyMouseFlag.LspeedHfrq = (KeyMouseFlag.LspeedHfrq == 1) ? 0 : 1;
    }
    Last_X_State = Key_X_State;
}

static void GetCutprediction(uint8_t Key_E_State)
{    
	static uint8_t Last_E_State=0;
    if( (Key_E_State == 0) && (Last_E_State == 1) ){//提取按键下降沿
        KeyMouseFlag.prediction = (KeyMouseFlag.prediction == 1) ? 0 : 1;
    }
    Last_E_State = Key_E_State;
}