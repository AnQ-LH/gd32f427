#include "bsp_i2c.h"
#include "bsp.h"

#define SOFE_IIC
//#define HARD_IIC

#ifdef SOFE_IIC

#define I2C_SCL_GPIO	GPIOB			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_GPIO	GPIOB			/* 连接到SDA数据线的GPIO */

#define I2C_SCL_PIN		GPIO_PIN_8			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_PIN_9			/* 连接到SDA数据线的GPIO */

#define ALL_I2C_GPIO_CLK_ENABLE()	rcu_periph_clock_enable(RCU_GPIOB);

/* 定义读写SCL和SDA的宏 */

#define I2C_SCL_1()		GPIO_BOP(I2C_SCL_GPIO) = I2C_SCL_PIN						/* SCL = 1 */
#define I2C_SCL_0()		GPIO_BOP(I2C_SCL_GPIO) = ((uint32_t)I2C_SCL_PIN << 16U)		/* SCL = 0 */

#define I2C_SDA_1()		GPIO_BOP(I2C_SDA_GPIO) = I2C_SDA_PIN						/* SDA = 1 */
#define I2C_SDA_0()		GPIO_BOP(I2C_SDA_GPIO) = ((uint32_t)I2C_SDA_PIN << 16U)		/* SDA = 0 */

#define I2C_SDA_READ()	((GPIO_ISTAT(I2C_SDA_GPIO) & I2C_SDA_PIN) != 0)					/* 读SDA口线状态 */
#define I2C_SCL_READ()	((GPIO_ISTAT(I2C_SCL_GPIO) & I2C_SCL_PIN) != 0)					/* 读SDA口线状态 */

/**
 * @name   bsp_InitI2C
 * @brief  配置I2C总线的GPIO，采用模拟IO的方式实现
 * @param  无
 * @retval 无
 */
void bsp_InitI2C(void)
{
	/* 第1步：打开GPIO时钟 */
	ALL_I2C_GPIO_CLK_ENABLE();
	
	
	gpio_mode_set(I2C_SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SCL_PIN);
	gpio_output_options_set(I2C_SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
	
	
	gpio_mode_set(I2C_SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SDA_PIN);
	gpio_output_options_set(I2C_SDA_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
}

/**
 * @name   i2c_Delay
 * @brief  I2C总线位延迟，最快400KHz
 * @param  无
 * @retval 无
 */
static void i2c_Delay(void)
{
	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	//for (i = 0; i < 30; i++);
	//for (i = 0; i < 60; i++);
	//bsp_DelayUS(2); 229.57KHz时钟
	bsp_DelayUS(2);
}

/**
 * @name   i2c_Start
 * @brief  CPU发起I2C总线启动信号
 * @param  无
 * @retval 无
 */
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
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
 * @brief  CPU发起I2C总线停止信号
 * @param  无
 * @retval 无
 */
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
	i2c_Delay();
}

/**
 * @name   i2c_SendByte
 * @brief  CPU向I2C总线设备发送8bit数据
 * @param  _ucByte ： 等待发送的字节
 * @retval 无
 */
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
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
		I2C_SCL_0();	/* 2019-03-14 针对GT811电容触摸，添加一行，相当于延迟几十ns */
		if (i == 7)
		{
			 I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */	
	}
}

/**
 * @name   i2c_ReadByte
 * @brief  CPU从I2C总线设备读取8bit数据
 * @param  无
 * @retval 读到的数据
 */
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
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
 * @brief  CPU产生一个时钟，并读取器件的ACK应答信号
 * @param  无
 * @retval 返回0表示正确应答，1表示无器件响应
 */
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
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
 * @brief  CPU产生一个ACK信号
 * @param  无
 * @retval 无
 */
void i2c_Ack(void)
{
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPU释放SDA总线 */
	
	i2c_Delay();
}

/**
 * @name   i2c_NAck
 * @brief  CPU产生1个NACK信号
 * @param  无
 * @retval 无
 */
void i2c_NAck(void)
{
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/**
 * @name   i2c_CheckDevice
 * @brief  检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @param  _Address：设备的I2C总线地址
 * @retval 返回值 0 表示正确， 返回1表示未探测到
 */
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (I2C_SDA_READ() && I2C_SCL_READ())
	{
		i2c_Start();		/* 发送启动信号 */

		/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
		i2c_SendByte(_Address | I2C_WR);
		ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

		i2c_Stop();			/* 发送停止信号 */

		return ucAck;
	}
	return 1;	/* I2C总线异常 */
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

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
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
 * @brief  I2C 工作模式配置
 * @param  无
 * @retval 无
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

