#include "m_os.h"
#include "bsp.h"
#include "m_app.h"


int main(void)
{
	BSP_Init();		//�弶��Դ��ʼ��
	
	m_init();			//MOSϵͳ��ʼ��

	//������������
	m_create_task(StartTask,&START_STK[TASK_START_SIZE-1],Start_Prio);
	
	//����MOSϵͳ
	m_start();
	
	//��Զ���᷵��
	return 0;
}




