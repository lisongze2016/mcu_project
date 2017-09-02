#include <reg52.h>
#include "LCD12864.h"
#include "keyscan.h"
#include "LCDMenu.h"
#include "E2PROM.h"
#include "string.h"
#define uchar unsigned char
#define uint unsinged int
uchar func_index=0;
unsigned char Menu_flag = 0;
unsigned char backflag = 1;
unsigned char fun7_flag = 1;
unsigned char fun10_flag = 1;
unsigned char fun13_flag = 1;
unsigned char fun16_flag = 1;
unsigned char fun19_flag = 1;
unsigned char phoneMenu[13] = {0};
unsigned char TMenu[3] = {3,0};
unsigned char TMenu_lower[3] = {0,0};
unsigned char RMenu_lower[3] = {0,0};
unsigned char RMenu[3] = {6,0};
unsigned char FMenu[3] = {5,0};
unsigned char Message_zxh[] ="3108904105F0";
//unsigned char Message_zxh[13] ={0};
//unsigned char phoneMenu[13] = "18329979125F";
void (*current_operation_index)();

typedef struct MENU
		{
			unsigned char current;
			unsigned char up;
			unsigned char down;
			unsigned char enter;
			unsigned char back;
			void (*current_operation)();
		}MENU;
		
MENU code table[23]=
{
{0,5,1,6,0,(*fun1)},  //第一层，显示 --菜单--、【设置手机号】、设置短信中心号、设置报警温度
{1,0,2,9,0,(*fun2)},  //第一层，显示 -- 菜单--、设置手机号、【设置短信中心号】、设置报警温度
{2,1,3,12,0,(*fun3)}, //第一层，显示 -- 菜单--、设置手机号、设置短信中心号、【设置报警温度】
{3,2,4,15,0,(*fun4)}, //第一层，显示 -- 菜单--、【设置报警湿度】、设置烟雾浓度、版本信息
{4,3,5,18,0,(*fun5)}, //第一层，显示 -- 菜单--、设置报警湿度、【设置烟雾浓度】、版本信息
{5,4,0,21,0,(*fun6)}, //第一层，显示 -- 菜单--、设置报警湿度、设置烟雾浓度、【版本信息】	

{6,8,7,6,0,(*fun7)},  //第二层，显示 设置手机号、【请输入：】、确认、取消
{7,6,8,0,0,(*fun8)},  //第二层，显示 设置手机号、请输入：、【确认】、取消
{8,7,6,0,0,(*fun9)},  //第二层，显示 设置手机号、请输入：、确认、【取消】

{9,11,10,9,1,(*fun10)},  //第二层，显示 设置短信中心号、【请输入：】、确认、取消
{10,9,11,1,1,(*fun11)}, //第二层，显示 设置短信中心号、请输入：、【确认】、取消
{11,10,9,1,1,(*fun12)}, //第二层，显示 设置短信中心号、请输入：、确认、【取消】


{12,14,13,12,2,(*fun13)},  //第二层，显示 设置报警温度、【报警上限：】、（确认    取消）
{13,12,14,2,2,(*fun14)},  //第二层，显示 设置报警温度、报警上限：、 （【确取】   取消）
{14,13,12,2,2,(*fun15)}, //第二层，显示 设置报警温度、报警上限：、 （确认  【取消】）

{15,17,16,15,3,(*fun16)}, //第二层，显示 设置报警湿度、【报警上限：】、报警下限
{16,15,17,3,3,(*fun17)}, //第二层，显示 设置报警湿度、报警上限： （【确取】   取消）
{17,16,15,3,3,(*fun18)},//第二层，显示 设置报警湿度、报警上限： （确认  【取消】）

{18,20,19,18,4,(*fun19)}, //第二层，显示 设置报警烟雾浓度、【报警上限：】、报警下限
{19,18,20,4,4,(*fun20)}, //第二层，显示 设置报警烟雾浓度、报警上限：、报警下限  （【确取】   取消）
{20,19,18,4,4,(*fun21)},//第二层，显示 设置报警烟雾浓度、报警上限：、报警下限   （确认  【取消】）

{21,21,21,21,5,(*fun22)} //第二层，显示 版本 v1.0  开发人员：李松泽  王树超   2015.4.6
//{19,100,100,100,(*fun20)}
};

void fun1()
{
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"->设置目标手机号");
	LCD_display(0x88,"  设置短信中心号");
	LCD_display(0x98,"  设置报警温度  ");
	fun7_flag = 1;
}
void fun2()
{
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"  设置目标手机号");
	LCD_display(0x88,"->设置短信中心号");
	LCD_display(0x98,"  设置报警温度  ");
	
}
void fun3()
{
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"  设置目标手机号");
	LCD_display(0x88,"  设置短信中心号");
	LCD_display(0x98,"->设置报警温度  ");
}
void fun4()
{
	fun16_flag = 1;
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"->设置报警湿度  ");
	LCD_display(0x88,"  设置烟雾浓度  ");
	LCD_display(0x98,"  版本信息      ");
}
void fun5()
{
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"  设置报警湿度  ");
	LCD_display(0x88,"->设置烟雾浓度  ");
	LCD_display(0x98,"  版本信息      ");
}
void fun6()
{
	LCD_display(0x80,"----菜单管理----");
	LCD_display(0x90,"  设置报警湿度  ");
	LCD_display(0x88,"  设置烟雾浓度  ");
	LCD_display(0x98,"->版本信息      ");
}

void fun7()
{
	
	unsigned char count = 0;
	unsigned char keyfun7;
	LCD_display(0x80,"--设置手机号--  ");
	LCD_display(0x90,"->请输入：      ");
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认    取消  ");
	
	while(1&fun7_flag)
	{
		keyfun7 = keypro();
		if(keyfun7 != 50)//说明此时有按键按下
		{
			if(count < 12)//输入12次
			{
				if(count == 0)
					write_LCD_command(0x88);
					write_LCD_command(0x0f);
				switch(keyfun7)
				{
					case 0: phoneMenu[count] = '0';break;
					case 1: phoneMenu[count] = '1';break;
					case 2: phoneMenu[count] = '2';break;
					case 3: phoneMenu[count] = '3';break;
					case 4: phoneMenu[count] = '4';break;
					case 5: phoneMenu[count] = '5';break;
					case 6: phoneMenu[count] = '6';break;
					case 7: phoneMenu[count] = '7';break;
					case 8: phoneMenu[count] = '8';break;
					case 9: phoneMenu[count] = '9';break;
					case 13:phoneMenu[count] = 'F';break;
				}
				write_LCD_data(phoneMenu[count]);
			}
			
			count++;
			if((count >= 12)||(keyfun7 == 12)||(keyfun7 == 16))
			{
				phoneMenu[count] = '\0';
				fun7_flag = 0;
				write_LCD_command(0x0c);
				break;
			}	
		}	
	}
}
void fun8()
{
	LCD_display(0x80,"--设置手机号--  ");
	LCD_display(0x90,"  请输入：      ");
	LCD_display(0x88,phoneMenu);
	//LCD_display(0x88,"                ");
	LCD_display(0x98,"->确认    取消  ");
	fun7_flag = 1;
}
void fun9()
{
	LCD_display(0x88,"                ");
	memset(phoneMenu,0,strlen(phoneMenu));
	LCD_display(0x80,"--设置手机号--  ");
	LCD_display(0x90,"  请输入：      ");
	LCD_display(0x88,phoneMenu);
	LCD_display(0x98,"  确认  ->取消  ");
}
void fun10()
{
	unsigned char count10 = 0;
	unsigned char keyfun10;
	unsigned char i;
	LCD_display(0x80,"  设置短信中心号");
	LCD_display(0x90,"->请输入：      ");
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认    取消  ");
	
	while(1&fun10_flag)
	{
		keyfun10 = keypro();
		if(keyfun10 != 50)//说明此时有按键按下
		{
			if(count10 < 12)//输入12次
			{
				if(count10 == 0)
					write_LCD_command(0x88);
					write_LCD_command(0x0f);
				switch(keyfun10)
				{
					case 0: Message_zxh[count10] = '0';break;
					case 1: Message_zxh[count10] = '1';break;
					case 2: Message_zxh[count10] = '2';break;
					case 3: Message_zxh[count10] = '3';break;
					case 4: Message_zxh[count10] = '4';break;
					case 5: Message_zxh[count10] = '5';break;
					case 6: Message_zxh[count10] = '6';break;
					case 7: Message_zxh[count10] = '7';break;
					case 8: Message_zxh[count10] = '8';break;
					case 9: Message_zxh[count10] = '9';break;
					case 13:Message_zxh[count10] = 'F';break;
				}
				write_LCD_data(Message_zxh[count10]);
			}
			
			count10++;
			if((count10 >= 12)||(keyfun10 == 12)||(keyfun10 == 16))
			{
				phoneMenu[count10] = '\0';
				fun10_flag = 0;
				write_LCD_command(0x0c);
				for(i = 0; i < 6; i++)
		        {
					Message_zxh[2*i] ^= Message_zxh[2*i+1];
					Message_zxh[2*i+1] ^= Message_zxh[2*i];
					Message_zxh[2*i] ^= Message_zxh[2*i+1];
		        }  
				break;
			}	
		}	
	}
}
void fun11()
{
	fun10_flag = 1;

	LCD_display(0x80,"  设置短信中心号");
	LCD_display(0x90,"  请输入：      ");
	LCD_display(0x88,Message_zxh);
	LCD_display(0x98,"->确认    取消  ");
	
	
}
void fun12()
{
	fun10_flag = 1;
	memset(Message_zxh,0,strlen(Message_zxh));
	LCD_display(0x88,"                ");
	LCD_display(0x80,"  设置短信中心号");
	LCD_display(0x90,"  请输入：      ");
	LCD_display(0x88,Message_zxh);
	LCD_display(0x98,"  确认  ->取消  ");
}
void fun13()
{
	
	unsigned char count13 = 0;
	unsigned char keyfun13 = 0;
	
	LCD_display(0x80,"  设置报警温度  ");
	LCD_display(0x90,"->报警上限：    ");
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认    取消  ");
	
	while(1&fun13_flag)
	{
		keyfun13 = keypro();
		if(keyfun13 != 50)//说明此时有按键按下
		{
			if(count13 < 2)//输入2次
			{
				if(count13 == 0)
				{
					write_LCD_command(0x90+6);
					write_LCD_command(0x0f);
				}	
				switch(keyfun13)
				{
					case 0:TMenu[count13] = 0; break;
					case 1:TMenu[count13] = 1; break;
					case 2:TMenu[count13] = 2; break;
					case 3:TMenu[count13] = 3; break;
					case 4:TMenu[count13] = 4; break;
					case 5:TMenu[count13] = 5; break;
					case 6:TMenu[count13] = 6; break;
					case 7:TMenu[count13] = 7; break;
					case 8:TMenu[count13] = 8; break;
					case 9:TMenu[count13] = 9; break;
					default:break;
				}
				write_LCD_data(0x30+TMenu[count13]);
				
			}
			count13++;
			if((count13 >= 2)||(keyfun13 == 12)||(keyfun13 == 16))
			{
				fun13_flag = 0;
				write_LCD_command(0x0c);
				break;
			}	
		}	
	}
}

void fun14()
{
	fun13_flag = 1;
	LCD_display(0x80,"  设置报警温度  ");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+TMenu[0]);
	write_LCD_data(0x30+TMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"->确认    取消  ");
}

void fun15()
{
	fun13_flag = 1;
	TMenu[0] = 0;
	TMenu[1] = 0;
	LCD_display(0x80,"  设置报警温度  ");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+TMenu[0]);
	write_LCD_data(0x30+TMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认  ->取消  ");
}

void fun16()
{
	unsigned char count16 = 0;
	unsigned char keyfun16 = 0;
	
	LCD_display(0x80,"  设置报警湿度  ");
	LCD_display(0x90,"->报警上限：    ");
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认    取消  ");
	
	while(1&fun16_flag)
	{
		keyfun16 = keypro();
		if(keyfun16 != 50)//说明此时有按键按下
		{
			if(count16 < 2)//输入2次
			{
				if(count16 == 0)
				{
					write_LCD_command(0x90+6);
					write_LCD_command(0x0f);
				}	
				switch(keyfun16)
				{
					case 0:RMenu[count16] = 0; break;
					case 1:RMenu[count16] = 1; break;
					case 2:RMenu[count16] = 2; break;
					case 3:RMenu[count16] = 3; break;
					case 4:RMenu[count16] = 4; break;
					case 5:RMenu[count16] = 5; break;
					case 6:RMenu[count16] = 6; break;
					case 7:RMenu[count16] = 7; break;
					case 8:RMenu[count16] = 8; break;
					case 9:RMenu[count16] = 9; break;
					default:break;
				}
				write_LCD_data(0x30+RMenu[count16]);
			}
			count16++;
			if((count16 >= 2)||(keyfun16 == 12)||(keyfun16 == 16))
			{
				fun16_flag = 0;
				write_LCD_command(0x0c);
				break;
			}	
		}	
	}
	
}

void fun17()
{
	fun16_flag = 1;
	LCD_display(0x80,"  设置报警湿度  ");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+RMenu[0]);
	write_LCD_data(0x30+RMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"->确认    取消  ");	
}

void fun18()
{
	fun16_flag = 1;
	RMenu[0] = 0;
	RMenu[1] = 0;
	LCD_display(0x80,"  设置报警湿度  ");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+RMenu[0]);
	write_LCD_data(0x30+RMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认  ->取消  ");
}

void fun19()
{
	unsigned char count19 = 0;
	unsigned char keyfun19 = 0;
	
	LCD_display(0x80,"设置报警烟雾浓度");
	LCD_display(0x90,"->报警上限：    ");
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认    取消  ");
	
	while(1&fun19_flag)
	{
		keyfun19 = keypro();
		if(keyfun19 != 50)//说明此时有按键按下
		{
			if(count19 < 2)//输入2次
			{
				if(count19 == 0)
				{
					write_LCD_command(0x90+6);
					write_LCD_command(0x0f);
				}	
				switch(keyfun19)
				{
					case 0:FMenu[count19] = 0; break;
					case 1:FMenu[count19] = 1; break;
					case 2:FMenu[count19] = 2; break;
					case 3:FMenu[count19] = 3; break;
					case 4:FMenu[count19] = 4; break;
					case 5:FMenu[count19] = 5; break;
					case 6:FMenu[count19] = 6; break;
					case 7:FMenu[count19] = 7; break;
					case 8:FMenu[count19] = 8; break;
					case 9:FMenu[count19] = 9; break;
					default:break;
				}
				write_LCD_data(0x30+FMenu[count19]);	
			}
			count19++;
			if((count19 >= 2)||(keyfun19 == 12)||(keyfun19 == 16))
			{
				fun19_flag = 0;
				write_LCD_command(0x0c);
				break;
			}	
		}	
	}
	
}

void fun20()
{
	fun19_flag = 1;
	
	LCD_display(0x80,"设置报警烟雾浓度");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+FMenu[0]);
	write_LCD_data(0x30+FMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"->确认    取消  ");
	
}

void fun21()
{
	fun19_flag = 1;
	FMenu[0] = 0;
	FMenu[1] = 0;
	
	LCD_display(0x80,"设置报警烟雾浓度");
	LCD_display(0x90,"  报警上限：");
	write_LCD_command(0x90+6);
	write_LCD_data(0x30+FMenu[0]);
	write_LCD_data(0x30+FMenu[1]);
	LCD_display(0x88,"                ");
	LCD_display(0x98,"  确认  ->取消  ");	
}

void fun22()
{
	LCD_display(0x80,"----版本信息----");
	LCD_display(0x90,"      v1.0      ");
	LCD_display(0x88,"开发人员 :SONGZE");
	LCD_display(0x98,"王树超    张江锋");
	
}

void ManageMenu()
{
	switch(keypro())
	{
		case 10: func_index=table[func_index].up; break;
		case 11: func_index=table[func_index].down; break;
		case 12: func_index=table[func_index].enter; break;
		case 16: func_index=table[func_index].back;break;
		case 15: Menu_flag = 1;//退出菜单标志位
		default: break;
	}
	
	current_operation_index=table[func_index].current_operation;
	(*current_operation_index)();
}