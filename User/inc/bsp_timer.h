#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "gd32f4xx.h"

/* �ṩ������C�ļ����õĺ��� */

void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

#endif  /* __BSP_TIMER_H */