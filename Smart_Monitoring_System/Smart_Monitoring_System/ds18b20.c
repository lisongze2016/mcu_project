/******************************************
**程序名称:ds18b20的温度显示
**程序功能：
**程序作者：李松泽
**创建时间 ：2014.3.22
**程序说明：
******************************************/
#include <reg52.h>
#include "delay.h"
#include "ds18b20.h"
#define uchar unsigned char
#define uint unsigned int
sbit DQ=P3^6;
uint tvalue;
bit tflag;

/******************************************
**函数名称:void ds18b20rst()
**功能描述：ds18b20初始化
**输入:
**输出：
**作者：李松泽
******************************************/
void ds18b20rst()
{
    DQ=1;
	  delayms(5);
	  DQ=0;
	  delayus(500);
	  DQ=1;
	  delayus(60);
}

/******************************************
**函数名称: uchar ds18b20read()
**功能描述：读ds18b20数据
**输入:
**输出：dat
**作者：李松泽
******************************************/
uchar ds18b20read()
{
    uchar i=0;
	  uchar dat=0;//用dat把读出来的数据存起来
    for(i=0;i<8;i++)
	  {
        DQ=0;
			  dat=dat>>1;//右移，还起到延时1us
			  DQ=1;
			  if(DQ)
					dat|=0x80;
				  delayus(15);
    }
		return(dat);	
}

/******************************************
**函数名称: uchar ds18b20write(uchar com)
**功能描述：写ds18b20数据
**输入:
**输出：dat
**作者：李松泽
******************************************/
void ds18b20write(uchar com)
{
    uchar i=0;
	  for(i=0;i<8;i++)
	  {
        DQ=0;
			  DQ=com&0x01;
			  delayus(15);
			  DQ=1;
			  com=com>>1;  
    }
}
/******************************************
**函数名称: uint read ds18b20temp()
**功能描述：读取ds18b20温度值
**输入:
**输出：
**作者：李松泽
******************************************/
uint read_ds18b20temp()//注意这里读取16数据不能用uchar
{
    uchar a,b;
    ds18b20rst();
	  ds18b20write(0xcc);//跳过读ROM
	  ds18b20write(0x44);//发送温度转换指令
	  ds18b20rst();
    ds18b20write(0xcc);//跳过读ROM
    ds18b20write(0xbe);
    a=ds18b20read();//读18b20低八位
	  b=ds18b20read();//读18b20高八位
	  tvalue=b;
	  tvalue=tvalue<<8;
	  tvalue|=a;
	  if(tvalue<0xffff)
			tflag=0;
		else
		{
      tvalue=~tvalue+1;
			tflag=1;
    }
	  tvalue=tvalue*0.625;//扩大10倍
		return(tvalue);    	
}




