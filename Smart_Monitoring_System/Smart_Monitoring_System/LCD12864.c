/******************************************
**程序名称:LCD12864液晶屏显示
**程序功能：
**程序作者：李松泽
**创建时间 ：2014.4.4
**程序说明：
******************************************/
#include <reg52.h>
#include "LCD12864.h"
#include "delay.h"
#define uchar unsigned char
#define uint  unsigned int	 
sbit RS=P0^5;
sbit RW=P0^6;
sbit EN=P0^7;	//液晶的三个控制端 


/******************************************
**函数名称:void read_busy()
**功能描述：读忙标志 
**输入:
**输出：
**作者：李松泽
**说明:读忙标志 最高位为1则busy，不能进行读写操作 
******************************************/
//void read_busy()
//{
//	RS=0;
//	RW=1;
//	EN=1;	//看时序 
//	while(P2 & 0x80); 
//	EN=0;
//}
/******************************************
**函数名称:void write_LCD_command(uchar value)
**功能描述：写命令函数
**输入:uchar value
**输出：
**作者：李松泽
**说明:
******************************************/
void write_LCD_command(uchar value)
{
	//read_busy();	//每次读写都要进行读忙标志 
	RS=0;
	RW=0;
	delayus(200);
	EN=1;		//EN从1--0锁存数据 
	P2=value;
	delayus(200);
	EN=0;
}
/******************************************
**函数名称:void write_LCD_data(uchar value)
**功能描述：写数据函数 
**输入:uchar value
**输出：
**作者：李松泽
**说明:
******************************************/
void write_LCD_data(uchar value)
{
	//read_busy();
	RS=1;
	RW=0;
	delayus(200);
	EN=1;	
	P2=value;
	delayus(200);
	EN=0;
}

/******************************************
**函数名称:void init_LCD()
**功能描述：8位并口方式LCD1864初始化函数 
**输入:
**输出：
**作者：李松泽
**说明:
******************************************/
void init_LCD()	
{
	delayus(4000);			//等待时间>40ms 
	write_LCD_command(0x30);//功能设定：8位数据、基本指令操作 
	delayus(100);				//等待时间>100us 
	write_LCD_command(0x30);//功能设定：8位数据、基本指令操作 
	delayus(37);				//等待时间>37us 
	write_LCD_command(0x0C);//显示设定：整体显示、游标关、不反白 
	delayus(100);				// 等待时间>100us 
	write_LCD_command(0x01);//清屏指令 
	delayus(10000);			//等待时间>10ms 
	write_LCD_command(0x06);//进入点设定：地址指针加1 
}
void LCD_display(unsigned char add,char *hz)
{
    write_LCD_command(add);
    delayus(5);
    while(*hz!='\0')
    {
        write_LCD_data(*hz);
        hz++;
        delayus(100);
    }
}
