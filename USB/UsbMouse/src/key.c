/******************************************************************
   本程序只供学习使用，未经作者许可，不得用于其它任何用途

        欢迎访问我的USB专区：http://group.ednchina.com/93/
        欢迎访问我的blog：   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        感谢PCB赞助商——电子园： http://bbs.cepark.com/

KEY.C  file

作者：电脑圈圈
建立日期: 2008.06.27
修改日期: 2008.06.27
版本：V1.2
版权所有，盗版必究。
Copyright(C) 电脑圈圈 2008-2018
All rights reserved            
*******************************************************************/


#include "include/Key.h"
#include "include/MyType.h"
#include "include/config.h"

volatile uint8 idata KeyCurrent,KeyOld,KeyNoChangedTime;
volatile uint8 idata KeyPress;
volatile uint8 idata KeyDown,KeyUp,KeyLast;

volatile uint8 KeyCanChange;

/********************************************************************
函数功能：定时器0初始化，用来做键盘扫描。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void InitTimer0(void)
{
 TMOD&=0xF0;
 TMOD|=0x01;
 ET0=1;
 TR0=1;
}
/*******************************************************************/


/********************************************************************
函数功能：键盘初始化
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void InitKeyboard(void)
{
 KeyIO=0xFF;	  //键盘对应的口设置为输入状态
 KeyPress=0;	      //无按键按下
 KeyNoChangedTime=0;
 KeyOld=0;
 KeyCurrent=0;
 KeyLast=0;
 KeyDown=0;
 KeyUp=0;
 InitTimer0();		  //初始化定时器
 KeyCanChange=1;	  //允许键值改变
}
/*******************************************************************/

/********************************************************************
函数功能：定时器0中断处理。
入口参数：无。
返    回：无。
备    注：22.1184M晶体约5ms中断一次。
********************************************************************/
void Timer0Isr(void) interrupt 1
{ 
//定时器0重装，定时间隔为5ms，加15是为了修正重装所花费时间
//这个值可以通过软件仿真来确定，在这里设置断点，调整使两次运行
//时间差刚好为5ms即可。
 TH0=(65536-Fclk/1000/12*5+15)/256;	 
 TL0=(65536-Fclk/1000/12*5+15)%256;   // 

 if(!KeyCanChange)return;     //如果正在处理按键，则不再扫描键盘 

 //开始键盘扫描
 //保存按键状态到当前按键情况
 //KeyCurrent总共有8个bit
 //当某个开关按下时，对应的bit为1
 KeyCurrent=GetKeyValue(); //读取键值，GetKeyValue()其实是个宏，不是函数，
                           //这里故意写成函数的样子，美观。它的定义在
                           //key.h文件中

 if(KeyCurrent!=KeyOld)  //如果两次值不等，说明按键情况发生了改变
 {
  KeyNoChangedTime=0;       //键盘按下时间为0
  KeyOld=KeyCurrent;        //保存当前按键情况
  return;  //返回
 }
 else
 {
  KeyNoChangedTime++;	     //按下时间累计
  if(KeyNoChangedTime>=1)	 //如果按下时间足够
  {
	  KeyNoChangedTime=1;
	  KeyPress=KeyOld;      //保存按键
	  KeyDown|=(~KeyLast)&(KeyPress); //求出新按下的键
	  KeyUp|=KeyLast&(~KeyPress);     //求出新释放的键
	  KeyLast=KeyPress;		         //保存当前按键情况
  }
 }
}
/*******************************************************************/


