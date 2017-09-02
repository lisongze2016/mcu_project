#include <reg52.h>
#include "ds18b20.h"
#include "GSM.h"
#include "LCD12864.h"
#include "LCDMenu.h"
#include "keyscan.h"
#include "DHT11.h"
#include "delay.h"
#include "string.h"
#include "ADC0832.h"
#include "beep.h"

unsigned char DS18b20_Temp[3] = {0};
bit SR_flag = 0;
sbit SR501_dat = P3^5;

void Init_LCD_display()
{
	LCD_display(0x80,"--智能监控系统--");
	write_LCD_command(0x90);
	write_LCD_data('T');
	write_LCD_data(':');
	write_LCD_data(0x30+TMenu[0]);
	write_LCD_data(0x30+TMenu[1]);
	write_LCD_data(' ');
	write_LCD_data(' ');
	write_LCD_data('R');
	write_LCD_data(':');
	write_LCD_data(0x30+RMenu[0]);
	write_LCD_data(0x30+RMenu[1]);
	write_LCD_data(' ');
	write_LCD_data(' ');
	write_LCD_data('F');
	write_LCD_data(':');
	write_LCD_data(0x30+FMenu[0]);
	write_LCD_data(0x30+FMenu[1]);
	LCD_display(0x88,"温度 :");
	LCD_display(0x88+4,"湿度 :");
	LCD_display(0x98,"烟雾 :");	
	LCD_display(0x98+4,"防盗 :OK");
	write_LCD_command(0x88+3);
	DS18b20_Temp[0] = read_ds18b20temp()%1000/100;
	DS18b20_Temp[1] = read_ds18b20temp()%100/10;
	write_LCD_data(0x30+DS18b20_Temp[0]);
	write_LCD_data(0x30+DS18b20_Temp[1]);
	init_DHT11();
	ADC_display();
}

void main()
{
	int count = 0;
	unsigned char key_value,key2_value ;
	init_LCD();
	Receive_Message_init();
	
	while(1)
	{
		Init_LCD_display();
		//Send_Message();
		Receive_Message();
		key_value = keypro();
		if(key_value == 1)
		{
			LCD_display(0x98+4,"防盗 :开");
			SR_flag = 1;
			delayms(5000);
			di_di();
		}
		if(key_value == 13)
		{ 
			Menu_flag = 0;//进入菜单标志位
			while(1)
			{
				ManageMenu();
				if(Menu_flag == 1)
					break;
			}
		}
		if((DS18b20_Temp[0] >= TMenu[0])&&(DS18b20_Temp[1] >TMenu[1]))
		{
			if(strlen(phoneMenu) > 10)
			{
				if(TNum != 0)
				{
					Send_Message();
					TNum--;
				}
				else
				{
					LCD_display(0x90,"温度报警次数已够");
					delayms(1000);
					LCD_display(0x90,"                ");
				}
			}
			else
			{
				LCD_display(0x90,"用户号码未设定1 ");
				delayms(1000);
				LCD_display(0x90,"                ");
			}
		}
		if((RH_data/10%10>=RMenu[0])&&(RH_data%10>=RMenu[1]))
		{
			//di_di();
			if(strlen(phoneMenu) > 10)
			{
				if(RNum != 0)
				{
					Send_Message();
					RNum--;
				}
				else
				{
					LCD_display(0x90,"湿度报警次数已够");
					delayms(1000);
					LCD_display(0x90,"                ");
				}
			}  
			else
			{
				LCD_display(0x90,"用户号码未设定2 ");
				delayms(1000);
				LCD_display(0x90,"                ");
			}
		}
		if((ADC_val/10>=FMenu[0])&&(ADC_val%10>=FMenu[1]))
		{
			if(strlen(phoneMenu) > 10)
			{
				if(FNum != 0)
				{
					Send_Message();
					FNum--;
				}
				else
				{
					LCD_display(0x90,"烟雾报警次数已够");
					delayms(1000);
					LCD_display(0x90,"                ");
				}
			}
			else
			{
				LCD_display(0x90,"用户号码未设定3 ");
				delayms(1000);
				LCD_display(0x90,"                ");
			}
		}
		//SR501_find();
		
		if(SR_flag ==1)
		{
			if(SR501_dat==1)
			{
				count = 0;
				GSM_SR501_flag = 1;
				while(1)
				{
					count++;
					LCD_display(0x98+4,"有人闯入");
			  		di();
					if(count ==1)
					{
						if(strlen(phoneMenu) > 10)
						{
							Send_Message();
						}
						else
						{
							LCD_display(0x90,"用户号码未设定  ");
							delayms(1000);
							LCD_display(0x90,"                ");
						} 
					}
					key2_value = keypro();
					if(key2_value == 2)
					{
						LCD_display(0x98+4,"防盗 :关");
						SR_flag = 0;
						break;
					}
				}
			}
		}
		
	}
}