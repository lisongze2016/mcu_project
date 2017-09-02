#include <reg52.h>
#include "beep.h"
#include "delay.h"
 sbit feng=P1^2;
 sbit LED_3W=P1^3;

void di()
{
    feng=0;
	  delayms(200);
	  feng=1;	  
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
void LED_3W_OPEN()
{
    LED_3W=0;
}
void LED_3W_CLOSE()
{
    LED_3W=1;
}