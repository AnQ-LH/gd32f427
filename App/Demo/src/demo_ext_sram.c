#include "demo_ext_sram.h"
#include "bsp_exmc_sram.h"
#include "bsp.h"

/* �������ļ��ڵ��õĺ������� */
static void DispMenu(void);
static void ReadTest(void);
static void WriteTest(uint8_t _Value);
static void KeyMenu(void);

#define TEST_BUF_SIZE	1024
#define TEST_ADDRESS	0

uint8_t g_TestBuf[TEST_BUF_SIZE];	/* ���ݲ����û����� */

/**
 * @name   DemoExtSRAM
 * @brief  SRAM��д����
 * @param  ��
 * @retval ��
 */
void DemoExtSRAM(void)
{
	uint8_t cmd;
	uint32_t err;
	
	bsp_StartAutoTimer(0, 500);	/* ����1��100ms���Զ���װ�Ķ�ʱ�� */

	DispMenu();		/* ��ӡ������ʾ */
	while(1)
	{
		bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */
		/* �ж϶�ʱ����ʱʱ�� */
		if (bsp_CheckTimer(0))	
		{
			/* ÿ��50ms ����һ�� */  
			bsp_LedToggle(LED1);
		}

		if (comGetChar(COM0, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
		{			
			switch (cmd)
			{
				case '1':
					printf("\r\n 1 --- ����SRAM (%d �ֽ�)\r\n", BANK_SRAM_SIZE);
					err = bsp_TestExtSRAM();
					if (err == 0)
					{
						printf("�ⲿSRAM����ͨ��\r\n");
					}
					else
					{
						printf("�ⲿSRAM��������Ԫ������%d\r\n", err);
					}
					break;

				case '2':
					printf("\r\n 2 --- дSRAM (%dM �ֽ�,0x55)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
					WriteTest(0x55);	/* дSRAM���� 0x55 */
					break;

				case '3':
					printf("\r\n 3 --- дSRAM (%dM �ֽ�,0xAA)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
					WriteTest(0xAA);	/* дSRAM���� 0xAA */
					break;

				case '4':
					printf("\r\n 4 --- ��SRAM����\r\n");
					ReadTest();	/* ��SRAM���� */
					break;

				default:
					DispMenu();	/* ��Ч������´�ӡ������ʾ */
					break;
			}
		}


	}
}


/**
 * @name   WriteTest
 * @brief  дSRAM
 * @param  _Value : д�������
 * @retval ��
 */
static void WriteTest(uint8_t _Value)
{
	uint32_t i, n;
	int32_t iTime1, iTime2;
	uint16_t *pBuf;
	uint16_t usTemp;

	pBuf = (uint16_t *)BANK_SRAM_ADDR;

	usTemp = (_Value << 8) + _Value;
	
	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	pBuf = (uint16_t *)BANK_SRAM_ADDR;
	for (n = 0; n < 10; n++)
	{
		for (i = 0; i < BANK_SRAM_SIZE / 2; i++)
		{
			*pBuf++ = usTemp;
		}
	}
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */

	/* ��ӡ�ٶ� */
	printf("20MB����д��ʱ: %dms, д�ٶ�: %dKB/s\r\n", iTime2 - iTime1, (10 * (BANK_SRAM_SIZE / 1024)  * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ReadTest
 * @brief  �����ݣ�����ӡ����
 * @param  ��
 * @retval ��
 */
static void ReadTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;
	uint8_t *pBuf;

	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	pBuf = (uint8_t *)(BANK_SRAM_ADDR + TEST_ADDRESS);

	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		g_TestBuf[i] = *pBuf++;
	}
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */

	printf("�����ַ��0x%08X  ��С: %d�ֽ�,  ��������: \r\n", BANK_SRAM_ADDR + TEST_ADDRESS, TEST_BUF_SIZE);

	/* ��ӡ���� */
	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		printf(" %02X", g_TestBuf[i]);

		if ((i & 31) == 31)
		{
			printf("\r\n");	/* ÿ����ʾ16�ֽ����� */
		}
		else if ((i & 31) == 15)
		{
			printf(" - ");
		}
	}

	/* ��ӡ���ٶ� */
	printf("����ʱ: %dms, ���ٶ�: %dB/s\r\n", iTime2 - iTime1, (TEST_BUF_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   DispMenu
 * @brief  ��ʾ������ʾ�˵�
 * @param  ��
 * @retval ��
 */
static void DispMenu(void)
{
	printf("\r\n�ͺ�: IS62WV51216BLL-55TL  ����1M�ֽڣ�16Bit��55ns�ٶ�\r\n");
	printf("------------------------------------------------------\r\n");
	printf("��ѡ���������:\r\n");
	printf("1 - ����SRAM (%d �ֽ�)\r\n", BANK_SRAM_SIZE);
	printf("2 - дSRAM (%dM �ֽ�,0x55)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
	printf("3 - дSRAM (%dM �ֽ�,0xAA)\r\n", BANK_SRAM_SIZE /(1024 * 1024));
	printf("4 - ��SRAM����\r\n");
	printf("5 - ��ʾ������ʾ\r\n");
	printf("\r\n\r\n\r\n\r\n");
}

static void KeyMenu(void)
{
	/* �����˲��ͼ���ɺ�̨systick�жϷ������ʵ�֣�����ֻ��Ҫ����bsp_GetKey��ȡ��ֵ���ɡ� */		
	switch (bsp_GetKey())	/* bsp_GetKey()��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
	{
		case KEY_DOWN_K1:			/* K1������ */
			bsp_LedToggle(LED2);
			break;

		case KEY_UP_K1:				/* K1������ */
			break;

		case KEY_DOWN_K2:			/* K2������ */
			break;

		case KEY_UP_K2:				/* K2������ */
			break;

		case KEY_NONE:				/* �޼����� */
		default:
			/* �����ļ�ֵ������ */
			break;
	}
}
