#include "bsp.h"
#include "m_os.h"

#define OS_TICKS_PER_SEC	1000	 		/* ����ÿ��Ľ�����Ŀ */

void BSP_Init(void)
{
	LED_Configuration();
	USART1_Configuration(115200);
}


//ϵͳʱ�����ã����1ms����һ���ж�
void SysTick_Configuration(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	
	SysTick_Config(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);
}

