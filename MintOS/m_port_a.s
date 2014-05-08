;/*********************** (C) COPYRIGHT 2010 Libraworks *************************
;* File Name	: m_cpu_a.asm 
;* Author		: Librae 
;* Version		: V1.0
;* Date			: 06/10/2010
;* Description	: μCOS-II asm port
;*******************************************************************************/
		;申明这些变量是在其他文件定义的，本文件只做引用
        IMPORT  m_cur_prio
        IMPORT  m_high_prio
        IMPORT  m_cur_tcb
        IMPORT  m_high_tcb
         
        ;申明这些函数是在本文件中定义的，给外部引用的。                 
        EXPORT  PendSV_Handler
		EXPORT  m_start_high               
		
		
;定义几个常量，类似C语言中的#define预处理指令。     
NVIC_INT_CTRL   	EQU     0xE000ED04  ; 中断控制寄存器
NVIC_SYSPRI14    	EQU     0xE000ED22  ; PendSV优先级寄存器的地址
NVIC_PENDSV_PRI 	EQU     0x000000FF  ; PendSV中断的优先级为255（最低）
NVIC_PENDSVSET  	EQU     0x10000000  ; 触发软件中断的值，位28为1.

		PRESERVE8 
		
		AREA    |.text|, CODE, READONLY
        THUMB 
		
;********************************************************************************************************
;                                         START MULTITASKING
;                                      void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set the main stack to OSRunning
;              d) Trigger PendSV exception;
;              e) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************
m_start_high
		;设置PendSV中断的优先级
        LDR     R4, =NVIC_SYSPRI14      ; set the PendSV exception priority
        LDR     R5, =NVIC_PENDSV_PRI
        STR     R5, [R4]
		;设置PSP为0
        MOV     R4, #0                 ; set the PSP to 0 for initial context switch call
        MSR     PSP, R4

        ;触发PendSV中断
        LDR     R4, =NVIC_INT_CTRL     ;rigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]

        CPSIE   I                      ;enable interrupts at processor level
		;死循环，应该不会到这里
OSStartHang
        B       OSStartHang            ;should never get here

;********************************************************************************************************
;                                       HANDLE PendSV EXCEPTION
;                                   void PendSV_Handler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCurPtr->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCurPtr = OSTCBHighRdyPtr;
;              g) Get new process SP from TCB, SP = OSTCBHighRdyPtr->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCurPtr      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdyPtr  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************
PendSV_Handler
	;xPSR, PC, LR, R12, R0-R3已自动保存
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer 如果在用PSP堆栈,则可以忽略保存寄存器,参考CM3权威中的双堆栈-白菜注
    CBZ     R0, PendSV_Handler_Nosave		                    ; Skip register save the first time
	
    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =m_cur_tcb                                       ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
PendSV_Handler_Nosave
    LDR     R0, =m_cur_prio                                      ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =m_high_prio
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =m_cur_tcb                                       ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =m_high_tcb
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context

    	ALIGN
					
        END
        
;/*********************** (C) COPYRIGHT 2010 Libraworks ************************/
