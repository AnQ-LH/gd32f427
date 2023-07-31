#ifndef __BSP_TFT_PORT_H
#define __BSP_TFT_PORT_H


#include "gd32f4xx.h"

/* ����LCD��ʾ����ķֱ��� */
#define LCD_35_HEIGHT	320		/* 3.0����� �߶ȣ���λ������ */
#define LCD_35_WIDTH	480		/* 3.0����� ��ȣ���λ������ */

/* ֧�ֵ�����оƬID */
enum
{
	IC_5310		= 0x5310
};

#define CHIP_STR_5310	"NT35310"

//PB15

/* ��������� */
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

/* �ɹ��ⲿģ����õĺ��� */
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


/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
extern uint16_t g_ChipID;			/* ����оƬID */
extern uint16_t g_LcdHeight;		/* ��ʾ���ֱ���-�߶� */
extern uint16_t g_LcdWidth;			/* ��ʾ���ֱ���-��� */
extern uint8_t g_LcdDirection;		/* ��ʾ����.0��1��2��3 */

extern LCD_DEV_T g_tLCD;

#endif	/* __BSP_TFT_PORT_H */


