#include "bsp.h"
#include "demo_i2c_eeprom.h"

/* 仅允许本文件内调用的函数声明 */
static void ee_DispMenu(void);
static void ee_ReadTest(void);
static void ee_WriteTest(void);
static void ee_Erase(void);

uint8_t buf[EE_SIZE];	/* 数据测试用缓冲区 16KB */

/**
 * @name   DemoEEPROM
 * @brief  串行EEPROM读写例程
 * @param  无
 * @retval 无
 */
void DemoEEPROM(void)
{
	uint8_t cmd;

	if (ee_CheckOk() == 0)
	{
		/* 没有检测到EEPROM */
		printf("没有检测到串行EEPROM!\r\n");

		while (1);	/* 停机 */
	}

	printf("已经检测到串行EEPROM : \r\n");
	printf("型号: %s, 容量 = %d 字节, 页面大小 = %d\r\n", EE_MODEL_NAME, EE_SIZE, EE_PAGE_SIZE);

	ee_DispMenu();		/* 打印命令提示 */
	while(1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */
		
		//cmd = getchar();	/* 从串口读入一个字符 (阻塞方式) */
		if (comGetChar(COM0, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{
			switch (cmd)
			{
				case '1':
					printf("\r\n 1 --- 读 EEPROM 测试\r\n");
					ee_ReadTest();		/* 读EEPROM数据，并打印出来数据内容 */
					break;

				case '2':
					printf("\r\n2 --- 写 EEPROM 测试\r\n");
					ee_WriteTest();		/* 写EEPROM数据，并打印写入速度 */
					break;

				case '3':
					printf("\r\n3 --- 擦除 EEPROM\r\n");
					ee_Erase();			/* 擦差EEPROM数据，实际上就是写入全0xFF */
					break;

				default:
					ee_DispMenu();	/* 无效命令，重新打印命令提示 */
					break;

			}
		}
	}
}

/**
 * @name   ee_ReadTest
 * @brief  读串行EEPROM全部数据，并打印出来
 * @param  无
 * @retval 无
 */
static void ee_ReadTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;

	/* 读EEPROM, 起始地址 = 0， 数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	if (ee_ReadBytes((uint8_t *)buf, 0, EE_SIZE) == 0)
	{
		printf("读eeprom出错！\r\n");
		return;
	}
	else
	{
		iTime2 = bsp_GetRunTime();	/* 记下结束时间 */
		printf("读eeprom成功，数据如下：\r\n");
	}

	/* 打印数据 */
	for (i = 0; i < EE_SIZE; i++)
	{
		printf(" %02X", buf[i]);

		if ((i & 31) == 31)
		{
			printf("\r\n");	/* 每行显示16字节数据 */
		}
		else if ((i & 31) == 15)
		{
			printf(" - ");
		}
	}

	/* 打印读速度 */
	printf("读耗时: %dms, 读速度: %dB/s\r\n", iTime2 - iTime1, (EE_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ee_ReadTest
 * @brief  写串行EEPROM全部数据
 * @param  无
 * @retval 无
 */
static void ee_WriteTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;

	/* 填充测试缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = i;
	}

	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	if (ee_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		printf("写eeprom出错！\r\n");
		return;
	}
	else
	{
		iTime2 = bsp_GetRunTime();	/* 记下结束时间 */
		printf("写eeprom成功！\r\n");
	}


	/* 打印读速度 */
	printf("写耗时: %dms, 写速度: %dB/s\r\n", iTime2 - iTime1, (EE_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ee_Erase
 * @brief  擦除EEPROM
 * @param  无
 * @retval 无
 */
static void ee_Erase(void)
{
	uint16_t i;

	/* 填充缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}

	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	if (ee_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		printf("擦除eeprom出错！\r\n");
		return;
	}
	else
	{
		printf("擦除eeprom成功！\r\n");
	}
}

/**
 * @name   ee_DispMenu
 * @brief  显示操作提示菜单
 * @param  无
 * @retval 无
 */
static void ee_DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("请选择操作命令:\r\n");
	printf("1 - 读EEPROM (%d 字节)\r\n", EE_SIZE);
	printf("2 - 写EEPROM (%d 字节,0x00-0xFF)\r\n", EE_SIZE);
	printf("3 - 擦除EEPROM\r\n");
	printf("4 - 显示命令提示\r\n");
	printf("\r\n\r\n");
}
