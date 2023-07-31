#include "bsp.h"

/**
 * @name   bsp_Init
 * @brief  ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
 * @param  ��
 * @retval ��
 */
void bsp_Init(void)
{
	/*
		����GD�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����

		ϵͳʱ��ȱʡ����Ϊ200MHz�������Ҫ���ģ������޸� system_gd32f4xx.c �ļ�
	*/
	   
	bsp_InitKey();    		/* ������ʼ����Ҫ���ڵδ�ʱ��֮ǰ����Ϊ��ť�����ͨ���δ�ʱ��ɨ�� */
	bsp_InitSystick();  	/* ��ʼ���δ�ʱ�� */
	bsp_InitLed();    		/* ��ʼ��LED */	
	bsp_InitHardTimer();	/* ��ʼ��Ӳ����ʱ�� */
	bsp_InitUart();			/* ��ʼ������ */
	
//	bsp_InitExtSRAM();		/* ��ʼ���ⲿSRAM */
//	bsp_InitI2C();			/* ����I2C���� */	
	
	TOUCH_InitHard();		/* ��ʼ������оƬ��LCD����ͺŵļ��Ҳ�ڴ˺���������Ҫ�ں���LCD_InitHardǰ���� */ 
	LCD_InitHard();			/* ��ʼ��LCD */		
	
	
//	bsp_InitSPIBus();		/* ����SPI���� */		
//	bsp_InitSFlash();		/* ��ʼ��SPI ����Flash */
	
}

/**
 * @name   bsp_RunPer10ms
 * @brief  �ú���ÿ��10ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ����ʱ��Ҫ���ϸ��
		   ������Է��ڴ˺��������磺����ɨ�衢���������п��Ƶȡ�
 * @param  ��
 * @retval ��
 */
void bsp_RunPer10ms(void)
{
	bsp_KeyScan10ms();
}

/**
 * @name   bsp_RunPer1ms
 * @brief  �ú���ÿ��1ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ��������
		   ���Է��ڴ˺��������磺��������ɨ�衣
 * @param  ��
 * @retval ��
 */
void bsp_RunPer1ms(void)
{
	TOUCH_Scan();	/* ������ */	
}

/**
 * @name   bsp_Idle
 * @brief  ����ʱִ�еĺ�����һ����������for��whileѭ������������Ҫ���� CPU_IDLE() �������ñ�������
		   ������ȱʡΪ�ղ������û��������ι��������CPU��������ģʽ�Ĺ��ܡ�
 * @param  ��
 * @retval ��
 */
void bsp_Idle(void)
{
	/* --- ι�� */

	/* --- ��CPU�������ߣ���Systick��ʱ�жϻ��ѻ��������жϻ��� */

	/* ���� emWin ͼ�ο⣬���Բ���ͼ�ο���Ҫ����ѯ���� */
	//GUI_Exec();

	/* ���� uIP Э�飬���Բ���uip��ѯ���� */
	TOUCH_CapScan();
}


