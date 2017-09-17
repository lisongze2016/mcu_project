/******************************************************************
   本程序只供学习使用，未经作者许可，不得用于其它任何用途
			
        欢迎访问我的USB专区：http://group.ednchina.com/93/
        欢迎访问我的blog：   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        感谢PCB赞助商——电子园： http://bbs.cepark.com/

UsbCore.h file

作者：电脑圈圈
建立日期: 2008.06.29
修改日期: 2008.07.10
版本：V1.1
版权所有，盗版必究。
Copyright(C) 电脑圈圈 2008-2018
All rights reserved            
*******************************************************************/

#ifndef __USBCORE_H__
#define __USBCORE_H__

#define GET_STATUS         0
#define CLEAR_FEATURE      1
#define SET_FEATURE        3
#define SET_ADDRESS        5
#define GET_DESCRIPTOR     6
#define SET_DESCRIPTOR     7
#define GET_CONFIGURATION  8
#define SET_CONFIGURATION  9
#define GET_INTERFACE      10
#define SET_INTERFACE      11
#define SYNCH_FRAME        12

#define DEVICE_DESCRIPTOR         1
#define CONFIGURATION_DESCRIPTOR  2
#define STRING_DESCRIPTOR         3
#define INTERFACE_DESCRIPTOR      4
#define ENDPOINT_DESCRIPTOR       5
#define REPORT_DESCRIPTOR         0x22

#define SET_IDLE 0x0A

void UsbDisconnect(void); //USB断开连接
void UsbConnect(void);    //USB连接
void UsbBusSuspend(void); //总线挂起中断处理
void UsbBusReset(void);   //总线复位中断处理
void UsbEp0Out(void);     //端点0输出中断处理
void UsbEp0In(void);      //端点0输入中断处理
void UsbEp1Out(void);     //端点1输出中断处理
void UsbEp1In(void);      //端点1输入中断处理
void UsbEp2Out(void);     //端点2输出中断处理
void UsbEp2In(void);      //端点2输入中断处理

extern uint8 ConfigValue;  //当前配置值
extern uint8 Ep1InIsBusy;  //端点1输入是否忙

#endif