#include "systick.h"

static u32 us=0;
static u32 ms=0;

void Systick_Configuration(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	//ѡ���ⲿʱ��	HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	us=72/8;
	ms=us*1000;
}


void Systick_DelayMs(u32 mstimes)
{
	vu32 temp,n=0;
	while(n++<mstimes)
	{
		SysTick->LOAD=(u32)ms;				//ʱ�����(SysTick->LoadΪ24bit)
		SysTick->VAL=0x00;						//��ռ�����
		SysTick->CTRL=0x01;						//��ʼ����
		do
		{
				temp=SysTick->CTRL;
		}
		while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��
	}
	SysTick->VAL=0x00;						//��ռ�����
	SysTick->CTRL=0x00;						//�رռ�����
}

void Systick_DelayUs(u32 ustimes)
{
	u32 temp;	    	 
	SysTick->LOAD=ustimes*us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	
	/*
	vu32 temp,n=0;
	while(n++<ustimes)
	{
		SysTick->LOAD=(u32)us;				//ʱ�����(SysTick->LoadΪ24bit)
		SysTick->VAL=0x00;						//��ռ�����
		SysTick->CTRL=0x01;						//��ʼ����
		do
		{
				temp=SysTick->CTRL;
		}
		while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��
	}
	SysTick->VAL=0x00;						//��ռ�����
	SysTick->CTRL=0x00;						//�رռ�����
	*/
}
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
//systick�жϷ�����,ʹ��ucosʱ�õ�
void SysTick_Handler(void)
{				   
	OSIntEnter();		//�����ж�
  OSTimeTick();       //����ucos��ʱ�ӷ������               
  OSIntExit();        //���������л����ж�
}
#endif
