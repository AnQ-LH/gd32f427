#ifndef __BSP_TS_TOUCH_H
#define __BSP_TS_TOUCH_H

#include "gd32f4xx.h"

#define CALIB_POINT_COUNT	2		/* 2 = 2��У׼�� 4 = �ĵ�У׼ */

#define TOUCH_FIFO_SIZE		20

/* ���������� */
enum
{
	CT_GT1151 = 0	/* 3.5����ݴ���GT1151 480 * 320 */
};

/* LCD������� */
enum
{
	LCD_35_480X320 = 0,	/* 3.5�� 480 * 320 */		
};

typedef struct
{
	/* 2��У׼ �� 4��У׼ */
	uint16_t usAdcX1;	/* ���Ͻ� */
	uint16_t usAdcY1;
	uint16_t usAdcX2;	/* ���½� */
	uint16_t usAdcY2;
	uint16_t usAdcX3;	/* ���½� */
	uint16_t usAdcY3;
	uint16_t usAdcX4;	/* ���Ͻ� */
	uint16_t usAdcY4;
	
	uint16_t usLcdX1;	/* ���Ͻ� */
	uint16_t usLcdY1;
	uint16_t usLcdX2;	/* ���½� */
	uint16_t usLcdY2;
	uint16_t usLcdX3;	/* ���½� */
	uint16_t usLcdY3;
	uint16_t usLcdX4;	/* ���Ͻ� */
	uint16_t usLcdY4;	

	uint16_t XYChange;	/* X, Y �Ƿ񽻻�  */

	uint16_t usMaxAdc;	/* ���������ADCֵ��������Ч���ж�. ��СADC = 0  */
	uint16_t usAdcNowX;
	uint16_t usAdcNowY;

	uint8_t Enable;		/* �������ʹ�ܱ�־ */

	uint8_t Event[TOUCH_FIFO_SIZE];	/* �����¼� */
	int16_t XBuf[TOUCH_FIFO_SIZE];	/* �������껺���� */
	int16_t YBuf[TOUCH_FIFO_SIZE];	/* �������껺���� */
	uint8_t Read;					/* ��������ָ�� */
	uint8_t Write;					/* ������дָ�� */

	
	uint8_t fReqReadTouch;			/* ������������꣬for RA8875 */	
}TOUCH_T;


/* ����ĵ�����У׼���� */
typedef struct
{
	uint8_t CalibPointCount;	/* У׼������2��ʾ2��У׼��4��ʾ4��У׼ */	
	
	uint8_t TouchDirection;	/* ��Ļ���� 0-3  0��ʾ������1��ʾ����180�� 2��ʾ���� 3��ʾ����180�� */
	
	uint8_t XYChange;		/* X, Y �Ƿ񽻻��� 1��ʾi�л���0��ʾ���л�  */
	
	uint16_t usAdcX1;	/* ���Ͻ� */
	uint16_t usAdcY1;
	uint16_t usAdcX2;	/* ���½� */
	uint16_t usAdcY2;
	uint16_t usAdcX3;	/* ���½� */
	uint16_t usAdcY3;
	uint16_t usAdcX4;	/* ���Ͻ� */
	uint16_t usAdcY4;
	
	uint16_t usLcdX1;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdY1;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdX2;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdY2;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdX3;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdY3;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdX4;	/* У׼ʱ����Ļ���� */
	uint16_t usLcdY4;	/* У׼ʱ����Ļ���� */	

}TP_CALIB_PARAM_T;

/* �����¼� */
enum
{
	TOUCH_NONE = 0,		/* �޴��� */
	TOUCH_DOWN = 1,		/* ���� */
	TOUCH_MOVE = 2,		/* �ƶ� */
	TOUCH_RELEASE = 3	/* �ͷ� */
};

/* ���ⲿ���õĺ������� */
void TOUCH_InitHard(void);
uint8_t TOUCH_PenInt(void);
void TOUCH_Calibration(uint8_t _PointCount);

uint16_t TOUCH_ReadAdcX(void);
uint16_t TOUCH_ReadAdcY(void);

uint8_t TOUCH_GetKey(int16_t *_pX, int16_t *_pY);
void TOUCH_CelarFIFO(void);

uint8_t TOUCH_InRect(uint16_t _usX, uint16_t _usY,
uint16_t _usRectX, uint16_t _usRectY, uint16_t _usRectH, uint16_t _usRectW);

void TOUCH_Scan(void);
int32_t TOUCH_Abs(int32_t x);
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY);

void TOUCH_CapScan(void);

static void TOUCH_WaitRelease(void);

extern TOUCH_T g_tTP;
extern uint8_t g_TouchType;
extern uint8_t g_LcdType;

#endif	/* __BSP_TS_TOUCH_H */

