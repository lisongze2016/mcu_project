#ifndef __DS18B20_H__
#define __DS18B20_H__


extern void ds18b20rst();
extern unsigned char ds18b20read();
extern void ds18b20write(unsigned char com);
extern unsigned int read_ds18b20temp();



#endif