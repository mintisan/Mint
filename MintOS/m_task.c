#include "m_os.h"

/*
	在任务就绪的时候，在任务就绪表中占个坑，此函数
	仅在m_tick_time中北检测延时到否，到了设置为就绪
*/
void m_set_prio(uint32_t prio)
{
	m_prio_tbl[prio >> 5] |= (0x01 << (prio % 32 ));
}

/*
	与m_set_prio执行相反的操作，在延时m_delay_time的时候被调用
*/
void m_reset_prio(uint32_t prio)
{
	m_prio_tbl[prio >> 5] &= ~(0x01 << (prio % 32 ));
}

/*
	在m_start和m_sched中北调用，因为MOS总是运行优先级最高的就绪任务
*/
void m_get_high_prio(void)
{
	uint32_t prio = 0;
	uint32_t grp = 0;
	while(m_prio_tbl[grp] == 0)
		grp++;
	
	while((m_prio_tbl[grp] & (0x01 << prio)) == 0)
		prio++;
	m_high_prio = (grp << 5) + prio;
}

/*
	创建一个新的任务，参数参考ucos
*/
void m_create_task(m_task task, uint32_t* sp, uint32_t prio)
{
	tcb[prio].sp = m_init_sp(task,sp);
	tcb[prio].ticks = 0;
	m_set_prio(prio);
}

/* 
	任务删除函数可以删除任意状态的任务，
	只有当其删除自身（即RUNNING状态），会引起调度。
*/
void m_delete_task(uint32_t prio)
{
	m_reset_prio(prio);
	m_get_high_prio();
	if( prio == m_cur_prio)
		m_sched();
}

/*
	被m_init调用，当MCU空闲时运行此任务
	可以再此处加入hook函数啥的来检测系统的状态
*/
void m_idle_task(void* pdata)
{
	static uint32_t counter;
	while(1)
	{
		counter++;
	}
}







