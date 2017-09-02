LCD12864: 
        sbit RS=P0^5;
	sbit RW=P0^6;
	sbit EN=P0^7;
	Dataport = P2;
LED(接收短信指示灯):P0^0
GSM:
	RXD(P3^0)
	TXD(P3^1)
矩阵键盘：
	P1	
DS18B20:
	P3^6
DHTll:
	P3^7

人体感应：P3^5
AT24C02:
	SDA  P0^3
	SCL  P0^2
ADC0832:
	CS   P3^2
	CLK  P3^3
	DIO  P3^4
继电器：P0^1

Beep：P0^1


