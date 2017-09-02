#ifndef __IR_H__
#define __IR_H__
extern bit irpro_ok,irok;
extern void tim0_isr (void);
extern void EX1_ISR (void);
extern void TIM0init(void);
extern void EX1init(void);
extern void Ir_work(void);
extern void Ircordpro(void);
#endif