#include "bsp_led.h"

/* private variables */

#define HARD_LED_NUM   	 	2	/* 3 LED */

/* enable the led clock */
#define ALL_LED_GPIO_CLK_ENABLE() {	    \
		rcu_periph_clock_enable(RCU_GPIOF);	\
	};

/* define GPIO */
typedef struct
{
	uint32_t gpio;
	uint16_t pin;
	uint8_t ActiveLevel;	/* Activation level */
}X_GPIO_T;


/* GPIO and PIN */
static const X_GPIO_T s_gpio_list[HARD_LED_NUM] = {
	{GPIOF, GPIO_PIN_9, 0},
	{GPIOF, GPIO_PIN_10, 0},
};

/**
 * @brief	configure led GPIO
 * @param	none
 * @retval	none
 */
void bsp_InitLed(void)
{
	uint8_t i;
	
	/* enable the led clock */
	ALL_LED_GPIO_CLK_ENABLE();
    
	
	/* configure led GPIO port */ 
	for (i = 0; i < HARD_LED_NUM; i++)
	{
		gpio_mode_set(s_gpio_list[i].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, s_gpio_list[i].pin);
		gpio_output_options_set(s_gpio_list[i].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, s_gpio_list[i].pin);
		
		/* turn off all led */
		GPIO_BOP(s_gpio_list[i].gpio) = s_gpio_list[i].pin;
	}
}

/**
 * @brief	turn on selected led
 * @param	led_num: specify the led to be turned on
 * @retval	none
 */
void bsp_LedOn(led_typedef_enum led_num)
{
	gpio_bit_write(s_gpio_list[led_num].gpio, s_gpio_list[led_num].pin, s_gpio_list[led_num].ActiveLevel);
}

/**
 * @brief	turn off selected led
 * @param	led_num: specify the led to be turned on
 * @retval	none
 */
void bsp_LedOff(led_typedef_enum led_num)
{
	gpio_bit_write(s_gpio_list[led_num].gpio, s_gpio_list[led_num].pin, !s_gpio_list[led_num].ActiveLevel);
}

/**
 * @brief	toggle selected led
 * @param	led_num: specify the led to be turned on
 * @retval	none
 */
void bsp_LedToggle(led_typedef_enum led_num)
{
	GPIO_TG(s_gpio_list[led_num].gpio) = s_gpio_list[led_num].pin;
}

/**
 * @brief	get selected led state
 * @param	led_num: specify the led to get state
 * @retval	1 LedOn 0 LedOff
 */
uint8_t bsp_IsLedOn(led_typedef_enum led_num)
{
	if (led_num == LED1)
	{
		if (gpio_output_bit_get(s_gpio_list[led_num].gpio, s_gpio_list[led_num].pin) == s_gpio_list[led_num].ActiveLevel)
		{
			return 1;
		}
		return 0;
	}
	else if (led_num == LED2)
	{
		if (gpio_output_bit_get(s_gpio_list[led_num].gpio, s_gpio_list[led_num].pin) == s_gpio_list[led_num].ActiveLevel)
		{
			return 1;
		}
		return 0;
	}

	return 0;
}