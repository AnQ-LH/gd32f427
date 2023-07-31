#include "bsp_timer.h"
#include "bsp.h"
/*
 * 定义用于硬件定时器的TIM， 可以使 TIM1 - TIM4
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

/* 保存 TIM定时中断到后执行的回调函数指针 */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);

#ifdef TIM_HARD

/**
 * @name   bsp_InitHardTimer
 * @brief  配置 TIMx，用于us级别硬件定时。TIMx将自由运行，永不停止.
		   TIMx可以用TIM1 - TIM4 之间的TIM, 这些TIM有4个通道, 挂在 APB1 上，输入时钟=SystemCoreClock / 4
 * @param  无
 * @retval 无
 */
void bsp_InitHardTimer(void)
{
    timer_parameter_struct  TimHandle = {0};
	uint32_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	uint32_t TIMx = TIM_HARD;
	
	RCC_TIM_HARD_CLK_ENABLE();		/* 使能TIM时钟 */
	
    /*-----------------------------------------------------------------------
		gd32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM1, TIM2 ,TIM3, TIM4, TIM5, TIM6, TIM11, TIM12,TIM13
		APB2 定时器有 TIM0, TIM7 ,TIM8, TIM9, TIM10

	----------------------------------------------------------------------- */
	uiTIMxCLK = SystemCoreClock / 2;

	usPrescaler = uiTIMxCLK / 1000000 - 1;	/* 分频比 = 1 */
	
	if (TIMx == TIMER1 || TIMx == TIMER4)
	{
		usPeriod = 0xFFFFFFFF;
	}
	else
	{
		usPeriod = 0xFFFF;
	}

	/* 
     * 设置分频为usPrescaler后，那么定时器计数器计1次就是1us
     * 而参数usPeriod的值是决定了最大计数：
     * usPeriod = 0xFFFF 表示最大0xFFFF微秒。
     * usPeriod = 0xFFFFFFFF 表示最大0xFFFFFFFF微秒。
     */
	TimHandle.prescaler         = usPrescaler;
	TimHandle.period            = usPeriod;
	TimHandle.clockdivision     = 0;
	TimHandle.counterdirection	= TIMER_COUNTER_UP;
	TimHandle.repetitioncounter = 0;
    TimHandle.alignedmode = TIMER_COUNTER_EDGE;
	
	timer_init(TIMx, &TimHandle);

	/* 配置定时器中断，给CC捕获比较中断使用 */
	{
		nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
		nvic_irq_enable(TIM_HARD_IRQn, 0, 2);
	}
    
    /* 启动定时器 */
	timer_enable(TIMx);
}

/**
 * @name   bsp_StartHardTimer
 * @brief  使用TIM1-4做单次定时器使用, 定时时间到后执行回调函数。可以同时启动4个定时器通道，互不干扰。
		   定时精度正负1us （主要耗费在调用本函数的执行时间）
		   TIM1和TIM4 是32位定时器。定时范围很大
		   TIM2和TIM3 是16位定时器。
 * @param  _CC : 捕获比较通道几，0，1，2, 3
 * @param  _uiTimeOut : 超时时间, 单位 1us. 对于16位定时器，最大 65.5ms; 对于32位定时器，最大 4294秒
 * @param  _pCallBack : 定时时间到后，被执行的函数
 * @retval 无
 */
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;
	uint32_t TIMx = TIM_HARD;
	
    /* 无需补偿延迟，实测精度正负1us */
    
    cnt_now = TIMER_CNT(TIMx); 
    cnt_tar = cnt_now + _uiTimeOut;			/* 计算捕获的计数器值 */
    if (_CC == 0)
    {
        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

		TIMER_CH0CV(TIMx) = cnt_tar;						/* 设置捕获比较计数器CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH0IF;	/* 清除CC0中断标志 */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH0IE;		/* 使能CC1中断 */
	}
    else if (_CC == 1)
    {
		s_TIM_CallBack2 = (void (*)(void))_pCallBack;

		TIMER_CH1CV(TIMx) = cnt_tar;						/* 设置捕获比较计数器CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH1IF;	/* 清除CC0中断标志 */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH1IE;		/* 使能CC1中断 */
    }
    else if (_CC == 2)
    {
        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

		TIMER_CH2CV(TIMx) = cnt_tar;						/* 设置捕获比较计数器CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH2IF;	/* 清除CC0中断标志 */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH2IE;		/* 使能CC1中断 */
    }
    else if (_CC == 3)
    {
        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

		TIMER_CH3CV(TIMx) = cnt_tar;						/* 设置捕获比较计数器CC0 */
        TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH3IF;	/* 清除CC0中断标志 */
		TIMER_DMAINTEN(TIMx) |= TIMER_DMAINTEN_CH3IE;		/* 使能CC1中断 */
    }
	else
    {
        return;
    }
}

/**
 * @name   TIMx_IRQHandler
 * @brief  TIM 中断服务程序
 * @param  无
 * @retval 无
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
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH0IE;		/* 禁能CC0中断 */	

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack1();
    }
	
	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH1IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH1IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH1IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH1IE;		/* 禁能CC1中断 */	

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack2();
    }
	
	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH2IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH2IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH2IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH2IE;		/* 禁能CC2中断 */		

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack3();
    }

	itstatus = TIMER_INTF(TIMx) & TIMER_INTF_CH3IF;
	itenable = TIMER_DMAINTEN(TIMx) & TIMER_DMAINTEN_CH3IE;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		TIMER_INTF(TIMx) &= (uint16_t)~TIMER_INTF_CH3IF;
		TIMER_DMAINTEN(TIMx) &= (uint16_t)~TIMER_DMAINTEN_CH3IE;		/* 禁能CC3中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack4();
    }	
}

#endif
