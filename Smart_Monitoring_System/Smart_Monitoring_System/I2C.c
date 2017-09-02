#include <reg52.h>
#include "I2C.h"
#include "delay.h"

sbit SCL = P0^2;
sbit SDA = P0^3;
/**************************************
**函数功能：总线初始化
**输入：void
**输出：void
**************************************/
void I2C_init()
{
	SCL = 1;
	delayus(5);
	SDA = 1;
	delayus(5);
}

/*************************************
**函数功能：起始信号
**输入：void
**输出：void
*************************************/
void I2C_start()
{
	SDA = 1;
	SCL = 1;
	delayus(5);//延时大于4.7us
	SDA = 0;
	delayus(5);//延时大于4us
	SCL = 0;
}

/*************************************
**函数功能：应答信号
**输入：void
**输出：void
**************************************/
void response()
{
	unsigned int i;
	SDA = 0;
	SCL = 1;
	delayus(5);//延时大于4us
	while((SDA==1)&&(i<255))i++;//若没有收到从机应答，默认从机收到数据不再等待应答信号	
	SCL = 0;
	delayus(5);
}
/*************************************
**函数功能：非应答信号
**输入：void
**输出：void
**************************************/
void noresponse()
{
	SDA = 1;
	SCL = 1;
	delayus(5);
	SCL = 0;
	SDA = 1;
}
/*************************************
**函数功能：写1byte数据
**输入：unsigned char date
**输出：void
**************************************/
void WriteByte(unsigned char dat)
{
	unsigned char i,temp;
	temp = dat;
	for(i = 0; i < 8; i++)
	{
		temp = temp<<1;
		SCL = 0;
		delayus(5);
		SDA = CY;
		delayus(5);
		SCL = 1;
		delayus(5); 
	}
	SCL = 0;//拉低SCL，为下次数据传输做好准备
	delayus(5);
	SDA = 1;//释放SDA总线
	delayus(5);
}
/*************************************
**函数功能：读1byte数据
**输入：void
**输出：unsigned char temp
**************************************/
unsigned char ReadByte()
{
	unsigned char i,temp;
	SCL = 0;
	delayus(5);
	SDA = 1;
	for(i = 0; i < 8; i++)
	{
		SCL = 1;
		delayus(5);
		temp = (temp<<1) | SDA;
		SCL = 0;
		delayus(5);
	}
	delayus(5);
	return temp;
}

/*************************************
**函数功能：终止信号
**输入：void
**输出：void
**************************************/
void I2C_stop()
{
	SDA = 0;
	SCL = 1;
	delayus(5);//延时大于4us
	SDA = 1;
	delayus(5);//延时大于4.7us
	SCL = 0;
}



