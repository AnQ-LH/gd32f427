#include "bsp_pwm.h"
#include "bsp.h"

/*
 	���������GPIO��TIMͨ��:
	
	TIM0_CH0	PA8,	PB
	TIM0_CH1	PA9
	TIM0_CH2	PA10
	TIM0_CH13	PA11
	
	TIM1_CH0	PA0,	PB8,	PA5,	PA15
	TIM1_CH1	PA1,	PB9,	PB3
	TIM1_CH2	PA2,	PB10,
	TIM1_CH3	PA3,	PB11,	PB2
	
	TIM2_CH0	PA6,	PC6,	PB4
	TIM2_CH1	PA7,	PC7,	PB5
	TIM2_CH2	PB0,	PC8
	TIM2_CH3	PB1,	PC9
	
	TIM3_CH0	PD12,	PB6
	TIM3_CH1	PD13,	PB7
	TIM3_CH2	PD14,	PB8
	TIM3_CH3	PD15,	PB9
	
	TIM4_CH0	PA0
	TIM4_CH1	PA1
	TIM4_CH2	PA2
	TIM4_CH3	PA3
	
	TIM7_CH0	PC6
	TIM7_CH1	PC7
	TIM7_CH2	PC8
	TIM7_CH3	PC9
	
	TIM8_CH0	PE5,	PA2
	TIM8_CH1	PE6,	PA3
	
	TIM9_CH0	PF6,	PB8
	TIM10_CH0	PF7,	PB9
	
	TIM11_CH0	PB14
	TIM11_CH1	PB15
	
	TIM12_CH0	PF8,	PA6
	TIM13_CH0	PF9,	PA7
	
	APB1 ��ʱ���� TIM1, TIM2 ,TIM3, TIM4, TIM5, TIM6, TIM11, TIM12, TIM13
	APB2 ��ʱ���� TIM0, TIM7 ,TIM8, TIM9, TIM10
	

	APB1 ��ʱ��������ʱ�� TIMxCLK = SystemCoreClock / 2;	100M
	APB2 ��ʱ��������ʱ�� TIMxCLK = SystemCoreClock;	200M
*/


/**
 * @name   bsp_RCC_GPIO_Enable
 * @brief  ʹ��GPIOʱ��
 * @param  GPIOx GPIOA - GPIOG
 * @retval ��
 */
void bsp_RCC_GPIO_Enable(uint32_t GPIOx)
{
	if (GPIOx == GPIOA)			rcu_periph_clock_enable(RCU_GPIOA);
	else if (GPIOx == GPIOB)	rcu_periph_clock_enable(RCU_GPIOB);
	else if (GPIOx == GPIOC)	rcu_periph_clock_enable(RCU_GPIOC);
	else if (GPIOx == GPIOD)	rcu_periph_clock_enable(RCU_GPIOD);
	else if (GPIOx == GPIOE)	rcu_periph_clock_enable(RCU_GPIOE);
	else if (GPIOx == GPIOF)	rcu_periph_clock_enable(RCU_GPIOF);
	else if (GPIOx == GPIOG)	rcu_periph_clock_enable(RCU_GPIOG);
	else
	{
		
	}
}

/**
 * @name   bsp_RCC_TIM_Enable
 * @brief  ʹ��TIM RCC ʱ��
 * @param  TIMx TIM0 - TIM13
 * @retval ��
 */
void bsp_RCC_TIM_Enable(uint32_t TIMx)
{
	if (TIMx == TIMER0)			rcu_periph_clock_enable(RCU_TIMER0);
	else if (TIMx == TIMER1)	rcu_periph_clock_enable(RCU_TIMER1);
	else if (TIMx == TIMER2)	rcu_periph_clock_enable(RCU_TIMER2);
	else if (TIMx == TIMER3)	rcu_periph_clock_enable(RCU_TIMER3);
	else if (TIMx == TIMER4)	rcu_periph_clock_enable(RCU_TIMER4);
	else if (TIMx == TIMER5)	rcu_periph_clock_enable(RCU_TIMER5);
	else if (TIMx == TIMER6)	rcu_periph_clock_enable(RCU_TIMER6);
	else if (TIMx == TIMER7)	rcu_periph_clock_enable(RCU_TIMER7);
	else if (TIMx == TIMER8)	rcu_periph_clock_enable(RCU_TIMER8);
	else if (TIMx == TIMER9)	rcu_periph_clock_enable(RCU_TIMER9);
	else if (TIMx == TIMER10)	rcu_periph_clock_enable(RCU_TIMER10);
	else if (TIMx == TIMER11)	rcu_periph_clock_enable(RCU_TIMER11);
	else if (TIMx == TIMER12)	rcu_periph_clock_enable(RCU_TIMER12);
	else if (TIMx == TIMER13)	rcu_periph_clock_enable(RCU_TIMER13);
	else
	{

	}
}

/**
 * @name   bsp_RCC_TIM_Disable
 * @brief  �ر�TIM RCC ʱ��
 * @param  TIMx TIM0 - TIM13
 * @retval TIM����ʱ����
 */
void bsp_RCC_TIM_Disable(uint32_t TIMx)
{
	if (TIMx == TIMER0)			rcu_periph_clock_disable(RCU_TIMER0);
	else if (TIMx == TIMER1)	rcu_periph_clock_disable(RCU_TIMER1);
	else if (TIMx == TIMER2)	rcu_periph_clock_disable(RCU_TIMER2);
	else if (TIMx == TIMER3)	rcu_periph_clock_disable(RCU_TIMER3);
	else if (TIMx == TIMER4)	rcu_periph_clock_disable(RCU_TIMER4);
	else if (TIMx == TIMER5)	rcu_periph_clock_disable(RCU_TIMER5);
	else if (TIMx == TIMER6)	rcu_periph_clock_disable(RCU_TIMER6);
	else if (TIMx == TIMER7)	rcu_periph_clock_disable(RCU_TIMER7);
	else if (TIMx == TIMER8)	rcu_periph_clock_disable(RCU_TIMER8);
	else if (TIMx == TIMER9)	rcu_periph_clock_disable(RCU_TIMER9);
	else if (TIMx == TIMER10)	rcu_periph_clock_disable(RCU_TIMER10);
	else if (TIMx == TIMER11)	rcu_periph_clock_disable(RCU_TIMER11);
	else if (TIMx == TIMER12)	rcu_periph_clock_disable(RCU_TIMER12);
	else if (TIMx == TIMER13)	rcu_periph_clock_disable(RCU_TIMER13);
	else
	{

	}
}

/**
 * @name   bsp_GetAFofTIM
 * @brief  ����TIM �õ�AF�Ĵ�������
 * @param  TIMx TIM0 - TIM13
 * @retval AF�Ĵ�������
 */
uint8_t bsp_GetAFofTIM(uint32_t TIMx)
{
	uint8_t ret = 0;

	if (TIMx == TIMER0) ret = GPIO_AF_1;
	else if (TIMx == TIMER1) ret = GPIO_AF_1;
	
	else if (TIMx == TIMER2) ret = GPIO_AF_2;
	else if (TIMx == TIMER3) ret = GPIO_AF_2;
	else if (TIMx == TIMER4) ret = GPIO_AF_2;
	
	else if (TIMx == TIMER7) ret = GPIO_AF_3;
	else if (TIMx == TIMER8) ret = GPIO_AF_3;
	else if (TIMx == TIMER9) ret = GPIO_AF_3;
	else if (TIMx == TIMER10) ret = GPIO_AF_3;
	
	else if (TIMx == TIMER11) ret = GPIO_AF_9;
	else if (TIMx == TIMER12) ret = GPIO_AF_9;
	else if (TIMx == TIMER13) ret = GPIO_AF_9;
	else
	{

	}
	
	return ret;
}

/**
 * @name   bsp_ConfigTimGpio
 * @brief  ����GPIO��TIMʱ�ӣ� GPIO���ӵ�TIM���ͨ��
 * @param  GPIOx : GPIOA - GPIOG
 * @param  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
 * @param  TIMx : TIM0 - TIM13
 * @retval AF�Ĵ�������
 */
void bsp_ConfigTimGpio(uint32_t GPIOx, uint16_t GPIO_PinX, uint32_t TIMx)
{
	/* ʹ��GPIOʱ�� */
	bsp_RCC_GPIO_Enable(GPIOx);

  	/* ʹ��TIMʱ�� */
	bsp_RCC_TIM_Enable(TIMx);
	
	gpio_mode_set(GPIOx, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PinX);
    gpio_output_options_set(GPIOx, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PinX);
	gpio_af_set(GPIOx, bsp_GetAFofTIM(TIMx), GPIO_PinX);
}

/**
 * @name   bsp_ConfigGpioOut
 * @brief  ����GPIOΪ�������,��Ҫ����PWM���,ռ�ձ�Ϊ0��100�������
 * @param  GPIOx : GPIOA - GPIOK
 * @param  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
 * @retval ��
 */
void bsp_ConfigGpioOut(uint32_t GPIOx, uint16_t GPIO_PinX)
{
	bsp_RCC_GPIO_Enable(GPIOx);		/* ʹ��GPIOʱ�� */
	
	gpio_mode_set(GPIOx, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PinX);
	gpio_output_options_set(GPIOx, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PinX);
}

/**
 * @name   bsp_SetTIMOutPWM
 * @brief  �������������PWM�źŵ�Ƶ�ʺ�ռ�ձ�.��Ƶ��Ϊ0,����ռ��Ϊ0ʱ,�رն�ʱ��,GPIO���0;
		   ��Ƶ��Ϊ0��ռ�ձ�Ϊ100%ʱ��GPIO���1.
 * @param  GPIOx : GPIOA - GPIOG
 * @param  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
 * @param  GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
 * @param  TIMx : TIM0 - TIM13
 * @param  _ucChannel��ʹ�õĶ�ʱ��ͨ������Χ1 - 4
 * @param  _ulFreq : PWM�ź�Ƶ�ʣ���λHz (ʵ�ʲ��ԣ��������100MHz����0 ��ʾ��ֹ���
 * @param  _ulDutyCycle : PWM�ź�ռ�ձ�,��λ: ���֮һ.��5000,��ʾ50.00%��ռ�ձ�
 * @retval ��
 */
void bsp_SetTIMOutPWM(uint32_t GPIOx, uint16_t GPIO_Pin, uint32_t TIMx, uint8_t _ucChannel, uint32_t _ulFreq, uint32_t _ulDutyCycle)
{
	timer_parameter_struct  TimHandle = {0};
	timer_oc_parameter_struct sConfig = {0};	
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t pulse;
	uint32_t uiTIMxCLK;
	const uint16_t TimChannel[4] = {TIMER_CH_0, TIMER_CH_1, TIMER_CH_2, TIMER_CH_3};

	if (_ucChannel > 4)
	{

	}
	
	if (_ulDutyCycle == 0)
	{		
		//bsp_RCC_TIM_Disable(TIMx);		/* �ر�TIMʱ��, ����Ӱ������ͨ�� */		
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* ����GPIOΪ������� */
		gpio_bit_reset(GPIOx, GPIO_Pin);	/* PWM = 0 */		/* PWM = 0 */		
		return;
	}
	else if (_ulDutyCycle == 10000)
	{
		//bsp_RCC_TIM_Disable(TIMx);		/* �ر�TIMʱ��, ����Ӱ������ͨ�� */
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* ����GPIOΪ������� */		
		gpio_bit_set(GPIOx, GPIO_Pin);		/* PWM = 1 */			
		return;
	}
	
	/* ������PWM��� */
	bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx);	/* ʹ��GPIO��TIMʱ�ӣ�������TIMͨ����GPIO */
	
	/*-----------------------------------------------------------------------
		gd32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 ��ʱ���� TIM1, TIM2 ,TIM3, TIM4, TIM5, TIM6, TIM11, TIM12,TIM13
		APB2 ��ʱ���� TIM0, TIM7 ,TIM8, TIM9, TIM10

	----------------------------------------------------------------------- */
	
	if ((TIMx == TIMER0) || (TIMx == TIMER7) || (TIMx == TIMER8) || (TIMx == TIMER9) || (TIMx == TIMER10))
	{
		/* APB2 ��ʱ��ʱ�� = 200M */
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		/* APB1 ��ʱ�� = 100M */
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* ��Ƶ�� = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else	/* ����4K��Ƶ�ʣ������Ƶ */
	{
		usPrescaler = 0;					/* ��Ƶ�� = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* �Զ���װ��ֵ */
	}
	pulse = (_ulDutyCycle * usPeriod) / 10000;
    
	timer_deinit(TIMx);
	
	/*  PWMƵ�� = TIMxCLK / usPrescaler + 1��/usPeriod + 1��*/
	TimHandle.prescaler         = usPrescaler;
    TimHandle.alignedmode       = TIMER_COUNTER_EDGE;
    TimHandle.counterdirection  = TIMER_COUNTER_UP;
    TimHandle.period            = usPeriod;
    TimHandle.clockdivision     = TIMER_CKDIV_DIV1;
    TimHandle.repetitioncounter = 0;
	
	timer_init(TIMx, &TimHandle);

	/* ���ö�ʱ��PWM���ͨ�� */
	sConfig.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    sConfig.outputstate = TIMER_CCX_ENABLE;
    sConfig.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    sConfig.outputnstate = TIMER_CCXN_DISABLE;
    sConfig.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    sConfig.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
	timer_channel_output_mode_config(TIMx, TimChannel[_ucChannel], TIMER_OC_MODE_PWM0);
	
	timer_channel_output_config(TIMx, TimChannel[_ucChannel], &sConfig);
	
    timer_channel_output_shadow_config(TIMx, TimChannel[_ucChannel], TIMER_OC_SHADOW_DISABLE);
	
	/* ռ�ձ� */
	timer_channel_output_pulse_value_config(TIMx, TimChannel[_ucChannel], pulse);
	
	/* ����PWM��� */
	timer_auto_reload_shadow_enable(TIMx);
	timer_enable(TIMx);
}

/**
 * @name   bsp_SetTIMforInt
 * @brief  ����TIM��NVIC�����ڼ򵥵Ķ�ʱ�жϣ�������ʱ�жϡ�����ע���жϷ��������Ҫ���û�Ӧ�ó���ʵ�֡�
 * @param  TIMx : ��ʱ��
 * @param  _ulFreq : ��ʱƵ�� ��Hz���� 0 ��ʾ�رա� 
 * @param  _PreemptionPriority : ��ռ���ȼ�
 * @param  _SubPriority : �����ȼ�
 * @retval ��
 */
/*	
TIM��ʱ�жϷ�����������������жϱ�־
void TIM6_DAC_IRQHandler(void)
{
	if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
	{
		TIM6->SR = ~ TIM_FLAG_UPDATE;
		//����û�����
	}
}
*/
void bsp_SetTIMforInt(uint32_t TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
	timer_parameter_struct   TimHandle = {0};
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	/* ʹ��TIMʱ�� */
	bsp_RCC_TIM_Enable(TIMx);
	
	/*-----------------------------------------------------------------------
		gd32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 ��ʱ���� TIM1, TIM2 ,TIM3, TIM4, TIM5, TIM6, TIM11, TIM12,TIM13
		APB2 ��ʱ���� TIM0, TIM7 ,TIM8, TIM9, TIM10

	----------------------------------------------------------------------- */
	if ((TIMx == TIMER0) || (TIMx == TIMER7) || (TIMx == TIMER8) || (TIMx == TIMER9) || (TIMx == TIMER10))
	{
		/* APB2 ��ʱ��ʱ�� = 168M */
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		/* APB1 ��ʱ�� = 84M */
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* ��Ƶ�� = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else	/* ����4K��Ƶ�ʣ������Ƶ */
	{
		usPrescaler = 0;					/* ��Ƶ�� = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* �Զ���װ��ֵ */
	}

	/* 
       ��ʱ���жϸ������� = TIMxCLK / usPrescaler + 1��/usPeriod + 1��
	*/
	TimHandle.prescaler         = usPrescaler;
    TimHandle.alignedmode       = TIMER_COUNTER_EDGE;
    TimHandle.counterdirection  = TIMER_COUNTER_UP;
    TimHandle.period            = usPeriod;
    TimHandle.clockdivision     = TIMER_CKDIV_DIV1;
    TimHandle.repetitioncounter = 0;
	
	timer_init(TIMx, &TimHandle);
	
	timer_auto_reload_shadow_enable(TIMx);
	
	
	/* ����TIM��ʱ�����ж� (Update) */
	{
        uint8_t irq = 0;	/* �жϺ�, ������ stm32h7xx.h */

        if (TIMx == TIMER0) irq = TIMER0_UP_TIMER9_IRQn;
        else if (TIMx == TIMER1) irq = TIMER1_IRQn;
        else if (TIMx == TIMER2) irq = TIMER2_IRQn;
        else if (TIMx == TIMER3) irq = TIMER3_IRQn;
        else if (TIMx == TIMER4) irq = TIMER4_IRQn;
        else if (TIMx == TIMER5) irq = TIMER5_DAC_IRQn;
        else if (TIMx == TIMER6) irq = TIMER6_IRQn;
        else if (TIMx == TIMER7) irq = TIMER7_UP_TIMER12_IRQn;
		else if (TIMx == TIMER8) irq = TIMER0_BRK_TIMER8_IRQn;
        else if (TIMx == TIMER9) irq = TIMER0_UP_TIMER9_IRQn;
        else if (TIMx == TIMER10) irq = TIMER0_TRG_CMT_TIMER10_IRQn;
        else if (TIMx == TIMER11) irq = TIMER7_BRK_TIMER11_IRQn;
        else if (TIMx == TIMER12) irq = TIMER7_UP_TIMER12_IRQn;
        else if (TIMx == TIMER13) irq = TIMER7_TRG_CMT_TIMER13_IRQn;
        else
        {
            
        }
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(irq, _PreemptionPriority, _SubPriority);
	}
	
	timer_enable(TIMx);
}
