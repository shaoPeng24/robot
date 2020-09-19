#ifndef REFEREE_SEND_H
#define REFEREE_SEND_H

#include "main.h"
#include "referee.h"

/* 
	学生机器人间通信 cmd_id 0x0301，内容 ID:0x0200~0x02FF
	交互数据 机器人间通信：0x0301。
	发送频率：上限 10Hz  

	字节偏移量 	大小 	说明 			备注 
	0 			2 		数据的内容 ID 	0x0200~0x02FF 
										可以在以上 ID 段选取，具体 ID 含义由参赛队自定义 
	
	2 			2 		发送者的 ID 	需要校验发送者的 ID 正确性， 
	
	4 			2 		接收者的 ID 	需要校验接收者的 ID 正确性，
										例如不能发送到敌对机器人的ID 
	
	6 			n 		数据段 			n 需要小于 113 

*/
typedef __packed struct 
{ 
	uint8_t data[2]; //数据段,n需要小于113
} robot_interactive_data_t;

typedef __packed struct
{ 
	uint8_t graphic_name[3]; //图形名,在删除，修改等操作中，作为客户端的索引。
	uint32_t operate_tpye:3; //图形操作：0：空操作；1：增加；2：修改；3：删除；
	uint32_t graphic_tpye:3; //图形类型：0：直线；1：矩形；2：整圆；3：椭圆；4：圆弧；
	uint32_t layer:4; //图层数，0~9
	uint32_t color:4; //颜色：0：红蓝主色；1：黄色；2：绿色；3：橙色；4：紫红色；5：粉色；6：青色；7：黑色；8：白色；
	uint32_t start_angle:9; //：起始角度，单位：°，范围[0,360]；
	uint32_t end_angle:9; //终止角度，单位：°，范围[0,360]。
	uint32_t width:10; //线宽；
	uint32_t start_x:11; //起点x坐标；
	uint32_t start_y:11; //起点y坐标。
	uint32_t radius:10; //字体大小或者半径；
	uint32_t end_x:11; //终点x坐标；
	uint32_t end_y:11; //终点y坐标。
} graphic_data_struct_t;


/* 客户端删除图形  机器人间通信： 0x0301 内容id：0x0100*/
typedef __packed struct
{ 
	uint8_t operate_tpye; 
	uint8_t layer; 
} graphic_delete_t;


/*客户端绘制一个图形 机器人间通信： 0x0301 内容id：0x0101*/
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
}ext_client_custom_graphic_single_t;

/*客户端绘制二个图形 机器人间通信： 0x0301 内容id：0x0102*/
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[2];
}ext_client_custom_graphic_double_t;

/*客户端绘制五个图形 机器人间通信： 0x0301 内容id：0x0103*/
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[5];
}ext_client_custom_graphic_five_t;

/*客户端绘制七个图形 机器人间通信： 0x0301 内容id：0x0104*/
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[7];
}ext_client_custom_graphic_seven_t;

/*客户端绘制字符 机器人间通信： 0x0301 内容id： 0x0110*/
typedef __packed struct
{
graphic_data_struct_t grapic_data_struct;
uint8_t data[30];
} ext_client_custom_character_t;

typedef __packed struct //0x0301
{//机器人之间数据接收信息
/*	数据段的 内容ID：（注意与cmd_id区分）
	0x0200~0x02FF	6+n	己方机器人间通信
	0x0100	6+2		客户端删除图形
	0x0101	6+15	客户端绘制一个图形
	0x0102	6+30	客户端绘制二个图形
	0x0103	6+75	客户端绘制五个图形
	0x0104	6+105	客户端绘制七个图形
	0x0110	6+45	客户端绘制字符图形
*/
	__packed struct
	{
	  uint8_t SOF;
	  uint16_t data_length;
	  uint8_t seq;
	  uint8_t CRC8;
	} frame_header_struct_t;
	
	uint16_t cmd_id;
	
	uint16_t data_cmd_id; //数据段的内容ID
	uint16_t sender_ID; //发送者的ID
	uint16_t receiver_ID; //接收者的ID
	graphic_data_struct_t grap[7];//数据段，最大113
	
	uint16_t crc16;
} graphic_add_t;


void UI_init(void);
void UI_show(void);














#endif
