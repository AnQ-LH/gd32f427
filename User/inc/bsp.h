#ifndef __BSP_H
#define __BSP_H

#define GD32_F4  


/* 检查是否定义了开发板型号 */
#if !defined (GD32_F4)
	#error "Please define the board model : GD32_F4"
#endif

/* 定义 BSP 版本号 */
#define __GD32F427_BSP_VERSION		"1.0"

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()		bsp_Idle()

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
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


/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
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

/* LCD相关 */
#include "bsp_tft_port.h"
#include "bsp_nt35310_port.h"
#include "LCD_NT35310.h"

/* 触摸相关 */
#include "bsp_ts_touch.h"
#include "bsp_gt1151.h"

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);

#endif	/* __BSP_H */
