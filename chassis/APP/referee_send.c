#include "referee_send.h"

#include "bsp_usart.h"
#include "referee_usart.h"


graphic_add_t graphic_add;

//	uint8_t graphic_name[3]; //图形名,在删除，修改等操作中，作为客户端的索引。
//	uint32_t operate_tpye:3; //图形操作：0：空操作；1：增加；2：修改；3：删除；
//	uint32_t graphic_tpye:3; //图形类型：0：直线；1：矩形；2：整圆；3：椭圆；4：圆弧；
//	uint32_t layer:4; //图层数，0~9
//	uint32_t color:4; //颜色：0：红蓝主色；1：黄色；2：绿色；3：橙色；4：紫红色；5：粉色；6：青色；7：黑色；8：白色；
//	uint32_t start_angle:9; //：起始角度，单位：°，范围[0,360]；
//	uint32_t end_angle:9; //终止角度，单位：°，范围[0,360]。
//	uint32_t width:10; //线宽；
//	uint32_t start_x:11; //起点x坐标；
//	uint32_t start_y:11; //起点y坐标。
//	uint32_t radius:10; //字体大小或者半径；
//	uint32_t end_x:11; //终点x坐标；
//	uint32_t end_y:11; //终点y坐标。

void UI_init(void)
{
	graphic_add.frame_header_struct_t.SOF=0xA5;
	graphic_add.frame_header_struct_t.data_length=111;
	graphic_add.frame_header_struct_t.seq=1;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_add.frame_header_struct_t,sizeof(graphic_add.frame_header_struct_t));
	//////////////////////////////////////////////
	graphic_add.cmd_id=0x0301;
	//////////////////////////////////////////////
	graphic_add.data_cmd_id=0x0104;
	graphic_add.sender_ID=104;
	graphic_add.receiver_ID=0x0168;
	//////////////////////////////////////////////
	graphic_add.grap[0].graphic_name[0]=0;
	graphic_add.grap[0].graphic_name[1]=0;
	graphic_add.grap[0].graphic_name[2]=1;
	
	graphic_add.grap[0].operate_tpye=1;
	graphic_add.grap[0].graphic_tpye=0;
	
	graphic_add.grap[0].layer=1;
	graphic_add.grap[0].color=3;
	graphic_add.grap[0].width=4;
	
	graphic_add.grap[0].start_x=805;
	graphic_add.grap[0].start_y=100;
	graphic_add.grap[0].end_x=805;
	graphic_add.grap[0].end_y=800;
	//////////////////////////////////////////////
	graphic_add.grap[1].graphic_name[0]=0;
	graphic_add.grap[1].graphic_name[1]=0;
	graphic_add.grap[1].graphic_name[2]=2;
	
	graphic_add.grap[1].operate_tpye=1;
	graphic_add.grap[1].graphic_tpye=0;
	
	graphic_add.grap[1].layer=1;
	graphic_add.grap[1].color=3;
	graphic_add.grap[1].width=4;
	
	graphic_add.grap[1].start_x=805;
	graphic_add.grap[1].start_y=200;
	graphic_add.grap[1].end_x=850;
	graphic_add.grap[1].end_y=200;
	//////////////////////////////////////////////
	graphic_add.grap[2].graphic_name[0]=0;
	graphic_add.grap[2].graphic_name[1]=0;
	graphic_add.grap[2].graphic_name[2]=3;
	
	graphic_add.grap[2].operate_tpye=1;
	graphic_add.grap[2].graphic_tpye=0;
	
	graphic_add.grap[2].layer=1;
	graphic_add.grap[2].color=8;
	graphic_add.grap[2].width=4;
	
	graphic_add.grap[2].start_x=805;
	graphic_add.grap[2].start_y=300;
	graphic_add.grap[2].end_x=850;
	graphic_add.grap[2].end_y=300;
	//////////////////////////////////////////////
	graphic_add.grap[3].graphic_name[0]=0;
	graphic_add.grap[3].graphic_name[1]=0;
	graphic_add.grap[3].graphic_name[2]=4;
	
	graphic_add.grap[3].operate_tpye=1;
	graphic_add.grap[3].graphic_tpye=0;
	
	graphic_add.grap[3].layer=1;
	graphic_add.grap[3].color=3;
	graphic_add.grap[3].width=4;
	
	graphic_add.grap[3].start_x=805;
	graphic_add.grap[3].start_y=400;
	graphic_add.grap[3].end_x=850;
	graphic_add.grap[3].end_y=400;
	
	//////////////////////////////////////////////
	graphic_add.grap[4].graphic_name[0]=0;
	graphic_add.grap[4].graphic_name[1]=0;
	graphic_add.grap[4].graphic_name[2]=5;
	
	graphic_add.grap[4].operate_tpye=1;
	graphic_add.grap[4].graphic_tpye=0;
	
	graphic_add.grap[4].layer=1;
	graphic_add.grap[4].color=8;
	graphic_add.grap[4].width=4;
	
	graphic_add.grap[4].start_x=805;
	graphic_add.grap[4].start_y=500;
	graphic_add.grap[4].end_x=850;
	graphic_add.grap[4].end_y=500;
	//////////////////////////////////////////////
	graphic_add.grap[5].graphic_name[0]=0;
	graphic_add.grap[5].graphic_name[1]=0;
	graphic_add.grap[5].graphic_name[2]=6;
	
	graphic_add.grap[5].operate_tpye=1;
	graphic_add.grap[5].graphic_tpye=0;
	
	graphic_add.grap[5].layer=1;
	graphic_add.grap[5].color=3;
	graphic_add.grap[5].width=4;
	
	graphic_add.grap[5].start_x=805;
	graphic_add.grap[5].start_y=600;
	graphic_add.grap[5].end_x=850;
	graphic_add.grap[5].end_y=600;
	//////////////////////////////////////////////
	graphic_add.grap[6].graphic_name[0]=0;
	graphic_add.grap[6].graphic_name[1]=0;
	graphic_add.grap[6].graphic_name[2]=7;
	
	graphic_add.grap[6].operate_tpye=1;
	graphic_add.grap[6].graphic_tpye=0;
	
	graphic_add.grap[6].layer=1;
	graphic_add.grap[6].color=8;
	graphic_add.grap[6].width=4;
	
	graphic_add.grap[6].start_x=805;
	graphic_add.grap[6].start_y=700;
	graphic_add.grap[6].end_x=850;
	graphic_add.grap[6].end_y=700;
	
//	graphic_add.crc16=get_CRC16_check_sum((uint8_t*)&graphic_add, sizeof(graphic_add) - 2,0xffff);
	
	
}

void UI_show(void)
{
	Append_CRC16_Check_Sum((uint8_t*)&graphic_add,sizeof(graphic_add));
	
	RE_usart_tx_dma_enable((uint8_t *)&graphic_add,sizeof(graphic_add));
}
