#include "bsp_ts_touch.h"
#include "bsp_gt1151.h"
#include "bsp.h"


/* 调试打印语句 */
#define touch_printf       printf

/*
	本程序未使用触笔中断功能。在1ms周期的 Systick定时中断服务程序中对触摸ADC值进行采样和
	滤波处理。当触笔按下超过30ms后，开始采集ADC值（每1ms采集1次，连续采集10次），然后对
	这10个样本进行排序，丢弃2头的样本，对中间一段样本进行求和并计算平均值。

	采样2点校准方法，校准后保存2个校准点的ADC值，实际工作时，根据2点直线方程计算屏幕坐标。
	校准参数有保存接口，本程序主要用于演示，未做保存功能。
	大家可以自己修改  TOUCH_SaveParam() 和 TOUCH_LoadParam() 两个函数实现保存功能。
*/

/* 每1ms扫描一次坐标 */
#define DOWN_VALID		30	/* 按下30ms 后, 开始统计ADC */
#define SAMPLE_COUNT	10	/* 按下后连续采集10个样本 */

/*
触摸屏校准点相对屏幕像素四角的偏移像素
第1个点 ： x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
第2个点 ： x2 = LCD_GetWidth() - CALIB_OFFSET, y2 = LCD_GetHeight() - CALIB_OFFSET
*/

#define CALIB_OFFSET	20
#define TP_X1	CALIB_OFFSET
#define TP_Y1	CALIB_OFFSET

#define TP_X2	(LCD_GetWidth() - CALIB_OFFSET)
#define TP_Y2	(LCD_GetHeight() - CALIB_OFFSET)

#define TP_X3	CALIB_OFFSET
#define TP_Y3	(LCD_GetHeight() - CALIB_OFFSET)

#define TP_X4	(LCD_GetWidth() - CALIB_OFFSET)
#define TP_Y4	CALIB_OFFSET

/* RA8875 有效ADC值的判断门限. 太接近ADC临界值的坐标认为无效 */
#define ADC_VALID_OFFSET	2


/* 校准参数存放在EEPROM(AT24C128容量16KB) 最后64字节区域 */
#define TP_PARAM_EE_ADDR		(16*1024 - 64)

/* TP_INT 定义GPIO端口 */
#define TP_INT_GPIO_CLK_ENABLE()	rcu_periph_clock_enable(RCU_GPIOB);
#define TP_INT_GPIO_PORT			GPIOB
#define TP_INT_PIN					GPIO_PIN_1
	
	
/* 触屏模块用到的全局变量 */
TOUCH_T g_tTP;

TP_CALIB_PARAM_T g_tTPParam;

static uint8_t	TOUCH_PressValid(uint16_t _usX, uint16_t _usY);
static uint16_t TOUCH_DataFilter(uint16_t *_pBuf, uint8_t _ucCount);
static void TOUCH_LoadParam(void);
static void TOUCH_SaveParam(void);
static int32_t CalTwoPoint(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x);
int32_t TOUCH_Abs(int32_t x);
	
uint8_t g_TouchType = 0xFF;		/* 触摸类型 */
uint8_t g_LcdType = 0xFF;		/* LCD屏类型 */


/**
 * @name   bsp_DetectLcdType
 * @brief  通过I2C触摸芯片，识别LCD模组类型。结果存放在全局变量 g_LcdType 和 g_TouchType
 * @param  无
 * @retval 无
 */
void bsp_DetectLcdType(void)
{
	uint8_t i;
	
	g_TouchType = 0xFF;
	g_LcdType = 0xFF;
	
	bsp_DelayUS(50000);
	
	touch_printf("正在识别触摸屏型号\r\n");
	
	/* 50ms，等待GT1151复位就绪，才能探测GT1151芯片 ID */
	for (i = 0; i < 5; i++)
	{		
		/*
			GT1151电容触摸板地址0xBA 两种。
			通过读取触摸IC的芯片ID来识别。
		*/
		if (gt1151_i2c_CheckDevice(0x14) == 1)
		{
			uint32_t id;
			bsp_DelayUS(500);
			g_GT1151.i2c_addr = 0x14;
			id = GT1151_ReadID();		
			if (id == 1)
			{			
				g_TouchType = CT_GT1151;
				g_LcdType = LCD_35_480X320;		
				touch_printf("检测到3.5寸 GT1151(0x14) 480x320\r\n");
			}
			else
			{	
				touch_printf("未检测到触摸屏GT1151的ID\r\n");
			}
			break;
		}

		bsp_DelayMS(10);
	}		
		
	if (g_ChipID == IC_5310)
	{
		if(g_TouchType == 0xFF)
		{
			g_TouchType = CT_GT1151;
			g_tTP.usMaxAdc = 1023;	/* 10bit ADC */	

			/* 4.3寸屏 */
			g_LcdType = LCD_35_480X320;
		}
	}
	
	if (i == 5)
	{
		touch_printf("未识别出显示模块\r\n");
	}
}

/**
 * @name   TOUCH_IntWakeUpForGT
 * @brief  INT唤醒函数。复位后INT给个高电平脉冲，可以唤醒处于休眠的芯片。
 * @param  无
 * @retval 无
 */
void TOUCH_IntWakeUpForGT(void)
{
	/* 第1步：打开GPIO时钟 */
	TP_INT_GPIO_CLK_ENABLE();
	
	/* 第2步：配置所有的按键GPIO为浮动输入模式 */
	gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TP_INT_PIN);
	gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, TP_INT_PIN);
	
	GPIO_BOP(TP_INT_GPIO_PORT) = ((uint32_t)TP_INT_PIN << 16U);	/* INT = 0 */
	bsp_DelayUS(200);
	
	GPIO_BOP(TP_INT_GPIO_PORT) = TP_INT_PIN;	/* INT = 1 */
	bsp_DelayUS(2000);
	GPIO_BOP(TP_INT_GPIO_PORT) = ((uint32_t)TP_INT_PIN << 16U);	/* INT = 0 */
	bsp_DelayUS(200);
	
	gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TP_INT_PIN);
	
}

/**
 * @name   TOUCH_InitHard
 * @brief  初始化触摸芯片。 再之前，必须先执行 bsp_DetectLcdType() 识别触摸出触摸芯片型号.
 * @param  无
 * @retval 无
 */
void TOUCH_InitHard(void)
{	
	//TOUCH_IntWakeUpForGT();
	
	/* 配置TP_INT触摸中断引脚 */
	{
		/* 第1步：打开GPIO时钟 */
		TP_INT_GPIO_CLK_ENABLE();
		
		/* 第2步：配置所有的按键GPIO为浮动输入模式 */
		gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TP_INT_PIN);
		gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, TP_INT_PIN);
	}
	
	GT1151_I2CInit();
	
	bsp_DetectLcdType();	/* 自动识别触摸芯片型号 */
	
	/* 电阻屏参数 */
    g_tTP.Enable = 0;
	
	switch (g_TouchType)
	{		
		case CT_GT1151:			/* 电容触摸 3.5寸 */
			GT1151_InitHard();
			break;
		default:
			break;
	}
}

/**
 * @name   TOUCH_PenInt
 * @brief  判断触摸按下. INT 为低表示有触摸按下。 注意在持续按下时，INT会周期性恢复为高电平
 *		   脉冲宽度约2ms.
 * @param  无
 * @retval 0 表示无触笔按下， 1表示有触摸按下
 */
uint8_t TOUCH_PenInt(void)
{
	if ((GPIO_ISTAT(TP_INT_GPIO_PORT) & TP_INT_PIN) == 0)
	{
		return 1;
	}
	return 0;
}

/**
 * @name   TOUCH_ReadAdcX
 * @brief  获得触摸板X方向ADC采样值， 已进行滤波处理
 * @param  无
 * @retval X 方向ADC值
 */
uint16_t TOUCH_ReadAdcX(void)
{
	uint16_t usAdc;

	__set_PRIMASK(1);  		/* 关中断 */
	usAdc = g_tTP.usAdcNowX;
	__set_PRIMASK(0);  		/* 开中断 */

	return usAdc;
}

/**
 * @name   TOUCH_ReadAdcY
 * @brief  获得触摸板Y方向ADC采样值， 已进行滤波处理
 * @param  无
 * @retval Y 坐标值，允许负值
 */
uint16_t TOUCH_ReadAdcY(void)
{
	uint16_t usAdc;

	__set_PRIMASK(1);  		/* 关中断 */
	usAdc = g_tTP.usAdcNowY;
	__set_PRIMASK(0);  		/* 开中断 */

	return usAdc;
}

/**
 * @name   TOUCH_PutKey
 * @brief  将1个触摸点坐标值压入触摸FIFO缓冲区。电阻触摸屏形参是ADC值，电容触摸屏形参是坐标值
 * @param  _usX, _usY 坐标值
 * @retval 无
 */
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
	uint16_t xx, yy;
	uint16_t x = 0, y = 0;

	g_tTP.Event[g_tTP.Write] = _ucEvent;

	if (g_tTP.Enable == 1)	/* 电阻屏。 形参是ADC值 */
	{

	}
	else	/* 电容触摸走此分之 */
	{
		/* 无需转换， 直接是坐标值 */
		xx = _usX;
		yy = _usY;		
	}
	
	switch (g_LcdDirection)
	{
		case 0:	/* 校准触摸时，屏幕方向为 0 竖屏 正 */
		{
			x = xx;
			y = yy;
		}		
		break;
		
		case 3:	/* 校准触摸时，屏幕方向为 3 竖屏 反转 */
		{
			x = g_LcdHeight - xx - 1;
			y = g_LcdWidth - yy - 1;
		}		
		break;
		
		case 6:	/* 校准触摸时，屏幕方向为 6 横屏 */
		{
			x = g_LcdHeight - yy - 1;
			y = xx;
		}
		break;
		
		case 7:	/* 校准触摸时，屏幕方向为 7 横屏 反转*/
		{
			x = yy;
			y = g_LcdWidth - xx - 1;
		}
		break;
		default:
			g_LcdDirection = 0;	/* 方向参数无效时，纠正为缺省的横屏 */
		break;
	}
	
	g_tTP.XBuf[g_tTP.Write] = x;
	g_tTP.YBuf[g_tTP.Write] = y;

	if (++g_tTP.Write  >= TOUCH_FIFO_SIZE)
	{
		g_tTP.Write = 0;
	}
	
	/* 调试语句，打印adc和坐标 */
	touch_printf("%d - (%d, %d)\r\n", _ucEvent, x, y);
}

/**
 * @name   TOUCH_GetKey
 * @brief  从触摸FIFO缓冲区读取一个坐标值。
 * @param  _pX, _pY 坐标
 * @retval TOUCH_NONE		表示无事件
 *		   TOUCH_DOWN		按下
 *		   TOUCH_MOVE		移动
 *		   TOUCH_RELEASE	释放
 */
uint8_t TOUCH_GetKey(int16_t *_pX, int16_t *_pY)
{
	uint8_t ret;

	if (g_tTP.Read == g_tTP.Write)
	{
		return TOUCH_NONE;
	}
	else
	{
		ret = g_tTP.Event[g_tTP.Read];
		*_pX = g_tTP.XBuf[g_tTP.Read];
		*_pY = g_tTP.YBuf[g_tTP.Read];

		if (++g_tTP.Read >= TOUCH_FIFO_SIZE)
		{
			g_tTP.Read = 0;
		}
		return ret;
	}
}

/**
 * @name   TOUCH_CelarFIFO
 * @brief  清除触摸FIFO缓冲区
 * @param  无
 * @retval 无
 */
void TOUCH_CelarFIFO(void)
{
	__set_PRIMASK(1);  		/* 关中断 */
	g_tTP.Write = g_tTP.Read;
	__set_PRIMASK(0);  		/* 开中断 */
}

/**
 * @name   TOUCH_InRect
 * @brief  判断当前坐标是否位于矩形框内
 * @param  _usX, _usY: 输入坐标
 * @param  _usRectX,_usRectY: 矩形起点
 * @param  _usRectH、_usRectW : 矩形高度和宽度
 * @retval 1 表示在范围内
 */
uint8_t TOUCH_InRect(uint16_t _usX, uint16_t _usY,
	uint16_t _usRectX, uint16_t _usRectY, uint16_t _usRectH, uint16_t _usRectW)
{
	if ((_usX > _usRectX) && (_usX < _usRectX + _usRectW)
		&& (_usY > _usRectY) && (_usY < _usRectY + _usRectH))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @name   TOUCH_MoveValid
 * @brief  判断当前坐标和上次坐标是否偏差太大
 * @param  _usX1, _usY1: 坐标1
 * @param  _usX2, _usY2: 坐标2
 * @retval 1 表示有效点， 0 表示飞点
 */
uint8_t TOUCH_MoveValid(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2)
{
	int16_t iX, iY;
	static uint8_t s_invalid_count = 0;

	iX = TOUCH_Abs(_usX1 - _usX2);
	iY = TOUCH_Abs(_usY1 - _usY2);

	if ((iX < 25) && (iY < 25))
	{
		s_invalid_count = 0;
		return 1;
	}
	else
	{
		if (++s_invalid_count >= 3)
		{
			s_invalid_count = 0;
			return 1;
		}
		return 0;
	}
}

/**
 * @name   TOUCH_CapScan
 * @brief  I2C接口电容触摸板扫描函数，放在 bsp_Idle()执行！
 * @param  无
 * @retval 无
 */
void TOUCH_CapScan(void)
{
	if (g_GT1151.Enable == 1)
	{
		GT1151_Scan();
		return;
	}
}

/**
 * @name   TOUCH_Scan
 * @brief  触摸板事件检测程序。该函数被周期性调用，每ms调用1次. 见 bsp_Timer.c
 * @param  无
 * @retval 无
 */
void TOUCH_Scan(void)
{
	uint16_t usAdcX = 0;
	uint16_t usAdcY = 0;
	static uint16_t s_usXBuf[SAMPLE_COUNT];
	static uint16_t s_usYBuf[SAMPLE_COUNT];
	static uint8_t s_ucPos = 0;
	static uint8_t s_count = 0;
	static uint8_t s_down = 0;
	static uint16_t s_usSaveAdcX, s_usSaveAdcY; /* 用于触笔抬起事件，保存按下和移动的最后采样值 */
	
	if (g_GT1151.Enable == 1)	/* 3.5寸电容管屏 GT1151 */
	{
		GT1151_Timer1ms();	/* 电容触摸屏程序计数器 */
		return;
	}

}

/**
 * @name   CalTwoPoint
 * @brief  根据2点直线方程，计算Y值
 * @param  2个点的坐标和x输入量
 * @retval x对应的y值
 */
static int32_t CalTwoPoint(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x)
{
	return y1 + ((int32_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/**
 * @name   TOUCH_DataFilter
 * @brief  对采样数据进行滤波
 * @param  无
 * @retval X 坐标值，允许负值
 */
static uint16_t TOUCH_DataFilter(uint16_t *_pBuf, uint8_t _ucCount)
{
#if 0
	uint8_t i;
	uint32_t uiSum;

	uiSum = 0;
	for (i = 0; i < _ucCount; i++)
	{
		uiSum += _pBuf[i];
	}
	return uiSum / _ucCount;
#else
	uint8_t flag;
	uint8_t i;
	uint16_t usTemp;
	uint32_t uiSum;

	/* 升序排列 */
    do
	{
		flag = 0;
		for (i = 0; i < _ucCount - 1; i++)
		{
			if (_pBuf[i] > _pBuf[i+1])
			{
				usTemp = _pBuf[i + 1];
				_pBuf[i+1] = _pBuf[i];
				_pBuf[i] = usTemp;
				flag = 1;
			}
		}
	}while(flag);

	uiSum = 0;
	for (i = 0; i < _ucCount / 3; i++)
	{
		uiSum += _pBuf[_ucCount / 3 + i];
	}
	usTemp = uiSum / (_ucCount / 3);
	return usTemp;
#endif
}

#if 0
/**
 * @name   TOUCH_DispPoint1
 * @brief  显示第1个校准点
 * @param  _ucIndex = 0 : 表示第1个点； _ucIndex = 1 表示第2个点;
 * @retval 无
 */
static void TOUCH_DispPoint(uint8_t _ucIndex)
{
	FONT_T tFont16;			/* 定义一个字体结构体变量，用于设置字体参数 */

	/* 设置字体参数 */
	{
		tFont16.FontCode = FC_ST_16;	/* 字体代码 16点阵 */
		tFont16.FrontColor = CL_WHITE;		/* 字体颜色 0 或 1 */
		tFont16.BackColor = CL_BLUE;	/* 文字背景颜色 */
		tFont16.Space = 0;			/* 文字间距，单位 = 像素 */
	}

/*
	第1个点 ： x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
	第2个点 ： x2 = LCD_GetHeight() - CALIB_OFFSET, y2 = LCD_GetWidth - CALIB_OFFSET
*/
	if (_ucIndex == 0)
	{
		LCD_ClrScr(CL_BLUE);  		/* 清屏，背景蓝色 */

		/* 在屏幕边沿绘制2个矩形框(用于检测面板边缘像素是否正常) */
		LCD_DrawRect(0, 0, LCD_GetHeight(), LCD_GetWidth(), CL_WHITE);
		LCD_DrawRect(2, 2, LCD_GetHeight() - 4, LCD_GetWidth() - 4, CL_YELLOW);

		LCD_DispStr(50, 10, "Touch Calibration", &tFont16);		/* 在(8,3)坐标处显示一串汉字 */

		LCD_DrawCircle(TP_X1, TP_Y1, 6, CL_WHITE);
	}
	else if (_ucIndex == 1)
	{
		LCD_DrawCircle(TP_X1, TP_Y1, 6, CL_BLUE);			/* 擦除第1个点 */

		LCD_DrawCircle(TP_X2, TP_Y2, 6, CL_WHITE);
	}
	else if (_ucIndex == 2)
	{
		LCD_DrawCircle(TP_X2, TP_Y2, 6, CL_BLUE);			/* 擦除第2个点 */

		LCD_DrawCircle(TP_X3, TP_Y3, 6, CL_WHITE);
	}
	else
	{
		LCD_DrawCircle(TP_X3, TP_Y3, 6, CL_BLUE);			/* 擦除第3个点 */

		LCD_DrawCircle(TP_X4, TP_Y4, 6, CL_WHITE);
	}
}
#endif

/**
 * @name   TOUCH_PressValid
 * @brief  判断按压是否有效，根据X, Y的ADC值进行大致判断
 * @param  无
 * @retval 1 表示有效； 0 表示无效
 */
static uint8_t	TOUCH_PressValid(uint16_t _usX, uint16_t _usY)
{
	if ((_usX <= ADC_VALID_OFFSET) || (_usY <= ADC_VALID_OFFSET)
		|| (_usX >= g_tTP.usMaxAdc - ADC_VALID_OFFSET)
		|| (_usY >= g_tTP.usMaxAdc - ADC_VALID_OFFSET))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @name   TOUCH_WaitRelease
 * @brief  等待触笔释放
 * @param  无
 * @retval 无
 */
static void TOUCH_WaitRelease(void)
{
	uint8_t usCount = 0;

	for (;;)
	{
		if (TOUCH_PressValid(TOUCH_ReadAdcX(), TOUCH_ReadAdcY()) == 0)
		{
			if (++usCount > 5)
			{
				break;
			}
		}
		else
		{
			usCount = 0;
		}
		bsp_DelayMS(10);
	}
}

/**
 * @name   TOUCH_Abs
 * @brief  计算绝对值
 * @param  x : 有符合整数
 * @retval 正整数
 */
int32_t TOUCH_Abs(int32_t x)
{
	if (x >= 0)
	{
		return x;
	}
	else
	{
		return -x;
	}
}

/**
 * @name   TOUCH_Calibration
 * @brief  触摸屏校准
 * @param  _PointCount : 校准点数，2 或 4.
 * @retval 无
 */
void TOUCH_Calibration(uint8_t _PointCount)
{
	uint16_t usAdcX;
	uint16_t usAdcY;
	uint8_t usCount;
	uint8_t i;
	uint32_t n;

	/* 校准点数，2点或4点 */
	if (_PointCount == 4)
	{
		g_tTPParam.CalibPointCount = 4;
	}
	else
	{
		g_tTPParam.CalibPointCount = 2;
	}	
	
	TOUCH_CelarFIFO();		/* 清除无效的触摸事件 */

	for (i = 0; i < g_tTPParam.CalibPointCount; i++)
	{
//		TOUCH_DispPoint(i);		/* 显示校准点 */

		TOUCH_WaitRelease(); 	/* 等待触笔释放 */

		usCount = 0;
		for (n = 0; n < 500; n++)
		{
			usAdcX = TOUCH_ReadAdcX();
			usAdcY = TOUCH_ReadAdcY();

			if (TOUCH_PressValid(usAdcX, usAdcY))
			{
				if (++usCount > 5)
				{
					/* 按压有效, 保存校准点ADC采样值 */
					if (i == 0)
					{
						g_tTPParam.usAdcX1 = usAdcX;
						g_tTPParam.usAdcY1 = usAdcY;
					}
					else if (i == 1)
					{
						g_tTPParam.usAdcX2 = usAdcX;
						g_tTPParam.usAdcY2 = usAdcY;
					}
					else if (i == 2)
					{
						g_tTPParam.usAdcX3 = usAdcX;
						g_tTPParam.usAdcY3 = usAdcY;
					}
					else
					{
						g_tTPParam.usAdcX4 = usAdcX;
						g_tTPParam.usAdcY4 = usAdcY;
					}
					break;
				}
			}
			else
			{
				usCount = 0;
			}
			bsp_DelayMS(10);
		}
		if (n == 500)
		{
			return;
		}
	}

	TOUCH_WaitRelease(); 	/* 等待触笔释放 */

	/* 识别触摸的 X, Y 和 显示面板的 X，Y 是否需要交换 */
	g_tTPParam.XYChange = 0;		/* 1表示X Y需要交换 */
	if (LCD_GetHeight() < LCD_GetWidth())
	{
		if (TOUCH_Abs(g_tTPParam.usAdcX1 - g_tTPParam.usAdcX2) < TOUCH_Abs(g_tTPParam.usAdcY1 - g_tTPParam.usAdcY2))
		{
			g_tTPParam.XYChange = 1;
		}
	}
	else
	{
		if (TOUCH_Abs(g_tTPParam.usAdcX1 - g_tTPParam.usAdcX2) > TOUCH_Abs(g_tTPParam.usAdcY1 - g_tTPParam.usAdcY2))
		{
			g_tTPParam.XYChange = 1;
		}
	}

	g_tTPParam.usLcdX1 = TP_X1;
	g_tTPParam.usLcdY1 = TP_Y1;
	g_tTPParam.usLcdX2 = TP_X2;
	g_tTPParam.usLcdY2 = TP_Y2;
	g_tTPParam.usLcdX3 = TP_X3;
	g_tTPParam.usLcdY3 = TP_Y3;
	g_tTPParam.usLcdX4 = TP_X4;
	g_tTPParam.usLcdY4 = TP_Y4;

	/* 在最后一步，将校准参数保存入Flash 或者EEPROM */
	TOUCH_SaveParam();
}

/**
 * @name   TOUCH_SaveParam
 * @brief  保存校准参数	s_usAdcX1 s_usAdcX2 s_usAdcY1 s_usAdcX2
 * @param  无
 * @retval 无
 */
static void TOUCH_SaveParam(void)
{
	
	g_tTPParam.TouchDirection = g_LcdDirection;	/* 2014-09-11 添加屏幕方向, 用于屏幕旋转时无需再次校准 */

	#if 1
		/* 写入EEPROM */
		ee_WriteBytes((uint8_t *)&g_tTPParam, TP_PARAM_EE_ADDR, sizeof(g_tTPParam));
	#else
		/* 写入CPU Flash */
		bsp_WriteCpuFlash(TP_PARAM_FLASH_ADDR, (uint8_t *)&g_tTPParam, sizeof(g_tTPParam));
	#endif	
}

/**
 * @name   TOUCH_LoadParam
 * @brief  读取校准参数
 * @param  无
 * @retval 无
 */
static void TOUCH_LoadParam(void)
{
	#if 1
		/* 读取EEPROM中的参数 */
		ee_ReadBytes((uint8_t *)&g_tTPParam, TP_PARAM_EE_ADDR, sizeof(g_tTPParam));
	#else
		/* 读取CPU Flash中的参数 */
		bsp_ReadCpuFlash(TP_PARAM_FLASH_ADDR, (uint8_t *)&g_tTPParam, sizeof(g_tTPParam
	#endif	
	
	if (g_tTPParam.TouchDirection > 4)
	{
		g_tTPParam.TouchDirection = 0;
		TOUCH_SaveParam();
	}
}

