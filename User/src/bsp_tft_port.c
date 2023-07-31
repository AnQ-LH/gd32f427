#include "bsp_tft_port.h"
#include "bsp.h"

/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
uint16_t g_ChipID = IC_5310;		/* ����оƬID */
uint16_t g_LcdHeight;			/* ��ʾ���ֱ���-�߶� */
uint16_t g_LcdWidth;			/* ��ʾ���ֱ���-��� */
uint8_t s_ucBright;					/* �������Ȳ��� */
uint8_t g_LcdDirection;				/* ��ʾ���� */

static void LCD_CtrlLinesConfig(void);
static void LCD_EXMCConfig(void);
static void LCD_HardReset(void);

void SOFT1_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t _usColor);
void SOFT_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode);

LCD_DEV_T g_tLCD;

/**
 * @name   LCD_InitHard
 * @brief  ��ʼ��LCD
 * @param  ��
 * @retval ��
 */
void LCD_InitHard(void)
{
	uint16_t id;

	/* ����LCD���ƿ���GPIO */
	LCD_CtrlLinesConfig();

	/* ����EXMC�ӿڣ��������� */
	LCD_EXMCConfig();

	LCD_HardReset();
	
	/* EXMC���ú������ӳٲ��ܷ��������豸  */
	bsp_DelayMS(50);

	id = NT35310_ReadID();
	
	if (id == 0x5310)
	{
		g_ChipID = IC_5310;
		NT35310_InitHard();			/* ��ʼ��NT35310оƬ */
	}
	
	if (g_ChipID == IC_5310)
	{
		g_tLCD.DispOn = NT35310_DispOn;
		g_tLCD.DispOff = NT35310_DispOff;
		g_tLCD.ClrScr = NT35310_ClrScr;
		g_tLCD.PutPixel = NT35310_PutPixel;
		g_tLCD.GetPixel = NT35310_GetPixel;
		g_tLCD.DrawLine = NT35310_DrawLine;
		g_tLCD.DrawRect = NT35310_DrawRect;
		g_tLCD.DrawCircle = NT35310_DrawCircle;
		g_tLCD.FillRect = NT35310_FillRect;
	}
	
	LCD_SetDirection(7);
	LCD_ClrScr(0x001F);	 /* ��������ʾȫ�� */
	LCD_SetBackLight(255); /* �򿪱��⣬����Ϊȱʡ���� */
}

/**
 * @name   LCD_HardReset
 * @brief  Ӳ����λ. ��Ը�λ������GPIO���ƵĲ�Ʒ��
 * @param  ��
 * @retval ��
 */
void LCD_HardReset(void)
{
	/* ʹ�� GPIOʱ�� */
	rcu_periph_clock_enable(RCU_GPIOG);
	
	/* ���ø�λGPIOΪ�������ģʽ */
	gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	
	gpio_bit_set(GPIOG, GPIO_PIN_6);
	bsp_DelayMS(1);
	gpio_bit_reset(GPIOG, GPIO_PIN_6);
	bsp_DelayMS(10);
	gpio_bit_set(GPIOG, GPIO_PIN_6);
	
}

/**
 * @name   LCD_GetChipDescribe
 * @brief  ��ȡLCD����оƬ���������ţ�������ʾ
 * @param  char *_str : �������ַ�������˻�����
 * @retval ��
 */
void LCD_GetChipDescribe(char *_str)
{
	switch (g_ChipID)
	{
		case IC_5310:
			strcpy(_str, CHIP_STR_5310);
			break;

		default:
			strcpy(_str, "Unknow");
			break;
	}
}

/**
 * @name   LCD_CtrlLinesConfig
 * @brief  ����LCD���ƿ��ߣ�FSMC�ܽ�����Ϊ���ù���
 * @param  ��
 * @retval ��
 */
/*
	GD32��������߷���:

	EXMC_D0 PD14
	EXMC_D1 PD15
	EXMC_D2 PD0
	EXMC_D3 PD1

	EXMC_D4 PE7
	EXMC_D5 PE8
	EXMC_D6 PE9
	EXMC_D7 PE10
	EXMC_D8 PE11
	EXMC_D9 PE12
	EXMC_D10 PE13
	EXMC_D11 PE14
	EXMC_D12 PE15

	EXMC_D13 PD8
	EXMC_D14 PD9
	EXMC_D15 PD10
	
	EXMC_A6 PF12
	
	EXMC_NOE PD4
	EXMC_NWE PD5
	
	EXMC_NE3 PG12

	---- ������ TFT LCD�ӿ������ź� ��FSMCģʽ��ʹ�ã�----
	PB15/LCD_PWM		--- LCD����PWM����
	PG6/LCD_RST			--- LCD��λ���� 

	PB1/T_PEN			--- ����оƬ���ж�����
	PC13/T_CS			--- ����оƬ��Ƭѡ
	PB0/T_SCK			--- ����оƬSPIʱ��
	PB2/T_MISO			--- ����оƬSPI������MISO
	PF11/T_MOSI			--- ����оƬSPI������MOSI
*/
static void LCD_CtrlLinesConfig(void)
{
	/* EXMC enable */
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_GPIOF);
	rcu_periph_clock_enable(RCU_GPIOG);
	

	/* EXMC clock enable */
	rcu_periph_clock_enable(RCU_EXMC);

	/* GPIOD configuration */
	gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |  GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |  GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |  GPIO_PIN_14 | GPIO_PIN_15);

	/* GPIOE configuration */
	gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

	/* GPIOF configuration */
	gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_12);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	
	/* GPIOG configuration */
	gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_12);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
}

/**
 * @name   LCD_FSMCConfig
 * @brief  ����FSMC���ڷ���ʱ��
 * @param  ��
 * @retval ��
 */
static void LCD_EXMCConfig(void)
{
	exmc_norsram_parameter_struct  nor_init_struct;
	exmc_norsram_timing_parameter_struct  sram_timing_read;
	exmc_norsram_timing_parameter_struct  sram_timing_write;
	
	/* configure timing parameter */
    sram_timing_read.asyn_access_mode = EXMC_ACCESS_MODE_A;
    sram_timing_read.syn_data_latency = EXMC_DATALAT_2_CLK;
    sram_timing_read.syn_clk_division = EXMC_SYN_CLOCK_RATIO_DISABLE;
    sram_timing_read.bus_latency = 0;
    sram_timing_read.asyn_data_setuptime = 26;
    sram_timing_read.asyn_address_holdtime = 0;
    sram_timing_read.asyn_address_setuptime = 80;
	
	sram_timing_write.asyn_access_mode = EXMC_ACCESS_MODE_A;
    sram_timing_write.syn_data_latency = EXMC_DATALAT_2_CLK;
    sram_timing_write.syn_clk_division = EXMC_SYN_CLOCK_RATIO_DISABLE;
    sram_timing_write.bus_latency = 0;
    sram_timing_write.asyn_data_setuptime = 20;
    sram_timing_write.asyn_address_holdtime = 0;
    sram_timing_write.asyn_address_setuptime = 20;

    /* configure EXMC bus parameters */
    nor_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION3;
    nor_init_struct.write_mode = EXMC_ASYN_WRITE;
    nor_init_struct.extended_mode = DISABLE;
    nor_init_struct.asyn_wait = DISABLE;
    nor_init_struct.nwait_signal = DISABLE;
    nor_init_struct.memory_write = ENABLE;
    nor_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    nor_init_struct.wrap_burst_mode = DISABLE;
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.burst_mode = DISABLE;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    nor_init_struct.address_data_mux = DISABLE;
    nor_init_struct.read_write_timing = &sram_timing_read;
    nor_init_struct.write_timing = &sram_timing_write;
	
    exmc_norsram_init(&nor_init_struct);
	
    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION3);
	
	bsp_DelayMS(50);
}

/**
 * @name   LCD_SetPwmBackLight
 * @brief  ��ʼ������LCD�������GPIO
 * @param  _bright ���ȣ�0����255������
 * @retval ��
 */
void LCD_SetPwmBackLight(uint8_t _bright)
{
	bsp_SetTIMOutPWM(GPIOB, GPIO_PIN_15, TIMER11, 1, 100, (_bright * 10000) /255);
}

/**
 * @name   LCD_SetBackLight
 * @brief  ��ʼ������LCD�������GPIO,����ΪPWMģʽ��
 * @param  _bright ���ȣ�0����255������
 * @retval ��
 */
void LCD_SetBackLight(uint8_t _bright)
{
	s_ucBright =  _bright;	/* ���汳��ֵ */

	LCD_SetPwmBackLight(_bright);
}

/**
 * @name   LCD_GetBackLight
 * @brief  ��ñ������Ȳ���
 * @param  ��
 * @retval �������Ȳ���
 */
uint8_t LCD_GetBackLight(void)
{
	return s_ucBright;
}

/**
 * @name   LCD_SetDirection
 * @brief  ������ʾ����ʾ���򣨺��� ������
 * @param  ��ʾ������� 0 ��������, 1=����180�ȷ�ת, 2=����, 3=����180�ȷ�ת
 * @retval ��
 */
void LCD_SetDirection(uint8_t _dir)
{
	g_LcdDirection =  _dir;		/* ������ȫ�ֱ��� */

	NT35310_SetDirection(_dir);
}

/**
 * @name   LCD_GetHeight
 * @brief  ��ȡLCD�ֱ���֮�߶�
 * @param  ��
 * @retval �߶�
 */
uint16_t LCD_GetHeight(void)
{
	return g_LcdHeight;
}

/**
 * @name   LCD_GetWidth
 * @brief  ��ȡLCD�ֱ���֮���
 * @param  ��
 * @retval ���
 */
uint16_t LCD_GetWidth(void)
{
	return g_LcdWidth;
}

/**
 * @name   LCD_DispOn
 * @brief  ����ʾ
 * @param  ��
 * @retval ��
 */
void LCD_DispOn(void)
{
	g_tLCD.DispOn();
}

/**
 * @name   LCD_DispOff
 * @brief  �ر���ʾ
 * @param  ��
 * @retval ��
 */
void LCD_DispOff(void)
{
	g_tLCD.DispOn();
}

/**
 * @name   LCD_ClrScr
 * @brief  �����������ɫֵ����
 * @param  _usColor : ����ɫ
 * @retval ��
 */
void LCD_ClrScr(uint16_t _usColor)
{
	g_tLCD.ClrScr(_usColor);
}

/**
 * @name   LCD_PutPixel
 * @brief  ��1������
 * @param  _usX,_usY : ��������
 *		   _usColor  : ������ɫ
 * @retval ��
 */
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	g_tLCD.PutPixel(_usX, _usY, _usColor);
}

/**
 * @name   LCD_GetPixel
 * @brief  ��ȡ1������
 * @param  _usX,_usY : ��������
 *		   _usColor  : ������ɫ
 * @retval RGB��ɫֵ
 */
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
	return g_tLCD.GetPixel(_usX, _usY);
}

/**
 * @name   LCD_DrawLine
 * @brief  ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
 * @param  _usX,_usY : ��������
 *		   _usColor  : ������ɫ
 *		   _usColor	 : ��ɫ
 * @retval ��
 */
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	g_tLCD.DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
}

/**
 * @name   LCD_DrawRect
 * @brief  ����ˮƽ���õľ��Ρ�
 * @param  _usX,_usY : �������Ͻǵ�����
 *		   _usHeight : ���εĸ߶�
 *		   _usWidth  : ���εĿ��
 * @retval ��
 */
void LCD_DrawRect(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
	g_tLCD.DrawRect(_usX1, _usY1, _usX2, _usY2, _usColor);
}

/**
 * @name   LCD_Fill_Rect
 * @brief  ��һ����ɫֵ���һ�����Ρ���emWin ����ͬ������ LCD_FillRect����˼����»������֡�
 * @param  _usX,_usY : �������Ͻǵ�����
 *		   _usHeight : ���εĸ߶�
 *		   _usWidth  : ���εĿ��
 * @retval ��
 */
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _ueX, uint16_t _ueY, uint16_t _usColor)
{
	g_tLCD.FillRect(_usX, _usY, _ueX, _ueY, _usColor);
}

/**
 * @name   SOFT_DrawLine
 * @brief  ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
 * @param  _usX1, _usY1 ����ʼ������
 *		   _usX2, _usY2 ����ֹ��Y����
 *		   _usColor     ����ɫ
 * @retval ��
 */
void SOFT_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	int32_t dx , dy ;
	int32_t tx , ty ;
	int32_t inc1 , inc2 ;
	int32_t d , iTag ;
	int32_t x , y ;

	/* ���� Bresenham �㷨����2��仭һ��ֱ�� */

	g_tLCD.PutPixel(_usX1 , _usY1 , _usColor);

	/* ��������غϣ���������Ķ�����*/
	if ( _usX1 == _usX2 && _usY1 == _usY2 )
	{
		return;
	}
	
	/* ���ƴ�ֱ�� */
	if (_usX1 == _usX2)
	{
		if (_usY2 > _usY1)
		{
			for (y = _usY1; y <= _usY2; y++)
			{
				g_tLCD.PutPixel(_usX1, y, _usColor);
			}
		}
		else
		{
			for (y = _usY2; y <= _usY1; y++)
			{
				g_tLCD.PutPixel(_usX1, y, _usColor);
			}			
		}
	}
	
	/* ����ˮƽ�� */
	if (_usY1 == _usY2)
	{
		if (_usX2 > _usX1)
		{
			for (x = _usX1; x <= _usX2; x++)
			{
				g_tLCD.PutPixel(x, _usY1, _usColor);
			}
		}
		else
		{
			for (x = _usX2; x <= _usX1; x++)
			{
				g_tLCD.PutPixel(x, _usY1, _usColor);
			}			
		}
	}	

	iTag = 0 ;
	/* dx = abs ( _usX2 - _usX1 ); */
	if (_usX2 >= _usX1)
	{
		dx = _usX2 - _usX1;
	}
	else
	{
		dx = _usX1 - _usX2;
	}

	/* dy = abs ( _usY2 - _usY1 ); */
	if (_usY2 >= _usY1)
	{
		dy = _usY2 - _usY1;
	}
	else
	{
		dy = _usY1 - _usY2;
	}

	if ( dx < dy )   /*���dyΪ�Ƴ������򽻻��ݺ����ꡣ*/
	{
		uint16_t temp;

		iTag = 1 ;
		temp = _usX1; _usX1 = _usY1; _usY1 = temp;
		temp = _usX2; _usX2 = _usY2; _usY2 = temp;
		temp = dx; dx = dy; dy = temp;
	}
	tx = _usX2 > _usX1 ? 1 : -1 ;    /* ȷ������1���Ǽ�1 */
	ty = _usY2 > _usY1 ? 1 : -1 ;
	x = _usX1 ;
	y = _usY1 ;
	inc1 = 2 * dy ;
	inc2 = 2 * ( dy - dx );
	d = inc1 - dx ;
	while ( x != _usX2 )     /* ѭ������ */
	{
		if ( d < 0 )
		{
			d += inc1 ;
		}
		else
		{
			y += ty ;
			d += inc2 ;
		}
		if ( iTag )
		{
			g_tLCD.PutPixel( y , x , _usColor) ;
		}
		else
		{
			g_tLCD.PutPixel( x , y , _usColor) ;
		}
		x += tx ;
	}
}

/**
 * @name   SOFT_DrawCircle
 * @brief  ����һ��Բ���ʿ�Ϊ1������, ʹ������㷨���ƣ�û������IC��Ӳ������
 * @param  _usX,_usY  : Բ�ĵ�����
 *		   _usRadius  : Բ�İ뾶
 * @retval ��
 */
void SOFT_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{
		g_tLCD.PutPixel(_usX + CurX, _usY + CurY, _usColor);
		g_tLCD.PutPixel(_usX + CurX, _usY - CurY, _usColor);
		g_tLCD.PutPixel(_usX - CurX, _usY + CurY, _usColor);
		g_tLCD.PutPixel(_usX - CurX, _usY - CurY, _usColor);
		g_tLCD.PutPixel(_usX + CurY, _usY + CurX, _usColor);
		g_tLCD.PutPixel(_usX + CurY, _usY - CurX, _usColor);
		g_tLCD.PutPixel(_usX - CurY, _usY + CurX, _usColor);
		g_tLCD.PutPixel(_usX - CurY, _usY - CurX, _usColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @name   SOFT_DrawQuterCircle
 * @brief  ����һ��1/4Բ���ʿ�Ϊ1������, ʹ������㷨����
 * @param  _usX,_usY  : Բ�ĵ�����
 *		   _usRadius  : Բ�İ뾶
 *		   _ucMode    : 0 ��ʾ���Ͻ�1/4Բ 1��ʾ���Ͻ�  2��ʾ���½� 3��ʾ���½�
 * @retval ��
 */
void SOFT_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{	
		if (_ucMode == 0)
		{
			g_tLCD.PutPixel(_usX - CurY, _usY - CurX, _usColor);   // �� -> ��
			g_tLCD.PutPixel(_usX - CurX, _usY - CurY, _usColor);   // �� -> ��
		}
		else if (_ucMode == 1)
		{
			g_tLCD.PutPixel(_usX + CurX, _usY - CurY, _usColor);	// �� -> ��
			g_tLCD.PutPixel(_usX + CurY, _usY - CurX, _usColor);	// �� -> ��	
		}
		else if (_ucMode == 2)
		{
			g_tLCD.PutPixel(_usX + CurX, _usY + CurY, _usColor);	// �� -> ��
			g_tLCD.PutPixel(_usX + CurY, _usY + CurX, _usColor);	// �� -> ��
		}
		else if (_ucMode == 3)
		{			
			g_tLCD.PutPixel(_usX - CurX, _usY + CurY, _usColor);	// �� -> ��
			g_tLCD.PutPixel(_usX - CurY, _usY + CurX, _usColor);    // �� -> ��
		}
		
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @name   SOFT_FillCircle
 * @brief  ���һ��Բ������㷨ʵ�֡�
 * @param  _usX,_usY  : Բ�ĵ�����
 *		   _usRadius  : Բ�İ뾶
 *		   _usColor   : ������ɫ
 * @retval ��
 */
void SOFT_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{			
		g_tLCD.DrawLine(_usX + CurX, _usY + CurY, _usX - CurX, _usY + CurY, _usColor);
		g_tLCD.DrawLine(_usX + CurX, _usY - CurY, _usX - CurX, _usY - CurY, _usColor);
		g_tLCD.DrawLine(_usX + CurY, _usY + CurX, _usX - CurY, _usY + CurX, _usColor);
		g_tLCD.DrawLine(_usX + CurY, _usY - CurX, _usX - CurY, _usY - CurX, _usColor);  	

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @name   SOFT_FillQuterCircle
 * @brief  ���һ��1/4Բ������㷨ʵ��
 * @param  _usX,_usY  : Բ�ĵ�����
 *		   _usRadius  : Բ�İ뾶
 *		   _usColor   : ������ɫ
 *		   _ucMode    : 0 ��ʾ���Ͻ�1/4Բ 1��ʾ���Ͻ�  2��ʾ���½� 3��ʾ���½�
 * @retval ��
 */
void SOFT_FillQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
	int32_t  D;
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{			
		if (_ucMode == 0)
		{
			g_tLCD.DrawLine(_usX - CurY, _usY - CurX, _usX, _usY - CurX, _usColor);   // �� -> ��
			g_tLCD.DrawLine(_usX - CurX, _usY - CurY, _usX, _usY - CurY, _usColor);   // �� -> ��
		}
		else if (_ucMode == 1)
		{
			g_tLCD.DrawLine(_usX + CurX, _usY - CurY, _usX, _usY - CurY, _usColor);	// �� -> ��
			g_tLCD.DrawLine(_usX + CurY, _usY - CurX, _usX, _usY - CurX, _usColor);	// �� -> ��	
		}
		else if (_ucMode == 2)
		{
			g_tLCD.DrawLine(_usX + CurX, _usY + CurY, _usX, _usY + CurY, _usColor);	// �� -> ��
			g_tLCD.DrawLine(_usX + CurY, _usY + CurX, _usX, _usY + CurX, _usColor);	// �� -> ��
		}
		else if (_ucMode == 3)
		{			
			g_tLCD.DrawLine(_usX - CurX, _usY + CurY, _usX, _usY + CurY, _usColor);	// �� -> ��
			g_tLCD.DrawLine(_usX - CurY, _usY + CurX, _usX, _usY + CurX, _usColor);    // �� -> ��
		}		
		
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @name   SOFT_DrawRoundRect
 * @brief  ����Բ�Ǿ����������ʿ��1����
 * @param  _usX,_usY:�������Ͻǵ�����
 *		   _usHeight :���εĸ߶�
 *		   _usWidth  :���εĿ��
 *		   _usArc    :Բ�ǵĻ��뾶
 *		   _usColor  :��ɫ
 * @retval ��
 */
void SOFT_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor)
{
	if (_usHeight < 2 *_usRadius)
	{
		_usHeight = 2 *_usRadius;
	}

	if (_usWidth < 2 *_usRadius)
	{
		_usWidth = 2 *_usRadius;
	}	
	
	SOFT_DrawQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0);	/* ���ϽǵĻ� */
	g_tLCD.DrawLine(_usX + _usRadius, _usY, _usX + _usWidth - _usRadius - 1, _usY, _usColor);
	
	SOFT_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1);	/* ���ϽǵĻ� */
	g_tLCD.DrawLine(_usX + _usWidth - 1, _usY + _usRadius, _usX + _usWidth - 1, _usY + _usHeight  - _usRadius - 1, _usColor);
	
	SOFT_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2);	/* ���½ǵĻ� */
	g_tLCD.DrawLine(_usX + _usRadius, _usY + _usHeight - 1, _usX + _usWidth - _usRadius - 1, _usY + _usHeight - 1, _usColor);
			
	SOFT_DrawQuterCircle(_usX + _usRadius,  _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3);	/* ���½ǵĻ� */
	g_tLCD.DrawLine(_usX, _usY + _usRadius, _usX,  _usY + _usHeight - _usRadius - 1, _usColor);
}

/**
 * @name   SOFT_FillRoundRect
 * @brief  ���Բ�Ǿ���
 * @param  _usX,_usY:�������Ͻǵ�����
 *		   _usHeight :���εĸ߶�
 *		   _usWidth  :���εĿ��
 *		   _usArc    :Բ�ǵĻ��뾶
 *		   _usColor  :��ɫ
 * @retval ��
 */
void SOFT_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor)
{
	if (_usHeight < 2 *_usRadius)
	{
		_usHeight = 2 *_usRadius;
	}

	if (_usWidth < 2 *_usRadius)
	{
		_usWidth = 2 *_usRadius;
	}	
	
	SOFT_FillQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0);	/* ���ϽǵĻ� */

	g_tLCD.FillRect(_usX + _usRadius + 1,  _usY,  _usRadius + 1, _usWidth - 2 * _usRadius - 2, _usColor);
	
	SOFT_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1);	/* ���ϽǵĻ� */

	g_tLCD.FillRect(_usX, _usY + _usRadius, _usHeight - 2 * _usRadius, _usWidth, _usColor);

	SOFT_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2);	/* ���½ǵĻ� */

	g_tLCD.FillRect(_usX + _usRadius + 1,  _usY + _usHeight - _usRadius - 1,  _usRadius + 1, _usWidth - 2 * _usRadius - 2, _usColor);	

	SOFT_FillQuterCircle(_usX + _usRadius,  _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3);	/* ���½ǵĻ� */
}

