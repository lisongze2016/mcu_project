#include <reg52.h>
#include "beep.h"
#include "delay.h"
 sbit feng=P0^1;

void di()
{
    feng=0;
	delayms(100);
	feng=1;	
	delayms(100);
}
void di_di()
{
    feng=0;
	  delayms(200);
	  feng=1;
    delayms(200);
	  feng=0;
    delayms(200);
	  feng=1;
    delayms(200);
	  feng=0;
    delayms(200);
	  feng=1;
    delayms(200);
	  feng=0;
    delayms(200);
	  feng=1;
    delayms(200);
	  feng=0;
    delayms(200);
	  feng=1;
    delayms(200);
}
