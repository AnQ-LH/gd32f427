#ifndef __BSP_NT35310_PORT_H
#define __BSP_NT35310_PORT_H

#include "gd32f4xx.h"

typedef struct
{
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE		((uint32_t)(0x6C000000U | 0x0000007EU))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

void NT35310_ConfigGPIO(void);
void NT35310_Delaly1us(void);
void NT35310_Delaly1ms(void);
void NT35310_WriteCmd(__IO uint16_t _ucRegAddr);
void NT35310_WriteData(__IO uint16_t _ucRegValue);
uint16_t NT35310_ReadData(void);
uint8_t NT35310_ReadStatus(void);
void NT35310_WriteData16(uint16_t _usRGB);
uint16_t NT35310_ReadData16(void);
uint32_t NT35310_GetDispMemAddr(void);

#endif	/* __BSP_NT35310_PORT_H */