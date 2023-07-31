#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "gd32f4xx.h"

/* exported types */
typedef enum 
{
    LED1 = 0,
	LED2
} led_typedef_enum;


/* A function declaration for external calls */
void bsp_InitLed(void);
void bsp_LedOn(led_typedef_enum led_num);
void bsp_LedOff(led_typedef_enum led_num);
void bsp_LedToggle(led_typedef_enum led_num);
uint8_t bsp_IsLedOn(led_typedef_enum led_num);

#endif	/* __BSP_LED_H */