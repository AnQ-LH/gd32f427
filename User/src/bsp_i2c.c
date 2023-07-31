#include "bsp_i2c.h"
#include "bsp.h"

#define SOFE_IIC
//#define HARD_IIC

#ifdef SOFE_IIC

#define I2C_SCL_GPIO	GPIOB			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_GPIO	GPIOB			/* ���ӵ�SDA�����ߵ�GPIO */

#define I2C_SCL_PIN		GPIO_PIN_8			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_PIN_9			/* ���ӵ�SDA�����ߵ�GPIO */

#define ALL_I2C_GPIO_CLK_ENABLE()	rcu_periph_clock_enable(RCU_GPIOB);

/* �����дSCL��SDA�ĺ� */

#define I2C_SCL_1()		GPIO_BOP(I2C_SCL_GPIO) = I2C_SCL_PIN						/* SCL = 1 */
#define I2C_SCL_0()		GPIO_BOP(I2C_SCL_GPIO) = ((uint32_t)I2C_SCL_PIN << 16U)		/* SCL = 0 */

#define I2C_SDA_1()		GPIO_BOP(I2C_SDA_GPIO) = I2C_SDA_PIN						/* SDA = 1 */
#define I2C_SDA_0()		GPIO_BOP(I2C_SDA_GPIO) = ((uint32_t)I2C_SDA_PIN << 16U)		/* SDA = 0 */

#define I2C_SDA_READ()	((GPIO_ISTAT(I2C_SDA_GPIO) & I2C_SDA_PIN) != 0)					/* ��SDA����״̬ */
#define I2C_SCL_READ()	((GPIO_ISTAT(I2C_SCL_GPIO) & I2C_SCL_PIN) != 0)					/* ��SDA����״̬ */

/**
 * @name   bsp_InitI2C
 * @brief  ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
 * @param  ��
 * @retval ��
 */
void bsp_InitI2C(void)
{
	/* ��1������GPIOʱ�� */
	ALL_I2C_GPIO_CLK_ENABLE();
	
	
	gpio_mode_set(I2C_SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SCL_PIN);
	gpio_output_options_set(I2C_SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
	
	
	gpio_mode_set(I2C_SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SDA_PIN);
	gpio_output_options_set(I2C_SDA_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);

	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop();
}

/**
 * @name   i2c_Delay
 * @brief  I2C����λ�ӳ٣����400KHz
 * @param  ��
 * @retval ��
 */
static void i2c_Delay(void)
{
	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	//for (i = 0; i < 30; i++);
	//for (i = 0; i < 60; i++);
	//bsp_DelayUS(2); 229.57KHzʱ��
	bsp_DelayUS(2);
}

/**
 * @name   i2c_Start
 * @brief  CPU����I2C���������ź�
 * @param  ��
 * @retval ��
 */
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	
	I2C_SCL_0();
	i2c_Delay();
}

/**
 * @name   i2c_Stop
 * @brief  CPU����I2C����ֹͣ�ź�
 * @param  ��
 * @retval ��
 */
void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
	i2c_Delay();
}

/**
 * @name   i2c_SendByte
 * @brief  CPU��I2C�����豸����8bit����
 * @param  _ucByte �� �ȴ����͵��ֽ�
 * @retval ��
 */
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		I2C_SCL_0();	/* 2019-03-14 ���GT811���ݴ��������һ�У��൱���ӳټ�ʮns */
		if (i == 7)
		{
			 I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */	
	}
}

/**
 * @name   i2c_ReadByte
 * @brief  CPU��I2C�����豸��ȡ8bit����
 * @param  ��
 * @retval ����������
 */
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/**
 * @name   i2c_WaitAck
 * @brief  CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
 * @param  ��
 * @retval ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
 */
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/**
 * @name   i2c_Ack
 * @brief  CPU����һ��ACK�ź�
 * @param  ��
 * @retval ��
 */
void i2c_Ack(void)
{
	I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	
	i2c_Delay();
}

/**
 * @name   i2c_NAck
 * @brief  CPU����1��NACK�ź�
 * @param  ��
 * @retval ��
 */
void i2c_NAck(void)
{
	I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/**
 * @name   i2c_CheckDevice
 * @brief  ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
 * @param  _Address���豸��I2C���ߵ�ַ
 * @retval ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
 */
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (I2C_SDA_READ() && I2C_SCL_READ())
	{
		i2c_Start();		/* ���������ź� */

		/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
		i2c_SendByte(_Address | I2C_WR);
		ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

		i2c_Stop();			/* ����ֹͣ�ź� */

		return ucAck;
	}
	return 1;	/* I2C�����쳣 */
}

#endif  /* SOFE_IIC */

#ifdef HARD_IIC

#define I2Cx                             I2C0
#define I2Cx_CLK_ENABLE()                rcu_periph_clock_enable(RCU_I2C0)
#define I2Cx_SDA_GPIO_CLK_ENABLE()       rcu_periph_clock_enable(RCU_GPIOB)
#define I2Cx_SCL_GPIO_CLK_ENABLE()       rcu_periph_clock_enable(RCU_GPIOB)

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF_4

#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF_4

/* �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������ */
#define I2C_OWN_ADDRESS7      0x72  

static void I2C_Mode_Config(uint32_t clkspeed);

/**
 * @brief  I2C Initialization 
 * @param  hi2c: I2C handle pointer
 * @retval None
 */
void bsp_InitIIC(void)
{  
	/* Enable peripherals and GPIO Clocks */
	/* Enable GPIO TX/RX clock */
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();
	/* Enable I2C1 clock */
	I2Cx_CLK_ENABLE();
	
	/* connect I2Cx_SCL_PIN to I2Cx_SCL */
    gpio_af_set(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_AF, I2Cx_SCL_PIN);
    /* connect I2Cx_SDA_PIN to I2Cx_SDA */
    gpio_af_set(I2Cx_SDA_GPIO_PORT, I2Cx_SCL_AF, I2Cx_SDA_PIN);
    
	/* configure I2C0 GPIO */
    gpio_mode_set(I2Cx_SCL_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2Cx_SCL_PIN);
    gpio_output_options_set(I2Cx_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2Cx_SCL_PIN);
	
    gpio_mode_set(I2Cx_SDA_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2Cx_SDA_PIN);
    gpio_output_options_set(I2Cx_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2Cx_SDA_PIN);
	
	I2C_Mode_Config(100000);
	
}

/**
 * @brief  I2C ����ģʽ����
 * @param  ��
 * @retval ��
 */
static void I2C_Mode_Config(uint32_t clkspeed)
{
	/* configure I2C clock */
    i2c_clock_config(I2Cx, clkspeed, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2Cx, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_OWN_ADDRESS7);
    /* enable I2C */
    i2c_enable(I2Cx);
    /* enable acknowledge */
    i2c_ack_config(I2Cx, I2C_ACK_ENABLE);  
}

#endif  /* HARD_IIC */

