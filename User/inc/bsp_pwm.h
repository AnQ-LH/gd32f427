#ifndef __BSP_TIM_PWM_H
#define __BSP_TIM_PWM_H

#include "gd32f4xx.h"

/* 提供给其他C文件调用的函数 */
void bsp_SetTIMOutPWM(uint32_t GPIOx, uint16_t GPIO_Pin, uint32_t TIMx, uint8_t _ucChannel, uint32_t _ulFreq, uint32_t _ulDutyCycle); 
void bsp_SetTIMforInt(uint32_t TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);

#endif  /* __BSP_TIM_PWM_H */
