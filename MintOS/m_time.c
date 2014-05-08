
#include "m_os.h"


/*
	延时函数，用于让出MCU资源
*/
void m_delay_time(uint32_t prio, uint32_t ticks)
{
	m_enter_critical();
	tcb[prio].ticks = ticks;
	m_reset_prio(prio);
	m_exit_critical();
	
	m_sched();
}
