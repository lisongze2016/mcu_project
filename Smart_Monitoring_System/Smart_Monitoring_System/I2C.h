#ifndef __I2C_H__
#define __I2C_H__



extern void I2C_init();
extern void I2C_start();
extern void response();
extern void noresponse();
extern void WriteByte(unsigned char dat);
extern unsigned char ReadByte();
extern void I2C_stop();
#endif