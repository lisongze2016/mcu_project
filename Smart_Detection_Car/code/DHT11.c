

/****************************************
DHT11温湿度传感器
DHT=P3^7；数据输入IO口
时间 2013.4.13
****************************************/

#include <reg52.h>
#include "DHT11.h"
#include "LCD12864.h"
#define uchar unsigned char
#define uint unsigned int

sbit DHT=P1^1;
uchar flag,temp;
uchar RH,RL,TH,TL,CK;
uchar RH_data,RL_data,TH_data,TL_data,CK_data;

/*void delay_us(unsigned int i)
{
    while(i--);
}*/
/************************
约8-10us延迟函数
***********************/
void delay_us()
{
	uchar i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}
/*****************************
1ms延迟函数
*****************************/
void delay_ms(uint t)
{
	 uchar i;
	 while(t--)
	 for(i=0;i<125;i++);
}

/************************************
检测DHT11读回来的数据
*************************************/
uchar read_byte()
{
	uchar i,dat;
	for(i=0;i<8;i++)
	{
		    flag=2;
			while((DHT==0)&&flag++);
			delay_us();
			delay_us();
			delay_us();		          //大概30us
			if(DHT==1)				  
			{
				temp=1;
				flag=2;
				while((DHT==1)&&flag++);
			}
			else
				temp=0;
				dat<<=1;
				dat|=temp;		
	}
	return dat;
}
/************************************
复位和响应函数
*************************************/
void init_DHT11()
{
	DHT=0;
	delay_ms(20);	 //约20ms
	DHT=1;			 //拉高30us
	delay_us();
	delay_us();
	delay_us();
	DHT=1;
	if(DHT==0)
	{
		flag=2;
		while((DHT==0)&&flag++);
		flag=2;
		while((DHT==1)&&flag++);
		RH=read_byte();		   //湿度整数部分
		RL=read_byte();		   //湿度小数部分
		TH=read_byte();		   //温度整数部分
		TL=read_byte();		   //温度小数部分
		CK=read_byte();		   //校验码
		DHT=1;				   //传送完拉高总线
		CK_data=(RH+RL+TH+TL); //检测校验码是否正确
		if(CK==CK_data)		   //如果正确 继续下一步处理
		{
			RH_data=RH;
			RL_data=RL;
			TH_data=TH;
			TL_data=TL;
			CK_data=CK;
		}
	}
	  write_LCD_command(0x98+7);
	  //write_LCD_data('R');
	  write_LCD_data(0x30+RH_data/10%10);
	  write_LCD_data(0x30+RH_data%10);
}
