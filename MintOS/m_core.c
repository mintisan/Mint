/* 将系统变量定义在core文件内 */
#define GLOBAL_VAR

#include "m_os.h"


void m_init(void)
{
	m_create_task(m_idle_task,&m_idle_stk[IDLE_STK_SIZE-1],IDLE_PRIO);
}

/*
	1. 获取创建的最高任务的优先级
	2. 设置一些全局变量
	3. 最高优先级任务，走你
*/
void m_start(void)
{
	m_get_high_prio();
	
	m_cur_prio = m_high_prio;
	m_cur_tcb = &tcb[m_cur_prio];
	m_high_tcb = &tcb[m_high_prio];
	
	m_start_high();
}
/* 
	1. 有人被延时（阻塞）了，所以看看有没有更高的就绪任务在
	2. 如果是则
		2.1 设置最该优先级以及当前优先级栈地址
		2.2 切换任务
*/
void m_sched(void)
{
	m_get_high_prio();
	m_enter_critical();
	if(m_high_prio != m_cur_prio)
	{
		/* 此处的顺序是有讲究的，上下文切换需要的是旧的栈地址m_cur_tcb，新的栈地址m_high_tcb，以及新的优先级m_cur_prio */
		m_cur_tcb = &tcb[m_cur_prio];
		m_high_tcb = &tcb[m_high_prio];
		m_cur_prio = m_high_prio;
		m_exit_critical();
		m_switch_task();
	}
	else
	{
		m_exit_critical();
	}
}
/* 
	告诉OS一声：我进入中断了
*/
void m_enter_int(void)
{
	m_int_nest++;
}
/*
	1. 记录系统运行时间+1
	2. 遍历TCB对delay参数减一
	3. 再看看有没有任务延时到期
	4. 如果有，则
		4.1 设置任务状态
*/
void m_tick_time(void)
{
	uint32_t i;
	m_time++;
	for(i = 0;i < N_TASKS;i++)		//数组循环（省空间） or 链表遍历（省时间）
	{
		if(tcb[i].ticks != 0)
		{
			tcb[i].ticks--;
			if(tcb[i].ticks == 0)
			{
				m_set_prio(i);
			}
		}
	}
}
/*
	1. 嵌套层数减一
	2. 启动任务调度函数看看有没有高优先级任务被就绪
*/
void m_exit_int(void)
{
	m_int_nest--;
	m_sched();
}


