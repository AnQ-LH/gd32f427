#ifndef __BSP_H
#define __BSP_H

#define GD32_F4  


/* ����Ƿ����˿������ͺ� */
#if !defined (GD32_F4)
	#error "Please define the board model : GD32_F4"
#endif

/* ���� BSP �汾�� */
#define __GD32F427_BSP_VERSION		"1.0"

/* CPU����ʱִ�еĺ��� */
//#define CPU_IDLE()		bsp_Idle()

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif


/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_sysTick.h"
#include "bsp_timer.h"
#include "bsp_pwm.h"
#include "bsp_uart.h"
#include "bsp_i2c.h"
#include "bsp_eeprom_24xx.h"
#include "bsp_spi.h"
#include "bsp_spi_flash.h"
#include "bsp_exmc_sram.h"

/* LCD��� */
#include "bsp_tft_port.h"
#include "bsp_nt35310_port.h"
#include "LCD_NT35310.h"

/* ������� */
#include "bsp_ts_touch.h"
#include "bsp_gt1151.h"

/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

#endif	/* __BSP_H */
