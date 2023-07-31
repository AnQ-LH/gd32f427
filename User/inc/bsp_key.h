#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "gd32f4xx.h"

/* ����Ӧ�ó���Ĺ��������������� */
#define KEY_DOWN_K1		KEY_1_DOWN
#define KEY_UP_K1		KEY_1_UP
#define KEY_LONG_K1		KEY_1_LONG
#define KEY_DOWN_K2		KEY_2_DOWN
#define KEY_UP_K2		KEY_2_UP
#define KEY_LONG_K2		KEY_2_LONG

/* ����ID, ��Ҫ����bsp_KeyState()��������ڲ��� */
typedef enum
{
	KID_K1 = 0,
	KID_K2
}KEY_ID_E;

/**
 * �����˲�ʱ��50ms, ��λ10ms��
 * ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
 * ��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define KEY_FILTER_TIME   5
#define KEY_LONG_TIME     100			/* ��λ10ms�� ����1�룬��Ϊ�����¼� */

/**
 *ÿ��������Ӧ1��ȫ�ֵĽṹ�������
 */
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	uint8_t (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	uint8_t  Count;			/* �˲��������� */
	uint16_t LongCount;		/* ���������� */
	uint16_t LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	uint8_t  State;			/* ������ǰ״̬�����»��ǵ��� */
	uint8_t  RepeatSpeed;	/* ������������ */
	uint8_t  RepeatCount;	/* �������������� */
}KEY_T;

/**
 * �����ֵ����, ���밴���´���ʱÿ�����İ��¡�����ͳ����¼�
 * �Ƽ�ʹ��enum, ����#define��ԭ��
 * (1) ����������ֵ,�������˳��ʹ���뿴���������
 * (2) �������ɰ����Ǳ����ֵ�ظ���
 */
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_1_DOWN,				/* 1������ */
	KEY_1_UP,				/* 1������ */
	KEY_1_LONG,				/* 1������ */
	
	KEY_2_DOWN,				/* 1������ */
	KEY_2_UP,				/* 1������ */
	KEY_2_LONG,				/* 1������ */

}KEY_ENUM;

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	10

typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	uint8_t Read;					/* ��������ָ��1 */
	uint8_t Write;					/* ������дָ�� */
	uint8_t Read2;					/* ��������ָ��2 */
}KEY_FIFO_T;

/* A function declaration for external calls */
void bsp_InitKey(void);
void bsp_KeyScan10ms(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);

#endif /* __BSP_KEY_H */
