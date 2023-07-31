#include "LCD_NT35310.h"
#include "bsp_nt35310_port.h"
#include "bsp.h"

static uint16_t s_WinHeight = 320;
static uint16_t s_WinWidth = 480;


/**
 * @name   NT35310_ReadID
 * @brief  读取LCD驱动芯片ID
 * @param  无
 * @retval 无
 */
uint16_t NT35310_ReadID(void)
{
	uint16_t value = 0;
	
	NT35310_WriteCmd(0xD4);
	value = NT35310_ReadData();	
	value = NT35310_ReadData();
	value = NT35310_ReadData();	
	value <<= 8;
	value |= NT35310_ReadData();
	printf(" LCD ID:%x\r\n", value);
	
	return value;
}

/**
 * @name   NT35310_WriteReg
 * @brief  写NT35310寄存器
 * @param  无
 * @retval 无
 */
void NT35310_WriteReg(uint16_t _ucRegAddr, uint16_t _ucRegValue)
{
	NT35310_WriteCmd(_ucRegAddr);
	NT35310_WriteData(_ucRegValue);
}

/**
 * @name   NT35310_ReadReg
 * @brief  读NT35310寄存器
 * @param  无
 * @retval 无
 */
uint16_t NT35310_ReadReg(uint16_t _ucRegAddr)
{
	uint16_t value;
	
	NT35310_WriteCmd(_ucRegAddr);
	value = NT35310_ReadData();
	
	return value;	
}

/**
 * @name   NT35310_InitHard
 * @brief  初始化NT35310。本函数不开启背光，主程序应该在初始化完毕后再调用 NT35310_SetBackLight()函数开背光
 * @param  无
 * @retval 无
 */
void NT35310_InitHard(void)
{
	uint8_t ucGPIX;
	
	NT35310_ConfigGPIO();	/* 配置GPIO FSMC等 */

	/* 屏幕初始化 */
	NT35310_WriteCmd(0xED);
	NT35310_WriteData(0x01);
	NT35310_WriteData(0xFE);

	NT35310_WriteCmd(0xEE);
	NT35310_WriteData(0xDE);
	NT35310_WriteData(0x21);

	NT35310_WriteCmd(0xF1);
	NT35310_WriteData(0x01);
	NT35310_WriteCmd(0xDF);
	NT35310_WriteData(0x10);

	//VCOMvoltage//
	NT35310_WriteCmd(0xC4);
	NT35310_WriteData(0x8F);	  //5f

	NT35310_WriteCmd(0xC6);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xE2);
	NT35310_WriteData(0xE2);
	NT35310_WriteData(0xE2);
	NT35310_WriteCmd(0xBF);
	NT35310_WriteData(0xAA);

	NT35310_WriteCmd(0xB0);
	NT35310_WriteData(0x0D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x0D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x11);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x19);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x21);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x5D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x5D);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB1);
	NT35310_WriteData(0x80);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x8B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x96);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x02);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x03);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB4);
	NT35310_WriteData(0x8B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x96);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA1);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB5);
	NT35310_WriteData(0x02);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x03);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x04);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB6);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3F);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x5E);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x64);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x8C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xAC);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDC);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x70);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x90);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xEB);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDC);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xB8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xBA);
	NT35310_WriteData(0x24);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC1);
	NT35310_WriteData(0x20);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x54);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xFF);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC2);
	NT35310_WriteData(0x0A);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x04);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC3);
	NT35310_WriteData(0x3C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3A);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x39);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x37);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x36);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x32);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2F);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x29);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x26);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x24);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x24);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x23);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x36);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x32);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2F);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x29);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x26);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x24);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x24);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x23);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC4);
	NT35310_WriteData(0x62);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x05);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x84);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF0);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x18);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA4);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x18);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x50);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x0C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x17);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x95);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xE6);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC5);
	NT35310_WriteData(0x32);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x65);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x76);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC6);
	NT35310_WriteData(0x20);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x17);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x01);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xC9);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE0);
	NT35310_WriteData(0x16);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x1C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x21);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x36);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x46);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x52);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x64);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x7A);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x8B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB9);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC4);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xCA);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD9);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xE0);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE1);
	NT35310_WriteData(0x16);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x1C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x22);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x36);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x45);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x52);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x64);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x7A);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x8B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB9);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC4);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xCA);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xE0);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE2);
	NT35310_WriteData(0x05);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x0B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x1B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x34);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x4F);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x61);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x79);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x97);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA6);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD1);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD6);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDD);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);
	NT35310_WriteCmd(0xE3);
	NT35310_WriteData(0x05);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x1C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x33);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x50);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x62);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x78);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x97);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA6);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC7);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD1);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD5);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDD);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE4);
	NT35310_WriteData(0x01);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x01);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x02);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x2A);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x4B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x5D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x74);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x84);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x93);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xBE);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC4);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xCD);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDD);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);
	NT35310_WriteCmd(0xE5);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x02);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x29);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x3C);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x4B);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x5D);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x74);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x84);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x93);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xA2);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xB3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xBE);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xC4);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xCD);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xD3);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xDC);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xF3);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE6);
	NT35310_WriteData(0x11);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x34);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x56);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x76);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x77);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x66);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xBB);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x66);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x45);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x43);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE7);
	NT35310_WriteData(0x32);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x76);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x66);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x67);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x67);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x87);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xBB);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x77);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x56);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x23); 
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x33);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x45);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE8);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x87);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x77);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x66);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x88);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xAA);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0xBB);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x99);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x66);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x44);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x55);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xE9);
	NT35310_WriteData(0xAA);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0x00);
	NT35310_WriteData(0xAA);

	NT35310_WriteCmd(0xCF);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xF0);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x50);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xF3);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0xF9);
	NT35310_WriteData(0x06);
	NT35310_WriteData(0x10);
	NT35310_WriteData(0x29);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0x3A);
	NT35310_WriteData(0x55);	//66

	NT35310_WriteCmd(0x11);
	bsp_DelayMS(100);
	NT35310_WriteCmd(0x29);
	NT35310_WriteCmd(0x35);
	NT35310_WriteData(0x00);

	NT35310_WriteCmd(0x51);
	NT35310_WriteData(0xFF);
	NT35310_WriteCmd(0x53);
	NT35310_WriteData(0x2C);
	NT35310_WriteCmd(0x55);
	NT35310_WriteData(0x82);
	NT35310_WriteCmd(0x2c);

}

/**
 * @name   NT35310_SetDirection
 * @brief  设置显示方向。
 * @param  _ucDir : 显示方向代码 0 竖屏正常, 1 = 横屏
 * @retval 无
 */
void NT35310_SetDirection(uint16_t _ucDir)
{
	uint16_t s_reg;
	
	/* 0 竖屏 正向， 3 竖屏 翻转180度 */
	if(_ucDir == 0 || _ucDir == 3)
	{
		g_LcdHeight = 480;
		g_LcdWidth = 320;
	}
	/* 5 横屏 正向， 6 横屏 翻转180度 */
	if(_ucDir == 6 || _ucDir == 7)
	{
		g_LcdHeight = 320;
		g_LcdWidth = 480;
	}
	switch (_ucDir)
	{
		/* 从左到右,从上到下 */
		case L2R_U2D:
			s_reg |= (0 << 7) | (0 << 6) | (0 << 5);
			break;
		/* 从左到右,从下到上 */
		case L2R_D2U:
			s_reg |= (1 << 7) | (0 << 6) | (0 << 5);
			break;
		/* 从右到左,从上到下 */
		case R2L_U2D:
			s_reg |= (0 << 7) | (1 << 6) | (0 << 5);
			break;
		/* 从右到左,从下到上 */
		case R2L_D2U:
			s_reg |= (1 << 7) | (1 << 6) | (0 << 5);
			break;
		/* 从上到下,从左到右 */
		case U2D_L2R:
			s_reg |= (0 << 7) | (0 << 6) | (1 << 5);
			break;
		/* 从上到下,从右到左 */
		case U2D_R2L:
			s_reg |= (0 << 7) | (1 << 6) | (1 << 5);
			break;
		/* 从下到上,从左到右 */
		case D2U_L2R:
			s_reg |= (1 << 7) | (0 << 6) | (1 << 5);
			break;
		/* 从下到上,从右到左 */
		case D2U_R2L:
			s_reg |= (1 << 7) | (1 << 6) | (1 << 5);
			break;
	}
	
	NT35310_WriteReg(0X36, s_reg);
	
	uint16_t temp;
	
	if (s_reg & 0X20)
	{
		if (g_LcdWidth < g_LcdHeight)
		{
			temp = g_LcdWidth;
			g_LcdWidth = g_LcdHeight;
			g_LcdHeight = temp;
		}
	}
	else
	{
		if (g_LcdWidth > g_LcdHeight)
		{
			temp = g_LcdWidth;
			g_LcdWidth = g_LcdHeight;
			g_LcdHeight = temp;
		}
	}
	
	/* 设置显示区域(开窗)大小 */
	NT35310_WriteCmd(0X2A);
	NT35310_WriteData(0);
	NT35310_WriteData(0);
	NT35310_WriteData((g_LcdWidth - 1) >> 8);
	NT35310_WriteData((g_LcdWidth - 1) & 0XFF);
	NT35310_WriteCmd(0X2B);
	NT35310_WriteData(0);
	NT35310_WriteData(0);
	NT35310_WriteData((g_LcdHeight - 1) >> 8);
	NT35310_WriteData((g_LcdHeight - 1) & 0XFF);
}

/**
 * @name   NT35310_DispOn
 * @brief  打开显示
 * @param  无
 * @retval 无
 */
void NT35310_DispOn(void)
{
	NT35310_WriteCmd(0X29);
}

/**
 * @name   RA8875_DispOff
 * @brief  关闭显示
 * @param  无
 * @retval 无
 */
void NT35310_DispOff(void)
{
	NT35310_WriteCmd(0X28);
}

/**
 * @name   NT35310_PutPixel
 * @brief  设置光标位置
 * @param  _usX,_usY : 坐标
 * @retval 无
 */
void NT35310_SetCursor(uint16_t _usX, uint16_t _usY)
{	
	/* 设置X坐标 */
	NT35310_WriteCmd(0X2A);
	NT35310_WriteData(_usX >> 8);
	NT35310_WriteData(_usX & 0XFF);
	
	/* 设置Y坐标 */
	NT35310_WriteCmd(0X2B);
	NT35310_WriteData(_usY >> 8);
	NT35310_WriteData(_usY & 0XFF);
}

/**
 * @name   NT35310_PutPixel
 * @brief  画1个像素
 * @param  _usX,_usY : 像素坐标
 *		   _usColor  :像素颜色
 * @retval 无
 */
void NT35310_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{	
	/* 设置光标位置 */
	NT35310_SetCursor(_usX, _usY);
	
	/* 开始写入GRAM */
	NT35310_WriteReg(0X2C, _usColor);
}

/**
 * @name   NT35310_GetPixel
 * @brief  读取1个像素
 * @param  _usX,_usY : 像素坐标
 * @retval RGB颜色值
 */
uint16_t NT35310_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usR, usG, usB;
	
	NT35310_WriteCmd(0X2E);
	
	/* 假读 */
	usR = NT35310_ReadData();
	
	usR = NT35310_ReadData();
	usB = NT35310_ReadData();
	/* 第一次读取的是RG的值,R在前,G在后,各占8位 */
	usG = usR & 0XFF;
	
	return (((usR >> 11) << 11) | ((usG >> 10) << 5) | (usB >> 11));;
}

/**
 * @name   NT35310_ClrScr
 * @brief  根据输入的颜色值清屏
 * @param  _usColor : 背景色
 * @retval 无
 */
void NT35310_ClrScr(uint16_t _usColor)
{
	uint32_t index = 0;
    uint32_t totalpoint = g_LcdWidth;
    totalpoint *= g_LcdHeight; 
	
	/* 设置光标位置 */
    NT35310_SetCursor(0x00, 0x00);
	NT35310_WriteCmd(0X2C);

    for (index = 0; index < totalpoint; index++)
    {
		NT35310_WriteData(_usColor);
    }
}

/**
 * @name   NT35310_DrawLine
 * @brief  在2点间画一条直线。
 * @param  _usX1, _usY1 :起始点坐标
 *		   _usX2, _usY2 :终止点Y坐标
 *		   _usColor     :颜色
 * @retval 无
 */
void NT35310_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	uint16_t i;
    int _usErrX = 0, _usErrY = 0, _usDeltaX, _usDeltaY, distance;
    int incx, incy, _usRow, _usCol;
	
    _usDeltaX = _usX2 - _usX1;
    _usDeltaY = _usY2 - _usY1;
    _usRow = _usX1;
    _usCol = _usY1;

    if (_usDeltaX > 0)			incx = 1;
	/* 垂直线 */
    else if (_usDeltaX == 0)	incx = 0; 
    else
    {
        incx = -1;
        _usDeltaX = -_usDeltaX;
    }

    if (_usDeltaY > 0)			incy = 1;
	/* 水平线 */
    else if (_usDeltaY == 0)	incy = 0;
    else
    {
        incy = -1;
        _usDeltaY = -_usDeltaY;
    }
	
	/* 选取基本增量坐标轴 */
    if ( _usDeltaX > _usDeltaY)	distance = _usDeltaX; 
    else distance = _usDeltaY;

    for (i = 0; i <= distance + 1; i++ )
    {
        NT35310_PutPixel(_usRow, _usCol, _usColor);
		
        _usErrX += _usDeltaX ;
        _usErrY += _usDeltaY ;

        if (_usErrX > distance)
        {
            _usErrX -= distance;
            _usRow += incx;
        }

        if (_usErrY > distance)
        {
            _usErrY -= distance;
            _usCol += incy;
        }
    }
}

/**
 * @name   NT35310_DrawRect
 * @brief  绘制矩形
 * @param  _usX1, _usY1 :起始点坐标
 *		   _usX2, _usY2 :终止点Y坐标
 *		   _usColor     :颜色
 * @retval 无
 */
void NT35310_DrawRect(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
	
	NT35310_DrawLine(_usX1, _usY1, _usX2, _usY1, _usColor);
	NT35310_DrawLine(_usX1, _usY1, _usX1, _usY2, _usColor);
	NT35310_DrawLine(_usX1, _usY2, _usX2, _usX2, _usColor);
	NT35310_DrawLine(_usX2, _usY1, _usX2, _usY2, _usColor);
}

/**
 * @name   NT35310_FillRect
 * @brief  填充一个矩形为单色
 * @param  _usX,_usY	:矩形左上角的坐标
 * @param  _ueX,_ueY	:矩形右下角的坐标
 * @param  _usColor		:颜色
 * @retval 无
 */
void NT35310_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _ueX, uint16_t _ueY, uint16_t _usColor)
{
	uint16_t i, j;
    uint16_t xlen = 0;

    xlen = _ueY - _usY + 1;

    for (i = _usY; i <= _ueY; i++)
    {
		NT35310_SetCursor(_usX, i);
		
		/* 开始写入GRAM */
        NT35310_WriteCmd(0X2C);

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = _usColor;     //设置光标位置
        }
    }
}

/**
 * @name   NT35310_DrawCircle
 * @brief  绘制一个圆，笔宽为1个像素
 * @param  _usX,_usY	:矩形左上角的坐标
 * @param  _usRadius  :圆的半径
 * @param  _usColor		:颜色
 * @retval 无
 */
void NT35310_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int a, b;
    int di;
    a = 0;
    b = _usRadius;
    di = 3 - (_usRadius << 1);       //判断下个点位置的标志

    while (a <= b)
    {
        NT35310_PutPixel(_usX + a, _usY - b, _usColor);
        NT35310_PutPixel(_usX + b, _usY - a, _usColor);
        NT35310_PutPixel(_usX + b, _usY + a, _usColor);
        NT35310_PutPixel(_usX + a, _usY + b, _usColor);
        NT35310_PutPixel(_usX - a, _usY + b, _usColor);
        NT35310_PutPixel(_usX - b, _usY + a, _usColor);
        NT35310_PutPixel(_usX - a, _usY - b, _usColor);
        NT35310_PutPixel(_usX - b, _usY - a, _usColor);
        a++;

        /* 使用Bresenham算法画圆 */
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

