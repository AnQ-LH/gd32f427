#include "bsp_ts_touch.h"
#include "bsp_gt1151.h"
#include "bsp.h"


/* ���Դ�ӡ��� */
#define touch_printf       printf

/*
	������δʹ�ô����жϹ��ܡ���1ms���ڵ� Systick��ʱ�жϷ�������жԴ���ADCֵ���в�����
	�˲����������ʰ��³���30ms�󣬿�ʼ�ɼ�ADCֵ��ÿ1ms�ɼ�1�Σ������ɼ�10�Σ���Ȼ���
	��10�������������򣬶���2ͷ�����������м�һ������������Ͳ�����ƽ��ֵ��

	����2��У׼������У׼�󱣴�2��У׼���ADCֵ��ʵ�ʹ���ʱ������2��ֱ�߷��̼�����Ļ���ꡣ
	У׼�����б���ӿڣ���������Ҫ������ʾ��δ�����湦�ܡ�
	��ҿ����Լ��޸�  TOUCH_SaveParam() �� TOUCH_LoadParam() ��������ʵ�ֱ��湦�ܡ�
*/

/* ÿ1msɨ��һ������ */
#define DOWN_VALID		30	/* ����30ms ��, ��ʼͳ��ADC */
#define SAMPLE_COUNT	10	/* ���º������ɼ�10������ */

/*
������У׼�������Ļ�����Ľǵ�ƫ������
��1���� �� x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
��2���� �� x2 = LCD_GetWidth() - CALIB_OFFSET, y2 = LCD_GetHeight() - CALIB_OFFSET
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

/* RA8875 ��ЧADCֵ���ж�����. ̫�ӽ�ADC�ٽ�ֵ��������Ϊ��Ч */
#define ADC_VALID_OFFSET	2


/* У׼���������EEPROM(AT24C128����16KB) ���64�ֽ����� */
#define TP_PARAM_EE_ADDR		(16*1024 - 64)

/* TP_INT ����GPIO�˿� */
#define TP_INT_GPIO_CLK_ENABLE()	rcu_periph_clock_enable(RCU_GPIOB);
#define TP_INT_GPIO_PORT			GPIOB
#define TP_INT_PIN					GPIO_PIN_1
	
	
/* ����ģ���õ���ȫ�ֱ��� */
TOUCH_T g_tTP;

TP_CALIB_PARAM_T g_tTPParam;

static uint8_t	TOUCH_PressValid(uint16_t _usX, uint16_t _usY);
static uint16_t TOUCH_DataFilter(uint16_t *_pBuf, uint8_t _ucCount);
static void TOUCH_LoadParam(void);
static void TOUCH_SaveParam(void);
static int32_t CalTwoPoint(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x);
int32_t TOUCH_Abs(int32_t x);
	
uint8_t g_TouchType = 0xFF;		/* �������� */
uint8_t g_LcdType = 0xFF;		/* LCD������ */


/**
 * @name   bsp_DetectLcdType
 * @brief  ͨ��I2C����оƬ��ʶ��LCDģ�����͡���������ȫ�ֱ��� g_LcdType �� g_TouchType
 * @param  ��
 * @retval ��
 */
void bsp_DetectLcdType(void)
{
	uint8_t i;
	
	g_TouchType = 0xFF;
	g_LcdType = 0xFF;
	
	bsp_DelayUS(50000);
	
	touch_printf("����ʶ�������ͺ�\r\n");
	
	/* 50ms���ȴ�GT1151��λ����������̽��GT1151оƬ ID */
	for (i = 0; i < 5; i++)
	{		
		/*
			GT1151���ݴ������ַ0xBA ���֡�
			ͨ����ȡ����IC��оƬID��ʶ��
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
				touch_printf("��⵽3.5�� GT1151(0x14) 480x320\r\n");
			}
			else
			{	
				touch_printf("δ��⵽������GT1151��ID\r\n");
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

			/* 4.3���� */
			g_LcdType = LCD_35_480X320;
		}
	}
	
	if (i == 5)
	{
		touch_printf("δʶ�����ʾģ��\r\n");
	}
}

/**
 * @name   TOUCH_IntWakeUpForGT
 * @brief  INT���Ѻ�������λ��INT�����ߵ�ƽ���壬���Ի��Ѵ������ߵ�оƬ��
 * @param  ��
 * @retval ��
 */
void TOUCH_IntWakeUpForGT(void)
{
	/* ��1������GPIOʱ�� */
	TP_INT_GPIO_CLK_ENABLE();
	
	/* ��2�����������еİ���GPIOΪ��������ģʽ */
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
 * @brief  ��ʼ������оƬ�� ��֮ǰ��������ִ�� bsp_DetectLcdType() ʶ����������оƬ�ͺ�.
 * @param  ��
 * @retval ��
 */
void TOUCH_InitHard(void)
{	
	//TOUCH_IntWakeUpForGT();
	
	/* ����TP_INT�����ж����� */
	{
		/* ��1������GPIOʱ�� */
		TP_INT_GPIO_CLK_ENABLE();
		
		/* ��2�����������еİ���GPIOΪ��������ģʽ */
		gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TP_INT_PIN);
		gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, TP_INT_PIN);
	}
	
	GT1151_I2CInit();
	
	bsp_DetectLcdType();	/* �Զ�ʶ����оƬ�ͺ� */
	
	/* ���������� */
    g_tTP.Enable = 0;
	
	switch (g_TouchType)
	{		
		case CT_GT1151:			/* ���ݴ��� 3.5�� */
			GT1151_InitHard();
			break;
		default:
			break;
	}
}

/**
 * @name   TOUCH_PenInt
 * @brief  �жϴ�������. INT Ϊ�ͱ�ʾ�д������¡� ע���ڳ�������ʱ��INT�������Իָ�Ϊ�ߵ�ƽ
 *		   ������Լ2ms.
 * @param  ��
 * @retval 0 ��ʾ�޴��ʰ��£� 1��ʾ�д�������
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
 * @brief  ��ô�����X����ADC����ֵ�� �ѽ����˲�����
 * @param  ��
 * @retval X ����ADCֵ
 */
uint16_t TOUCH_ReadAdcX(void)
{
	uint16_t usAdc;

	__set_PRIMASK(1);  		/* ���ж� */
	usAdc = g_tTP.usAdcNowX;
	__set_PRIMASK(0);  		/* ���ж� */

	return usAdc;
}

/**
 * @name   TOUCH_ReadAdcY
 * @brief  ��ô�����Y����ADC����ֵ�� �ѽ����˲�����
 * @param  ��
 * @retval Y ����ֵ������ֵ
 */
uint16_t TOUCH_ReadAdcY(void)
{
	uint16_t usAdc;

	__set_PRIMASK(1);  		/* ���ж� */
	usAdc = g_tTP.usAdcNowY;
	__set_PRIMASK(0);  		/* ���ж� */

	return usAdc;
}

/**
 * @name   TOUCH_PutKey
 * @brief  ��1������������ֵѹ�봥��FIFO�����������败�����β���ADCֵ�����ݴ������β�������ֵ
 * @param  _usX, _usY ����ֵ
 * @retval ��
 */
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
	uint16_t xx, yy;
	uint16_t x = 0, y = 0;

	g_tTP.Event[g_tTP.Write] = _ucEvent;

	if (g_tTP.Enable == 1)	/* �������� �β���ADCֵ */
	{

	}
	else	/* ���ݴ����ߴ˷�֮ */
	{
		/* ����ת���� ֱ��������ֵ */
		xx = _usX;
		yy = _usY;		
	}
	
	switch (g_LcdDirection)
	{
		case 0:	/* У׼����ʱ����Ļ����Ϊ 0 ���� �� */
		{
			x = xx;
			y = yy;
		}		
		break;
		
		case 3:	/* У׼����ʱ����Ļ����Ϊ 3 ���� ��ת */
		{
			x = g_LcdHeight - xx - 1;
			y = g_LcdWidth - yy - 1;
		}		
		break;
		
		case 6:	/* У׼����ʱ����Ļ����Ϊ 6 ���� */
		{
			x = g_LcdHeight - yy - 1;
			y = xx;
		}
		break;
		
		case 7:	/* У׼����ʱ����Ļ����Ϊ 7 ���� ��ת*/
		{
			x = yy;
			y = g_LcdWidth - xx - 1;
		}
		break;
		default:
			g_LcdDirection = 0;	/* ���������Чʱ������Ϊȱʡ�ĺ��� */
		break;
	}
	
	g_tTP.XBuf[g_tTP.Write] = x;
	g_tTP.YBuf[g_tTP.Write] = y;

	if (++g_tTP.Write  >= TOUCH_FIFO_SIZE)
	{
		g_tTP.Write = 0;
	}
	
	/* ������䣬��ӡadc������ */
	touch_printf("%d - (%d, %d)\r\n", _ucEvent, x, y);
}

/**
 * @name   TOUCH_GetKey
 * @brief  �Ӵ���FIFO��������ȡһ������ֵ��
 * @param  _pX, _pY ����
 * @retval TOUCH_NONE		��ʾ���¼�
 *		   TOUCH_DOWN		����
 *		   TOUCH_MOVE		�ƶ�
 *		   TOUCH_RELEASE	�ͷ�
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
 * @brief  �������FIFO������
 * @param  ��
 * @retval ��
 */
void TOUCH_CelarFIFO(void)
{
	__set_PRIMASK(1);  		/* ���ж� */
	g_tTP.Write = g_tTP.Read;
	__set_PRIMASK(0);  		/* ���ж� */
}

/**
 * @name   TOUCH_InRect
 * @brief  �жϵ�ǰ�����Ƿ�λ�ھ��ο���
 * @param  _usX, _usY: ��������
 * @param  _usRectX,_usRectY: �������
 * @param  _usRectH��_usRectW : ���θ߶ȺͿ��
 * @retval 1 ��ʾ�ڷ�Χ��
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
 * @brief  �жϵ�ǰ������ϴ������Ƿ�ƫ��̫��
 * @param  _usX1, _usY1: ����1
 * @param  _usX2, _usY2: ����2
 * @retval 1 ��ʾ��Ч�㣬 0 ��ʾ�ɵ�
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
 * @brief  I2C�ӿڵ��ݴ�����ɨ�躯�������� bsp_Idle()ִ�У�
 * @param  ��
 * @retval ��
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
 * @brief  �������¼������򡣸ú����������Ե��ã�ÿms����1��. �� bsp_Timer.c
 * @param  ��
 * @retval ��
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
	static uint16_t s_usSaveAdcX, s_usSaveAdcY; /* ���ڴ���̧���¼������水�º��ƶ���������ֵ */
	
	if (g_GT1151.Enable == 1)	/* 3.5����ݹ��� GT1151 */
	{
		GT1151_Timer1ms();	/* ���ݴ�������������� */
		return;
	}

}

/**
 * @name   CalTwoPoint
 * @brief  ����2��ֱ�߷��̣�����Yֵ
 * @param  2����������x������
 * @retval x��Ӧ��yֵ
 */
static int32_t CalTwoPoint(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x)
{
	return y1 + ((int32_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/**
 * @name   TOUCH_DataFilter
 * @brief  �Բ������ݽ����˲�
 * @param  ��
 * @retval X ����ֵ������ֵ
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

	/* �������� */
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
 * @brief  ��ʾ��1��У׼��
 * @param  _ucIndex = 0 : ��ʾ��1���㣻 _ucIndex = 1 ��ʾ��2����;
 * @retval ��
 */
static void TOUCH_DispPoint(uint8_t _ucIndex)
{
	FONT_T tFont16;			/* ����һ������ṹ���������������������� */

	/* ����������� */
	{
		tFont16.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont16.FrontColor = CL_WHITE;		/* ������ɫ 0 �� 1 */
		tFont16.BackColor = CL_BLUE;	/* ���ֱ�����ɫ */
		tFont16.Space = 0;			/* ���ּ�࣬��λ = ���� */
	}

/*
	��1���� �� x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
	��2���� �� x2 = LCD_GetHeight() - CALIB_OFFSET, y2 = LCD_GetWidth - CALIB_OFFSET
*/
	if (_ucIndex == 0)
	{
		LCD_ClrScr(CL_BLUE);  		/* ������������ɫ */

		/* ����Ļ���ػ���2�����ο�(���ڼ������Ե�����Ƿ�����) */
		LCD_DrawRect(0, 0, LCD_GetHeight(), LCD_GetWidth(), CL_WHITE);
		LCD_DrawRect(2, 2, LCD_GetHeight() - 4, LCD_GetWidth() - 4, CL_YELLOW);

		LCD_DispStr(50, 10, "Touch Calibration", &tFont16);		/* ��(8,3)���괦��ʾһ������ */

		LCD_DrawCircle(TP_X1, TP_Y1, 6, CL_WHITE);
	}
	else if (_ucIndex == 1)
	{
		LCD_DrawCircle(TP_X1, TP_Y1, 6, CL_BLUE);			/* ������1���� */

		LCD_DrawCircle(TP_X2, TP_Y2, 6, CL_WHITE);
	}
	else if (_ucIndex == 2)
	{
		LCD_DrawCircle(TP_X2, TP_Y2, 6, CL_BLUE);			/* ������2���� */

		LCD_DrawCircle(TP_X3, TP_Y3, 6, CL_WHITE);
	}
	else
	{
		LCD_DrawCircle(TP_X3, TP_Y3, 6, CL_BLUE);			/* ������3���� */

		LCD_DrawCircle(TP_X4, TP_Y4, 6, CL_WHITE);
	}
}
#endif

/**
 * @name   TOUCH_PressValid
 * @brief  �жϰ�ѹ�Ƿ���Ч������X, Y��ADCֵ���д����ж�
 * @param  ��
 * @retval 1 ��ʾ��Ч�� 0 ��ʾ��Ч
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
 * @brief  �ȴ������ͷ�
 * @param  ��
 * @retval ��
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
 * @brief  �������ֵ
 * @param  x : �з�������
 * @retval ������
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
 * @brief  ������У׼
 * @param  _PointCount : У׼������2 �� 4.
 * @retval ��
 */
void TOUCH_Calibration(uint8_t _PointCount)
{
	uint16_t usAdcX;
	uint16_t usAdcY;
	uint8_t usCount;
	uint8_t i;
	uint32_t n;

	/* У׼������2���4�� */
	if (_PointCount == 4)
	{
		g_tTPParam.CalibPointCount = 4;
	}
	else
	{
		g_tTPParam.CalibPointCount = 2;
	}	
	
	TOUCH_CelarFIFO();		/* �����Ч�Ĵ����¼� */

	for (i = 0; i < g_tTPParam.CalibPointCount; i++)
	{
//		TOUCH_DispPoint(i);		/* ��ʾУ׼�� */

		TOUCH_WaitRelease(); 	/* �ȴ������ͷ� */

		usCount = 0;
		for (n = 0; n < 500; n++)
		{
			usAdcX = TOUCH_ReadAdcX();
			usAdcY = TOUCH_ReadAdcY();

			if (TOUCH_PressValid(usAdcX, usAdcY))
			{
				if (++usCount > 5)
				{
					/* ��ѹ��Ч, ����У׼��ADC����ֵ */
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

	TOUCH_WaitRelease(); 	/* �ȴ������ͷ� */

	/* ʶ������ X, Y �� ��ʾ���� X��Y �Ƿ���Ҫ���� */
	g_tTPParam.XYChange = 0;		/* 1��ʾX Y��Ҫ���� */
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

	/* �����һ������У׼����������Flash ����EEPROM */
	TOUCH_SaveParam();
}

/**
 * @name   TOUCH_SaveParam
 * @brief  ����У׼����	s_usAdcX1 s_usAdcX2 s_usAdcY1 s_usAdcX2
 * @param  ��
 * @retval ��
 */
static void TOUCH_SaveParam(void)
{
	
	g_tTPParam.TouchDirection = g_LcdDirection;	/* 2014-09-11 �����Ļ����, ������Ļ��תʱ�����ٴ�У׼ */

	#if 1
		/* д��EEPROM */
		ee_WriteBytes((uint8_t *)&g_tTPParam, TP_PARAM_EE_ADDR, sizeof(g_tTPParam));
	#else
		/* д��CPU Flash */
		bsp_WriteCpuFlash(TP_PARAM_FLASH_ADDR, (uint8_t *)&g_tTPParam, sizeof(g_tTPParam));
	#endif	
}

/**
 * @name   TOUCH_LoadParam
 * @brief  ��ȡУ׼����
 * @param  ��
 * @retval ��
 */
static void TOUCH_LoadParam(void)
{
	#if 1
		/* ��ȡEEPROM�еĲ��� */
		ee_ReadBytes((uint8_t *)&g_tTPParam, TP_PARAM_EE_ADDR, sizeof(g_tTPParam));
	#else
		/* ��ȡCPU Flash�еĲ��� */
		bsp_ReadCpuFlash(TP_PARAM_FLASH_ADDR, (uint8_t *)&g_tTPParam, sizeof(g_tTPParam
	#endif	
	
	if (g_tTPParam.TouchDirection > 4)
	{
		g_tTPParam.TouchDirection = 0;
		TOUCH_SaveParam();
	}
}

