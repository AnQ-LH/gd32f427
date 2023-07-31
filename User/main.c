#include "main.h"

/*!
    \brief	main function
    \retval	none
*/
int main(void)
{
	/*
		GD�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� gd32f4xx.c �ļ�����Ҫ����������CPUϵͳ��ʱ��
	*/
	bsp_Init();
	
	bsp_StartAutoTimer(0, 100);	/* ����1��100ms���Զ���װ�Ķ�ʱ�� */
	
	while(1)
	{
		bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */
		
		/* �ж϶�ʱ����ʱʱ�� */
		if (bsp_CheckTimer(0))	
		{
			/* ÿ��100ms ����һ�� */  
			bsp_LedToggle(LED1);
		}
	}
	
	return 0;
}


