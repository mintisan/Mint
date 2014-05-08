#include "bsp.h"
#include "m_os.h"

#define OS_TICKS_PER_SEC	1000	 		/* 设置每秒的节拍数目 */

void BSP_Init(void)
{
	LED_Configuration();
	USART1_Configuration(115200);
}


//系统时钟配置，设计1ms产生一次中断
void SysTick_Configuration(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	
	SysTick_Config(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);
}

