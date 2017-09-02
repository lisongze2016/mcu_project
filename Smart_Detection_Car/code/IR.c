/*-----------------------------------------------
  名称：遥控器红外解码数液晶显示
  内容：按配套遥控器按键，液晶显示4组码值，分别是用户码 用户码 数据码 数据反码
        显示如下：
		 BOLE-HORSE
		 Code:1E-1E-00-FF
------------------------------------------------*/
#include <reg52.h>  
#include "LCD12864.h"
#include "IR.h"
#include "beep.h"
#include "GSM.h"
sbit IR=P3^3;  //红外接口标志
sbit IN1=P3^4;//左电机
sbit IN2=P3^5;
sbit IN3=P3^6;//右电机
sbit IN4=P3^7;

//char code Tab[16]="0123456789ABCDEF";
unsigned char  irtime;//红外用全局变量
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];
//unsigned char TempData[16];

/****************定时器0中断处理*******************/

void tim0_isr (void) interrupt 1 using 1
{
  irtime++;  //用于计数2个下降沿之间的时间
}

/******************外部中断1中断处理****************/
void EX1_ISR (void) interrupt 2 //外部中断1服务函数
{
    static unsigned char  i;             //接收红外信号处理
    static bit startflag;                //是否开始处理标志位
    if(startflag)                         
    {
        if(irtime<63&&irtime>=33)//引导码 TC9012的头码，9ms+4.5ms
            i=0;
    		irdata[i]=irtime;//存储每个电平的持续时间，用于以后判断是0还是1
    		irtime=0;
    		i++;
   			if(i==33)
      	{
	  			 irok=1;
				   i=0;
	  		}
    }
  	else
		{
		    irtime=0;
		    startflag=1;
		}

}

/************定时器0初始化****************/
void TIM0init(void)//定时器0初始化
{
    TMOD|=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
    TH0=0x00; //重载值
    TL0=0x00; //初始化值
    ET0=1;    //开中断
    TR0=1;    
}
/*******************外部中断1初始*******************/
void EX1init(void)
{
    IT1 = 1;   //指定外部中断1下降沿触发，INT1 (P3.3)
    EX1 = 1;   //使能外部中断
    EA = 1;    //开总中断
}
/*********************键值处理**********************/

void Ir_work(void)
{
    /* TempData[0] = Tab[IRcord[0]/16];   //处理客户码
	   TempData[1] = Tab[IRcord[0]%16];
     TempData[2] = '-';
	   TempData[3] = Tab[IRcord[1]/16];   //处理客户码
	   TempData[4] = Tab[IRcord[1]%16];
	   TempData[5] = '-';
	   TempData[6] = Tab[IRcord[2]/16];   //处理数据码
	   TempData[7] = Tab[IRcord[2]%16];
	   TempData[8] = '-';
	   TempData[9] = Tab[IRcord[3]/16];   //处理数据反码
	   TempData[10] = Tab[IRcord[3]%16];*/
     if((IRcord[2]/16==4)&&(IRcord[2]%16==6))
		 {
			 
        IN1=1;
	      IN2=0;
	      IN3=1;
	      IN4=0;
			  //LCD_display(0x90,"小车状态：前进");
     }
	   if((IRcord[2]/16==1)&&(IRcord[2]%16==5))
		 {
			  
        IN1=0;
	      IN2=1;
	      IN3=0;
	      IN4=1;
			 //LCD_display(0x90,"小车状态：后退");
     }
		 if((IRcord[2]/16==4)&&(IRcord[2]%16==4))
		 {
			 
        IN1=1;
	      IN2=1;
	      IN3=1;
	      IN4=0;
			  //LCD_display(0x90,"小车状态：左转");
     }
		 if((IRcord[2]/16==4)&&(IRcord[2]%16==3))
		 {
			  
        IN1=1;
	      IN2=0;
	      IN3=1;
	      IN4=1;
			  //LCD_display(0x90,"小车状态：右转");
     }
		 if((IRcord[2]/16==4)&&(IRcord[2]%16==0))
		 {
			  
        IN1=1;
	      IN2=1;
	      IN3=1;
	      IN4=1;
			  //LCD_display(0x90,"小车状态：停车");
     }
		 if((IRcord[2]/16==4)&&(IRcord[2]%16==5))//开车灯
		 {
          LED_3W_OPEN();			  
     }
		 if((IRcord[2]/16==4)&&(IRcord[2]%16==7))//关车灯
		 {
			    LED_3W_CLOSE();	
     }
		 if((IRcord[2]/16==0)&&(IRcord[2]%16==7))//发送探测状况
		 {
			    Send_Message();  
     }
	   irpro_ok=0;//处理完成标志

  }
/*------------------------------------------------
                红外码值处理
------------------------------------------------*/
void Ircordpro(void)//红外码值处理函数
{ 
    unsigned char i, j, k;
    unsigned char cord,value;
    k=1;
    for(i=0;i<4;i++)      //处理4个字节
    {
        for(j=1;j<=8;j++) //处理1个字节8位
        {
            cord=irdata[k];
            if(cord>7)//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差
                value|=0x80;
            if(j<8)
		        {
			          value>>=1;
			      }
                k++;
         }
     IRcord[i]=value;
     value=0;     
     } 
	   irpro_ok=1;//处理完毕标志位置1
}

  