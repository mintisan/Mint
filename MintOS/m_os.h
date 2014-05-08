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

/* 任务的函数指针类型 */
typedef void (*m_task)(void* pdata);


/* 系统类型定义 */
typedef struct m_tcb{			//任务控制块
	uint32_t* 	sp;
	uint32_t 	ticks;
}m_tcb;



#define USER_TASKS	300
#define SYS_TASKS 	2
#define N_TASKS		(USER_TASKS+SYS_TASKS)

GLOBAL m_tcb tcb[N_TASKS];            //任务数个TCB

/* 记录任务优先级的查询表 */
GLOBAL uint32_t	m_prio_tbl[N_TASKS/32+1];

GLOBAL m_tcb*	m_cur_tcb;        //当前运行任务tcb指针
GLOBAL m_tcb*	m_high_tcb;    //最高就绪任务tcb指针

GLOBAL uint32_t	m_cur_prio;    //当前运行任务优先级
GLOBAL uint32_t	m_high_prio;    //最高就绪任务tcb优先级

GLOBAL uint32_t m_int_nest;        //中断嵌套层数，可不要

GLOBAL uint32_t m_time;               //系统开始运行时间

#define IDLE_PRIO (N_TASKS-1)				//空闲任务优先级，最低
#define IDLE_STK_SIZE	100						//空闲任务优先级的私有栈大小
GLOBAL uint32_t m_idle_stk[IDLE_STK_SIZE];	//栈空间
void m_idle_task(void* pdata);							//空闲任务


/* 系统对内服务 */

void m_sched(void);																//任务调度函数，在此函数内会发生任务切换
uint32_t* m_init_sp(m_task task, uint32_t* sp);		//在任务创建函数m_create_task中被调用，初始化任务的堆栈空间，就像刚被中断过一样

void m_set_prio(uint32_t prio);										//当任务就绪时，在任务就绪表中占个坑
void m_reset_prio(uint32_t prio);									//当任务无法获取资源时，清楚此任务在就绪表中坑
void m_get_high_prio(void);												//获取优先级最高的就绪任务优先级

void m_enter_int(void);														//进入临界区，因为此后需要操作全局变量
void m_tick_time(void);														//系统时钟，放在系统滴答中断SysTick_Handler中
void m_exit_int(void);														//退出临界区，操作完毕

	
/* 系统对外服务 */
void m_init(void);																//系统初始化，此处仅仅用来初始化系统空闲任务
void m_start(void);																//开始运行最高就绪任务，在此之前需要m_create_task一些任务

void m_create_task(m_task task, uint32_t*  sp, uint32_t prio);	//创建任务，在m_start之前创建一些任务供MOS调度
void m_delete_task(uint32_t prio);															//删除任务，从MOS中消失,但由于内存空间是静态分配，所以还是占着内存

void m_delay_time(uint32_t prio, uint32_t ticks);								//延时函数，用于让出MCU的资源

#endif
