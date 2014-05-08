#ifndef _M_APP_CFG_H
#define _M_APP_CFG_H

#ifdef GLOBAL_APP
#define APP
#else
#define APP extern
#endif


//�����ջ��С����
#define TASK_START_SIZE	50
#define TASK0_TCB_SIZE 	50
#define TASK1_TCB_SIZE 	50

//�����ջ����
APP uint32_t START_STK[TASK_START_SIZE];
APP uint32_t TASK0_STK[TASK0_TCB_SIZE];
APP uint32_t TASK1_STK[TASK1_TCB_SIZE];
//�������ȼ�����
#define Start_Prio 0
#define Task0_Prio 128
#define Task1_Prio 64
//������
void StartTask(void *pdata);
void Task0(void *pdata);
void Task1(void *pdata);







#endif

