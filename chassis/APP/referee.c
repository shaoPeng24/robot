#include "referee.h"

#include "string.h"
#include "usart.h"

#define BLUE  0
#define RED   1

/*****************系统数据定义**********************/
ext_game_state_t       					GameState;								//0x0001
ext_game_result_t            		GameResult;								//0x0002
ext_game_robot_HP_t							GameRobotHP;							//0x0003
ext_dart_status_t								DartState;								//0x0004

ext_event_data_t        				EventData;								//0x0101
ext_supply_projectile_action_t	SupplyProjectileAction;		//0x0102
ext_referee_warning_t						RefereeWarning;						//0x0104
ext_dart_remaining_time_t				DartRemaingTime;					//0x0105

ext_game_robot_state_t			  	GameRobotState;						//0x0201
ext_power_heat_data_t		  			PowerHeatData;						//0x0202
ext_game_robot_pos_t						GameRobotPos;							//0x0203
ext_buff_musk_t									BuffMusk;									//0x0204
aerial_robot_energy_t						AerialRobotEnergy;				//0x0205
ext_robot_hurt_t								RobotHurt;								//0x0206
ext_shoot_data_t								ShootData;								//0x0207

xFrameHeader              FrameHeader;		//发送帧头信息
/****************************************************/

uint8_t 	Self_ID;//当前机器人的ID
uint16_t 	SelfClient_ID;//发送者机器人对应的客户端ID



int referee_data_solve(uint8_t *ReadFromUsart)
{
	uint16_t referee_length;//统计一帧数据长度 
	
	int CmdID = 0;//数据命令码解析
	
	if (ReadFromUsart == NULL)
	{
		return -1;
	}
	
	memcpy(&FrameHeader,ReadFromUsart,LEN_HEADER);
	
	if(ReadFromUsart[0] == REFEREE_FRAME_HEADER)
	{
		if(Verify_CRC8_Check_Sum( ReadFromUsart, LEN_HEADER ) == 1)
		{
			referee_length = ReadFromUsart[1] + LEN_HEADER + LEN_CMDID + LEN_TAIL;
			
			if(Verify_CRC16_Check_Sum(ReadFromUsart,referee_length) == 1)
			{
				CmdID = (ReadFromUsart[6] << 8 | ReadFromUsart[5]);
					//解析数据命令码,将数据拷贝到相应结构体中(注意拷贝数据的长度)
				switch(CmdID)
				{
					case ID_game_state:        			//0x0001
						memcpy(&GameState, (ReadFromUsart + DATA), LEN_game_state);
					break;
					
					case ID_game_result:          		//0x0002
						memcpy(&GameResult, (ReadFromUsart + DATA), LEN_game_result);
					break;
					
					case ID_game_robot_HP:       //0x0003
						memcpy(&GameRobotHP, (ReadFromUsart + DATA), LEN_game_robot_HP);
					break;
					
					case ID_dart_state:       //0x0004
						memcpy(&DartState, (ReadFromUsart + DATA), LEN_dart_state);
					break;
					 				
					
					case ID_event_data:    				//0x0101
						memcpy(&EventData, (ReadFromUsart + DATA), LEN_event_data);
					break;
					
					case ID_supply_projectile_action:   //0x0102
						memcpy(&SupplyProjectileAction, (ReadFromUsart + DATA), LEN_supply_projectile_action);
					break;
					
					case ID_referee_warning:   //0x0104
						memcpy(&RefereeWarning, (ReadFromUsart + DATA), LEN_referee_warning);
					break;
					
					case ID_dart_remaining_time:   //0x0105
						memcpy(&DartRemaingTime, (ReadFromUsart + DATA), LEN_dart_remaining_time);
					break;
					
					
					case ID_game_robot_state:      		//0x0201
						memcpy(&GameRobotState, (ReadFromUsart + DATA), LEN_game_robot_state);
					break;
					
					case ID_power_heat_data:      		//0x0202
						memcpy(&PowerHeatData, (ReadFromUsart + DATA), LEN_power_heat_data);
					break;
					
					case ID_game_robot_pos:      		//0x0203
						memcpy(&GameRobotPos, (ReadFromUsart + DATA), LEN_game_robot_pos);
					break;
					
					case ID_buff_musk:      			//0x0204
						memcpy(&BuffMusk, (ReadFromUsart + DATA), LEN_buff_musk);
					break;
					
					case ID_aerial_robot_energy:      	//0x0205
						memcpy(&AerialRobotEnergy, (ReadFromUsart + DATA), LEN_aerial_robot_energy);
					break;
					
					case ID_robot_hurt:      			//0x0206
						memcpy(&RobotHurt, (ReadFromUsart + DATA), LEN_robot_hurt);
					break;
					
					case ID_shoot_data:      			//0x0207
						memcpy(&ShootData, (ReadFromUsart + DATA), LEN_shoot_data);
					break;
				}
			}		
		}
				//首地址加帧长度,指向CRC16下一字节,用来判断是否为0xA5,用来判断一个数据包是否有多帧数据
		if(*(ReadFromUsart + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5)
		{
			//如果一个数据包出现了多帧数据,则再次读取
			referee_data_solve(ReadFromUsart + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
		}	
	}		
	return 0;
}




int is_red_or_blue(void)
{
		Self_ID = GameRobotState.robot_id;
		
		if(GameRobotState.robot_id > 100)
		{
			return BLUE;
		}
		else 
		{
			return RED;
		}
}

void determine_ID(void)
{
	int Color = is_red_or_blue();
	if(Color == BLUE)
	{
		SelfClient_ID = 0x0110 + (Self_ID-100);//计算客户端ID
	}
	else if(Color == RED)
	{
		SelfClient_ID = 0x0100 + Self_ID;//计算客户端ID
	}
}

uint8_t get_robot_id(void)
{
    return GameRobotState.robot_id;
}

void get_chassis_power_and_buffer(float *power, float *buffer,uint16_t *powmax)
{
    *power = PowerHeatData.chassis_power;
    *buffer = PowerHeatData.chassis_power_buffer;
		*powmax = GameRobotState.max_chassis_power;

}

void get_shoot_heat0_limit_and_heat0(uint16_t *heat0_limit, uint16_t *heat0)
{
    *heat0_limit = GameRobotState.shooter_heat0_cooling_limit;
    *heat0 = PowerHeatData.shooter_heat0;
	
}

void get_shoot_heat1_limit_and_heat1(uint16_t *heat1_limit, uint16_t *heat1)
{
    *heat1_limit = GameRobotState.shooter_heat1_cooling_limit;
    *heat1 = PowerHeatData.shooter_heat1;
}

void get_shoot_data(uint8_t *bullet_freq,	float *bullet_speed, uint8_t *bullet_speedlimit)
{
    *bullet_freq = ShootData.bullet_freq;
    *bullet_speed = ShootData.bullet_speed;
		*bullet_speedlimit = GameRobotState.shooter_heat0_speed_limit;
}


const unsigned char CRC8_INIT = 0xff;
const unsigned char CRC8_TAB[256] =
{
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};
uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] =
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/**
  * @brief          calculate the crc8  
  * @param[in]      pch_message: data
  * @param[in]      dw_length: stream length = data + checksum
  * @param[in]      ucCRC8: init CRC8
  * @retval         calculated crc8
  */
/**
  * @brief          计算CRC8
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @param[in]      ucCRC8:初始CRC8
  * @retval         计算完的CRC8
  */
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned dwLength,unsigned char ucCRC8)
{
		unsigned char ucIndex;
		while (dwLength--)
		{
				ucIndex = ucCRC8^(*pchMessage++);
				ucCRC8 = CRC8_TAB[ucIndex];
		}
		return(ucCRC8);
}
/**
  * @brief          CRC8 verify function  
  * @param[in]      pch_message: data
  * @param[in]      dw_length:stream length = data + checksum
  * @retval         true of false
  */
/**
  * @brief          CRC8校验函数
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @retval         真或者假
  */
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
		unsigned char ucExpected = 0;
		if ((pchMessage == 0) || (dwLength <= 2)) 
				return 0;
		ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT);
		return ( ucExpected == pchMessage[dwLength-1] );
}

/**
  * @brief          append CRC8 to the end of data
  * @param[in]      pch_message: data
  * @param[in]      dw_length:stream length = data + checksum
  * @retval         none
  */
/**
  * @brief          添加CRC8到数据的结尾
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @retval         none
  */
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
		unsigned char ucCRC = 0;
		if ((pchMessage == 0) || (dwLength <= 2))
				return;
		ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
		pchMessage[dwLength-1] = ucCRC;
}

/**
  * @brief          calculate the crc16  
  * @param[in]      pch_message: data
  * @param[in]      dw_length: stream length = data + checksum
  * @param[in]      wCRC: init CRC16
  * @retval         calculated crc16
  */
/**
  * @brief          计算CRC16
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @param[in]      wCRC:初始CRC16
  * @retval         计算完的CRC16
  */
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
		uint8_t chData;
		if (pchMessage == NULL)
		{
				return 0xFFFF;
		}
		while(dwLength--)
		{
				chData = *pchMessage++;
				(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) &0x00ff];
		}
		return wCRC;
}

/**
  * @brief          CRC16 verify function  
  * @param[in]      pch_message: data
  * @param[in]      dw_length:stream length = data + checksum
  * @retval         true of false
  */
/**
  * @brief          CRC16校验函数
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @retval         真或者假
  */
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
		uint16_t wExpected = 0;
		if ((pchMessage == NULL) || (dwLength <= 2))
		{
				return 0;
		}
		wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
		return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) ==pchMessage[dwLength - 1]);
}
/**
  * @brief          append CRC16 to the end of data
  * @param[in]      pch_message: data
  * @param[in]      dw_length:stream length = data + checksum
  * @retval         none
  */
/**
  * @brief          添加CRC16到数据的结尾
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @retval         none
  */
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)
{
		uint16_t wCRC = 0;
		if ((pchMessage == NULL) || (dwLength <= 2))
		{
				return;
		}
		wCRC = Get_CRC16_Check_Sum ( (uint8_t *)pchMessage, dwLength-2, CRC_INIT );
		pchMessage[dwLength-2] = (uint8_t)(wCRC & 0x00ff);
		pchMessage[dwLength-1] = (uint8_t)((wCRC >> 8)& 0x00ff);
}
