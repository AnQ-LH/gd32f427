#ifndef __BSP_TFT_PORT_H
#define __BSP_TFT_PORT_H


#include "gd32f4xx.h"

/* 定义LCD显示区域的分辨率 */
#define LCD_35_HEIGHT	320		/* 3.0寸宽屏 高度，单位：像素 */
#define LCD_35_WIDTH	480		/* 3.0寸宽屏 宽度，单位：像素 */

/* 支持的驱动芯片ID */
enum
{
	IC_5310		= 0x5310
};

#define CHIP_STR_5310	"NT35310"

//PB15

/* 背景光控制 */
#define BRIGHT_MAX		255
#define BRIGHT_MIN		0
#define BRIGHT_DEFAULT	200
#define BRIGHT_STEP		5

typedef struct
{
	void (*SetBackLight)(uint8_t _bright);
	uint8_t (*GetBackLight)(void);		
	void (*DispOn)(void);
	void (*DispOff)(void);
	void (*ClrScr)(uint16_t _usColor);
	void (*PutPixel)(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
	uint16_t (*GetPixel)(uint16_t _usX, uint16_t _usY);
	void (*DrawLine)(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
	void (*DrawRect)(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
	void (*DrawCircle)(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
	void (*DrawBMP)(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
	void (*FillRect)(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);	
}LCD_DEV_T;

/* 可供外部模块调用的函数 */
void LCD_InitHard(void);
void LCD_GetChipDescribe(char *_str);
uint16_t LCD_GetHeight(void);
uint16_t LCD_GetWidth(void);

void LCD_DispOn(void);
void LCD_DispOff(void);

void LCD_ClrScr(uint16_t _usColor);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY);
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void LCD_DrawRect(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor);
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _ueX, uint16_t _ueY, uint16_t _usColor);

void LCD_SetBackLight(uint8_t _bright);
uint8_t LCD_GetBackLight(void);
void LCD_SetDirection(uint8_t _dir);

void SOFT_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void SOFT_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void SOFT_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode);
void SOFT_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void SOFT_FillQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode);
void SOFT_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor);
void SOFT_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor);	


/* 下面3个变量，主要用于使程序同时支持不同的屏 */
extern uint16_t g_ChipID;			/* 驱动芯片ID */
extern uint16_t g_LcdHeight;		/* 显示屏分辨率-高度 */
extern uint16_t g_LcdWidth;			/* 显示屏分辨率-宽度 */
extern uint8_t g_LcdDirection;		/* 显示方向.0，1，2，3 */

extern LCD_DEV_T g_tLCD;

#endif	/* __BSP_TFT_PORT_H */


