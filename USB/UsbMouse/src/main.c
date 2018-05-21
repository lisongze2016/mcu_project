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
#include "include/pdiusbd12.h"
#include "include/UsbCore.h"


code uint8 HeadTable[][100]={
"********************************************************************\r\n",
"******                      USB鼠标                           ******\r\n",
"******                      AT89S52 CPU                       ******\r\n",
"******                  建立日期：",__DATE__,"                 ******\r\n",
"******                   建立时间：",__TIME__,"                   ******\r\n",
"******                    作者：lisongze                      ******\r\n",
"******                    欢迎访问作者的                      ******\r\n",
"******      BLOG：http://www.lisongze.cn                      ******\r\n",
"******      CSDN BLOG：http://blog.csdn.net/songze_lee        ******\r\n",
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
	//需要返回的4字节报告的缓冲
	//Buf[0]的D0就是左键，D1就是右键，D2就是中键（这里没有）
	//Buf[1]为X轴，Buf[2]为Y轴，Buf[3]为滚轮
	uint8 Buf[4]={0,0,0,0};

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
			Buf[1] = -5;  //这里一次往左移动一个单位。
		}
		if(KeyPress & KEY2) //如果KEY2按住，则光标需要右移，即X轴为正值。
		{
			Prints("key2 按下，光标右移\r\n");
			Buf[1] = 5;   //这里一次往右移动一个单位。
		}
		if(KeyPress & KEY3) //如果KEY3按住，则光标需要上移，即Y轴为负值。
		{
			Prints("key3 按下，光标上移\r\n");
			Buf[2] = -5;   //这里一次往上移动一个单位。
		}
		if(KeyPress & KEY4)  //如果KEY4按住，则光标需要下移，即Y轴为正值。
		{
			Prints("key4 按下，光标下移\r\n");
			Buf[2] = 5;  //这里一次往下移动一个单位。
		}
		if(KeyPress & KEY5)  //如果KEY5按住，则滚轮下滚，即滚轮值为负。
		{
			Prints("key5 按下，光标下滚\r\n");
			Buf[3] = -1;  //这里一次往下滚动一个单位。
		}
		if(KeyPress & KEY6)  //如果KEY6按住，则滚轮上滚，既滚轮值为正
		{
			Prints("key6 按下，光标上滚\r\n");
			 Buf[3] = 1;   //这里一次往上滚动一个单位。
		}
		if(KeyPress & KEY7)  //鼠标左键
		{
			Prints("key7 按下，鼠标左键\r\n");
			Buf[0] |= 0x01;  //D0为鼠标左键
		}
		if(KeyPress & KEY8)  //鼠标右键
		{
			Prints("key8 按下，鼠标右键\r\n");
			Buf[0] |= 0x02;  //D1为鼠标右键
		}

		//报告准备好了，通过端点1返回，长度为4字节。
		D12WriteEndpointBuffer(3,4,Buf);
		Ep1InIsBusy=1;  //设置端点忙标志。
 }
	//记得清除KeyUp和KeyDown
	KeyUp=0;
	KeyDown=0;
}
////////////////////////End of function//////////////////////////////


void display_info(void)
{
	int i;

	for(i = 0; i < 18; i++)
	{
		Prints(HeadTable[i]);
	}
}

int PDIUSBD12_detect(void)
{
	uint16 id;

	id=D12ReadID();
	Prints("Your D12 chip\'s ID is: ");
	PrintShortIntHex(id);
	if(id == 0x1012)
	{
		Prints(". ID is correrct!\r\n\r\n");
	}
	else
	{
		Prints(". ID is incorrerct!\r\n\r\n");
		return -1;
	}

	return 0;
}

/********************************************************************
函数功能：USB中断处理函数。
入口参数：无。
返    回：无。
备    注：无。					
********************************************************************/
void PDIUSBD12_interrupt_handle(void)
{
	uint8 InterruptSource;

	D12WriteCommand(READ_INTERRUPT_REGISTER);	//写读中断寄存器的命令
	InterruptSource = D12ReadByte(); 					//读回第一字节的中断寄存器
	if(InterruptSource&0x80)UsbBusSuspend();	//总线挂起中断处理
	if(InterruptSource&0x40)UsbBusReset();		//总线复位中断处理
	if(InterruptSource&0x01)UsbEp0Out();			//端点0输出中断处理
	if(InterruptSource&0x02)UsbEp0In();				//端点0输入中断处理
	if(InterruptSource&0x04)UsbEp1Out();			//端点1输出中断处理
	if(InterruptSource&0x08)UsbEp1In();				//端点1输入中断处理
	if(InterruptSource&0x10)UsbEp2Out();			//端点2输出中断处理
	if(InterruptSource&0x20)UsbEp2In();				//端点2输入中断处理
}

void main(void)
{
	EA=1;									//打开中断
	InitKeyboard(); 			//初始化按键
	InitUART();						//初始化串口
	display_info();				//显示信息
	PDIUSBD12_detect();		//识别芯片

	UsbDisconnect();			//先断开USB连接
	UsbConnect();					//将USB连接上
	while(1) {
		if(D12GetIntPin() == 0) {//如果有中断发生 低电平有效
			PDIUSBD12_interrupt_handle();
		}
		if(ConfigValue != 0) {//如果已经设置为非0的配置，则可以返回报告数据
			LEDs = ~KeyPress;  //利用板上8个LED显示按键状态，按下时亮
			if(!Ep1InIsBusy) { //如果端点1输入没有处于忙状态，则可以发送数据
				KeyCanChange=0;  //禁止按键扫描
				if(KeyUp||KeyDown||KeyPress) {//如果有按键事件发生
					SendReport();  //则返回报告
				}
				KeyCanChange = 1;  //允许按键扫描
			}
		}
	}
}