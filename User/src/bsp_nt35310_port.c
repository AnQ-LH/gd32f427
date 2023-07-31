#include "bsp_nt35310_port.h"
#include "bsp.h"

/**
 * @name   NT35310_ConfigGPIO
 * @brief  配置CPU访问接口，如FSMC. SPI等
 * @param  无
 * @retval 无
 */
void NT35310_ConfigGPIO(void)
{
	static uint8_t s_run_first = 0;
	
	/* 如果已经运行过，则不再执行 */
	if (s_run_first == 1)
	{
		return;
	}
	
	s_run_first = 1;
	
	
	/* FSMC在 bsp_tft_lcd.c中已经配置好 */
}

/**
 * @name   NT35310_Delaly1us
 * @brief  主要用于NT35310 PLL启动之前发送指令间的延迟
 * @param  无
 * @retval 无
 */
void NT35310_Delaly1us(void)
{
	bsp_DelayUS(1);
}

/**
 * @name   NT35310_Delaly1ms
 * @brief  主要用于NT35310 PLL启动之前发送指令间的延迟
 * @param  无
 * @retval 无
 */
void NT35310_Delaly1ms(void)
{
	bsp_DelayUS(1000);
}

/**
 * @name   NT35310_WriteCmd
 * @brief  写NT35310指令寄存器
 * @param  无
 * @retval 无
 */
void NT35310_WriteCmd(__IO uint16_t _ucRegAddr)
{
	_ucRegAddr = _ucRegAddr;
	LCD->LCD_REG = _ucRegAddr;	/* 设置寄存器地址 */
}

/**
 * @name   NT35310_WriteData
 * @brief  写NT35310数据寄存器
 * @param  无
 * @retval 无
 */
void NT35310_WriteData(__IO uint16_t _ucRegValue)
{
	_ucRegValue = _ucRegValue;
	LCD->LCD_RAM = _ucRegValue;	/* 写LCD数据 */
}

/**
 * @name   NT35310_ReadData
 * @brief  读NT353105寄存器值
 * @param  无
 * @retval 寄存器值
 */
uint16_t NT35310_ReadData(void)
{
	__IO uint16_t value;
	
	value = LCD->LCD_RAM;		/* 读取寄存器值 */
	
	return value;	
}

/**
 * @name   NT35310_ReadStatus
 * @brief  读NT35310状态寄存器
 * @param  无
 * @retval 无
 */
uint8_t NT35310_ReadStatus(void)
{
	uint8_t value = 0;
		
	value = LCD->LCD_REG;
	
	return value;	
}

/**
 * @name   NT35310_WriteData16
 * @brief  写NT35310数据总线，16bit，用于RGB显存写入
 * @param  无
 * @retval 无
 */
void NT35310_WriteData16(uint16_t _usRGB)
{
	LCD->LCD_RAM = _usRGB;	/* 设置寄存器地址 */
}

/**
 * @name   NT35310_ReadData16
 * @brief  读NT35310显存，16bit RGB
 * @param  无
 * @retval 无
 */
uint16_t NT35310_ReadData16(void)
{
	uint16_t value;
	
	value = LCD->LCD_RAM;		/* 读取寄存器值 */
	
	return value;	
}



/**
 * @name   NT35310_GetDispMemAddr
 * @brief  获得显存地址。
 * @param  无
 * @retval 显存地址
 */
uint32_t NT35310_GetDispMemAddr(void)
{
	return LCD_BASE;
}
