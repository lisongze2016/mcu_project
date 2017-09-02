#ifndef __GSM_H__
#define __GSM_H__

extern void serial_int(void);//串口初始化
extern void send(unsigned char *tab);//发送指令数据
extern void CLR_Buf();//清除缓存数据
extern bit hand(unsigned char *a);//判断缓存中是否含有指定的字符串
extern void Send_Value();//发送采集数据
extern void Send_Message();//发送中文短信
extern void Receive_Message();//接收信息并进行数据处理
extern void serial();//串口中断处理
extern void Receive_Message_init();
extern void Send_Wetness_Value();
#endif