#ifndef __KEYMOUSE_H_
#define __KEYMOUSE_H_

#include "rc.h"

/*Driver_KeyMouse Command Define*/
//------------------------------------------------------------------------------|
//按键值：  Q       |   W       |   E       |   R       |           |           |
//对应位：  bit[6]  |   bit[0]  |   bit[7]  |   bit[8]  |           |           |
//------------------------------------------------------------------------------|
//按键值：  A       |   S       |   D       |   F       |   G       |           |
//对应位：  bit[2]  |   bit[1]  |   bit[3]  |   bit[9]  |   bit[10] |           |
//------------------------------------------------------------------------------|
//按键值：  Shift   |   Z       |   X       |   C       |   V       |   B       |        
//对应位：  bit[4]  |   bit[11] |   bit[12] |   bit[13] |   bit[14] |   bit[15] |       
//------------------------------------------------------------------------------|
//按键值：  Ctrl    |           |           |           |           |           |
//对应位：  bit[5]  |           |           |           |           |           |
//------------------------------------------------------------------------------|


#define KEY_VALUE_W              KeyValue[0]         //键鼠对应按键值的状态，1表示该按键按下 0未按下
#define KEY_VALUE_S              KeyValue[1]
#define KEY_VALUE_A              KeyValue[2]
#define KEY_VALUE_D              KeyValue[3]

#define KEY_VALUE_SHIFT          KeyValue[4]
#define KEY_VALUE_CTRL           KeyValue[5]

#define KEY_VALUE_Q              KeyValue[6]
#define KEY_VALUE_E              KeyValue[7]
#define KEY_VALUE_R              KeyValue[8] 

#define KEY_VALUE_F              KeyValue[9]
#define KEY_VALUE_G              KeyValue[10]

#define KEY_VALUE_Z              KeyValue[11]
#define KEY_VALUE_X              KeyValue[12]
#define KEY_VALUE_C              KeyValue[13]
#define KEY_VALUE_V              KeyValue[14]
#define KEY_VALUE_B              KeyValue[15]

#define KEY_PRESSED_OFFSET_W            ((uint16_t)1 << 0)
#define KEY_PRESSED_OFFSET_S            ((uint16_t)1 << 1)
#define KEY_PRESSED_OFFSET_A            ((uint16_t)1 << 2)
#define KEY_PRESSED_OFFSET_D            ((uint16_t)1 << 3)
#define KEY_PRESSED_OFFSET_SHIFT        ((uint16_t)1 << 4)
#define KEY_PRESSED_OFFSET_CTRL         ((uint16_t)1 << 5)
#define KEY_PRESSED_OFFSET_Q            ((uint16_t)1 << 6)
#define KEY_PRESSED_OFFSET_E            ((uint16_t)1 << 7)
#define KEY_PRESSED_OFFSET_R            ((uint16_t)1 << 8)
#define KEY_PRESSED_OFFSET_F            ((uint16_t)1 << 9)
#define KEY_PRESSED_OFFSET_G            ((uint16_t)1 << 10)
#define KEY_PRESSED_OFFSET_Z            ((uint16_t)1 << 11)
#define KEY_PRESSED_OFFSET_X            ((uint16_t)1 << 12)
#define KEY_PRESSED_OFFSET_C            ((uint16_t)1 << 13)
#define KEY_PRESSED_OFFSET_V            ((uint16_t)1 << 14)
#define KEY_PRESSED_OFFSET_B            ((uint16_t)1 << 15)

/*Driver_KeyMouse Param Class Define*/
///*键鼠操作*/
typedef struct
{
    int8_t  movfb;          //左右移动          1：右    0：无动作    -1：左
    int8_t  movlr;          //前后移动         -1：前    0：无动作     1：后
    int8_t  movrt;			//左右旋转		   -1：右	 0：无动作	   1：左
	
    int8_t  servo;          //弹仓盖动作标志    1：开    0：关
	int8_t  top;         //小陀螺状态   1:开启   0：关闭
	int8_t  corgi;				//扭腰				1:开启   0：关闭
	int8_t  superstate; //超电状态      
	int8_t	fireshoot;		//摩擦轮开启		1：开	 0：关
	int8_t  shootfrq  ;	//速度环速度			1；高速	 0：低速
	int8_t  shootspeed;		//射速				1：低	 0：高
	int8_t	LspeedHfrq;
	int8_t	prediction;  //预判					1：没预判  0：有预判

}_KeyMouseFlagStruct;

extern _KeyMouseFlagStruct KeyMouseFlag;//键鼠所对应的动作标志 
extern uint8_t KeyValue[16];//键鼠对应按键值的状态

void GetKeyValue(void);
void KeyStatusReset(void);
#endif
