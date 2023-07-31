#include "bsp_key.h"


/**
 * KEY_0 	: PE4   (High indicates pressed)
 * WK_UP 	: PA0   (High indicates pressed)
 */

#define HARD_KEY_NUM	    2					/* The number of physical keys */
#define KEY_COUNT   	 	( HARD_KEY_NUM )	/* The number of physical keys and  key combinations */

/* enable the led clock */
#define ALL_KEY_GPIO_CLK_ENABLE() {	    \
		rcu_periph_clock_enable(RCU_GPIOA);	\
		rcu_periph_clock_enable(RCU_GPIOE);	\
	};

/* define GPIO */
typedef struct
{
	uint32_t gpio;
	uint32_t pin;
	uint8_t ActiveLevel;	/* Activation level */
}X_GPIO_T;

/* GPIO and PIN */
static const X_GPIO_T s_gpio_list[HARD_KEY_NUM] = {
	{GPIOE, GPIO_PIN_4, 1},		/* KEY_0 */
	{GPIOA, GPIO_PIN_0, 1},		/* WK_UP */
};	

static KEY_T s_tBtn[KEY_COUNT] = { 0 };
static KEY_FIFO_T s_tKey;		/* the FIFO structure variable of Key */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

#define KEY_PIN_ACTIVE(id)	

/**
 * @brief  Determine if the key is pressed
 * @param  none
 * @retval 1 press, 0 release
 */
static uint8_t KeyPinActive(uint8_t _id)
{
	uint8_t level;
	
	if ( ( gpio_input_bit_get(s_gpio_list[_id].gpio, s_gpio_list[_id].pin) ) == 0 )
	{
		level = 0;
	}
	else
	{
		level = 1;
	}

	if (level == s_gpio_list[_id].ActiveLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief  Determine if the key is pressed
 * @param  none
 * @retval 1 press, 0 release
 */
static uint8_t IsKeyDownFunc(uint8_t _id)
{
	/* physical keys */
	if (_id < HARD_KEY_NUM)
	{
		uint8_t i;
		uint8_t count = 0;
		uint8_t save = 255;
		
		/* Judge how many keys are pressed */
		for (i = 0; i < HARD_KEY_NUM; i++)
		{
			if (KeyPinActive(i)) 
			{
				count++;
				save = i;
			}
		}
		
		if (count == 1 && save == _id)
		{
			return 1;	/* Only valid when 1 key is pressed */
		}		

		return 0;
	}

	return 0;
}

/**
 * @brief  Initialize the key
 * @param  none
 * @retval none
 */
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* Initialize key variables */
	bsp_InitKeyHard();		/* Initialize the key hardware */
}

/**
 * @brief  Configure the GPIO
 * @param  none
 * @retval none
 */
static void bsp_InitKeyHard(void)
{	
	uint8_t i;
	
	/* enable the key clock */
	ALL_KEY_GPIO_CLK_ENABLE();
	
	/* configure key GPIO port */ 
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		gpio_mode_set(s_gpio_list[i].gpio, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, s_gpio_list[i].pin);
	}
}

/**
 * @brief  Initialize key variables
 * @param  none
 * @retval none
 */
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* �԰���FIFO��дָ������ */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;

	/* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;				/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;			/* ����������Ϊ�˲�ʱ���һ�� */
		s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
		s_tBtn[i].RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
		s_tBtn[i].RepeatCount = 0;						/* ���������� */
	}

	/* �����Ҫ��������ĳ�������Ĳ����������ڴ˵������¸�ֵ */
	
//	bsp_SetKeyParam(KID_JOY_U, 100, 6);
}

/**
 * @brief  ��1����ֵѹ�밴��FIFO������,������ģ��һ������.
 * @param  KeyCode : ��������
 * @retval ��
 */
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/**
 * @brief  �Ӱ���FIFO��������ȡһ����ֵ.
 * @param  ��
 * @retval ��������
 */
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/**
 * @brief  �Ӱ���FIFO��������ȡһ����ֵ,�����Ķ�ָ��.
 * @param  ��
 * @retval ��������
 */
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/**
 * @brief  ��ȡ������״̬
 * @param  _ucKeyID : ����ID����0��ʼ
 * @retval 1 ��ʾ���£� 0 ��ʾδ����
 */
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/**
 * @brief  ���ð�������
 * @param  _ucKeyID 		����ID����0��ʼ
 * @param  _LongTime 		�����¼�ʱ��
 * @param  _RepeatSpeed	�����ٶ�
 * @retval 1 ��ʾ���£� 0 ��ʾδ����
 */
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;	/* �����������ٶȣ�0��ʾ��֧������ */
	s_tBtn[_ucKeyID].RepeatCount = 0;				/* ���������� */
}

/**
 * @brief  ��հ���FIFO������
 * @param  ��
 * @retval ��������
 */
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/**
 * @brief  ���һ������,������״̬,���뱻�����Եĵ���.
 * @param  IO��id�� ��0��ʼ����
 * @retval ��
 */
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	pBtn = &s_tBtn[i];
	if (IsKeyDownFunc(i))
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/**
 * @brief  �����ٵ�����IO,1msˢ��һ��.
 * @param  IO��id�� ��0��ʼ����
 * @retval ��
 */
static void bsp_DetectFastIO(uint8_t i)
{
	KEY_T *pBtn;

	pBtn = &s_tBtn[i];
	if (IsKeyDownFunc(i))
	{
		if (pBtn->State == 0)
		{
			pBtn->State = 1;

			/* ���Ͱ�ť���µ���Ϣ */
			bsp_PutKey((uint8_t)(3 * i + 1));
		}

		if (pBtn->LongTime > 0)
		{
			if (pBtn->LongCount < pBtn->LongTime)
			{
				/* ���Ͱ�ť�������µ���Ϣ */
				if (++pBtn->LongCount == pBtn->LongTime)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey((uint8_t)(3 * i + 3));
				}
			}
			else
			{
				if (pBtn->RepeatSpeed > 0)
				{
					if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
					{
						pBtn->RepeatCount = 0;
						/* ��������ÿ��10ms����1������ */
						bsp_PutKey((uint8_t)(3 * i + 1));
					}
				}
			}
		}
	}
	else
	{
		if (pBtn->State == 1)
		{
			pBtn->State = 0;

			/* ���Ͱ�ť�������Ϣ */
			bsp_PutKey((uint8_t)(3 * i + 2));
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/**
 * @brief  ɨ�����а�����������,��systick�ж������Եĵ���,10msһ��
 * @param  ��
 * @retval ��
 */
void bsp_KeyScan10ms(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}

/**
 * @brief  ɨ�����а���������������systick�ж������Եĵ��ã�1msһ��.
 * @param  ��
 * @retval ��
 */
void bsp_KeyScan1ms(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectFastIO(i);
	}
}
