#include "bsp_gt1151.h"
#include <string.h>
#include "bsp.h"

/* enable the led clock */
#define ALL_GT1151_I2C_GPIO_CLK_ENABLE() {	    \
		rcu_periph_clock_enable(RCU_GPIOB);	\
		rcu_periph_clock_enable(RCU_GPIOC);	\
		rcu_periph_clock_enable(RCU_GPIOF);	\
	};

#define GT1151_I2C_SCL_GPIO	GPIOB			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define GT1151_I2C_SDA_GPIO	GPIOF			/* ���ӵ�SDA�����ߵ�GPIO */
#define GT1151_I2C_RST_GPIO	GPIOC			/* ���ӵ�RST�����ߵ�GPIO */
	
#define GT1151_I2C_SCL_PIN		GPIO_PIN_0			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define GT1151_I2C_SDA_PIN		GPIO_PIN_11			/* ���ӵ�SDA�����ߵ�GPIO */
#define GT1151_I2C_RST_PIN		GPIO_PIN_13			/* ���ӵ�RST�����ߵ�GPIO */
	
#define GT1151_I2C_SCL_1()		GPIO_BOP(GT1151_I2C_SCL_GPIO) = GT1151_I2C_SCL_PIN						/* SCL = 1 */
#define GT1151_I2C_SCL_0()		GPIO_BOP(GT1151_I2C_SCL_GPIO) = ((uint32_t)GT1151_I2C_SCL_PIN << 16U)	/* SCL = 0 */

#define GT1151_I2C_SDA_1()		GPIO_BOP(GT1151_I2C_SDA_GPIO) = GT1151_I2C_SDA_PIN						/* SDA = 1 */
#define GT1151_I2C_SDA_0()		GPIO_BOP(GT1151_I2C_SDA_GPIO) = ((uint32_t)GT1151_I2C_SDA_PIN << 16U)	/* SDA = 0 */

#define GT1151_I2C_RST_1()		GPIO_BOP(GT1151_I2C_RST_GPIO) = GT1151_I2C_RST_PIN						/* RST = 1 */
#define GT1151_I2C_RST_0()		GPIO_BOP(GT1151_I2C_RST_GPIO) = ((uint32_t)GT1151_I2C_RST_PIN << 16U)	/* RST = 0 */

#define GT1151_I2C_SDA_READ()	((GPIO_ISTAT(GT1151_I2C_SDA_GPIO) & GT1151_I2C_SDA_PIN) != 0)			/* ��SDA����״̬ */
#define GT1151_I2C_SCL_READ()	((GPIO_ISTAT(GT1151_I2C_SCL_GPIO) & GT1151_I2C_SCL_PIN) != 0)			/* ��SDA����״̬ */


/* GT1151�����������ò�����һ����д�� */ 
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
 * @brief  ���ô���оƬI2C��ʼ��
 * @param  ��
 * @retval ��
 */
void GT1151_I2CInit(void)
{
	/* I2C���߳�ʼ�� */
	ALL_GT1151_I2C_GPIO_CLK_ENABLE();
	
	/* ���Ի�I2Cʱ�� */
	gpio_mode_set(GT1151_I2C_SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_SCL_PIN);
	gpio_output_options_set(GT1151_I2C_SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_SCL_PIN);
	
	/* ���Ի�I2C������ */
	gpio_mode_set(GT1151_I2C_SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_SDA_PIN);
	gpio_output_options_set(GT1151_I2C_SDA_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_SDA_PIN);
	
	/* ���Ի�I2C��λ�� */
	gpio_mode_set(GT1151_I2C_RST_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GT1151_I2C_RST_PIN);
	gpio_output_options_set(GT1151_I2C_RST_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GT1151_I2C_RST_PIN);
	
	GT1151_I2C_RST_0();
	bsp_DelayMS(10);
	GT1151_I2C_RST_1();
	bsp_DelayMS(10);
	
	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	gt1151_i2c_Stop();
}

/**
 * @name   GT1151_InitHard
 * @brief  ���ô���оƬ.  �ڵ��øú���ǰ������ִ�� bsp_touch.c �еĺ��� bsp_DetectLcdType() ʶ��id
 * @param  ��
 * @retval ��
 */
void GT1151_InitHard(void)
{
	//uint32_t id;
	
	g_GT1151.TimerCount = 0;
	
#if 0	/* ���Դ��� */
	id = GT1151_ReadID();	
	
	printf("GT1151 ID :%08X\r\n", id);
#endif
	
	bsp_DelayMS(50);
	
	/*
		���贫�����ò���
		GT911_WriteReg(GT911_CONFIG_REG, (uint8_t *)s_GT911_CfgParams, sizeof(s_GT911_CfgParams));
	*/
	
	g_GT1151.TimerCount = 0;
	g_GT1151.Enable = 1;
}

/**
 * @name   GT1151_ReadID
 * @brief  ���GT1151��оƬID
 * @param  ��
 * @retval 1 ��ȷ��ȡGT1151ID
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
 * @brief  д1���������Ķ���Ĵ���
 * @param  _usRegAddr : �Ĵ�����ַ
 *		   _pRegBuf : �Ĵ������ݻ�����
 *		   _ucLen : ���ݳ���
 * @retval ��
 */
void GT1151_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	uint8_t i;

    gt1151_i2c_Start();					/* ���߿�ʼ�ź� */

    gt1151_i2c_SendByte(GT1151_CMD_WR);	/* ����д�ź� */
	gt1151_i2c_WaitAck();

    gt1151_i2c_SendByte(_usRegAddr >> 8);	/* ��ַ��8λ */
	gt1151_i2c_WaitAck();

    gt1151_i2c_SendByte(_usRegAddr&0XFF);	/* ��ַ��8λ */
	gt1151_i2c_WaitAck();

	for (i = 0; i < _ucLen; i++)
	{
	    gt1151_i2c_SendByte(_pRegBuf[i]);	/* �Ĵ������� */
		gt1151_i2c_WaitAck();
	}

    gt1151_i2c_Stop();						/* ����ֹͣ�ź� */
}

/**
 * @name   GT1151_ReadReg
 * @brief  ��1���������Ķ���Ĵ���
 * @param  _usRegAddr : �Ĵ�����ַ
 *		   _pRegBuf : �Ĵ������ݻ�����
 *		   _ucLen : ���ݳ���
 * @retval ��
 */
void GT1151_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	{
		gt1151_i2c_Start();					/* ���߿�ʼ�ź� */

		gt1151_i2c_SendByte(GT1151_CMD_WR);	/* �����豸��ַ+д�ź� */
		gt1151_i2c_WaitAck();

		gt1151_i2c_SendByte(_usRegAddr >> 8);	/* ��ַ��8λ */
		gt1151_i2c_WaitAck();

		gt1151_i2c_SendByte(_usRegAddr);		/* ��ַ��8λ */
		gt1151_i2c_WaitAck();

		gt1151_i2c_Start();
		gt1151_i2c_SendByte(GT1151_CMD_RD);	/* �����豸��ַ+���ź� */

		gt1151_i2c_WaitAck();
	}
	
	for (i = 0; i < _ucLen; i++)
	{	   
    	_pRegBuf[i] = gt1151_i2c_ReadByte(i == (_ucLen-1) ? 0 : 1); //������	  
	}
	
//	for (i = 0; i < 30; i++);

//	for (i = 0; i < _ucLen - 1; i++)
//	{
//	    _pRegBuf[i] = gt1151_i2c_ReadByte();	/* ���Ĵ������� */
//		gt1151_i2c_Ack();
//	}

//	/* ���һ������ */
//	 _pRegBuf[i] = gt1151_i2c_ReadByte();		/* ���Ĵ������� */
//	gt1151_i2c_NAck();

    gt1151_i2c_Stop();							/* ����ֹͣ�ź� */
}

/**
 * @name   GT1151_Timer1ms
 * @brief  ÿ��1ms����1��
 * @param  ��
 * @retval ��
 */
void GT1151_Timer1ms(void)
{
	g_GT1151.TimerCount++;
}

const uint16_t GT1151_TPX_TBL[5]={GT1151_TP0_REG, GT1151_TP1_REG, GT1151_TP2_REG, GT1151_TP3_REG, GT1151_TP4_REG};
/**
 * @name   GT1151_Scan
 * @brief  ��ȡGT1151�������ݡ���ȡȫ�������ݡ�
 * @param  ��
 * @retval ��
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
	
	/* 20ms ִ��һ�� */
	if (g_GT1151.TimerCount < 10)
	{
		return;
	}

	g_GT1151.TimerCount = 0;

#if 1	/* ����1: ���INT���ŵ�ƽ. */
	if (TOUCH_PenInt() == 0)
	{		
#else	/* ����2������INT���ţ���״̬�Ĵ��� */
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

#if 1		/* һ��Ӧ��ֻ��1�� */
	GT1151_ReadReg(GT1151_GSTID_REG, buf, 8);
#else		/* ��5�������� */
	GT1151_ReadReg(GT1151_GSTID_REG, buf, 40);
#endif
	
	GT1151_WriteReg(GT1151_GSTID_REG, &clear_flag, 1);		/* ������������д0��� */
	
	/*
		0x814E R/W Bufferstatus Large_Detect number of touch points 
			Bit7: Buffer status��1��ʾ���꣨�򰴼����Ѿ�׼���ã����ؿ��Զ�ȡ��0��ʾδ������������Ч�������ض�ȡ������󣬱���ͨ��I2C���˱�־���������ֽڣ�дΪ0��
			Bit4: HaveKey, 1��ʾ�а�����0��ʾ�ް������Ѿ��ɼ�����
			Bit3~0: Number of touch points, ���ϵ���������
	
		0x814F R Point1 track id 
		0x8150 R Point1Xl ������ 1��X ����� 8 λ 
		0x8151 R Point1Xh ������ 1��X ����� 8 λ 
		0x8152 R Point1Yl ������ 1��Y ����� 8 λ 
		0x8153 R Point1Yh ������ 1��Y ����� 8 λ 
		0x8154 R Point1 ������ 1����������� 8 λ 
		0x8155 R Point1 ������ 1����������� 8 λ 
		0x8156 ----
	
		0x8157 R Point2 track id 
		0x8158 R Point2Xl ������ 2��X ����� 8 λ 
		0x8159 R Point2Xh ������ 2��X ����� 8 λ 
		0x815A R Point2Yl ������ 2��Y ����� 8 λ 
		0x815B R Point2Yh ������ 2��Y ����� 8 λ 
		0x815C R Point2 ������ 2����������� 8 λ 
		0x815D R Point2 ������ 2����������� 8 λ 
		0x815E ----

		0x815F R Point3 track id 
		0x8160 R Point3Xl ������ 3��X ����� 8 λ 
		0x8161 R Point3Xh ������ 3��X ����� 8 λ 
		0x8162 R Point3Yl ������ 3��Y ����� 8 λ 
		0x8163 R Point3Yh ������ 3��Y ����� 8 λ 
		0x8164 R Point3 ������ 3����������� 8 λ 
		0x8165 R Point3 ������ 3����������� 8 λ 
		0x8166 ----

		0x8167 R Point4 track id 
		0x8168 R Point4Xl ������ 4��X ����� 8 λ 
		0x8169 R Point4Xh ������ 4��X ����� 8 λ 
		0x816A R Point4Yl ������ 4��Y ����� 8 λ 
		0x816B R Point4Yh ������ 4��Y ����� 8 λ 
		0x816C R Point4 ������ 4����������� 8 λ 
		0x816D R Point4 ������ 4����������� 8 λ 
		0x816E ----

		0x816F R Point5 track id 
		0x8170 R Point5Xl ������ 5��X ����� 8 λ 
		0x8171 R Point5Xh ������ 5��X ����� 8 λ 
		0x8172 R Point5Yl ������ 5��Y ����� 8 λ 
		0x8173 R Point5Yh ������ 5��Y ����� 8 λ 
		0x8174 R Point5 ������ 5����������� 8 λ 
		0x8175 R Point5 ������ 5����������� 8 λ 
		0x8176 --
		
	*/
	g_GT1151.TouchpointFlag = buf[0];
	g_GT1151.Touchkeystate = buf[1];

	g_GT1151.X0 = ((uint16_t)buf[3] << 8) + buf[2];
	g_GT1151.Y0 = ((uint16_t)buf[5] << 8) + buf[4];
	g_GT1151.P0 = ((uint16_t)buf[7] << 8) + buf[6];

	#if 0	/* ����4��һ�㲻�� */
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

	/* ��ⰴ�� */
	{
		{
			/* ���� 0 ���Ͻ�����Ϊ0,0  3 ���½�����Ϊ0,0*/
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
			
			/* ����Ļ */
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
	x_save = x;	/* �������꣬�����ͷ��¼� */
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
 * @brief  I2C����λ�ӳ٣����400KHz
 * @param  ��
 * @retval ��
 */
static void gt1151_i2c_Delay(void)
{
	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	bsp_DelayUS(2);
}

/**
 * @name   gt1151_i2c_Start
 * @brief  CPU����I2C���������ź�
 * @param  ��
 * @retval ��
 */
void gt1151_i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
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
 * @brief  CPU����I2C����ֹͣ�ź�
 * @param  ��
 * @retval ��
 */
void gt1151_i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	GT1151_I2C_SDA_0();
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();
	gt1151_i2c_Delay();
	GT1151_I2C_SDA_1();
	gt1151_i2c_Delay();
}

/**
 * @name   gt1151_i2c_SendByte
 * @brief  CPU��I2C�����豸����8bit����
 * @param  _ucByte �� �ȴ����͵��ֽ�
 * @retval ��
 */
void gt1151_i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
		GT1151_I2C_SCL_0();	/* 2019-03-14 ���GT811���ݴ��������һ�У��൱���ӳټ�ʮns */
		if (i == 7)
		{
			 GT1151_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */	
	}
}

/**
 * @name   gt1151_i2c_ReadByte
 * @brief  CPU��I2C�����豸��ȡ8bit����
 * @param  ��
 * @retval ����������
 */
uint8_t gt1151_i2c_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
	
	if (!ack)gt1151_i2c_NAck();//����nACK
    else gt1151_i2c_Ack(); //����ACK
	
	return value;
}

/**
 * @name   gt1151_i2c_WaitAck
 * @brief  CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
 * @param  ��
 * @retval ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
 */
uint8_t gt1151_i2c_WaitAck(void)
{
	uint8_t re;

	GT1151_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	gt1151_i2c_Delay();
	if (GT1151_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
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
 * @brief  CPU����һ��ACK�ź�
 * @param  ��
 * @retval ��
 */
void gt1151_i2c_Ack(void)
{
	GT1151_I2C_SDA_0();	/* CPU����SDA = 0 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU����1��ʱ�� */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
	GT1151_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	
	gt1151_i2c_Delay();
}

/**
 * @name   i2c_NAck
 * @brief  CPU����1��NACK�ź�
 * @param  ��
 * @retval ��
 */
void gt1151_i2c_NAck(void)
{
	GT1151_I2C_SDA_1();	/* CPU����SDA = 1 */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_1();	/* CPU����1��ʱ�� */
	gt1151_i2c_Delay();
	GT1151_I2C_SCL_0();
	gt1151_i2c_Delay();
}

/**
 * @name   i2c_CheckDevice
 * @brief  ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
 * @param  _Address���豸��I2C���ߵ�ַ
 * @retval ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
 */
uint8_t gt1151_i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	
	if (GT1151_I2C_SDA_READ() && GT1151_I2C_SCL_READ())
	{
		gt1151_i2c_Start();		/* ���������ź� */

		/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
		gt1151_i2c_SendByte(_Address | GT1151_I2C_WR);
		ucAck = gt1151_i2c_WaitAck();	/* ����豸��ACKӦ�� */

		gt1151_i2c_Stop();			/* ����ֹͣ�ź� */

		return ucAck;
	}
	return 1;	/* I2C�����쳣 */
}