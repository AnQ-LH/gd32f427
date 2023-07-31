#include "bsp_uart.h"
#include "bsp.h"

/* 串口0的GPIO  PA9, PA10 */
#define USART0_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART0)

#define USART0_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART0_TX_GPIO_PORT              GPIOA
#define USART0_TX_PIN                    GPIO_PIN_9
#define USART0_TX_AF                     GPIO_AF_7

#define USART0_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART0_RX_GPIO_PORT              GPIOA
#define USART0_RX_PIN                    GPIO_PIN_10
#define USART0_RX_AF                     GPIO_AF_7

/* 串口1的GPIO  PA2 PA3 */
#define USART1_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART1)

#define USART1_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_PIN                    GPIO_PIN_2
#define USART1_TX_AF                     GPIO_AF_7

#define USART1_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOA)
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_3
#define USART1_RX_AF                     GPIO_AF_7

/* 串口2的GPIO  PD8 PD9 */
#define USART2_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART2);

#define USART2_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD);
#define USART2_TX_GPIO_PORT              GPIOD
#define USART2_TX_PIN                    GPIO_PIN_8
#define USART2_TX_AF                     GPIO_AF_7

#define USART2_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD);
#define USART2_RX_GPIO_PORT              GPIOD
#define USART2_RX_PIN                    GPIO_PIN_9
#define USART2_RX_AF                     GPIO_AF_7

/* 串口3的GPIO  PC10 PC11 */
#define UART3_CLK_ENABLE()              rcu_periph_clock_enable(RCU_UART3);

#define UART3_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC);
#define UART3_TX_GPIO_PORT              GPIOC
#define UART3_TX_PIN                    GPIO_PIN_10
#define UART3_TX_AF                     GPIO_AF_8

#define UART3_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC);
#define UART3_RX_GPIO_PORT              GPIOC
#define UART3_RX_PIN                    GPIO_PIN_11
#define UART3_RX_AF                     GPIO_AF_8

/* 串口4的GPIO  PC12/UART5_TX PD2/UART5_RX */
#define UART4_CLK_ENABLE()              rcu_periph_clock_enable(RCU_UART4)

#define UART4_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define UART4_TX_GPIO_PORT              GPIOC
#define UART4_TX_PIN                    GPIO_PIN_12
#define UART4_TX_AF                     GPIO_AF_8

#define UART4_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOD)
#define UART4_RX_GPIO_PORT              GPIOD
#define UART4_RX_PIN                    GPIO_PIN_2
#define UART4_RX_AF                     GPIO_AF_8

/* 串口5的GPIO  PG14 PC7 */
#define USART5_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART5)

#define USART5_TX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define USART5_TX_GPIO_PORT              GPIOC
#define USART5_TX_PIN                    GPIO_PIN_6
#define USART5_TX_AF                     GPIO_AF_7

#define USART5_RX_GPIO_CLK_ENABLE()      rcu_periph_clock_enable(RCU_GPIOC)
#define USART5_RX_GPIO_PORT              GPIOC
#define USART5_RX_PIN                    GPIO_PIN_7
#define USART5_RX_AF                     GPIO_AF_7


/* 定义每个串口结构体变量 */
#if UART0_FIFO_EN == 1
	static UART_T g_tUart0;
	static uint8_t g_TxBuf0[UART0_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf0[UART0_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

		
static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);


/**
 * @name   bsp_InitUart
 * @brief  初始化串口硬件，并对全局变量赋初值.
 * @param  无
 * @retval 无
 */
void bsp_InitUart(void)
{
	
	UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */

	InitHardUart();		/* 配置串口的硬件参数(波特率等) */
}

/**
 * @name   ComToUart
 * @brief  将COM端口号转换为UART指针
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @retval uart指针
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
		/* 不做任何处理 */
		return 0;
	}
}

/**
 * @name   ComToUSARTx
 * @brief  将COM端口号转换为 USART_TypeDef* USARTx
 * @param  _ucPort: 端口号(COM1 - COM8)
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
		/* 不做任何处理 */
		return 0;
	}
}

/**
 * @name   comSendBuf
 * @brief  向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @param  _ucaBuf: 待发送的数据缓冲区
 * @param  _usLen : 数据长度
 * @retval 无
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
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/**
 * @name   comSendChar
 * @brief  向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @param  _ucByte: 待发送的数据
 * @retval 无
 */
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/**
 * @name   comGetChar
 * @brief  从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回.
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @param  _pByte: 接收到的数据存放在这个地址
 * @retval 0 表示无数据, 1 表示读取到有效字节
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
 * @brief  清零串口发送缓冲区
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @retval 无
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
 * @brief  清零串口接收缓冲区
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @retval 无
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
 * @brief  设置串口的波特率. 本函数固定设置为无校验,收发都使能模式
 * @param  _ucPort: 端口号(COM1 - COM8)
 * @param  _BaudRate: 波特率  8倍过采样  波特率.0-12.5Mbps
							 16倍过采样 波特率.0-6.25Mbps
 * @retval 无
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
 * @brief  初始化串口相关的变量
 * @param  无
 * @retval 无
 */
static void UartVarInit(void)
{
#if UART0_FIFO_EN == 1
	g_tUart0.uart = USART0;						/* STM32 串口设备 */
	g_tUart0.pTxBuf = g_TxBuf0;					/* 发送缓冲区指针 */
	g_tUart0.pRxBuf = g_RxBuf0;					/* 接收缓冲区指针 */
	g_tUart0.usTxBufSize = UART0_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart0.usRxBufSize = UART0_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart0.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart0.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart0.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart0.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart0.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart0.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart0.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart0.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart0.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart0.Sending = 0;						/* 正在发送中标志 */
#endif

#if UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32 串口设备 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart1.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart1.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart1.Sending = 0;						/* 正在发送中标志 */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 串口设备 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* 发送缓冲区指针 */
	g_tUart2.pRxBuf = g_RxBuf2;					/* 接收缓冲区指针 */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart2.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart2.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart2.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart2.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart2.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart2.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart2.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart2.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart2.Sending = 0;						/* 正在发送中标志 */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = UART3;						/* STM32 串口设备 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* 发送缓冲区指针 */
	g_tUart3.pRxBuf = g_RxBuf3;					/* 接收缓冲区指针 */
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart3.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart3.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart3.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart3.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart3.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart3.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart3.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart3.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart3.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart3.Sending = 0;						/* 正在发送中标志 */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 串口设备 */
	g_tUart4.pTxBuf = g_TxBuf4;					/* 发送缓冲区指针 */
	g_tUart4.pRxBuf = g_RxBuf4;					/* 接收缓冲区指针 */
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart4.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart4.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart4.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart4.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart4.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart4.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart4.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart4.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart4.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart4.Sending = 0;						/* 正在发送中标志 */
#endif


#if UART5_FIFO_EN == 1
	g_tUart5.uart = USART5;						/* STM32 串口设备 */
	g_tUart5.pTxBuf = g_TxBuf5;					/* 发送缓冲区指针 */
	g_tUart5.pRxBuf = g_RxBuf5;					/* 接收缓冲区指针 */
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart5.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart5.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart5.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart5.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart5.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart5.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart5.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart5.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart5.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart5.Sending = 0;						/* 正在发送中标志 */
#endif

}

/**
 * @name   bsp_SetUartParam
 * @brief  配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32- H7开发板
 * @param  UARTx   USART_TypeDef类型结构体
 * @param  BaudRate   波特率
 * @param  Parity     校验类型，奇校验或者偶校验
 * @retval 无
 */
void bsp_SetUartParam(uint32_t USARTx,  uint32_t BaudRate, uint32_t Parity)
{
	
	/*##-1- 配置串口硬件参数 ######################################*/
	/* 异步串口模式 (UART Mode) */
	/* 配置如下:
	  - 字长    = 8 位
	  - 停止位  = 1 个停止位
	  - 校验    = 参数Parity
	  - 波特率  = 参数BaudRate
	  - 硬件流控制关闭 (RTS and CTS signals) */
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
 * @brief  配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32- H7开发板
 * @param  无
 * @retval 无
 */
static void InitHardUart(void)
{

#if UART0_FIFO_EN == 1		/* 串口1 */
	/* 使能 GPIO TX/RX 时钟 */
	USART0_TX_GPIO_CLK_ENABLE();
	USART0_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART0_CLK_ENABLE();	

	/* 配置TX引脚 */
	gpio_af_set(USART0_TX_GPIO_PORT, USART0_TX_AF, USART0_TX_PIN);
	gpio_mode_set(USART0_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_TX_PIN);
	gpio_output_options_set(USART0_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(USART0_RX_GPIO_PORT, USART0_RX_AF, USART0_RX_PIN);
	gpio_mode_set(USART0_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_RX_PIN);
	gpio_output_options_set(USART0_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_RX_PIN);

	/* 配置NVIC the NVIC for UART */
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART0_IRQn, 0, 1);;
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(USART0,  UART0_BAUD, USART_PM_NONE);

	USART_STAT0(USART0) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(USART0) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(USART0) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */

#endif

#if UART1_FIFO_EN == 1		/* 串口2 */
	/* 使能 GPIO TX/RX 时钟 */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART1_CLK_ENABLE();	
	
	/* 配置TX引脚 */
	gpio_af_set(USART1_TX_GPIO_PORT, USART1_TX_AF, USART1_TX_PIN);
	gpio_mode_set(USART1_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_TX_PIN);
	gpio_output_options_set(USART1_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART1_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(USART1_RX_GPIO_PORT, USART0_RX_AF, USART1_RX_PIN);
	gpio_mode_set(USART1_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_RX_PIN);
	gpio_output_options_set(USART1_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART1_RX_PIN);
	

	/* 配置NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART1_IRQn, 0, 2);;
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(USART1,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART1) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(USART1) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(USART1) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */
#endif

#if UART2_FIFO_EN == 1			/* 串口3 */
	/* 使能 GPIO TX/RX 时钟 */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART2_CLK_ENABLE();	

	/* 配置TX引脚 */
	gpio_af_set(USART2_TX_GPIO_PORT, USART1_TX_AF, USART2_TX_PIN);
	gpio_mode_set(USART2_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART2_TX_PIN);
	gpio_output_options_set(USART2_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART2_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(USART2_RX_GPIO_PORT, USART0_RX_AF, USART2_RX_PIN);
	gpio_mode_set(USART2_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART2_RX_PIN);
	gpio_output_options_set(USART2_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART2_RX_PIN);
	

	/* 配置NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART2_IRQn, 0, 3);
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(USART2,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART2) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(USART2) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(USART2) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */
#endif

#if UART3_FIFO_EN == 1			/* 串口4 TX = PC10   RX = PC11 */
	/* 使能 GPIO TX/RX 时钟 */
	UART3_TX_GPIO_CLK_ENABLE();
	UART3_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	UART3_CLK_ENABLE();	

	/* 配置TX引脚 */
	gpio_af_set(UART3_TX_GPIO_PORT, USART1_TX_AF, UART3_TX_PIN);
	gpio_mode_set(UART3_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART3_TX_PIN);
	gpio_output_options_set(UART3_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART3_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(UART3_RX_GPIO_PORT, USART0_RX_AF, UART3_RX_PIN);
	gpio_mode_set(UART3_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART3_RX_PIN);
	gpio_output_options_set(UART3_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART3_RX_PIN);
	

	/* 配置NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(UART3_IRQn, 0, 4);;
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(UART3, UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(UART3) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(UART3) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(UART3) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */
#endif

#if UART4_FIFO_EN == 1			/* 串口5 TX = PC12   RX = PD2 */
	/* 使能 GPIO TX/RX 时钟 */
	UART4_TX_GPIO_CLK_ENABLE();
	UART4_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	UART4_CLK_ENABLE();	

	/* 配置TX引脚 */
	gpio_af_set(UART4_TX_GPIO_PORT, USART1_TX_AF, UART4_TX_PIN);
	gpio_mode_set(UART4_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART4_TX_PIN);
	gpio_output_options_set(UART4_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART4_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(UART4_RX_GPIO_PORT, USART0_RX_AF, UART4_RX_PIN);
	gpio_mode_set(UART4_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART4_RX_PIN);
	gpio_output_options_set(UART4_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, UART4_RX_PIN);
	

	/* 配置NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(UART4_IRQn, 0, 5);;
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(UART4, UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(UART4) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(UART4) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(UART4) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */
#endif

#if UART5_FIFO_EN == 1			/* USART6 */
	/* 使能 GPIO TX/RX 时钟 */
	USART5_TX_GPIO_CLK_ENABLE();
	USART5_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART5_CLK_ENABLE();	

	/* 配置TX引脚 */
	gpio_af_set(USART5_TX_GPIO_PORT, USART1_TX_AF, USART5_TX_PIN);
	gpio_mode_set(USART5_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART5_TX_PIN);
	gpio_output_options_set(USART5_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART5_TX_PIN);
	
	/* 配置RX引脚 */
	gpio_af_set(USART5_RX_GPIO_PORT, USART0_RX_AF, USART5_RX_PIN);
	gpio_mode_set(USART5_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART5_RX_PIN);
	gpio_output_options_set(USART5_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART5_RX_PIN);
	

	/* 配置NVIC the NVIC for UART */   
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(USART5_IRQn, 0, 6);;
  
	/* 配置波特率、奇偶校验 */
	bsp_SetUartParam(USART5,  UART1_BAUD, USART_PM_NONE);
	
	USART_STAT0(USART5) &= ~USART_STAT0_TC;     /* 清除TC发送完成标志 */
	USART_STAT0(USART5) &= ~USART_STAT0_RBNE;   /* 清除RXNE接收标志 */

	USART_CTL0(USART5) |= USART_CTL0_RBNEIE;	/* 使能PE. RX接受中断 */
#endif

}

/**
 * @name   UartSend
 * @brief  填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
 * @param  无
 * @retval 无
 */
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
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
			else if(usCount == _pUart->usTxBufSize)/* 数据已填满缓冲区 */
			{
				if((USART_CTL0(_pUart->uart) & USART_CTL0_TBEIE) == 0)
				{
					USART_CTL0(_pUart->uart) |= USART_CTL0_TBEIE;
				}  
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}
	
	USART_CTL0(_pUart->uart) |= USART_CTL0_TBEIE;	/* 使能发送中断（缓冲区空） */
}

/**
 * @name   UartGetChar
 * @brief  从串口接收缓冲区读取1字节数据 （用于主程序调用）
 * @param  _pUart : 串口设备
 * @param  _pByte : 存放读取数据的指针
 * @retval 无
 */
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
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
 * @brief  判断发送缓冲区是否为空.
 * @param  _pUart : 串口设备
 * @retval 1为空,0为不空.
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
 * @brief  供中断服务程序调用，通用串口中断处理函数.
 * @param  _pUart : 串口设备
 * @retval 无
 */
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = USART_STAT0(_pUart->uart);
	uint32_t ctl0its	= USART_CTL0(_pUart->uart);
	uint32_t ctl2its	= USART_CTL2(_pUart->uart);

	/* 处理接收中断  */
	if ((isrflags & USART_STAT0_RBNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
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

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
			}
		}
	}

	/* 处理发送缓冲区空中断 */
	if ( ((isrflags & USART_STAT0_TBE) != RESET) && (ctl0its & USART_CTL0_TBEIE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			USART_CTL0(_pUart->uart) &= ~(USART_CTL0_TBEIE);

			/* 使能数据发送完毕中断 */
			USART_CTL0(_pUart->uart) |= USART_CTL0_TCIE;
		}
		else
		{
			_pUart->Sending = 1;
			
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			USART_DATA(_pUart->uart) = _pUart->pTxBuf[_pUart->usTxRead];
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* 数据bit位全部发送完毕的中断 */
	if (((isrflags & USART_STAT0_TC) != RESET) && ((ctl0its & USART_CTL0_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			USART_CTL0(_pUart->uart) &= ~(USART_CTL0_TCIE);
			
			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
			
			_pUart->Sending = 0;
		}
		else
		{
			/* 正常情况下，不会进入此分支 */

			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
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
 * @name   USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler等
 * @brief  USART中断服务程序
 * @param  无
 * @retval 无
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
 * @brief  重定义putc函数，这样可以使用printf函数从串口1打印输出
 * @param  无
 * @retval 无
 */
int fputc(int ch, FILE *f)
{
#if 1	/* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
	comSendChar(COM0, ch);
	
	return ch;
#else	/* 采用阻塞方式发送每个字符,等待数据发送完毕 */
	/* 写一个字节到USART1 */
	USART_DATA(USART0) = ch;
	
	/* 等待发送结束 */
	while((USART_STAT0(USART0) & USART_STAT0_TC) == 0)
	{}
	
	return ch;
#endif
}

/**
 * @name   fgetc
 * @brief  重定义getc函数，这样可以使用getchar函数从串口1输入数据
 * @param  无
 * @retval 无
 */
int fgetc(FILE *f)
{

#if 1	/* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
	uint8_t ucData;

	while(comGetChar(COM0, &ucData) == 0);

	return ucData;
#else
	/* 等待接收到数据 */
	while((USART_STAT0(USART0) & USART_STAT0_RBNE) == 0)
	{}

	return (int)USART_DATA(USART0);
#endif
}
