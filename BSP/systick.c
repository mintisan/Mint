#include "systick.h"

static u32 us=0;
static u32 ms=0;

void Systick_Configuration(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	//选择外部时钟	HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	us=72/8;
	ms=us*1000;
}


void Systick_DelayMs(u32 mstimes)
{
	vu32 temp,n=0;
	while(n++<mstimes)
	{
		SysTick->LOAD=(u32)ms;				//时间加载(SysTick->Load为24bit)
		SysTick->VAL=0x00;						//清空计数器
		SysTick->CTRL=0x01;						//开始倒数
		do
		{
				temp=SysTick->CTRL;
		}
		while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达
	}
	SysTick->VAL=0x00;						//清空计数器
	SysTick->CTRL=0x00;						//关闭计数器
}

void Systick_DelayUs(u32 ustimes)
{
	u32 temp;	    	 
	SysTick->LOAD=ustimes*us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	
	/*
	vu32 temp,n=0;
	while(n++<ustimes)
	{
		SysTick->LOAD=(u32)us;				//时间加载(SysTick->Load为24bit)
		SysTick->VAL=0x00;						//清空计数器
		SysTick->CTRL=0x01;						//开始倒数
		do
		{
				temp=SysTick->CTRL;
		}
		while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达
	}
	SysTick->VAL=0x00;						//清空计数器
	SysTick->CTRL=0x00;						//关闭计数器
	*/
}
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
//systick中断服务函数,使用ucos时用到
void SysTick_Handler(void)
{				   
	OSIntEnter();		//进入中断
  OSTimeTick();       //调用ucos的时钟服务程序               
  OSIntExit();        //触发任务切换软中断
}
#endif
