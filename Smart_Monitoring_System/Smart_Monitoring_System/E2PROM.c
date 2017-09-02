#include "I2C.h"
#include "E2PROM.h"

/*********************************************
**函数功能：向任意地址写入数据
**输入：unsigned char add 写入数据的地址（0x00—0xFF ） 
**      unsigned char dat 写入具体的数据值
**输出：void
*********************************************/
void Write_E2PROM_Add(unsigned char add,unsigned char dat)
{
	I2C_start();
	WriteByte(0xa0);//发送从设备地址
	response();//等待从设备响应
	WriteByte(add);//发送芯片内地址
	response();//等待从设备响应
	WriteByte(dat);//发送数据
	response();//等待从设备响应
	I2C_stop();
}

/***********************************************
**函数功能：读出该地址存储的数据值
**输入：unsigned char add 读数据的地址
**输出：dat 该地址的存储的数据值
***********************************************/
unsigned char Read_E2PROM_Add(unsigned char add)
{
	unsigned char dat;
	I2C_start();
	WriteByte(0xa0);//发送从设备地址 写操作
	response();//等待从设备响应
	WriteByte(add);//发送芯片内地址
	response();//等待从设备响应
	I2C_start();
	WriteByte(0xa1);//发送从设备地址 读操作
	response();//等待从设备响应
	dat = ReadByte();//获取数据
	noresponse();
	I2C_stop();	
	return dat;
}