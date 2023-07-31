#include "bsp.h"

/**
 * @name   bsp_Init
 * @brief  初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
 * @param  无
 * @retval 无
 */
void bsp_Init(void)
{
	/*
		由于GD固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为200MHz，如果需要更改，可以修改 system_gd32f4xx.c 文件
	*/
	   
	bsp_InitKey();    		/* 按键初始化，要放在滴答定时器之前，因为按钮检测是通过滴答定时器扫描 */
	bsp_InitSystick();  	/* 初始化滴答定时器 */
	bsp_InitLed();    		/* 初始化LED */	
	bsp_InitHardTimer();	/* 初始化硬件定时器 */
	bsp_InitUart();			/* 初始化串口 */
	
//	bsp_InitExtSRAM();		/* 初始化外部SRAM */
//	bsp_InitI2C();			/* 配置I2C总线 */	
	
	TOUCH_InitHard();		/* 初始化触摸芯片，LCD面板型号的检查也在此函数，所以要在函数LCD_InitHard前调用 */ 
	LCD_InitHard();			/* 初始化LCD */		
	
	
//	bsp_InitSPIBus();		/* 配置SPI总线 */		
//	bsp_InitSFlash();		/* 初始化SPI 串行Flash */
	
}

/**
 * @name   bsp_RunPer10ms
 * @brief  该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
		   任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
 * @param  无
 * @retval 无
 */
void bsp_RunPer10ms(void)
{
	bsp_KeyScan10ms();
}

/**
 * @name   bsp_RunPer1ms
 * @brief  该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
		   可以放在此函数。比如：触摸坐标扫描。
 * @param  无
 * @retval 无
 */
void bsp_RunPer1ms(void)
{
	TOUCH_Scan();	/* 触摸屏 */	
}

/**
 * @name   bsp_Idle
 * @brief  空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
		   本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
 * @param  无
 * @retval 无
 */
void bsp_Idle(void)
{
	/* --- 喂狗 */

	/* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */

	/* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
	//GUI_Exec();

	/* 例如 uIP 协议，可以插入uip轮询函数 */
	TOUCH_CapScan();
}


