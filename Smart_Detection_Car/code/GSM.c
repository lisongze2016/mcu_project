/**************************************************
**ÎÄ¼şÃû³Æ£ºÍ¨¹ıTC35I½ÓÊÕÓ¢ÎÄ¶ÌĞÅ
**¹¦ÄÜÃèÊö:½ÓÊÕÓ¢ÎÄ¶ÌĞÅ²½Öè£º
**                          1.·¢ËÍATÖ¸Áî
**                          2.·¢ËÍAT+CPMS=\"MT\",\"MT\",\"MT\" ÉèÖÃËùÓĞ²Ù×÷¶¼ÔÚMT£¨Ä£¿éÖÕ¶Ë£©ÖĞ½øĞĞ
**                          3.·¢ËÍAT+CNMI=2£¬1ÉèÖÃµ±ÓĞĞÂ¶ÌĞÅµ½À´Ê±ÌáÊ¾
**                          4.·¢ËÍAT+CMGF=1,ÉèÖÃÎÄ±¾Ä£Ê½
**                          5.·¢ËÍAT+CMGD=1,É¾³ıµÚÒ»Ìõ¶ÌĞÅ
**                          6.ÅĞ¶Ï½ÓÊÕ×Ö·û´®ÖĞÊÇ·ñº¬ÓĞ¡°+CMTI¡±£¬º¬ÓĞ±íÊ¾ÓĞĞÂµÄ¶ÌĞÅ
**                          7.·¢ËÍAT+CMGR=1£¬ÉèÖÃ¶ÁÈ¡ĞÅÏ¢Ö¸Áî
**                          8.ÅĞ¶Ï½ÓÊÕĞÅÏ¢ÊÇ·ñÒ»ÖÂ£¬×ö³öÏàÓ¦²Ù×÷
**                          9.·¢ËÍAT+CMGD=1£¬É¾³ı¶ÁÈ¡ºóµÄĞÅÏ¢
**×÷Õß£ºÀîËÉÔó
**Ê±¼ä£º2014¡ª11-16
**************************************************/
#include <reg52.h>
#include <string.h>
#include "ds18b20.h"
#include "delay.h"
#include "LCD12864.h"
#include "DHT11.h"
#include "GSM.h"
#include "beep.h"
#define uchar unsigned char
#define uint unsigned int
/******************½ÓÊÕ¶ÌĞÅÖ¸Áî*****************************/
uchar code AT[]="AT\r";
uchar code CPMS[]={"AT+CPMS=\"MT\",\"MT\",\"MT\"\r"};
uchar code CNMI[]={"AT+CNMI=2,1\r"};
uchar code CMGF[]={"AT+CMGF=1\r"};
uchar code CMGR[]={"AT+CMGR=1\r"};
uchar code CMGD[]={"AT+CMGD=1\r"};
/********************·¢ËÍÖĞÎÄ¶ÌĞÅÖ¸Áî***********************/
uchar code MSXZ[]={"AT+CMGF=0\r"};//·¢ËÍÄ£Ê½Ñ¡Ôñ
uchar code MSCS[]={"AT+CSMP=17,167,0,8\r"} ;//ÉèÖÃPDUÄ£Ê½²ÎÊı£¬8±íÊ¾Unicode±àÂë
uchar code CSCS[]={"AT+CSCS=GSM\r"};
uchar code MBSJ[]={"AT+CMGS=54\r"};//ÊäÈëPDU³¤¶È
uchar code DXNR[]={"0891683108904105F011000B818123999721F50008A7"};//18329979125F   8123999721F5              
uchar code MESS[]={"285F53524D68C06D4B73AF58836E295EA64E3AFF1A"};//µ±Ç°¼ì²â»·¾³ÎÂ¶ÈÎª£º25¶È  µÄPDU±àÂ
uchar code MESS3[]={"002C6E7F5EA64E3AFF1A"};
/**********************Êı×Ö0-9µÄ£Ğ£Ä£Õ±àÂë****************************/
uchar code num0[]={"0030"};
uchar code num1[]={"0031"};
uchar code num2[]={"0032"};
uchar code num3[]={"0033"};
uchar code num4[]={"0034"};
uchar code num5[]={"0035"};
uchar code num6[]={"0036"};
uchar code num7[]={"0037"};
uchar code num8[]={"0038"};
uchar code num9[]={"0039"};
uchar code MESS1[]={"5EA6"};//¶ÈµÄ£Ğ£Ä£Õ±àÂë
uchar Rec_Buf[40];
uchar i=0;
uint T_Value;
sbit led1=P0^0;
sbit led2=P0^1;

/******************************************
**º¯ÊıÃû³Æ:void uart_int(void)
**¹¦ÄÜÃèÊö:´®¿Ú³õÊ¼»¯
**ÊäÈë:void
**Êä³ö:
******************************************/
void serial_int(void)
{
    SCON=0x50;//´®ĞĞ¿Ú¹¤×÷·½Ê½1£¬REN=1£¬ÔÊĞí´®ĞĞ¿Ú½ÓÊÕÊı¾İ
    PCON=0x00;//SMOD=0£»²¨ÌØÂÊ²»¼Ó±¶
    TMOD|=0x20;//T1·½Ê½2
    TH1=0xFD;//×°³õÖµ
    TL1=0xFD;//×°³õÖµ
    TR1=1;//Æô¶¯¶¨Ê±Æ÷1
	  ES=1;//¿ªÆô´®¿ÚÖĞ¶Ï
	  EA=1;//¿ªÆôÈ«¾ÖÖĞ¶Ï
}
/******************************************
**º¯ÊıÃû³Æ:void send(uchar *tab)
**¹¦ÄÜÃèÊö:·¢ËÍÖ¸ÁîÊı¾İ
**ÊäÈë:uchar *tab
**Êä³ö:
******************************************/
void send(uchar *tab)
{
	  ES=0;
    while((*tab)!='\0')
    {
        SBUF=*tab;
        while(TI==0);
        TI=0;
        tab++;
    }
		ES=1;
}
/******************************************
**º¯ÊıÃû³Æ:void CLR_Buf()
**¹¦ÄÜÃèÊö:Çå³ı»º´æÊı¾İ
**ÊäÈë:ÎŞ
**Êä³ö:
******************************************/
void CLR_Buf()
{
    unsigned char k;
    for(k=0;k<40;k++)
    {
        Rec_Buf[k]=0;
    }
    i=0;
}

/******************************************
**º¯ÊıÃû³Æ:bit hand(unsigned char *a)
**¹¦ÄÜÃèÊö:ÅĞ¶Ï»º´æÖĞÊÇ·ñº¬ÓĞÖ¸¶¨µÄ×Ö·û´®
**ÊäÈë:unsigned char *a
**Êä³ö:bit 1¡ª¡ª¡ªº¬ÓĞ   0¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª²»º¬ÓĞ
******************************************/
bit hand(unsigned char *a)
{
    if(strstr(Rec_Buf,a)!=NULL)
			  return 1;
		else
			return 0;
}

/******************************************
**º¯ÊıÃû³Æ:void Send_Value()
**¹¦ÄÜÃèÊö:·¢ËÍ²É¼¯Êı¾İ
**ÊäÈë:
**Êä³ö:
******************************************/
void Send_Value()
{
    uchar bai,shi;
	  T_Value=read_ds18b20temp();
	  bai=T_Value%1000/100;
	  shi=T_Value%100/10;
	  write_LCD_command(0x90);
	  write_LCD_data('T');
	  write_LCD_data(0x30+bai);
	  write_LCD_data(0x30+shi);
	  if(bai==0)
		{
			send(num0);
			delayms(1000);
		}
		  
		if(bai==1)
		{
			send(num1);
			delayms(1000);
		}
		if(bai==2)
			{
			send(num2);
			delayms(1000);
		}
		if(bai==3)
			{
			send(num3);
			delayms(1000);
		}
		if(bai==4)
			{
			send(num4);
			delayms(1000);
		}
		if(bai==5)
			{
			send(num5);
			delayms(1000);
		}
		if(bai==6)
			{
			send(num6);
			delayms(1000);
		}
		if(bai==7)
			{
			send(num7);
			delayms(1000);
		}
		if(bai==8)
			{
			send(num8);
			delayms(1000);
		}
		if(bai==9)
			{
			send(num9);
			delayms(1000);
		}
		
		if(shi==0)
			{
			send(num0);
			delayms(1000);
		}
		if(shi==1)
			{
			send(num1);
			delayms(1000);
		}
		if(shi==2)
			{
			send(num2);
			delayms(1000);
		}
		if(shi==3)
			{
			send(num3);
			delayms(1000);
		}
		if(shi==4)
			{
			send(num4);
			delayms(1000);
		}
		if(shi==5)
			{
			send(num5);
			delayms(1000);
		}
		if(shi==6)
			{
			send(num6);
			delayms(1000);
		}
		if(shi==7)
			{
			send(num7);
			delayms(1000);
		}
		if(shi==8)
			{
			send(num8);
			delayms(1000);
		}
		if(shi==9)
			{
			send(num9);
			delayms(1000);
		}
}
/******************************************
**º¯ÊıÃû³Æ:void Send_Value()
**¹¦ÄÜÃèÊö:·¢ËÍ²É¼¯Êª¶ÈÊı¾İ
**ÊäÈë:
**Êä³ö:
******************************************/
void Send_Wetness_Value()
{
    uchar bai,shi;
	
	  bai=RH_data/10%10;
	  shi=RH_data%10;
	  //bai=5;
	  //shi=7;
	  write_LCD_command(0x90+4);
	  write_LCD_data('R');
	  write_LCD_data(0x30+bai);
	  write_LCD_data(0x30+shi);
	  if(bai==0)
		{
			send(num0);
			delayms(1000);
		}
		  
		if(bai==1)
		{
			send(num1);
			delayms(1000);
		}
		if(bai==2)
			{
			send(num2);
			delayms(1000);
		}
		if(bai==3)
			{
			send(num3);
			delayms(1000);
		}
		if(bai==4)
			{
			send(num4);
			delayms(1000);
		}
		if(bai==5)
			{
			send(num5);
			delayms(1000);
		}
		if(bai==6)
			{
			send(num6);
			delayms(1000);
		}
		if(bai==7)
			{
			send(num7);
			delayms(1000);
		}
		if(bai==8)
			{
			send(num8);
			delayms(1000);
		}
		if(bai==9)
			{
			send(num9);
			delayms(1000);
		}
		
		if(shi==0)
			{
			send(num0);
			delayms(1000);
		}
		if(shi==1)
			{
			send(num1);
			delayms(1000);
		}
		if(shi==2)
			{
			send(num2);
			delayms(1000);
		}
		if(shi==3)
			{
			send(num3);
			delayms(1000);
		}
		if(shi==4)
			{
			send(num4);
			delayms(1000);
		}
		if(shi==5)
			{
			send(num5);
			delayms(1000);
		}
		if(shi==6)
			{
			send(num6);
			delayms(1000);
		}
		if(shi==7)
			{
			send(num7);
			delayms(1000);
		}
		if(shi==8)
			{
			send(num8);
			delayms(1000);
		}
		if(shi==9)
			{
			send(num9);
			delayms(1000);
		}
}
void Receive_Message_init()
{
    serial_int();//´®¿Ú³õÊ¼»¯
	  CLR_Buf();//Çå³ı»º´æÇø
	  while(!hand("OK"))
		{
        send(AT);       // ·¢ËÍATÖ¸Áî
        delayms(500);
		}
	  CLR_Buf();
		//led1=0;
		//delayms(1000);
    send(CPMS); //ÉèÖÃËùÓĞÄ£Ê½ÔÚMT£¨Ä£¿éÖÕ¶Ë£©ÖĞ½øĞĞ      
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led2=0;
		//delayms(1000);
    send(CNMI);  //ÉèÖÃµ±ÓĞĞÂµÄ¶ÌĞÅÀ´Ê±ÌáÊ¾     
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led1=1;
		//delayms(1000);
	  send(CMGF);//ÉèÖÃÎªÎÄ±¾Ä£Ê½       
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led2=1;
		//delayms(1000);
	  send(CMGD); //É¾³ıµÚÒ»Ìõ¶ÌĞÅ      
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led1=0;
		//delayms(500);
		LCD_display(0x90,"½ÓÊÕ¶ÌĞÅWaiting");
		di();
		di();
		di();
		di();
}
/******************************************
**º¯ÊıÃû³Æ:void Send_Message()
**¹¦ÄÜÃèÊö:·¢ËÍÖĞÎÄ¶ÌĞÅ
**ÊäÈë:
**Êä³ö:
******************************************/
void Send_Message()
{
	  LCD_display(0x90,"                ");
	  LCD_display(0x88,"·¢ËÍ¶ÌĞÅÖĞ...");
    send(AT);       // 1.·¢ËÍATÖ¸Áî
    delayms(500);
    send(MSXZ);     //·¢ËÍ¶ÌĞÅÄ£Ê½Ñ¡Ôñ£¬2.·¢ËÍAT+CMGF=0  ÉèÖÃPDUÄ£Ê½
    delayms(500);
	  led1 =0;
    send(MSCS);     //3.·¢ËÍAT+CSMP=17¡¢167¡¢0¡¢8ÉèÖÃPDUÄ£Ê½²ÎÊı
    delayms(500);
    send(CSCS);     //4.·¢ËÍAT+CSCS=GSM ÉèÖÃGSM×Ö·û¼¯
    delayms(500);
	  led2=0;
    send(MBSJ);// 5.·¢ËÍAT+CMGS=40 ÊäÈëPDU³¤¶È
    delayms(500);
    send(DXNR);//·¢ËÍ¶ÌĞÅÕıÎÄ,6.·¢ËÍ¶ÌĞÅÄÚÈİ£¬´ËÉèÖÃÏÂÖ»ÄÜ·¢ËÍÖĞÎÄ
    send(MESS);
	  Send_Value();
	  led1=1;
	  send(MESS1);
	  delayms(500);
	  send(MESS3);
	  Send_Wetness_Value();
	  led2=1;
    delayms(500);
    SBUF=0x1A;
    //while(TI==0);//Èç¹ûTI=0£»Î´·¢ËÍÍê¡£Ñ­»·µÈ´ı
    //TI=0;//ÒÑ·¢ËÍÍê¡£TIÇåÁã
		di();
		di();
		LCD_display(0x90,"                ");
		LCD_display(0x88,"                ");
		Receive_Message_init();
}

/******************************************
**º¯ÊıÃû³Æ:void Receive_Message()
**¹¦ÄÜÃèÊö:½ÓÊÕĞÅÏ¢²¢½øĞĞÊı¾İ´¦Àí
**ÊäÈë:
**Êä³ö:
******************************************/
void Receive_Message()
{
	  uint j;
   // n=read_ds18b20temp();
	  //write_LCD_command(0x88);
	  //write_LCD_data('T');
	  //write_LCD_data(0x30+n%1000/100);
	  //write_LCD_data(0x30+n%100/10);
		if(strstr(Rec_Buf,"+CMTI")!=NULL)//Èô»º´æ×Ö·û´®ÖĞº¬ÓĞ ¡°+CMTI¡° ¾Í±íÊ¾ÓĞĞÂµÄ¶ÌĞÅ
    {
        CLR_Buf();
			  delayms(1000);
				send(CMGR);//¶ÁÈ¡ĞÅÏ¢
        delayms(1000);
        if(strstr(Rec_Buf,"report")!=NULL)//Èç¹ûĞÅÏ¢ÄÚÈİÎªopen
				{
            //LCD_display(0x90,"·¢ËÍ¶ÌĞÅÖĞ");
					  for(j=0;j<8;j++)
					  {
					      led1=0;
					      delayms(500);
				        led1=1;
				        delayms(500);
					  }
            Send_Message();
				}
				if(strstr(Rec_Buf,"openled")!=NULL)//Èç¹ûĞÅÏ¢ÄÚÈİÎªopen
				{
            LED_3W_OPEN();
				}
				if(strstr(Rec_Buf,"closeled")!=NULL)//Èç¹ûĞÅÏ¢ÄÚÈİÎªopen
				{
            LED_3W_CLOSE();
				}
        CLR_Buf();
        delayms(500);
        send(CMGD);
			  while(!hand("OK")); 
	      CLR_Buf();
        Receive_Message_init();				
    }					
	
}
/******************************************
**º¯ÊıÃû³Æ:void serial()
**¹¦ÄÜÃèÊö:´®¿ÚÖĞ¶Ï´¦Àí
**ÊäÈë:
**Êä³ö:
******************************************/
void serial()interrupt 4
{
    ES=0;
	  if(TI)
		{
        TI=0;
    }
		if(RI)
		{
        RI=0;
			  Rec_Buf[i]=SBUF;
		    i++;
			  if(i>40)
			      i=0;
			  ES=1;
    }
}

