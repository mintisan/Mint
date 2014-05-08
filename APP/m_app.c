#include "m_os.h"

/* 定义任务变量 */
#define GLOBAL_APP
#include "m_app.h"
#include "bsp.h"


//启动任务，在创建其他两个任务之后自我毁灭
void StartTask(void *pdata)
{
	SysTick_Configuration();
	
	m_create_task(Task0,&TASK0_STK[TASK0_TCB_SIZE-1],Task0_Prio);
	m_create_task(Task1,&TASK1_STK[TASK1_TCB_SIZE-1],Task1_Prio);
	
	/* 建立其他任务之后删除自己，但是其实空间还是被占据的 */
	m_delete_task(Start_Prio);
}

//控制LED0
void Task0(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		//__current_sp()为MDK自带的特殊函数，可直接获得SP的地址
		uint32_t Task0SP = __current_sp();
		printf("Task0's SP is %X \r\n",Task0SP);
		
		m_delay_time(Task0_Prio,500);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
		m_delay_time(Task0_Prio,500);
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}
}

//控制LED1
void Task1(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		uint32_t Task1SP = __current_sp();
		printf("Task1's SP is %X \r\n",Task1SP);
		
		m_delay_time(Task1_Prio,500);
		GPIO_SetBits(GPIOD, GPIO_Pin_2);	
		m_delay_time(Task1_Prio,500);
		GPIO_ResetBits(GPIOD, GPIO_Pin_2);	

	}
}










