#ifndef __LCD_NT35310_H
#define __LCD_NT35310_H

#include "gd32f4xx.h"

/* 扫描方向定义 */
enum
{
	L2R_U2D = 0,		/* 从左到右,从上到下 */
	L2R_D2U = 1,		/* 从左到右,从下到上 */
	R2L_U2D = 2,		/* 从右到左,从上到下 */
	R2L_D2U = 3,		/* 从右到左,从下到上 */
	
	U2D_L2R = 4,		/* 从上到下,从左到右 */
	U2D_R2L = 5,		/* 从上到下,从右到左 */
	D2U_L2R = 6,		/* 从下到上,从左到右 */
	D2U_R2L = 7,		/* 从下到上,从右到左 */
};
	
uint16_t NT35310_ReadID(void);

void NT35310_WriteReg(uint16_t _ucRegAddr, uint16_t _ucRegValue);
uint16_t NT35310_ReadReg(uint16_t _ucRegAddr);
void NT35310_InitHard(void);
void NT35310_SetDirection(uint16_t _ucDir);

void NT35310_DispOn(void);
void NT35310_DispOff(void);

void NT35310_SetCursor(uint16_t _usX, uint16_t _usY);
void NT35310_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t NT35310_GetPixel(uint16_t _usX, uint16_t _usY);
void NT35310_ClrScr(uint16_t _usColor);
void NT35310_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void NT35310_DrawRect(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor);
void NT35310_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _ueX, uint16_t _ueY, uint16_t _usColor);
void NT35310_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);

#endif	/* __LCD_NT35310_H */

