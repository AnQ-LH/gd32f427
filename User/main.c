#include "main.h"

/*!
    \brief	main function
    \retval	none
*/
int main(void)
{
	/*
		GD固件库中的启动文件已经执行了 SystemInit() 函数，该函数在 gd32f4xx.c 文件，主要功能是配置CPU系统的时钟
	*/
	bsp_Init();
	
	bsp_StartAutoTimer(0, 100);	/* 启动1个100ms的自动重装的定时器 */
	
	while(1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */
		
		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			/* 每隔100ms 进来一次 */  
			bsp_LedToggle(LED1);
		}
	}
	
	return 0;
}


