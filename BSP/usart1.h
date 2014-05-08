#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"
#include <stdio.h>

void USART1_Configuration(u32 baudrate);
int fputc(int ch, FILE *f);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);

#endif /* __USART_H */

