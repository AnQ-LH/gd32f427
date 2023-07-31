#include "bsp_gt1151.h"
#include <string.h>
#include "bsp.h"

/* enable the led clock */
#define ALL_GT1151_I2C_GPIO_CLK_ENABLE() {	    \
		rcu_periph_clock_enable(RCU_GPIOB);	\
		rcu_periph_clock_enable(RCU_GPIOC);	\
		rcu_periph_clock_enable(RCU_GPIOF);	\
	};

#define GT1151_I2C_SCL_GPIO	GPIOB			/* 连接到SCL时钟线的GPIO */
#define GT1151_I2C_SDA_GPIO	GPIOF			/* 连接到SDA数据线的GPIO */
#define GT1151_I2C_RST_GPIO	GPIOC			/* 连接到RST数据线的GPIO */
	
#define GT1151_I2C_SCL_PIN		GPIO_PIN_0			/* 连接到SCL时钟线的GPIO */
#define GT1151_I2C_SDA_PIN		GPIO_PIN_11			/* 连接到SDA数据线的GPIO */
#define GT1151_I2C_RST_PIN		GPIO_PIN_13			/* 连接到RST数据线的GPIO */
	
#define GT1151_I2C_SCL_1()		GPIO_BOP(GT1151_I2C_SCL_GPIO) = GT1151_I2C_SCL_PIN						/* SCL = 1 */
#define GT1151_I2C_SCL_0()		GPIO_BOP(GT1151_I2C_SCL_GPIO) = ((uint32_t)GT1151_I2C_SCL_PIN << 16U)	/* SCL = 0 */

#define GT1151_I2C_SDA_1()		GPIO_BOP(GT1151_I2C_SDA_GPIO) = GT1151_I2C_SDA_PIN						/* SDA = 1 */
#define GT1151_I2C_SDA_0()		GPIO_BOP(GT1151_I2C_SDA_GPIO) = ((uint32_t)GT1151_I2C_SDA_PIN << 16U)	/* SDA = 0 */

#define GT1151_I2C_RST_1()		GPIO_BOP(GT1151_I2C_RST_GPIO) = GT1151_I2C_RST_PIN						/* RST = 1 */
#define GT1151_I2C_RST_0()		GPIO_BOP(GT1151_I2C_RST_GPIO) = ((uint32_t)GT1151_I2C_RST_PIN << 16U)	/* RST = 0 */

#define GT1151_I2C_SDA_READ()	((GPIO_ISTAT(GT1151_I2C_SDA_GPIO) & GT1151_I2C_SDA_PIN) != 0)			/* 读SDA口线状态 */
#define GT1151_I2C_SCL_READ()	((GPIO_ISTAT(GT1151_I2C_SCL_GPIO) & GT1151_I2C_SCL_PIN) != 0)			/* 读SDA口线状态 */


/* GT1151单个触点配置参数，一次性写入 */ 
const uint8_t s_GT1151_CfgParams[]= 
{ 
	0x63,0xE0,0x01,0x20,0x03,0x05,0x3D,0x04,0x00,0x08,
	0x09,0x0F,0x55,0x37,0x33,0x11,0x00,0x03,0x08,0x56,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x00,0x00,
	0x3C,0x08,0x0A,0x28,0x1E,0x50,0x00,0x00,0x82,0xB4,
	0xD2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x85,0x25,0x10,0x41,0x43,0x31,
	0x0D,0x00,0xAD,0x22,0x24,0x7D,0x1D,0x1D,0x32,0xDF,
	0x4F,0x44,0x0F,0x80,0x2C,0x50,0x50,0x00,0x00,0x00,
	0x00,0xD3,0x00,0x00,0x00,0x00,0x0F,0x28,0x1E,0xFF,
	0xF0,0x37,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x50,0xB4,0xC0,0x94,0x53,0x2D,
	0x0A,0x02,0xBE,0x60,0xA2,0x71,0x8F,0x82,0x80,0x92,
	0x74,0xA3,0x6B,0x01,0x0F,0x14,0x03,0x1E,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x0D,0x0E,0x0F,0x10,0x12,
	0x13,0x14,0x15,0x1F,0x1D,0x1B,0x1A,0x19,0x18,0x17,
	0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x06,0x08,0x0C,
	0x12,0x13,0x14,0x15,0x17,0x18,0x19,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0xC4,0x09,0x23,0x23,0x50,0x5D,0x54,0x4B,0x3C,0x0F,
	0x32,0xFF,0xE4,0x04,0x40,0x00,0x8A,0x05,0x40,0x00,
	0xAA,0x00,0x22,0x22,0x00,0x00,0x73,0x22,0x01
}; 

GT1151_T g_GT1151;

/**
 * @name   GT1151_I2CInit
 * @brief  配置触摸芯片I2C初始化
 * @param  无
 * @retval 无
 */
void GT1151_I2CInit(void)
{
	/* I2C总线初始化 */
	ALL_GT1151_I2C_GPIO_CLK_ENABLE();
	
	/* 初试化I2C时钟 */
	gpio_mode_set(GT1151_I2C_SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_SCL_PIN);
	gpio_output_options_set(GT1151_I2C_SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_SCL_PIN);
	
	/* 初试化I2C数据线 */
	gpio_mode_set(GT1151_I2C_SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_SDA_PIN);
	gpio_output_options_set(GT1151_I2C_SDA_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_SDA_PIN);
	
	/* 初试化I2C复位线 */
	gpio_mode_set(GT1151_I2C_RST_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_RST_PIN);
	gpio_output_options_set(GT1151_I2C_RST_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_RST_PIN);
	
	GT1151_I2C_RST_0();
	bsp_DelayMS(10);
	GT1151_I2C_RST_1();
	bsp_DelayMS(10);
	
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	gt1151_i2c_Stop();
}

/**
 * @name   GT1151_InitHard
 * @brief  配置触摸芯片.  在调用该函数前，请先执行 bsp_touch.c 中的函数 bsp_DetectLcdType() 识别id
 * @param  无
 * @retval 无
 */
void GT1151_InitHard(void)
{
	//uint32_t id;
	
	g_GT1151.TimerCount = 0;
	
#if 0	/* 调试代码 */
	id = GT1151_ReadID();	
	
	printf("GT1151 ID :%08X\r\n", id);
#endif
	
	bsp_DelayMS(50);
	
	/*
		无需传送配置参数
		GT911_WriteReg(GT911_CONFIG_REG, (uint8_t *)s_GT911_CfgParams, sizeof(s_GT911_CfgParams));
	*/
	
	g_GT1151.TimerCount = 0;
	g_GT1151.Enable = 1;
}

/**
 * @name   GT1151_ReadID
 * @brief  获得GT1151的芯片ID
 * @param  无
 * @retval 1 正确读取GT1151ID
 */
uint32_t GT1151_ReadID(void)
{
	uint8_t buf[5]; 

	GT1151_ReadReg(GT1151_PID_REG, buf, 4); 
	
	buf[4] = 0;
	if( strcmp((char*)buf, "1158") == 0)
	{
		return 1;
	}

	return 0; 
}

/**
 * @name   GT1151_WriteReg
 * @brief  写1个或连续的多个寄存器
 * @param  _usRegAddr : 寄存器地址
 *		   _pRegBuf : 寄存器数据缓冲区
 *		   _ucLen : 数据长度
 * @retval 无
 */
void GT1151_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	uint8_t i;

    gt1151_i2c_Start();					/* 总线开始信号 */

    gt1151_i2c_SendByte(GT1151_CMD_WR);	/* 发送写信号 */
	gt1151_i2c_WaitAck();

    gt1151_i2c_SendByte(_usRegAddr >> 8);	/* 地址高8位 */
	gt1151_i2c_WaitAck();

    gt1151_i2c_SendByte(_usRegAddr&0XFF);	/* 地址低8位 */
	gt1151_i2c_WaitAck();

	for (i = 0; i < _ucLen; i++)
	{
	    gt1151_i2c_SendByte(_pRegBuf[i]);	/* 寄存器数据 */
		gt1151_i2c_WaitAck();
	}

    gt1151_i2c_Stop();						/* 总线停止信号 */
}

/**
 * @name   GT1151_ReadReg
 * @brief  读1个或连续的多个寄存器
 * @param  _usRegAddr : 寄存器地址
 *		   _pRegBuf : 寄存器数据缓冲区
 *		   _ucLen : 数据长度
 * @retval 无
 */
void GT1151_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	{
		gt1151_i2c_Start();					/* 总线开始信号 */

		gt1151_i2c_SendByte(GT1151_CMD_WR);	/* 发送设备地址+写信号 */
		gt1151_i2c_WaitAck();

		gt1151_i2c_SendByte(_usRegAddr >> 8);	/* 地址高8位 */
		gt1151_i2c_WaitAck();

		gt1151_i2c_SendByte(_usRegAddr);		/* 地址低8位 */
		gt1151_i2c_WaitAck();

		gt1151_i2c_Start();
		gt1151_i2c_SendByte(GT1151_CMD_RD);	/* 发送设备地址+读信号 */

		gt1151_i2c_WaitAck();
	}
	
	for (i = 0; i < _ucLen; i++)
	{	   
    	_pRegBuf[i] = gt1151_i2c_ReadByte(i == (_ucLen-1) ? 0 : 1); //发数据	  
	}
	
//	for (i = 0; i < 30; i++);

//	for (i = 0; i < _ucLen - 1; i++)
//	{
//	    _pRegBuf[i] = gt1151_i2c_ReadByte();	/* 读寄存器数据 */
//		gt1151_i2c_Ack();
//	}

//	/* 最后一个数据 */
//	 _pRegBuf[i] = gt1151_i2c_ReadByte();		/* 读寄存器数据 */
//	gt1151_i2c_NAck();

    gt1151_i2c_Stop();							/* 总线停止信号 */
}

/**
 * @name   GT1151_Timer1ms
 * @brief  每隔1ms调用1次
 * @param  无
 * @retval 无
 */
void GT1151_Timer1ms(void)
{
	g_GT1151.TimerCount++;
}

const uint16_t GT1151_TPX_TBL[5]={GT1151_TP0_REG, GT1151_TP1_REG, GT1151_TP2_REG, GT1151_TP3_REG, GT1151_TP4_REG};
/**
 * @name   GT1151_Scan
 * @brief  读取GT1151触摸数据。读取全部的数据。
 * @param  无
 * @retval 无
 */
void GT1151_Scan(void)
{
	uint8_t i;
	uint8_t buf[40];
	static uint8_t s_tp_down = 0;
	uint16_t x, y;
	static uint16_t x_save, y_save;
	uint8_t clear_flag = 0;
	static uint8_t s_count = 0;	

	if (g_GT1151.Enable == 0)
	{
		return;
	}
	
	/* 20ms 执行一次 */
	if (g_GT1151.TimerCount < 10)
	{
		return;
	}

	g_GT1151.TimerCount = 0;

#if 1	/* 方案1: 检测INT引脚电平. */
	if (TOUCH_PenInt() == 0)
	{		
#else	/* 方案2：不用INT引脚，读状态寄存器 */
	GT1151_ReadReg(GT1151_GSTID_REG, buf, 1);
	if (buf[0] == 0)
	{
#endif	
		if (s_tp_down == 1)
		{
			if (++s_count > 1)
			{
				s_count = 0;
				s_tp_down = 0;
				TOUCH_PutKey(TOUCH_RELEASE, x_save, y_save);
			}
		}
		return;
	}	
	s_count = 0;	

#if 1		/* 一般应用只读1点 */
	GT1151_ReadReg(GT1151_GSTID_REG, buf, 8);
#else		/* 读5个触摸点 */
	GT1151_ReadReg(GT1151_GSTID_REG, buf, 40);
#endif
	
	GT1151_WriteReg(GT1151_GSTID_REG, &clear_flag, 1);		/* 读完坐标后必须写0清除 */
	
	/*
		0x814E R/W Bufferstatus Large_Detect number of touch points 
			Bit7: Buffer status，1表示坐标（或按键）已经准备好，主控可以读取；0表示未就绪，数据无效。当主控读取完坐标后，必须通过I2C将此标志（或整个字节）写为0。
			Bit4: HaveKey, 1表示有按键，0表示无按键（已经松键）。
			Bit3~0: Number of touch points, 屏上的坐标点个数
	
		0x814F R Point1 track id 
		0x8150 R Point1Xl 触摸点 1，X 坐标低 8 位 
		0x8151 R Point1Xh 触摸点 1，X 坐标高 8 位 
		0x8152 R Point1Yl 触摸点 1，Y 坐标低 8 位 
		0x8153 R Point1Yh 触摸点 1，Y 坐标高 8 位 
		0x8154 R Point1 触摸点 1，触摸面积低 8 位 
		0x8155 R Point1 触摸点 1，触摸面积高 8 位 
		0x8156 ----
	
		0x8157 R Point2 track id 
		0x8158 R Point2Xl 触摸点 2，X 坐标低 8 位 
		0x8159 R Point2Xh 触摸点 2，X 坐标高 8 位 
		0x815A R Point2Yl 触摸点 2，Y 坐标低 8 位 
		0x815B R Point2Yh 触摸点 2，Y 坐标高 8 位 
		0x815C R Point2 触摸点 2，触摸面积低 8 位 
		0x815D R Point2 触摸点 2，触摸面积高 8 位 
		0x815E ----

		0x815F R Point3 track id 
		0x8160 R Point3Xl 触摸点 3，X 坐标低 8 位 
		0x8161 R Point3Xh 触摸点 3，X 坐标高 8 位 
		0x8162 R Point3Yl 触摸点 3，Y 坐标低 8 位 
		0x8163 R Point3Yh 触摸点 3，Y 坐标高 8 位 
		0x8164 R Point3 触摸点 3，触摸面积低 8 位 
		0x8165 R Point3 触摸点 3，触摸面积高 8 位 
		0x8166 ----

		0x8167 R Point4 track id 
		0x8168 R Point4Xl 触摸点 4，X 坐标低 8 位 
		0x8169 R Point4Xh 触摸点 4，X 坐标高 8 位 
		0x816A R Point4Yl 触摸点 4，Y 坐标低 8 位 
		0x816B R Point4Yh 触摸点 4，Y 坐标高 8 位 
		0x816C R Point4 触摸点 4，触摸面积低 8 位 
		0x816D R Point4 触摸点 4，触摸面积高 8 位 
		0x816E ----

		0x816F R Point5 track id 
		0x8170 R Point5Xl 触摸点 5，X 坐标低 8 位 
		0x8171 R Point5Xh 触摸点 5，X 坐标高 8 位 
		0x8172 R Point5Yl 触摸点 5，Y 坐标低 8 位 
		0x8173 R Point5Yh 触摸点 5，Y 坐标高 8 位 
		0x8174 R Point5 触摸点 5，触摸面积低 8 位 
		0x8175 R Point5 触摸点 5，触摸面积高 8 位 
		0x8176 --
		
	*/
	g_GT1151.TouchpointFlag = buf[0];
	g_GT1151.Touchkeystate = buf[1];

	g_GT1151.X0 = ((uint16_t)buf[3] << 8) + buf[2];
	g_GT1151.Y0 = ((uint16_t)buf[5] << 8) + buf[4];
	g_GT1151.P0 = ((uint16_t)buf[7] << 8) + buf[6];

	#if 0	/* 其余4点一般不用 */
		g_GT1151.X1 = ((uint16_t)buf[11] << 8) + buf[10];
		g_GT1151.Y1 = ((uint16_t)buf[13] << 8) + buf[12];
		g_GT1151.P1 = ((uint16_t)buf[15] << 8) + buf[14];

		g_GT1151.X2 = ((uint16_t)buf[19] << 8) + buf[18];
		g_GT1151.Y2 = ((uint16_t)buf[21] << 8) + buf[20];
		g_GT1151.P2 = ((uint16_t)buf[23] << 8) + buf[22];

		g_GT1151.X3 = ((uint16_t)buf[27] << 8) + buf[26];
		g_GT1151.Y3 = ((uint16_t)buf[29] << 8) + buf[28];
		g_GT1151.P3 = ((uint16_t)buf[31] << 8) + buf[30];

		g_GT1151.X4 = ((uint16_t)buf[35] << 8) + buf[34];
		g_GT1151.Y4 = ((uint16_t)buf[37] << 8) + buf[36];
		g_GT1151.P4 = ((uint16_t)buf[39] << 8) + buf[38];
	#endif

	/* 检测按下 */
	{
		{
			/* 竖屏 0 左上角坐标为0,0  3 左下角坐标为0,0*/
			if(g_LcdDirection == 0 || g_LcdDirection == 3)
			{
				x = g_GT1151.Y0;
				y = g_GT1151.X0;
				if (x > 479)
				{
					x = 479;
				}
				
				if (y > 319)
				{
					y = 319;
				}
			}
			
			/* 横屏幕 */
			if(g_LcdDirection == 6 || g_LcdDirection == 7)
			{
				x = g_GT1151.Y0;
				y = g_GT1151.X0;
				if (x > 479)
				{
					x = 479;
				}
				
				if (y > 319)
				{
					y = 319;
				}
			}
			
		}
	}
	
	if (s_tp_down == 0)
	{
		s_tp_down = 1;
		
		TOUCH_PutKey(TOUCH_DOWN, x, y);
	}
	else
	{
		TOUCH_PutKey(TOUCH_MOVE, x, y);
	}
	x_save = x;	/* 保存坐标，用于释放事件 */
	y_save = y;

#if 0
	{
		
		
		for (i = 0; i < 40; i++)
		{
			printf("%02X ", buf[i]);
		}
		printf("\r\n");

		printf("(%5d,%5d,%3d)\r\n ",  g_GT1151.X0, g_GT1151.Y0, g_GT1151.P0);
		printf("(%5d,%5d,%3d)\r\n ",  g_GT1151.X1, g_GT1151.Y1, g_GT1151.P1);
		printf("(%5d,%5d,%3d)\r\n ",  g_GT1151.X2, g_GT1151.Y2, g_GT1151.P2);
		printf("(%5d,%5d,%3d)\r\n ",  g_GT1151.X3, g_GT1151.Y3, g_GT1151.P3);
		printf("(%5d,%5d,%3d)\r\n ",  x, y, g_GT1151.P4);
		printf("\r\n");
	}
#endif	
}


static void gt1151_i2c_Delay(void);

/**
 * @name   i2c_Delay
 * @brief  I2C总线位延迟，最快400KHz
 * @param  无
 * @retval 无
 */
static void gt1151_i2c_Delay(void)
{
	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	bsp_DelayUS(2);
}

/**
 * @name   gt1151_i2c_Start
 * @brief  CPU发起I2C总线启动信号
 * @param  无
 * @retval 无
 */
void gt1151_i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	GT1151_I2C_SDA_1();
	GT1151_I2C_SCL_1();
	gt1151_i2c_Delay();
	GT1151_I2C_SDA_0();
	gt1151_i2c_Delay();
	
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
}

/**
 * @name   gt1151_i2c_Stop
 * @brief  CPU发起I2C总线停止信号
 * @param  无
 * @retval 无
 */
void gt1151_i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	GT1151_I2C_SDA_0();
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();
	gt1151_i2c_Delay();
	GT1151_I2C_SDA_1();
	gt1151_i2c_Delay();
}

/**
 * @name   gt1151_i2c_SendByte
 * @brief  CPU向I2C总线设备发送8bit数据
 * @param  _ucByte ： 等待发送的字节
 * @retval 无
 */
void gt1151_i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			GT1151_I2C_SDA_1();
		}
		else
		{
			GT1151_I2C_SDA_0();
		}
		gt1151_i2c_Delay();
		GT1151_I2C_SCL_1();
		gt1151_i2c_Delay();
		GT1151_I2C_SCL_0();
		GT1151_I2C_SCL_0();	/* 2019-03-14 针对GT811电容触摸，添加一行，相当于延迟几十ns */
		if (i == 7)
		{
			 GT1151_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */	
	}
}

/**
 * @name   gt1151_i2c_ReadByte
 * @brief  CPU从I2C总线设备读取8bit数据
 * @param  无
 * @retval 读到的数据
 */
uint8_t gt1151_i2c_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		GT1151_I2C_SCL_1();
		gt1151_i2c_Delay();
		if (GT1151_I2C_SDA_READ())
		{
			value++;
		}
		GT1151_I2C_SCL_0();
		gt1151_i2c_Delay();
	}
	
	if (!ack)gt1151_i2c_NAck();//发送nACK
    else gt1151_i2c_Ack(); //发送ACK
	
	return value;
}

/**
 * @name   gt1151_i2c_WaitAck
 * @brief  CPU产生一个时钟，并读取器件的ACK应答信号
 * @param  无
 * @retval 返回0表示正确应答，1表示无器件响应
 */
uint8_t gt1151_i2c_WaitAck(void)
{
	uint8_t re;

	GT1151_I2C_SDA_1();	/* CPU释放SDA总线 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	gt1151_i2c_Delay();
	if (GT1151_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
	return re;
}

/**
 * @name   i2c_Ack
 * @brief  CPU产生一个ACK信号
 * @param  无
 * @retval 无
 */
void gt1151_i2c_Ack(void)
{
	GT1151_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU产生1个时钟 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
	GT1151_I2C_SDA_1();	/* CPU释放SDA总线 */
	
	gt1151_i2c_Delay();
}

/**
 * @name   i2c_NAck
 * @brief  CPU产生1个NACK信号
 * @param  无
 * @retval 无
 */
void gt1151_i2c_NAck(void)
{
	GT1151_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU产生1个时钟 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
}

/**
 * @name   i2c_CheckDevice
 * @brief  检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @param  _Address：设备的I2C总线地址
 * @retval 返回值 0 表示正确， 返回1表示未探测到
 */
uint8_t gt1151_i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	
	if (GT1151_I2C_SDA_READ() && GT1151_I2C_SCL_READ())
	{
		gt1151_i2c_Start();		/* 发送启动信号 */

		/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
		gt1151_i2c_SendByte(_Address | GT1151_I2C_WR);
		ucAck = gt1151_i2c_WaitAck();	/* 检测设备的ACK应答 */

		gt1151_i2c_Stop();			/* 发送停止信号 */

		return ucAck;
	}
	return 1;	/* I2C总线异常 */
}