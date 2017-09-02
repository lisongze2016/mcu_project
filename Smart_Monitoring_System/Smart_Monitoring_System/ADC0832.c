#include "ADC0832.h"
#include <reg52.h>
#include "LCD12864.h"

unsigned int ADC_val;
sbit CS =  P3^2;
sbit DIO = P3^4;
sbit CLK = P3^3;

unsigned char getvalue0832 (bit channel)
{
    
	unsigned char i,dat1 = 0,dat2 = 0;

    CLK = 0; 
	DIO = 1;
	CS = 0; 
    CLK = 1;//产生开始信号
	
	CLK = 0;
    DIO = 1;//模拟输入选择模式（1单模信号 0 双模差分信号）
	CLK = 1;
	
	CLK = 0;
    DIO = channel;//模拟通道输入选择（1,0）
    CLK = 1;
	
    CLK = 0;
    DIO = 1;//数据线置高准备接收数据
   
    for (i = 0;i < 8;i++)
    {
       CLK = 1;
       CLK = 0;
       //if (DIO) 
       //dat1 |= 0x80 >> i;
		dat1 = dat1<<1;
		if(DIO == 1)
			dat1 =dat1|0x01;
    }

    for (i = 0;i < 8;i++)
    {
      //if (DIO)
      //dat2 |= 0x80>>i;
		
		dat2 = dat2>>1;
		if(DIO ==1)
		{
			dat2 = dat2|0x80;
		}
      CLK = 1;
      CLK = 0;
    } 	
	
    CLK = 1; 
	CS = 1;
	DIO = 1;
	
	if (dat1 == dat2) 
		return dat1;
	else
		return 0;
}
void ADC_display()
{
	
	
	ADC_val = getvalue0832(0)*100/255;
	write_LCD_command(0x98+3);
	write_LCD_data(0x30+ADC_val/10);
	write_LCD_data(0x30+ADC_val%10);
}
		
	

