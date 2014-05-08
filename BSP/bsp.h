#ifndef _BSP_H
#define _BSP_H

#include "usart1.h"
#include "led.h"

void BSP_Init(void);
//系统时钟配置函数
void SysTick_Configuration(void);

#endif
