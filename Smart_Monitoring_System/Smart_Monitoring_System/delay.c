/******************************************
**函数名称:void delayms(uchar x)
**功能描述：延时函数 延时x ms
**输入:uchar x
**输出：
**作者：李松泽
******************************************/
#include "delay.h"
void delayms(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
/******************************************
**函数名称:void delayus(unsigned int x)
**功能描述：延时函数 延时x ms
**输入:uchar x
**输出：
**作者：李松泽
******************************************/
void delayus(unsigned int i)
{
    while(i--);
}

