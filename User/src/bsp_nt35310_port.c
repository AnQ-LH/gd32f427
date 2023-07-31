#include "bsp_nt35310_port.h"
#include "bsp.h"

/**
 * @name   NT35310_ConfigGPIO
 * @brief  ����CPU���ʽӿڣ���FSMC. SPI��
 * @param  ��
 * @retval ��
 */
void NT35310_ConfigGPIO(void)
{
	static uint8_t s_run_first = 0;
	
	/* ����Ѿ����й�������ִ�� */
	if (s_run_first == 1)
	{
		return;
	}
	
	s_run_first = 1;
	
	
	/* FSMC�� bsp_tft_lcd.c���Ѿ����ú� */
}

/**
 * @name   NT35310_Delaly1us
 * @brief  ��Ҫ����NT35310 PLL����֮ǰ����ָ�����ӳ�
 * @param  ��
 * @retval ��
 */
void NT35310_Delaly1us(void)
{
	bsp_DelayUS(1);
}

/**
 * @name   NT35310_Delaly1ms
 * @brief  ��Ҫ����NT35310 PLL����֮ǰ����ָ�����ӳ�
 * @param  ��
 * @retval ��
 */
void NT35310_Delaly1ms(void)
{
	bsp_DelayUS(1000);
}

/**
 * @name   NT35310_WriteCmd
 * @brief  дNT35310ָ��Ĵ���
 * @param  ��
 * @retval ��
 */
void NT35310_WriteCmd(__IO uint16_t _ucRegAddr)
{
	_ucRegAddr = _ucRegAddr;
	LCD->LCD_REG = _ucRegAddr;	/* ���üĴ�����ַ */
}

/**
 * @name   NT35310_WriteData
 * @brief  дNT35310���ݼĴ���
 * @param  ��
 * @retval ��
 */
void NT35310_WriteData(__IO uint16_t _ucRegValue)
{
	_ucRegValue = _ucRegValue;
	LCD->LCD_RAM = _ucRegValue;	/* дLCD���� */
}

/**
 * @name   NT35310_ReadData
 * @brief  ��NT353105�Ĵ���ֵ
 * @param  ��
 * @retval �Ĵ���ֵ
 */
uint16_t NT35310_ReadData(void)
{
	__IO uint16_t value;
	
	value = LCD->LCD_RAM;		/* ��ȡ�Ĵ���ֵ */
	
	return value;	
}

/**
 * @name   NT35310_ReadStatus
 * @brief  ��NT35310״̬�Ĵ���
 * @param  ��
 * @retval ��
 */
uint8_t NT35310_ReadStatus(void)
{
	uint8_t value = 0;
		
	value = LCD->LCD_REG;
	
	return value;	
}

/**
 * @name   NT35310_WriteData16
 * @brief  дNT35310�������ߣ�16bit������RGB�Դ�д��
 * @param  ��
 * @retval ��
 */
void NT35310_WriteData16(uint16_t _usRGB)
{
	LCD->LCD_RAM = _usRGB;	/* ���üĴ�����ַ */
}

/**
 * @name   NT35310_ReadData16
 * @brief  ��NT35310�Դ棬16bit RGB
 * @param  ��
 * @retval ��
 */
uint16_t NT35310_ReadData16(void)
{
	uint16_t value;
	
	value = LCD->LCD_RAM;		/* ��ȡ�Ĵ���ֵ */
	
	return value;	
}



/**
 * @name   NT35310_GetDispMemAddr
 * @brief  ����Դ��ַ��
 * @param  ��
 * @retval �Դ��ַ
 */
uint32_t NT35310_GetDispMemAddr(void)
{
	return LCD_BASE;
}
