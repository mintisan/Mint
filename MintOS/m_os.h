#ifndef _MINT_OS_H
#define _MINT_OS_H

#include <stdint.h>
#include <stdio.h>
#include <stm32f10x.h>

#include "m_port.h"
#include "m_os_cfg.h"


#ifdef GLOBAL_VAR
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* ����ĺ���ָ������ */
typedef void (*m_task)(void* pdata);


/* ϵͳ���Ͷ��� */
typedef struct m_tcb{			//������ƿ�
	uint32_t* 	sp;
	uint32_t 	ticks;
}m_tcb;



#define USER_TASKS	300
#define SYS_TASKS 	2
#define N_TASKS		(USER_TASKS+SYS_TASKS)

GLOBAL m_tcb tcb[N_TASKS];            //��������TCB

/* ��¼�������ȼ��Ĳ�ѯ�� */
GLOBAL uint32_t	m_prio_tbl[N_TASKS/32+1];

GLOBAL m_tcb*	m_cur_tcb;        //��ǰ��������tcbָ��
GLOBAL m_tcb*	m_high_tcb;    //��߾�������tcbָ��

GLOBAL uint32_t	m_cur_prio;    //��ǰ�����������ȼ�
GLOBAL uint32_t	m_high_prio;    //��߾�������tcb���ȼ�

GLOBAL uint32_t m_int_nest;        //�ж�Ƕ�ײ������ɲ�Ҫ

GLOBAL uint32_t m_time;               //ϵͳ��ʼ����ʱ��

#define IDLE_PRIO (N_TASKS-1)				//�����������ȼ������
#define IDLE_STK_SIZE	100						//�����������ȼ���˽��ջ��С
GLOBAL uint32_t m_idle_stk[IDLE_STK_SIZE];	//ջ�ռ�
void m_idle_task(void* pdata);							//��������


/* ϵͳ���ڷ��� */

void m_sched(void);																//������Ⱥ������ڴ˺����ڻᷢ�������л�
uint32_t* m_init_sp(m_task task, uint32_t* sp);		//�����񴴽�����m_create_task�б����ã���ʼ������Ķ�ջ�ռ䣬����ձ��жϹ�һ��

void m_set_prio(uint32_t prio);										//���������ʱ���������������ռ����
void m_reset_prio(uint32_t prio);									//�������޷���ȡ��Դʱ������������ھ������п�
void m_get_high_prio(void);												//��ȡ���ȼ���ߵľ����������ȼ�

void m_enter_int(void);														//�����ٽ�������Ϊ�˺���Ҫ����ȫ�ֱ���
void m_tick_time(void);														//ϵͳʱ�ӣ�����ϵͳ�δ��ж�SysTick_Handler��
void m_exit_int(void);														//�˳��ٽ������������

	
/* ϵͳ������� */
void m_init(void);																//ϵͳ��ʼ�����˴�����������ʼ��ϵͳ��������
void m_start(void);																//��ʼ������߾��������ڴ�֮ǰ��Ҫm_create_taskһЩ����

void m_create_task(m_task task, uint32_t*  sp, uint32_t prio);	//����������m_start֮ǰ����һЩ����MOS����
void m_delete_task(uint32_t prio);															//ɾ�����񣬴�MOS����ʧ,�������ڴ�ռ��Ǿ�̬���䣬���Ի���ռ���ڴ�

void m_delay_time(uint32_t prio, uint32_t ticks);								//��ʱ�����������ó�MCU����Դ

#endif
