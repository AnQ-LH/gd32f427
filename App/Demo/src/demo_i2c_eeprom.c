#include "bsp.h"
#include "demo_i2c_eeprom.h"

/* �������ļ��ڵ��õĺ������� */
static void ee_DispMenu(void);
static void ee_ReadTest(void);
static void ee_WriteTest(void);
static void ee_Erase(void);

uint8_t buf[EE_SIZE];	/* ���ݲ����û����� 16KB */

/**
 * @name   DemoEEPROM
 * @brief  ����EEPROM��д����
 * @param  ��
 * @retval ��
 */
void DemoEEPROM(void)
{
	uint8_t cmd;

	if (ee_CheckOk() == 0)
	{
		/* û�м�⵽EEPROM */
		printf("û�м�⵽����EEPROM!\r\n");

		while (1);	/* ͣ�� */
	}

	printf("�Ѿ���⵽����EEPROM : \r\n");
	printf("�ͺ�: %s, ���� = %d �ֽ�, ҳ���С = %d\r\n", EE_MODEL_NAME, EE_SIZE, EE_PAGE_SIZE);

	ee_DispMenu();		/* ��ӡ������ʾ */
	while(1)
	{
		bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */
		
		//cmd = getchar();	/* �Ӵ��ڶ���һ���ַ� (������ʽ) */
		if (comGetChar(COM0, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
		{
			switch (cmd)
			{
				case '1':
					printf("\r\n 1 --- �� EEPROM ����\r\n");
					ee_ReadTest();		/* ��EEPROM���ݣ�����ӡ������������ */
					break;

				case '2':
					printf("\r\n2 --- д EEPROM ����\r\n");
					ee_WriteTest();		/* дEEPROM���ݣ�����ӡд���ٶ� */
					break;

				case '3':
					printf("\r\n3 --- ���� EEPROM\r\n");
					ee_Erase();			/* ����EEPROM���ݣ�ʵ���Ͼ���д��ȫ0xFF */
					break;

				default:
					ee_DispMenu();	/* ��Ч������´�ӡ������ʾ */
					break;

			}
		}
	}
}

/**
 * @name   ee_ReadTest
 * @brief  ������EEPROMȫ�����ݣ�����ӡ����
 * @param  ��
 * @retval ��
 */
static void ee_ReadTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;

	/* ��EEPROM, ��ʼ��ַ = 0�� ���ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	if (ee_ReadBytes((uint8_t *)buf, 0, EE_SIZE) == 0)
	{
		printf("��eeprom����\r\n");
		return;
	}
	else
	{
		iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */
		printf("��eeprom�ɹ����������£�\r\n");
	}

	/* ��ӡ���� */
	for (i = 0; i < EE_SIZE; i++)
	{
		printf(" %02X", buf[i]);

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
	printf("����ʱ: %dms, ���ٶ�: %dB/s\r\n", iTime2 - iTime1, (EE_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ee_ReadTest
 * @brief  д����EEPROMȫ������
 * @param  ��
 * @retval ��
 */
static void ee_WriteTest(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;

	/* �����Ի����� */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = i;
	}

	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	if (ee_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		printf("дeeprom����\r\n");
		return;
	}
	else
	{
		iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */
		printf("дeeprom�ɹ���\r\n");
	}


	/* ��ӡ���ٶ� */
	printf("д��ʱ: %dms, д�ٶ�: %dB/s\r\n", iTime2 - iTime1, (EE_SIZE * 1000) / (iTime2 - iTime1));
}

/**
 * @name   ee_Erase
 * @brief  ����EEPROM
 * @param  ��
 * @retval ��
 */
static void ee_Erase(void)
{
	uint16_t i;

	/* ��仺���� */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}

	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	if (ee_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		printf("����eeprom����\r\n");
		return;
	}
	else
	{
		printf("����eeprom�ɹ���\r\n");
	}
}

/**
 * @name   ee_DispMenu
 * @brief  ��ʾ������ʾ�˵�
 * @param  ��
 * @retval ��
 */
static void ee_DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("��ѡ���������:\r\n");
	printf("1 - ��EEPROM (%d �ֽ�)\r\n", EE_SIZE);
	printf("2 - дEEPROM (%d �ֽ�,0x00-0xFF)\r\n", EE_SIZE);
	printf("3 - ����EEPROM\r\n");
	printf("4 - ��ʾ������ʾ\r\n");
	printf("\r\n\r\n");
}
