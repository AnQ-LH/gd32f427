#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "gd32f4xx.h"

/* 提供给其他C文件调用的函数 */

void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

#endif  /* __BSP_TIMER_H */