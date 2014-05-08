#ifndef _MINT_PORT_H
#define _MINT_PORT_H

#include "m_os.h"

/* ��ֲ��أ�ϵͳ�ڷ��� */
void	m_start_high(void);


#define	NVIC_INT_CTRL				*((volatile uint32_t *)0xE000ED04)	//�жϿ��ƼĴ���ICSR
#define	NVIC_PENDSVSET				0x10000000	//��������жϵ�ֵ��λ28Ϊ1.

#define	m_switch_task()				NVIC_INT_CTRL = NVIC_PENDSVSET

#define	m_enter_critical()			__asm("CPSID   I")		//�����������
#define m_exit_critical()			__asm("CPSIE   I")

#endif
