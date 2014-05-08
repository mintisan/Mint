/* ��ϵͳ����������core�ļ��� */
#define GLOBAL_VAR

#include "m_os.h"


void m_init(void)
{
	m_create_task(m_idle_task,&m_idle_stk[IDLE_STK_SIZE-1],IDLE_PRIO);
}

/*
	1. ��ȡ�����������������ȼ�
	2. ����һЩȫ�ֱ���
	3. ������ȼ���������
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
	1. ���˱���ʱ���������ˣ����Կ�����û�и��ߵľ���������
	2. �������
		2.1 ����������ȼ��Լ���ǰ���ȼ�ջ��ַ
		2.2 �л�����
*/
void m_sched(void)
{
	m_get_high_prio();
	m_enter_critical();
	if(m_high_prio != m_cur_prio)
	{
		/* �˴���˳�����н����ģ��������л���Ҫ���Ǿɵ�ջ��ַm_cur_tcb���µ�ջ��ַm_high_tcb���Լ��µ����ȼ�m_cur_prio */
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
	����OSһ�����ҽ����ж���
*/
void m_enter_int(void)
{
	m_int_nest++;
}
/*
	1. ��¼ϵͳ����ʱ��+1
	2. ����TCB��delay������һ
	3. �ٿ�����û��������ʱ����
	4. ����У���
		4.1 ��������״̬
*/
void m_tick_time(void)
{
	uint32_t i;
	m_time++;
	for(i = 0;i < N_TASKS;i++)		//����ѭ����ʡ�ռ䣩 or ���������ʡʱ�䣩
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
	1. Ƕ�ײ�����һ
	2. ����������Ⱥ���������û�и����ȼ����񱻾���
*/
void m_exit_int(void)
{
	m_int_nest--;
	m_sched();
}


