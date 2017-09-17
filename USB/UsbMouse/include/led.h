/******************************************************************
   本程序只供学习使用，未经作者许可，不得用于其它任何用途
			
        欢迎访问我的USB专区：http://group.ednchina.com/93/
        欢迎访问我的blog：   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        感谢PCB赞助商——电子园： http://bbs.cepark.com/

Led.h file

作者：电脑圈圈
建立日期: 2008.06.27
修改日期: 2008.06.27
版本：V1.1
版权所有，盗版必究。
Copyright(C) 电脑圈圈 2008-2018
All rights reserved            
*******************************************************************/

#ifndef __LED_H__
#define __LED_H__
//全部LED
#define LEDs  P2
//单个LED，LED1~LED7
//注意“^”这个操作符，只有跟sbit搭配时才表示定义一个位，
//C语言中，“^”表示异或操作，不要在程序中直接使用 “^”来表示某一位。
sbit LED1  =     LEDs^0;
sbit LED2  =     LEDs^1;
sbit LED3  =     LEDs^2;
sbit LED4  =     LEDs^3;
sbit LED5  =     LEDs^4;
sbit LED6  =     LEDs^5;
sbit LED7  =     LEDs^6;
sbit LED8  =     LEDs^7;
//点亮某个LED
#define OnLed1()  LED1=0
#define OnLed2()  LED2=0
#define OnLed3()  LED3=0
#define OnLed4()  LED4=0
#define OnLed5()  LED5=0
#define OnLed6()  LED6=0
#define OnLed7()  LED7=0
#define OnLed8()  LED8=0
//关闭某个LED
#define OffLed1() LED1=1
#define OffLed2() LED2=1
#define OffLed3() LED3=1
#define OffLed4() LED4=1
#define OffLed5() LED5=1
#define OffLed6() LED6=1
#define OffLed7() LED7=1
#define OffLed8() LED8=1

#endif