/*
* USB 鼠标
* 串口工具请用utf-8编码格式
*
* Version				Date				Author				Description
* V1.0				2008-06-27		电脑圈圈				created
* V1.2				2008-07-10		电脑圈圈
* V1.3				2017-09-17		lisongze
*/


#include <reg52.h>
#include "include/uart.h"
#include "include/key.h"
#include "include/led.h"


code uint8 HeadTable[][100]={
"********************************************************************\r\n",
"******                      USB鼠标                           ******\r\n",
"******                      AT89S52 CPU                       ******\r\n",
"******                  建立日期：",__DATE__,"                 ******\r\n",
"******                   建立时间：",__TIME__,"                   ******\r\n",
"******                    作者：lisongze                      ******\r\n",
"******                    欢迎访问作者的                      ******\r\n",
"******      BLOG：http://blog.csdn.net/songze_lee             ******\r\n",
"******      说明：优化电脑圈圈的USB鼠标                       ******\r\n",
"******                  请按K1-K8分别进行测试                 ******\r\n",
"******    K1:光标左移  K2:光标右移  K3:光标上移 K4:光标下移   ******\r\n",
"******    K5:滚轮下滚  K6:滚轮上滚  K7:鼠标左键 K8:鼠标右键   ******\r\n",
"********************************************************************\r\n",
};


/********************************************************************
函数功能：根据按键情况返回报告的函数。
入口参数：无。
返    回：无。
备    注：无。					
********************************************************************/
void SendReport(void)
{
 //我们不需要KEY1~KEY6按键改变的信息，所以先将它们清0
 KeyUp &=~(KEY1|KEY2|KEY3|KEY4|KEY5|KEY6);
 KeyDown &=~(KEY1|KEY2|KEY3|KEY4|KEY5|KEY6);
 
 //如果有按键按住，并且不是KEY7、KEY8（左、右键）
 //或者KEY7、KEY8任何一个键有变动的话，则需要返回报告
 if((KeyPress&(~(KEY7|KEY8)))||KeyUp||KeyDown)
 {
		if(KeyPress & KEY1) //如果KEY1按住，则光标需要左移，即X轴为负值。
		{
			Prints("key1 按下，光标左移\r\n");
		}
		if(KeyPress & KEY2) //如果KEY2按住，则光标需要右移，即X轴为正值。
		{
			Prints("key2 按下，光标右移\r\n");
		}
		if(KeyPress & KEY3) //如果KEY3按住，则光标需要上移，即Y轴为负值。
		{
			Prints("key3 按下，光标上移\r\n");
		}
		if(KeyPress & KEY4)  //如果KEY4按住，则光标需要下移，即Y轴为正值。
		{
			Prints("key4 按下，光标下移\r\n");
		}
		if(KeyPress & KEY5)  //如果KEY5按住，则滚轮下滚，即滚轮值为负。
		{
			Prints("key5 按下，光标下滚\r\n");
		}
		if(KeyPress & KEY6)  //如果KEY6按住，则滚轮上滚，既滚轮值为正
		{
			Prints("key6 按下，光标上滚\r\n");
		}
		if(KeyPress & KEY7)  //鼠标左键
		{
			Prints("key7 按下，鼠标左键\r\n");
		}
		if(KeyPress & KEY8)  //鼠标右键
		{
			Prints("key8 按下，鼠标右键\r\n");
		}
 }
	//记得清除KeyUp和KeyDown
	KeyUp=0;
	KeyDown=0;
}
////////////////////////End of function//////////////////////////////


void display_info(void)
{
	int i;

	for(i = 0; i < 17; i++)
	{
		Prints(HeadTable[i]);
	}
}

void main(void)
{
	EA=1;						//打开中断
	InitKeyboard(); //初始化按键
	InitUART();			//初始化串口
	display_info();	//显示信息

	while(1)
	{
		LEDs=~KeyPress;  //利用板上8个LED显示按键状态，按下时亮
		SendReport();
	}
}