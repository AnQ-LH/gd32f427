#include "demo_ext_sram.h"
#include "bsp_exmc_sram.h"
#include "bsp.h"

/* 仅允许本文件内调用的函数声明 */
static void DispMenu(void);
static void ReadTest(void);
static void WriteTest(uint8_t _Value);
static void KeyMenu(void);

#define TEST_BUF_SIZE	1024
#define TEST_ADDRESS	0

uint8_t g_TestBuf[TEST_BUF_SIZE];	/* 数据测试用缓冲区 */

/**
 * @name   DemoExtSRAM
 * @brief  SRAM读写例程
 * @param  无
 * @retval 无
 */
void DemoExtSRAM(void)
{
	uint8_t cmd;
	uint32_t err;
	
	bsp_StartAutoTimer(0, 500);	/* 启动1个100ms的自动重装的定时器 */

	DispMenu();		/* 打印命令提示 */
	while(1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */
		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			/* 每隔50ms 进来一次 */  
			bsp_LedToggle(LED1);
		}

		if (comGetChar(COM0, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{			
			switch (cmd)
			{
				case '1':
					printf("\r\n 1 --- 测试SRAM (%d 字节)\r\n", BANK_SRAM_SIZE);
					err = bsp_TestExtSRAM();
					if (err == 0)
					{
						printf("外部SRAM测试通过\r\n");
					}
					else
					{
						printf("外部SRAM出错，错误单元个数：%d\r\n", err);
					}
					break;

				case '2':
					printf("\r\n 2 --- 写SRAM (%dM 字节,0x55)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
					WriteTest(0x55);	/* 写SRAM测试 0x55 */
					break;

				case '3':
					printf("\r\n 3 --- 写SRAM (%dM 字节,0xAA)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
					WriteTest(0xAA);	/* 写SRAM测试 0xAA */
					break;

				case '4':
					printf("\r\n 4 --- 读SRAM数据\r\n");
					ReadTest();	/* 读SRAM测试 */
					break;

				default:
					DispMenu();	/* 无效命令，重新打印命令提示 */
					break;
			}
		}


	}
}


/**
 * @name   WriteTest
 * @brief  写SRAM
 * @param  _Value : 写入的数据
 * @retval 无
 */
static void WriteTest(uint8_t _Value)
{
	uint32_t i, n;
	int32_t iTime1, iTime2;
	uint16_t *pBuf;
	uint16_t usTemp;

	pBuf = (uint16_t *)BANK_SRAM_ADDR;

	usTemp = (_Value << 8) + _Value;
	
	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	pBuf = (uint16_t *)BANK_SRAM_ADDR;
	for (n = 0; n < 10; n++)
	{
		for (i = 0; i < BANK_SRAM_SIZE / 2; i++)
		{
			*pBuf++ = usTemp;
		}
	}
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */

	/* 打印速度 */
	printf("20MB数据写耗时: %dms, 写速度: %dKB/s\r\n", iTime2 - iTime1, (10 * (BANK_SRAM_SIZE / 1024)  * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ReadTest
 * @brief  读数据，并打印出来
 * @param  无
 * @retval 无
 */
static void ReadTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;
	uint8_t *pBuf;

	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	pBuf = (uint8_t *)(BANK_SRAM_ADDR + TEST_ADDRESS);

	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		g_TestBuf[i] = *pBuf++;
	}
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */

	printf("物理地址：0x%08X  大小: %d字节,  数据如下: \r\n", BANK_SRAM_ADDR + TEST_ADDRESS, TEST_BUF_SIZE);

	/* 打印数据 */
	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		printf(" %02X", g_TestBuf[i]);

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
	printf("读耗时: %dms, 读速度: %dB/s\r\n", iTime2 - iTime1, (TEST_BUF_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   DispMenu
 * @brief  显示操作提示菜单
 * @param  无
 * @retval 无
 */
static void DispMenu(void)
{
	printf("\r\n型号: IS62WV51216BLL-55TL  容量1M字节，16Bit，55ns速度\r\n");
	printf("------------------------------------------------------\r\n");
	printf("请选择操作命令:\r\n");
	printf("1 - 测试SRAM (%d 字节)\r\n", BANK_SRAM_SIZE);
	printf("2 - 写SRAM (%dM 字节,0x55)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
	printf("3 - 写SRAM (%dM 字节,0xAA)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
	printf("4 - 读SRAM数据\r\n");
	printf("5 - 显示命令提示\r\n");
	printf("\r\n\r\n\r\n\r\n");
}

static void KeyMenu(void)
{
	/* 按键滤波和检测由后台systick中断服务程序实现，我们只需要调用bsp_GetKey读取键值即可。 */		
	switch (bsp_GetKey())	/* bsp_GetKey()读取键值, 无键按下时返回 KEY_NONE = 0 */
	{
		case KEY_DOWN_K1:			/* K1键按下 */
			bsp_LedToggle(LED2);
			break;

		case KEY_UP_K1:				/* K1键弹起 */
			break;

		case KEY_DOWN_K2:			/* K2键按下 */
			break;

		case KEY_UP_K2:				/* K2键弹起 */
			break;

		case KEY_NONE:				/* 无键按下 */
		default:
			/* 其它的键值不处理 */
			break;
	}
}
