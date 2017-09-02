#include <reg52.h>
#include "keyscan.h"
#include "delay.h"
#define DataPort P1

unsigned char keyscan()
{
	unsigned char key_l,key_h;
	DataPort = 0xf0;
	key_l = DataPort&0xf0;//将行数据清零，列数据置1
	if(key_l != 0xf0)
	{
		delayms(5);
		if(key_l != 0xf0)
		{
			key_l = DataPort&0xf0;//将行数据清零，列数据置1
			DataPort = key_l|0x0f;//将行数置1，此时因按键未松手，列数据某一位为零，将行数据某位拉低
			key_h = DataPort&0x0f;//将列数据清零，保存行数据
			while((DataPort&0x0f) != 0x0f);//等待按键松手
			return(key_h|key_l);
		}
	}
	return (0xff);
}


unsigned char keypro()
{
	switch(keyscan())
	{
		case 0x7e: return 10;break;//4
		case 0xbe: return 3;break;
		case 0xde: return 2;break;
		case 0xee: return 1;break;
		case 0x7d: return 11;break;//8
		case 0xbd: return 6;break;//7
		case 0xdd: return 5;break;//6
		case 0xed: return 4;break;//5
		case 0x7b: return 12;break;
		case 0xbb: return 9;break;//11
		case 0xdb: return 8;break;//10
		case 0xeb: return 7;break;//9
		case 0x77: return 16;break;
		case 0xb7: return 15;break;
		case 0xd7: return 0;break;//14
		case 0xe7: return 13;break;
		default: return 50;break;
	}
}