#ifndef __SYSTICK_H
#define	__SYSTICK_H

#include "stm32f10x.h"

void Systick_Configuration(void);
void Systick_DelayMs(u32 mstimes);
void Systick_DelayUs(u32 ustimes);

#endif /* __SYSTICK_H */
