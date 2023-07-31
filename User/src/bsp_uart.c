#include "bsp_uart.h"
#include "bsp.h"

/* ����0��GPIO  PA9, PA10 */
#define USART0_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART0)

#define USART0_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART0_TX_GPIO_PORT              GPIOA
#define USART0_TX_PIN                    GPIO_PIN_9
#define USART0_TX_AF                     GPIO_AF_7

#define USART0_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART0_RX_GPIO_PORT              GPIOA
#define USART0_RX_PIN                    GPIO_PIN_10
#define USART0_RX_AF                     GPIO_AF_7

/* ����1��GPIO  PA2 PA3 */
#define USART1_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART1)

#define USART1_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_PIN                    GPIO_PIN_2
#define USART1_TX_AF                     GPIO_AF_7

#define USART1_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_3
#define USART1_RX_AF                     GPIO_AF_7

/* ����2��GPIO  PD8 PD9 */
#define USART2_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART2);

#define USART2_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD);
#define USART2_TX_GPIO_PORT              GPIOD
#define USART2_TX_PIN                    GPIO_PIN_8
#define USART2_TX_AF                     GPIO_AF_7

#define USART2_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD);
#define USART2_RX_GPIO_PORT              GPIOD
#define USART2_RX_PIN                    GPIO_PIN_9
#define USART2_RX_AF                     GPIO_AF_7

/* ����3��GPIO  PC10 PC11 */
#define UART3_CLK_ENABLE()              rcu_periph_clock_enable(RCU_UART3);

#define UART3_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC);
#define UART3_TX_GPIO_PORT              GPIOC
#define UART3_TX_PIN                    GPIO_PIN_10
#define UART3_TX_AF                     GPIO_AF_8

#define UART3_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC);
#define UART3_RX_GPIO_PORT              GPIOC
#define UART3_RX_PIN                    GPIO_PIN_11
#define UART3_RX_AF                     GPIO_AF_8

/* ����4��GPIO  PC12/UART5_TX PD2/UART5_RX */
#define UART4_CLK_ENABLE()              rcu_periph_clock_enable(RCU_UART4)

#define UART4_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define UART4_TX_GPIO_PORT              GPIOC
#define UART4_TX_PIN                    GPIO_PIN_12
#define UART4_TX_AF                     GPIO_AF_8

#define UART4_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD)
#define UART4_RX_GPIO_PORT              GPIOD
#define UART4_RX_PIN                    GPIO_PIN_2
#define UART4_RX_AF                     GPIO_AF_8

/* ����5��GPIO  PG14 PC7 */
#define USART5_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART5)

#define USART5_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define USART5_TX_GPIO_PORT              GPIOC
#define USART5_TX_PIN                    GPIO_PIN_6
#define USART5_TX_AF                     GPIO_AF_7

#define USART5_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define USART5_RX_GPIO_PORT              GPIOC
#define USART5_RX_PIN                    GPIO_PIN_7
#define USART5_RX_AF                     GPIO_AF_7


/* ����ÿ�����ڽṹ����� */
#if UART0_FIFO_EN == 1
	static UART_T g_tUart0;
	static uint8_t g_TxBuf0[UART0_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf0[UART0_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* ���ջ����� */
#endif

		
static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);


/**
 * @name   bsp_InitUart
 * @brief  ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
 * @param  ��
 * @retval ��
 */
void bsp_InitUart(void)
{
	
	UartVarInit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */

	InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */
}

/**
 * @name   ComToUart
 * @brief  ��COM�˿ں�ת��ΪUARTָ��
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @retval uartָ��
 */
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM0)
	{
		#if UART0_FIFO_EN == 1
			return &g_tUart0;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}

/**
 * @name   ComToUSARTx
 * @brief  ��COM�˿ں�ת��Ϊ USART_TypeDef* USARTx
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @retval USART_TypeDef*  USART1,USART2,USART3,UART4,UART5,USART6,UART7,UART8.
 */
uint32_t ComToUSARTx(COM_PORT_E _ucPort)
{
	if (_ucPort == COM0)
	{
		#if UART0_FIFO_EN == 1
			return USART0;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return USART1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return USART2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return UART3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return UART4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return USART5;
		#else
			return 0;
		#endif
	}
	
	else
	{
		/* �����κδ��� */
		return 0;
	}
}

/**
 * @name   comSendBuf
 * @brief  �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @param  _ucaBuf: �����͵����ݻ�����
 * @param  _usLen : ���ݳ���
 * @retval ��
 */
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/**
 * @name   comSendChar
 * @brief  �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @param  _ucByte: �����͵�����
 * @retval ��
 */
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/**
 * @name   comGetChar
 * @brief  �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ���������.
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @param  _pByte: ���յ������ݴ���������ַ
 * @retval 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
 */
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}

/**
 * @name   comClearTxFifo
 * @brief  ���㴮�ڷ��ͻ�����
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @retval ��
 */
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/**
 * @name   comClearRxFifo
 * @brief  ���㴮�ڽ��ջ�����
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @retval ��
 */
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}

/**
 * @name   comSetBaud
 * @brief  ���ô��ڵĲ�����. �������̶�����Ϊ��У��,�շ���ʹ��ģʽ
 * @param  _ucPort: �˿ں�(COM1 - COM8)
 * @param  _BaudRate: ������  8��������  ������.0-12.5Mbps
							 16�������� ������.0-6.25Mbps
 * @retval ��
 */
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
	uint32_t USARTx;
	
	USARTx = ComToUSARTx(_ucPort);
	if (USARTx == 0)
	{
		return;
	}
	
	bsp_SetUartParam(USARTx, _BaudRate, USART_PM_NONE);
}

/**
 * @name   UartVarInit
 * @brief  ��ʼ��������صı���
 * @param  ��
 * @retval ��
 */
static void UartVarInit(void)
{
#if UART0_FIFO_EN == 1
	g_tUart0.uart = USART0;						/* STM32 �����豸 */
	g_tUart0.pTxBuf = g_TxBuf0;					/* ���ͻ�����ָ�� */
	g_tUart0.pRxBuf = g_RxBuf0;					/* ���ջ�����ָ�� */
	g_tUart0.usTxBufSize = UART0_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart0.usRxBufSize = UART0_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart0.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart0.usTxRead = 0;						/* ����FIFO������ */
	g_tUart0.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart0.usRxRead = 0;						/* ����FIFO������ */
	g_tUart0.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart0.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart0.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart0.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart0.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart0.Sending = 0;						/* ���ڷ����б�־ */
#endif

#if UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart1.Sending = 0;						/* ���ڷ����б�־ */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 �����豸 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usTxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usRxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart2.Sending = 0;						/* ���ڷ����б�־ */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = UART3;						/* STM32 �����豸 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* ���ͻ�����ָ�� */
	g_tUart3.pRxBuf = g_RxBuf3;					/* ���ջ�����ָ�� */
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart3.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usTxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usRxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart3.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart3.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart3.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart3.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart3.Sending = 0;						/* ���ڷ����б�־ */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 �����豸 */
	g_tUart4.pTxBuf = g_TxBuf4;					/* ���ͻ�����ָ�� */
	g_tUart4.pRxBuf = g_RxBuf4;					/* ���ջ�����ָ�� */
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart4.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usTxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usRxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart4.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart4.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart4.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart4.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart4.Sending = 0;						/* ���ڷ����б�־ */
#endif


#if UART5_FIFO_EN == 1
	g_tUart5.uart = USART5;						/* STM32 �����豸 */
	g_tUart5.pTxBuf = g_TxBuf5;					/* ���ͻ�����ָ�� */
	g_tUart5.pRxBuf = g_RxBuf5;					/* ���ջ�����ָ�� */
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart5.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usTxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usRxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart5.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart5.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart5.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart5.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart5.Sending = 0;						/* ���ڷ����б�־ */
#endif

}

/**
 * @name   bsp_SetUartParam
 * @brief  ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32- H7������
 * @param  UARTx   USART_TypeDef���ͽṹ��
 * @param  BaudRate   ������
 * @param  Parity     У�����ͣ���У�����żУ��
 * @retval ��
 */
void bsp_SetUartParam(uint32_t USARTx,  uint32_t BaudRate, uint32_t Parity)
{
	
	/*##-1- ���ô���Ӳ������ ######################################*/
	/* �첽����ģʽ (UART Mode) */
	/* ��������:
	  - �ֳ�    = 8 λ
	  - ֹͣλ  = 1 ��ֹͣλ
	  - У��    = ����Parity
	  - ������  = ����BaudRate
	  - Ӳ�������ƹر� (RTS and CTS signals) */
	usart_baudrate_set(USARTx, BaudRate);
	usart_word_length_set(USARTx, USART_WL_8BIT);
	usart_stop_bit_set(USARTx, USART_STB_1BIT);
	usart_parity_config(USARTx, Parity);
	usart_hardware_flow_rts_config(USARTx, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USARTx, USART_CTS_DISABLE);
	usart_receive_config(USARTx, USART_RECEIVE_ENABLE);
	usart_transmit_config(USARTx, USART_TRANSMIT_ENABLE);
	
	usart_enable(USARTx);
}

/**
 * @name   InitHardUart
 * @brief  ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32- H7������
 * @param  ��
 * @retval ��
 */
static void InitHardUart(void)
{

#if UART0_FIFO_EN == 1		/* ����1 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART0_TX_GPIO_CLK_ENABLE();
	USART0_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART0_CLK_ENABLE();	

	/* ����TX���� */
	gpio_af_set(USART0_TX_GPIO_PORT, USART0_TX_AF, USART0_TX_PIN);
	gpio_mode_set(USART0_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_TX_PIN);
	gpio_output_options_set(USART0_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(USART0_RX_GPIO_PORT, USART0_RX_AF, USART0_RX_PIN);
	gpio_mode_set(USART0_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_RX_PIN);
	gpio_output_options_set(USART0_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_RX_PIN);

	/* ����NVIC the NVIC for UART */
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART0_IRQn, 0, 1);;
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(USART0,  UART0_BAUD, USART_PM_NONE);

	USART_STAT0(USART0) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(USART0) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(USART0) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */

#endif

#if UART1_FIFO_EN == 1		/* ����2 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART1_CLK_ENABLE();	
	
	/* ����TX���� */
	gpio_af_set(USART1_TX_GPIO_PORT, USART1_TX_AF, USART1_TX_PIN);
	gpio_mode_set(USART1_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_TX_PIN);
	gpio_output_options_set(USART1_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART1_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(USART1_RX_GPIO_PORT, USART0_RX_AF, USART1_RX_PIN);
	gpio_mode_set(USART1_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_RX_PIN);
	gpio_output_options_set(USART1_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART1_RX_PIN);
	

	/* ����NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART1_IRQn, 0, 2);;
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(USART1,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART1) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(USART1) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(USART1) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */
#endif

#if UART2_FIFO_EN == 1			/* ����3 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART2_CLK_ENABLE();	

	/* ����TX���� */
	gpio_af_set(USART2_TX_GPIO_PORT, USART1_TX_AF, USART2_TX_PIN);
	gpio_mode_set(USART2_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART2_TX_PIN);
	gpio_output_options_set(USART2_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART2_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(USART2_RX_GPIO_PORT, USART0_RX_AF, USART2_RX_PIN);
	gpio_mode_set(USART2_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART2_RX_PIN);
	gpio_output_options_set(USART2_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART2_RX_PIN);
	

	/* ����NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART2_IRQn, 0, 3);
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(USART2,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART2) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(USART2) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(USART2) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */
#endif

#if UART3_FIFO_EN == 1			/* ����4 TX = PC10   RX = PC11 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	UART3_TX_GPIO_CLK_ENABLE();
	UART3_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	UART3_CLK_ENABLE();	

	/* ����TX���� */
	gpio_af_set(UART3_TX_GPIO_PORT, USART1_TX_AF, UART3_TX_PIN);
	gpio_mode_set(UART3_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART3_TX_PIN);
	gpio_output_options_set(UART3_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART3_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(UART3_RX_GPIO_PORT, USART0_RX_AF, UART3_RX_PIN);
	gpio_mode_set(UART3_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART3_RX_PIN);
	gpio_output_options_set(UART3_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART3_RX_PIN);
	

	/* ����NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(UART3_IRQn, 0, 4);;
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(UART3, UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(UART3) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(UART3) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(UART3) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */
#endif

#if UART4_FIFO_EN == 1			/* ����5 TX = PC12   RX = PD2 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	UART4_TX_GPIO_CLK_ENABLE();
	UART4_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	UART4_CLK_ENABLE();	

	/* ����TX���� */
	gpio_af_set(UART4_TX_GPIO_PORT, USART1_TX_AF, UART4_TX_PIN);
	gpio_mode_set(UART4_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART4_TX_PIN);
	gpio_output_options_set(UART4_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART4_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(UART4_RX_GPIO_PORT, USART0_RX_AF, UART4_RX_PIN);
	gpio_mode_set(UART4_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART4_RX_PIN);
	gpio_output_options_set(UART4_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART4_RX_PIN);
	

	/* ����NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(UART4_IRQn, 0, 5);;
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(UART4, UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(UART4) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(UART4) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(UART4) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */
#endif

#if UART5_FIFO_EN == 1			/* USART6 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART5_TX_GPIO_CLK_ENABLE();
	USART5_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART5_CLK_ENABLE();	

	/* ����TX���� */
	gpio_af_set(USART5_TX_GPIO_PORT, USART1_TX_AF, USART5_TX_PIN);
	gpio_mode_set(USART5_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART5_TX_PIN);
	gpio_output_options_set(USART5_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART5_TX_PIN);
	
	/* ����RX���� */
	gpio_af_set(USART5_RX_GPIO_PORT, USART0_RX_AF, USART5_RX_PIN);
	gpio_mode_set(USART5_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART5_RX_PIN);
	gpio_output_options_set(USART5_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART5_RX_PIN);
	

	/* ����NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART5_IRQn, 0, 6);;
  
	/* ���ò����ʡ���żУ�� */
	bsp_SetUartParam(USART5,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART5) &= ~USART_STAT0_TC;     /* ���TC������ɱ�־ */
	USART_STAT0(USART5) &= ~USART_STAT0_RBNE;   /* ���RXNE���ձ�־ */

	USART_CTL0(USART5) |= USART_CTL0_RBNEIE;	/* ʹ��PE. RX�����ж� */
#endif

}

/**
 * @name   UartSend
 * @brief  ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
 * @param  ��
 * @retval ��
 */
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
		while (1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
			else if(usCount == _pUart->usTxBufSize)/* ���������������� */
			{
				if((USART_CTL0(_pUart->uart) & USART_CTL0_TBEIE) == 0)
				{
					USART_CTL0(_pUart->uart) |= USART_CTL0_TBEIE;
				}  
			}
		}

		/* �����������뷢�ͻ����� */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}
	
	USART_CTL0(_pUart->uart) |= USART_CTL0_TBEIE;	/* ʹ�ܷ����жϣ��������գ� */
}

/**
 * @name   UartGetChar
 * @brief  �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
 * @param  _pUart : �����豸
 * @param  _pByte : ��Ŷ�ȡ���ݵ�ָ��
 * @retval ��
 */
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* �������д������ͬ���򷵻�0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

/**
 * @name   UartTxEmpty
 * @brief  �жϷ��ͻ������Ƿ�Ϊ��.
 * @param  _pUart : �����豸
 * @retval 1Ϊ��,0Ϊ����.
 */
uint8_t UartTxEmpty(COM_PORT_E _ucPort)
{
   UART_T *pUart;
   uint8_t Sending;
   
   pUart = ComToUart(_ucPort);
   if (pUart == 0)
   {
      return 0;
   }

   Sending = pUart->Sending;

   if (Sending != 0)
   {
      return 0;
   }
   return 1;
}

/**
 * @name   UartIRQ
 * @brief  ���жϷ��������ã�ͨ�ô����жϴ�����.
 * @param  _pUart : �����豸
 * @retval ��
 */
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = USART_STAT0(_pUart->uart);
	uint32_t ctl0its	= USART_CTL0(_pUart->uart);
	uint32_t ctl2its	= USART_CTL2(_pUart->uart);

	/* ��������ж�  */
	if ((isrflags & USART_STAT0_RBNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;

		ch = USART_DATA(_pUart->uart);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* ���磬����MODBUS����������ֽ��� */
			}
		}
	}

	/* �����ͻ��������ж� */
	if ( ((isrflags & USART_STAT0_TBE) != RESET) && (ctl0its & USART_CTL0_TBEIE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_CTL0(_pUart->uart) &= ~(USART_CTL0_TBEIE);

			/* ʹ�����ݷ�������ж� */
			USART_CTL0(_pUart->uart) |= USART_CTL0_TCIE;
		}
		else
		{
			_pUart->Sending = 1;
			
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			USART_DATA(_pUart->uart) = _pUart->pTxBuf[_pUart->usTxRead];
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	if (((isrflags & USART_STAT0_TC) != RESET) && ((ctl0its & USART_CTL0_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_CTL0(_pUart->uart) &= ~(USART_CTL0_TCIE);
			
			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
			
			_pUart->Sending = 0;
		}
		else
		{
			/* ��������£��������˷�֧ */

			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			USART_DATA(_pUart->uart) = _pUart->pTxBuf[_pUart->usTxRead];
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

/**
 * @name   USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler��
 * @brief  USART�жϷ������
 * @param  ��
 * @retval ��
 */
#if UART0_FIFO_EN == 1
void USART0_IRQHandler(void)
{
	UartIRQ(&g_tUart0);
}
#endif

#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
	UartIRQ(&g_tUart2);
}
#endif

#if UART3_FIFO_EN == 1
void UART3_IRQHandler(void)
{
	UartIRQ(&g_tUart3);
}
#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
	UartIRQ(&g_tUart4);
}
#endif

#if UART5_FIFO_EN == 1
void USART5_IRQHandler(void)
{
	UartIRQ(&g_tUart5);
}
#endif

/**
 * @name   fputc
 * @brief  �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
 * @param  ��
 * @retval ��
 */
int fputc(int ch, FILE *f)
{
#if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
	comSendChar(COM0, ch);
	
	return ch;
#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
	/* дһ���ֽڵ�USART1 */
	USART_DATA(USART0) = ch;
	
	/* �ȴ����ͽ��� */
	while((USART_STAT0(USART0) & USART_STAT0_TC) == 0)
	{}
	
	return ch;
#endif
}

/**
 * @name   fgetc
 * @brief  �ض���getc��������������ʹ��getchar�����Ӵ���1��������
 * @param  ��
 * @retval ��
 */
int fgetc(FILE *f)
{

#if 1	/* �Ӵ��ڽ���FIFO��ȡ1������, ֻ��ȡ�����ݲŷ��� */
	uint8_t ucData;

	while(comGetChar(COM0, &ucData) == 0);

	return ucData;
#else
	/* �ȴ����յ����� */
	while((USART_STAT0(USART0) & USART_STAT0_RBNE) == 0)
	{}

	return (int)USART_DATA(USART0);
#endif
}
