#include <reg52.h>
#include "LCD12864.h"
#include "delay.h"
#include "IR.h"
#include "GSM.h"
#include "DHT11.h"
#include "ds18b20.h"
#include "beep.h"
void main(void)
{
    EX1init();            //初始化外部中断1
    TIM0init();           //初始化定时器
    init_LCD();           //初始化液晶
    delayms(20);          //延时有助于稳定
	  LCD_display(0x80,"  智能探测小车");
	  LCD_display(0x98,"温度:");
	  LCD_display(0x98+4,"湿度:");
	  Receive_Message_init();
	Send_Message(); //测试
    while(1)//主循环
    {
			  write_LCD_command(0x98+3);
			 //write_LCD_data('T');
	     write_LCD_data(0x30+read_ds18b20temp()%1000/100);
	     write_LCD_data(0x30+read_ds18b20temp()%100/10);
			 init_DHT11();
       if(irok)                        //如果接收好了进行红外处理
	     {   
	         Ircordpro();
 	         irok=0;
	     }
       if(irpro_ok)                   //如果处理好后进行工作处理，如按对应的按键后显示对应的数字等
	     {
	        Ir_work();
  	   }
			 if((read_ds18b20temp()%1000/100>=2)&&(read_ds18b20temp()%100/10>7))
			 {
				  di_di();
          Send_Message();  
       }
			 if((RH_data/10%10>=6)&&(RH_data%10>=0))
			 {
				  di_di();
          Send_Message();  
       }
			 Receive_Message();
			 
   }
}