#include "m_os.h"

/*
	�����������ʱ���������������ռ���ӣ��˺���
	����m_tick_time�б������ʱ���񣬵�������Ϊ����
*/
void m_set_prio(uint32_t prio)
{
	m_prio_tbl[prio >> 5] |= (0x01 << (prio % 32 ));
}

/*
	��m_set_prioִ���෴�Ĳ���������ʱm_delay_time��ʱ�򱻵���
*/
void m_reset_prio(uint32_t prio)
{
	m_prio_tbl[prio >> 5] &= ~(0x01 << (prio % 32 ));
}

/*
	��m_start��m_sched�б����ã���ΪMOS�����������ȼ���ߵľ�������
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
	����һ���µ����񣬲����ο�ucos
*/
void m_create_task(m_task task, uint32_t* sp, uint32_t prio)
{
	tcb[prio].sp = m_init_sp(task,sp);
	tcb[prio].ticks = 0;
	m_set_prio(prio);
}

/* 
	����ɾ����������ɾ������״̬������
	ֻ�е���ɾ��������RUNNING״̬������������ȡ�
*/
void m_delete_task(uint32_t prio)
{
	m_reset_prio(prio);
	m_get_high_prio();
	if( prio == m_cur_prio)
		m_sched();
}

/*
	��m_init���ã���MCU����ʱ���д�����
	�����ٴ˴�����hook����ɶ�������ϵͳ��״̬
*/
void m_idle_task(void* pdata)
{
	static uint32_t counter;
	while(1)
	{
		counter++;
	}
}







