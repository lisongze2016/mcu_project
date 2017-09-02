#ifndef __LCD12864_H__
#define __LCD12864_H__



extern void read_busy();
extern void write_LCD_command(unsigned char value);
extern void write_LCD_data(unsigned char value); 
extern void init_LCD();
extern void LCD_display(unsigned char add,char *hz);
#endif