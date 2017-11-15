#ifndef __PC_H__
#define __PC_H__

#include "uart.h"

#define DataType_recv  0xaa;   //判断数据是否是由上位机发送
#define DataType_Send  0xbb;   //上传给PC的数据使用这个包头

#define PID_ACK  0x07;   //应答包
#define PID_DATA 0x02;   //数据包
#define PID_CMD  0x01;   //命令包

#define CMD_ADD  0x01;   //添加指纹
#define CMD_DEL  0x02;   //删除指纹

#define MATCH_OK 0x00;   //指纹匹配成功
#define MATCH_ERR 0x01;  //指纹匹配失败


typedef struct pc_message_pack{
	uint8_t Head;						//命令包头
	uint8_t PID;						//包标识   0x01(cmd_package),0x02(data_package),0x07(ack_package),0x08(end_package)
	uint8_t Length[2];			//包长度 = DATA_LENGTH + 2
	uint8_t *Data;          //包内容
}PC_PACK;

void Send_Match_OK(uint8_t num);
uint8_t Data_Length(uint8_t para_num, ...);
uint8_t Send_PACK(uint8_t *pack, uint8_t num);
void ASCK_DEl(uint8_t asck);
void ASCK_ADD(uint8_t asck,uint8_t num);


#endif
