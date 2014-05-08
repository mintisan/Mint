#include "m_os.h"
#include "bsp.h"
#include "m_app.h"


int main(void)
{
	BSP_Init();		//板级资源初始化
	
	m_init();			//MOS系统初始化

	//创建启动任务
	m_create_task(StartTask,&START_STK[TASK_START_SIZE-1],Start_Prio);
	
	//启动MOS系统
	m_start();
	
	//永远不会返回
	return 0;
}




