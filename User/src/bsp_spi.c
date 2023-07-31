#include "bsp_spi.h"
#include "bsp.h"

/*
	SPI��ѯ��ʽ�����⣬������ʽδ��֤
*/

/*
 * ѡ��DMA���жϻ��߲�ѯ��ʽ
 */
//#define USE_SPI_DMA    /* DMA��ʽ  */
//#define USE_SPI_INT    /* �жϷ�ʽ */
#define USE_SPI_POLL   /* ��ѯ��ʽ */

/*
 * ʱ�ӣ����ţ�DMA���жϵȺ궨��
 */
#define SPIx						SPI0
#define DMAx						DMA0

#define SPI_CRC_ENABLE 0

#define SPIx_CLK_ENABLE()			rcu_periph_clock_enable(RCU_SPI0)

#define DMAx_CLK_ENABLE()			rcu_periph_clock_enable(RCU_DMA0);

#define SPIx_SCK_CLK_ENABLE()		rcu_periph_clock_enable(RCU_GPIOB)
#define SPIx_SCK_GPIO				GPIOB
#define SPIx_SCK_PIN				GPIO_PIN_3
#define SPIx_SCK_AF					GPIO_AF_5

#define SPIx_MISO_CLK_ENABLE()		rcu_periph_clock_enable(RCU_GPIOB)
#define SPIx_MISO_GPIO				GPIOB
#define SPIx_MISO_PIN 				GPIO_PIN_4
#define SPIx_MISO_AF				GPIO_AF_5

#define SPIx_MOSI_CLK_ENABLE()		rcu_periph_clock_enable(RCU_GPIOB)
#define SPIx_MOSI_GPIO				GPIOB
#define SPIx_MOSI_PIN 				GPIO_PIN_5
#define SPIx_MOSI_AF				GPIO_AF_5



#define SPIx_TX_DMA_CHANNEL         DMA_CH4
#define SPIx_TX_DMA_STREAM          DMA_SUBPERI0
#define SPIx_RX_DMA_CHANNEL         DMA_CH3
#define SPIx_RX_DMA_STREAM          DMA_SUBPERI0


#define SPIx_IRQn                   SPI0_IRQn
#define SPIx_IRQHandler             SPI0_IRQHandler
#define SPIx_DMA_TX_IRQn            DMA0_Channel4_IRQn
#define SPIx_DMA_RX_IRQn            DMA0_Channel3_IRQn
#define SPIx_DMA_TX_IRQHandler      DMA0_Channel4_IRQHandler
#define SPIx_DMA_RX_IRQHandler      DMA0_Channel3_IRQHandler

enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

static spi_parameter_struct hspi = {0};
static dma_single_data_parameter_struct hdma_tx;
static dma_single_data_parameter_struct hdma_rx;

static uint32_t s_BaudRatePrescaler;
static uint32_t s_CLKPolarityAndPhase;


uint8_t  g_spi_busy; /* SPIæ״̬��0��ʾ��æ��1��ʾæ */
__IO uint32_t wTransferState = TRANSFER_WAIT;

uint32_t g_spiTxLen = 0, g_spiRxLen = 0;	
uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];  
uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];


static void bsp_InitSPIGPIO(void);

/**
 * @name   bsp_InitSPIBus
 * @brief  ����SPI����
 * @param  ��
 * @retval ��
 */
void bsp_InitSPIBus(void)
{	
	g_spi_busy = 0;
	
	bsp_InitSPIGPIO();
	
	bsp_InitSPIParam(SPI_PSC_4, SPI_CK_PL_LOW_PH_1EDGE);
}

/**
 * @name   bsp_InitSPIParam
 * @brief  ����SPI���߲�����ʱ�ӷ�Ƶ��ʱ����λ��ʱ�Ӽ��ԡ�
 * @param  _BaudRatePrescaler	SPI����ʱ�ӷ�Ƶ���ã�֧�ֵĲ������£�
 *								SPI_PSC_2    2��Ƶ
 *								SPI_PSC_4    4��Ƶ
 *								SPI_PSC_8    8��Ƶ
 *								SPI_PSC_16   16��Ƶ
 *								SPI_PSC_32   32��Ƶ
 *								SPI_PSC_64   64��Ƶ
 *								SPI_PSC_128  128��Ƶ
 *								SPI_PSC_256  256��Ƶ
 * @param  _CLKPolarityAndPhase ʱ�Ӽ�����λ��֧�ֵĲ������£�
 *							   SPI_CK_PL_LOW_PH_1EDGE		SCK�����ڿ���״̬���ڵ͵�ƽ ���ŵĵ�1�����ز�����ĵ�1������
 *							   SPI_CK_PL_HIGH_PH_1EDGE		SCK�����ڿ���״̬���ڸߵ�ƽ ���ŵĵ�2�����ز�����ĵ�1������
 *							   SPI_CK_PL_LOW_PH_2EDGE		SCK�����ڿ���״̬���ڵ͵�ƽ ���ŵĵ�1�����ز�����ĵ�1������
 *							   SPI_CK_PL_HIGH_PH_2EDGE		SCK�����ڿ���״̬���ڸߵ�ƽ ���ŵĵ�2�����ز�����ĵ�1������
 * @retval ��
 */
void bsp_InitSPIParam(uint32_t _BaudRatePrescaler, uint32_t _CLKPolarityAndPhase)
{
	/* ���ִ��Ч�ʣ�ֻ����SPIӲ�����������仯ʱ����ִ�� */
	if (s_BaudRatePrescaler == _BaudRatePrescaler && s_CLKPolarityAndPhase == _CLKPolarityAndPhase)
	{		
		return;
	}

	s_BaudRatePrescaler = _BaudRatePrescaler;	
	s_CLKPolarityAndPhase = _CLKPolarityAndPhase;

	/* ����SPI���� */
	hspi.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    hspi.device_mode          = SPI_MASTER;
    hspi.frame_size           = SPI_FRAMESIZE_8BIT;
    hspi.clock_polarity_phase = _CLKPolarityAndPhase;
    hspi.nss                  = SPI_NSS_SOFT;
    hspi.prescale             = _BaudRatePrescaler;
    hspi.endian               = SPI_ENDIAN_MSB;

    spi_init(SPIx, &hspi);
	
#if SPI_CRC_ENABLE
    /* configure SPI CRC function */
    spi_crc_polynomial_set(SPIx, 7);
    spi_crc_on(SPIx);
#endif /* enable CRC function */
	
	spi_enable(SPIx);
}

/**
 * @name   bsp_InitSPIParam
 * @brief  ����SPI����ʱ�ӣ�GPIO���жϣ�DMA��
 * @param  SPI_HandleTypeDef ����ָ�����
 * @retval ��
 */
static void bsp_InitSPIGPIO(void)
{
	/* ���� SPI����GPIO : SCK MOSI MISO */
	{
		/* SPI��GPIPʱ�� */
		SPIx_SCK_CLK_ENABLE();
		SPIx_MISO_CLK_ENABLE();
		SPIx_MOSI_CLK_ENABLE();
		SPIx_CLK_ENABLE();

		/* SPI SCK */
		gpio_af_set(SPIx_SCK_GPIO, SPIx_SCK_AF, SPIx_SCK_PIN);
		gpio_mode_set(SPIx_SCK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPIx_SCK_PIN);
		gpio_output_options_set(SPIx_SCK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPIx_SCK_PIN);

		/* SPI MISO */
		gpio_af_set(SPIx_MISO_GPIO, SPIx_MISO_AF, SPIx_MISO_PIN);
		gpio_mode_set(SPIx_MISO_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPIx_MISO_PIN);
		gpio_output_options_set(SPIx_MISO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPIx_MISO_PIN);

		/* SPI MOSI */
		gpio_af_set(SPIx_MOSI_GPIO, SPIx_MOSI_AF, SPIx_MOSI_PIN);
		gpio_mode_set(SPIx_MOSI_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPIx_MOSI_PIN);
		gpio_output_options_set(SPIx_MOSI_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPIx_MOSI_PIN);
	}

	/* ����DMA��NVIC */
	#ifdef USE_SPI_DMA
	{
		/* ʹ��DMAʱ�� */
		DMAx_CLK_ENABLE();      

		/* SPI DMA�������� */
		dma_deinit(DMAx, SPIx_TX_DMA_CHANNEL);
		
		hdma_tx.periph_addr         = (uint32_t)&SPI_DATA(SPIx);
		hdma_tx.memory0_addr        = (uint32_t)g_spiTxBuf;
		hdma_tx.direction           = DMA_MEMORY_TO_PERIPH;
		hdma_tx.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
		hdma_tx.priority            = DMA_PRIORITY_LOW;
		hdma_tx.number              = SPI_BUFFER_SIZE;
		hdma_tx.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
		hdma_tx.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
		hdma_tx.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
		
		dma_single_data_mode_init(DMAx, SPIx_TX_DMA_CHANNEL, &hdma_tx);
		dma_channel_subperipheral_select(DMAx, SPIx_TX_DMA_CHANNEL, SPIx_TX_DMA_STREAM);
		
		 /* ��ʼ��DMA */
		dma_channel_enable(DMAx, SPIx_TX_DMA_CHANNEL);

		/* ʹ��spi dma */
		spi_dma_enable(SPIx, SPI_DMA_TRANSMIT);


		/* SPI DMA�������� */	
		dma_deinit(DMAx, SPIx_RX_DMA_CHANNEL);
		
		hdma_rx.periph_addr         = (uint32_t)&SPI_DATA(SPIx);
		hdma_rx.memory0_addr        = (uint32_t)g_spiRxBuf;
		hdma_rx.direction           = DMA_PERIPH_TO_MEMORY;
		hdma_rx.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
		hdma_rx.priority            = DMA_PRIORITY_HIGH;
		hdma_rx.number              = SPI_BUFFER_SIZE;
		hdma_rx.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
		hdma_rx.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
		hdma_rx.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
		
		dma_single_data_mode_init(DMAx, SPIx_RX_DMA_CHANNEL, &hdma_rx);
		dma_channel_subperipheral_select(DMAx, SPIx_RX_DMA_CHANNEL, SPIx_RX_DMA_STREAM);
		
		 /* ��ʼ��DMA */
		dma_channel_enable(DMAx, SPIx_RX_DMA_CHANNEL);
		
		/* ʹ��spi dma */
		spi_dma_enable(SPIx, SPI_DMA_RECEIVE);

#if 0
		/* ����DMA�����ж� */
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(SPIx_DMA_TX_IRQn, 1, 0);
		
		/* ����DMA�����ж� */
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(SPIx_DMA_RX_IRQn, 1, 0);
		
		/* ����SPI�ж� */
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(SPIx_IRQn, 1, 0);
#endif
	}
	#endif
	
	#ifdef USE_SPI_INT
	{
		/* ����SPI�ж����ȼ���ʹ���ж� */
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(SPIx_IRQn, 1, 0);
		
		spi_i2s_interrupt_enable(SPIx, SPI_I2S_INT_TBE);
	}
	#endif
}

/**
 * @name   bsp_spiTransfer
 * @brief  �������ݴ���
 * @param  ��
 * @retval ��
 */
uint8_t sf_SendByte(uint8_t _ucValue)
{

	
	/* DMA��ʽ���� */
#ifdef USE_SPI_DMA
	/* wait dma transmit complete */
	while(!dma_flag_get(DMAx, SPIx_TX_DMA_CHANNEL, DMA_FLAG_FTF));
    while(!dma_flag_get(DMAx, SPIx_RX_DMA_CHANNEL, DMA_FLAG_FTF));
	return 0;
#endif

	/* �жϷ�ʽ���� */	
#ifdef USE_SPI_INT
	wTransferState = TRANSFER_WAIT;
	
	while (wTransferState == TRANSFER_WAIT);
	
	return 0;
#endif

	/* ��ѯ��ʽ���� */	
#ifdef USE_SPI_POLL
	/* Check TXE flag */
	while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
	spi_i2s_data_transmit(SPIx, _ucValue);	

	/* Wait until RXNE flag is reset */
	while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
	return spi_i2s_data_receive(SPIx);
#endif
}


/**
 * @name   bsp_SpiBusEnter
 * @brief  ռ��SPI����
 * @param  ��
 * @retval ��
 * @notice ��
 */
void bsp_SpiBusEnter(void)
{
	g_spi_busy = 1;
}

/**
 * @name   bsp_SpiBusExit
 * @brief  �ͷ�ռ�õ�SPI����
 * @param  ��
 * @retval ��
 */
void bsp_SpiBusExit(void)
{
	g_spi_busy = 0;
}

/**
 * @name   bsp_SpiBusBusy
 * @brief  �ж�SPI����æ�������Ǽ������SPIоƬ��Ƭѡ�ź��Ƿ�Ϊ1
 * @param  ��
 * @retval 0 ��ʾ��æ  1��ʾæ
 */
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}

/**
 * @name   SPIx_IRQHandler��SPIx_DMA_RX_IRQHandler��SPIx_DMA_TX_IRQHandler
 * @brief  �жϷ������
 * @param  ��
 * @retval ��
 */
#ifdef USE_SPI_INT
	void SPIx_IRQHandler(void)
	{
		if(RESET != spi_i2s_interrupt_flag_get(SPIx, SPI_I2S_INT_TBE))
		{
			/* send data */
			while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE))
			{
			};
			spi_i2s_data_transmit(SPIx, g_spiTxBuf[g_spiTxLen++]);
			if(SPI_BUFFER_SIZE == g_spiTxLen)
			{
#if SPI_CRC_ENABLE
				/* send the CRC value */
				spi_crc_next(SPIx);
#endif /* enable CRC function */
//				spi_i2s_interrupt_disable(SPIx, SPI_I2S_INT_TBE);
			}

			/* received data */
			if(RESET != spi_i2s_interrupt_flag_get(SPIx, SPI_I2S_INT_RBNE))
			{
				g_spiRxBuf[g_spiRxLen++] = spi_i2s_data_receive(SPIx);
#if SPI_CRC_ENABLE
				if((SPI_BUFFER_SIZE - 1) == g_spiRxLen)
				{
					/* receive the CRC value */
					spi_crc_next(SPIx);
				}
#endif /* enable CRC function */
			}
		}
		wTransferState = TRANSFER_COMPLETE;
	}	
#endif

#ifdef USE_SPI_DMA
	void SPIx_IRQHandler(void)
	{
		if(RESET != spi_i2s_interrupt_flag_get(SPIx, SPI_I2S_INT_TBE))
		{
			/* send data */
			while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE))
			{
			};
			spi_i2s_data_transmit(SPIx, g_spiTxBuf[g_spiTxLen++]);
			if(SPI_BUFFER_SIZE == g_spiTxLen)
			{
#if SPI_CRC_ENABLE
				/* send the CRC value */
				spi_crc_next(SPIx);
#endif /* enable CRC function */
//				spi_i2s_interrupt_disable(SPIx, SPI_I2S_INT_TBE);
			}

			/* received data */
			if(RESET != spi_i2s_interrupt_flag_get(SPIx, SPI_I2S_INT_RBNE))
			{
				g_spiRxBuf[g_spiRxLen++] = spi_i2s_data_receive(SPIx);
#if SPI_CRC_ENABLE
				if((SPI_BUFFER_SIZE - 1) == g_spiRxLen)
				{
					/* receive the CRC value */
					spi_crc_next(SPIx);
				}
#endif /* enable CRC function */
			}
		}
		wTransferState = TRANSFER_COMPLETE;
	}
#endif

