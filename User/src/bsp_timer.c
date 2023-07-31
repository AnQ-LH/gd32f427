#include "bsp_timer.h"
#include "bsp.h"
/*
 * ��������Ӳ����ʱ����TIM�� ����ʹ TIM1 - TIM4
 */
#define USE_TIM1
//#define USE_TIM2
//#define USE_TIM3
//#define USE_TIM4

#ifdef USE_TIM1
	#define TIM_HARD					TIMER1
	#define	RCC_TIM_HARD_CLK_ENABLE()	rcu_periph_clock_enable(RCU_TIMER1)
	#define TIM_HARD_IRQn				TIMER1_IRQn
	#define TIM_HARD_IRQHandler			TIMER1_IRQHandler
#endif

#ifdef USE_TIM2
	#define TIM_HARD					TIMER2
	#define	RCC_TIM_HARD_CLK_ENABLE()	rcu_periph_clock_enable(RCU_TIMER2)
	#define TIM_HARD_IRQn				TIMER2_IRQn
	#define TIM_HARD_IRQHandler			TIMER2_IRQHandler
#endif

#ifdef USE_TIM3
	#define TIM_HARD					TIMER3
	#define	RCC_TIM_HARD_CLK_ENABLE()	rcu_periph_clock_enable(RCU_TIMER3)
	#define TIM_HARD_IRQn				TIMER3_IRQn
	#define TIM_HARD_IRQHandler			TIMER3_IRQHandler
#endif

#ifdef USE_TIM4
	#define TIM_HARD					TIMER4
	#define	RCC_TIM_HARD_CLK_ENABLE()	rcu_periph_clock_enable(RCU_TIMER4)
	#define TIM_HARD_IRQn				TIMER4_IRQn
	#define TIM_HARD_IRQHandler			TIMER4_IRQHandler
#endif

/* ���� TIM��ʱ�жϵ���ִ�еĻص�����ָ�� */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);

#ifdef TIM_HARD

/**
 * @name   bsp_InitHardTimer
 * @brief  ���� TIMx������us����Ӳ����ʱ��TIMx���������У�����ֹͣ.
		   TIMx������TIM1 - TIM4 ֮���TIM, ��ЩTIM��4��ͨ��, ���� APB1 �ϣ�����ʱ��=SystemCoreClock / 4
 * @param  ��
 * @retval ��
 */
void bsp_InitHardTimer(void)
{
    timer_parameter_struct  TimHandle = {0};
	uint32_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	uint32_t TIMx = TIM_HARD;
	
	RCC_TIM_HARD_CLK_ENABLE();		/* ʹ��TIMʱ�� */
	
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
	uiTIMxCLK = SystemCoreClock / 2;

	usPrescaler = uiTIMxCLK / 1000000 - 1;	/* ��Ƶ�� = 1 */
	
	if (TIMx == TIMER1 || TIMx == TIMER4)
	{
		usPeriod = 0xFFFFFFFF;
	}
	else
	{
		usPeriod = 0xFFFF;
	}

	/* 
     * ���÷�ƵΪusPrescaler����ô��ʱ����������1�ξ���1us
     * ������usPeriod��ֵ�Ǿ�������������
     * usPeriod = 0xFFFF ��ʾ���0xFFFF΢�롣
     * usPeriod = 0xFFFFFFFF ��ʾ���0xFFFFFFFF΢�롣
     */
	TimHandle.prescaler         = usPrescaler;
	TimHandle.period            = usPeriod;
	TimHandle.clockdivision     = 0;
	TimHandle.counterdirection	= TIMER_COUNTER_UP;
	TimHandle.repetitioncounter = 0;
    TimHandle.alignedmode = TIMER_COUNTER_EDGE;
	
	timer_init(TIMx, &TimHandle);

	/* ���ö�ʱ���жϣ���CC����Ƚ��ж�ʹ�� */
	{
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(TIM_HARD_IRQn, 0, 2);
	}
    
    /* ������ʱ�� */
	timer_enable(TIMx);
}

/**
 * @name   bsp_StartHardTimer
 * @brief  ʹ��TIM1-4�����ζ�ʱ��ʹ��, ��ʱʱ�䵽��ִ�лص�����������ͬʱ����4����ʱ��ͨ�����������š�
		   ��ʱ��������1us ����Ҫ�ķ��ڵ��ñ�������ִ��ʱ�䣩
		   TIM1��TIM4 ��32λ��ʱ������ʱ��Χ�ܴ�
		   TIM2��TIM3 ��16λ��ʱ����
 * @param  _CC : ����Ƚ�ͨ������0��1��2, 3
 * @param  _uiTimeOut : ��ʱʱ��, ��λ 1us. ����16λ��ʱ������� 65.5ms; ����32λ��ʱ������� 4294��
 * @param  _pCallBack : ��ʱʱ�䵽�󣬱�ִ�еĺ���
 * @retval ��
 */
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;
	uint32_t TIMx = TIM_HARD;
	
    /* ���貹���ӳ٣�ʵ�⾫������1us */
    
    cnt_now = TIMER_CNT(TIMx); 
    cnt_tar = cnt_now + _uiTimeOut;			/* ���㲶��ļ�����ֵ */
    if (_CC == 0)
    {
        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

		TIMER_CH0CV(TIMx) = cnt_tar;						/* ���ò���Ƚϼ�����CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH0IF;	/* ���CC0�жϱ�־ */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH0IE;		/* ʹ��CC1�ж� */
	}
    else if (_CC == 1)
    {
		s_TIM_CallBack2 = (void (*)(void))_pCallBack;

		TIMER_CH1CV(TIMx) = cnt_tar;						/* ���ò���Ƚϼ�����CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH1IF;	/* ���CC0�жϱ�־ */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH1IE;		/* ʹ��CC1�ж� */
    }
    else if (_CC == 2)
    {
        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

		TIMER_CH2CV(TIMx) = cnt_tar;						/* ���ò���Ƚϼ�����CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH2IF;	/* ���CC0�жϱ�־ */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH2IE;		/* ʹ��CC1�ж� */
    }
    else if (_CC == 3)
    {
        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

		TIMER_CH3CV(TIMx) = cnt_tar;						/* ���ò���Ƚϼ�����CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH3IF;	/* ���CC0�жϱ�־ */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH3IE;		/* ʹ��CC1�ж� */
    }
	else
    {
        return;
    }
}

/**
 * @name   TIMx_IRQHandler
 * @brief  TIM �жϷ������
 * @param  ��
 * @retval ��
 */
void TIM_HARD_IRQHandler(void)
{
	uint16_t itstatus = 0x0, itenable = 0x0;
	uint32_t TIMx = TIM_HARD;
	
    
  	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH0IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH0IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH0IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH0IE;		/* ����CC0�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack1();
    }
	
	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH1IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH1IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH1IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH1IE;		/* ����CC1�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack2();
    }
	
	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH2IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH2IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH2IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH2IE;		/* ����CC2�ж� */		

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack3();
    }

	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH3IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH3IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH3IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH3IE;		/* ����CC3�ж� */

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack4();
    }	
}

#endif
