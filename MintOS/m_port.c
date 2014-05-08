#include "m_port.h"


uint32_t* m_init_sp(m_task task, uint32_t* sp)
{
	*(sp) = (uint32_t)0x01000000L;					/*  xPSR                        */
	*(--sp) = (uint32_t)task;						/*  Entry Point of the task  任务入口地址   */
	*(--sp) = (uint32_t)0xFFFFFFFEL;				/*  R14 (LR)  (init value will  */
	*(--sp) = (uint32_t)0x12121212L;				/*  R12                         */
	*(--sp) = (uint32_t)0x03030303L;				/*  R3                          */
	*(--sp) = (uint32_t)0x02020202L;				/*  R2                          */
	*(--sp) = (uint32_t)0x01010101L;				/*  R1                          */
	*(--sp) = (uint32_t)0x00000000L;				/*  R0 : argument  输入参数     */
	*(--sp) = (uint32_t)0x11111111L;				/*  R11                         */
	*(--sp) = (uint32_t)0x10101010L;				/*  R10                         */
	*(--sp) = (uint32_t)0x09090909L;				/*  R9                          */
	*(--sp) = (uint32_t)0x08080808L;				/*  R8                          */
	*(--sp) = (uint32_t)0x07070707L;				/*  R7                          */
	*(--sp) = (uint32_t)0x06060606L;				/*  R6                          */
	*(--sp) = (uint32_t)0x05050505L;				/*  R5                          */
	*(--sp) = (uint32_t)0x04040404L;				/*  R4                          */
	
	return sp;
}


