#ifndef __BSP_USART_FIFO_H
#define __BSP_USART_FIFO_H

#include "gd32f4xx.h"

#define	UART0_FIFO_EN	1
#define	UART1_FIFO_EN	0
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0


/* ����˿ں� */
typedef enum
{
	COM0 = 0,	/* USART0 */
	COM1 = 1,	/* USART1 */
	COM2 = 2,	/* USART2 */
	COM3 = 3,	/* UART3 */
	COM4 = 4,	/* UART4 */
	COM5 = 5,	/* USART5 */
}COM_PORT_E;

/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
#if UART0_FIFO_EN == 1
	#define UART0_BAUD			115200
	#define UART0_TX_BUF_SIZE	1*1024
	#define UART0_RX_BUF_SIZE	1*1024
#endif

#if UART1_FIFO_EN == 1
	#define UART1_BAUD			9600
	#define UART1_TX_BUF_SIZE	10
	#define UART1_RX_BUF_SIZE	2*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			115200
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			115200
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

/* �����豸�ṹ�� */
typedef struct
{
	uint32_t uart;				/* GD32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
	uint8_t Sending;			/* ���ڷ����� */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);

void bsp_SetUartParam(uint32_t USARTx,  uint32_t BaudRate, uint32_t Parity);

uint8_t UartTxEmpty(COM_PORT_E _ucPort);

#endif  /* __BSP_USART_FIFO_H */
