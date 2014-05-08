#ifndef _MINT_PORT_H
#define _MINT_PORT_H

#include "m_os.h"

/* 移植相关，系统内服务 */
void	m_start_high(void);


#define	NVIC_INT_CTRL				*((volatile uint32_t *)0xE000ED04)	//中断控制寄存器ICSR
#define	NVIC_PENDSVSET				0x10000000	//触发软件中断的值，位28为1.

#define	m_switch_task()				NVIC_INT_CTRL = NVIC_PENDSVSET

#define	m_enter_critical()			__asm("CPSID   I")		//单行内敛汇编
#define m_exit_critical()			__asm("CPSIE   I")

#endif
