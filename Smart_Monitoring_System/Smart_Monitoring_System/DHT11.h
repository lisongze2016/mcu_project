#ifndef __DHT11_H__
#define __DHT11_H__
extern unsigned char RH_data,RL_data;
//extern unsigned char DHTll_RL,DHTll_RH;
extern void delay_us();
extern void delay_ms(unsigned int t);
extern unsigned char read_byte();
extern void init_DHT11();

#endif