#include "bsp_spi_flash.h"
#include "bsp.h"

/*
	ֻ������SPI��ѯ��ʽ��������ʽ��Ҫ�޸Ĵ���
*/

/* ����Flash��ƬѡGPIO�˿ڣ� PF8  */
#define SF_CS_CLK_ENABLE() 			rcu_periph_clock_enable(RCU_GPIOB)
#define SF_CS_GPIO					GPIOB
#define SF_CS_PIN					GPIO_PIN_14

#define SF_CS_0()					GPIO_BOP(SF_CS_GPIO) = ((uint32_t)SF_CS_PIN << 16U) 
#define SF_CS_1()					GPIO_BOP(SF_CS_GPIO) = SF_CS_PIN
	
#define CMD_AAI       0xAD  	/* AAI �������ָ��(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_EWRSR	  0x50		/* ����д״̬�Ĵ��������� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_SE        0x20		/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define DUMMY_BYTE    0xA5		/* ���������Ϊ����ֵ�����ڶ����� */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */

SFLASH_T g_tSF;

#if 0
static void sf_WriteStatus(uint8_t _ucValue);
#endif

static void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

static uint8_t s_spiBuf[4*1024];	/* ����д�������ȶ��������������޸Ļ�������������������д */

/**
 * @name   bsp_InitSFlash
 * @brief  ����falshӲ����ʼ���� ����CS GPIO�� ��ȡID��
 * @param  ��
 * @retval ��
 */
void bsp_InitSFlash(void)
{
	/* ����CS GPIO */
	{
		/* ��GPIOʱ�� */
		SF_CS_CLK_ENABLE();
		
		gpio_mode_set(SF_CS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SF_CS_PIN);
		gpio_output_options_set(SF_CS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SF_CS_PIN);
		
	}
	
	/* ��ȡоƬID, �Զ�ʶ��оƬ�ͺ� */
	sf_ReadInfo();
}	

/**
 * @name   sf_SetCS
 * @brief  ����FALSHƬѡ���ƺ���
 * @param  ��
 * @retval ��
 */
void sf_SetCS(uint8_t _Level)
{
	if (_Level == 0)
	{
		bsp_SpiBusEnter();
		bsp_InitSPIParam(SPI_PSC_4, SPI_CK_PL_LOW_PH_1EDGE);
		SF_CS_0();
	}
	else
	{		
		SF_CS_1();	
		bsp_SpiBusExit();		
	}
}

/**
 * @name   sf_EraseSector
 * @brief  ����ָ��������
 * @param  _uiSectorAddr : ������ַ
 * @retval ��
 */
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_SE);							/* ���Ͳ������� */
	sf_SendByte((_uiSectorAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiSectorAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiSectorAddr & 0xFF);				/* ����������ַ��8bit */
	
	sf_SetCS(1);									/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}

/**
 * @name   sf_EraseChip
 * @brief  ��������оƬ
 * @param  ��
 * @retval ��
 */
void sf_EraseChip(void)
{	
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	sf_SetCS(0);		/* ʹ��Ƭѡ */
	sf_SendByte(CMD_BE);							/* ������Ƭ�������� */
	sf_SetCS(1);									/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}

/**
 * @name   sf_PageWrite
 * @brief  ҳ���
 * @param  _pBuf : ����Դ��������
 *		   _uiWriteAddr ��Ŀ�������׵�ַ��
 *		   _usSize �����ݸ�����ҳ��С����������256�ֽڵ���������
 * @retval ��
 */
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	if (g_tSF.ChipID == SST25VF016B_ID)
	{
		/* AAIָ��Ҫ��������ݸ�����ż�� */
		if ((_usSize < 2) && (_usSize % 2))
		{
			return ;
		}

		sf_WriteEnable();								/* ����дʹ������ */

		sf_SetCS(0);									/* ʹ��Ƭѡ */
		sf_SendByte(CMD_AAI);							/* ����AAI����(��ַ�Զ����ӱ��) */
		sf_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
		sf_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
		sf_SendByte(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */
		sf_SendByte(*_pBuf++);							/* ���͵�1������ */
		sf_SendByte(*_pBuf++);							/* ���͵�2������ */
		sf_SetCS(1);									/* ����Ƭѡ */

		sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */

		_usSize -= 2;									/* ����ʣ���ֽ��� */

		for (i = 0; i < _usSize / 2; i++)
		{
			sf_SetCS(0);	/* ʹ��Ƭѡ */	
			sf_SendByte(CMD_AAI);						/* ����AAI����(��ַ�Զ����ӱ��) */
			sf_SendByte(*_pBuf++);						/* �������� */
			sf_SendByte(*_pBuf++);						/* �������� */
			sf_SetCS(1);								/* ����Ƭѡ */
			sf_WaitForWriteEnd();						/* �ȴ�����Flash�ڲ�д������� */
		}

		/* ����д����״̬ */
		sf_SetCS(0);
		sf_SendByte(CMD_DISWR);
		sf_SetCS(1);

		sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
	}
	else	/* for MX25L1606E �� W25Q64BV */
	{
		for (j = 0; j < _usSize / 256; j++)
		{
			sf_WriteEnable();								/* ����дʹ������ */

			sf_SetCS(0);									/* ʹ��Ƭѡ */
			sf_SendByte(0x02);								/* ����AAI����(��ַ�Զ����ӱ��) */
			sf_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
			sf_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
			sf_SendByte(_uiWriteAddr & 0xFF);
			for (i = 0; i < 256; i++)
			{
				sf_SendByte(*_pBuf++);					/* �������� */
			}
			sf_SetCS(1);								/* ��ֹƬѡ */

			sf_WaitForWriteEnd();						/* �ȴ�����Flash�ڲ�д������� */

			_uiWriteAddr += 256;
		}

		/* ����д����״̬ */
		sf_SetCS(0);
		sf_SendByte(CMD_DISWR);
		sf_SetCS(1);

		sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
	}
}

/**
 * @name   sf_ReadBuffer
 * @brief  ������ȡ�����ֽڣ��ֽڸ������ܳ���оƬ������
 * @param  _pBuf : ����Դ��������
 *		   _uiReadAddr ���׵�ַ
 *		   _usSize �����ݸ���, ���ܳ���оƬ������
 * @retval ��
 */
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
	{
		return;
	}

	/* ������������ */
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_READ);							/* ���Ͷ����� */
	sf_SendByte((_uiReadAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiReadAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiReadAddr & 0xFF);				/* ����������ַ��8bit */
	while (_uiSize--)
	{
		*_pBuf++ = sf_SendByte(DUMMY_BYTE);			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	}
	sf_SetCS(1);									/* ����Ƭѡ */
}

/**
 * @name   sf_CmpData
 * @brief  �Ƚ�Flash�����ݡ�
 * @param  _ucpTar : ���ݻ�������
 *		   _uiSrcAddr ��Flash��ַ
 *		   _uiSize �����ݸ���, ���ܳ���оƬ������
 * @retval 0 = ���, 1 = ����
 */
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
	uint8_t ucValue;

	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	sf_SetCS(0);									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_READ);							/* ���Ͷ����� */
	sf_SendByte((_uiSrcAddr & 0xFF0000) >> 16);		/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiSrcAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiSrcAddr & 0xFF);					/* ����������ַ��8bit */
	while (_uiSize--)
	{
		/* ��һ���ֽ� */
		ucValue = sf_SendByte(DUMMY_BYTE);
		if (*_ucpTar++ != ucValue)
		{
			sf_SetCS(1);
			return 1;
		}
	}
	sf_SetCS(1);
	return 0;		/* ��� */
}

/**
 * @name   sf_NeedErase
 * @brief  �ж�дPAGEǰ�Ƿ���Ҫ�Ȳ�����
 * @param  _ucpOldBuf �� �����ݡ�
 *		   _ucpNewBuf �� ������
 *		   _uiLen �����ݸ���
 * @retval 0 : ����Ҫ������ 1 ����Ҫ����
 */
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	�㷨��1����old ��, new ����
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

	�㷨��2��: old �󷴵Ľ���� new λ��
		  0010   old
	&	  0101   new
		 =0000

	�㷨��3��: ���Ϊ0,���ʾ�������. �����ʾ��Ҫ����
	*/

	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;

		/* ע������д��: if (ucOld & (*_ucpNewBuf++) != 0) */
		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * @name   sf_AutoWriteSector
 * @brief  д1��������У��,�������ȷ������д���Σ��������Զ���ɲ�������
 * @param  _pBuf : ����Դ������
 *		   _uiWriteAddr ��Ŀ�������׵�ַ
 *		   _usSize �����ݸ��������ܳ���������С
 * @retval 0 : ���� 1 �� �ɹ�
 */
static uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;					/* ������ʱ */
	uint32_t uiFirstAddr;		/* ������ַ */
	uint8_t ucNeedErase;		/* 1��ʾ��Ҫ���� */
	uint8_t cRet;

	/* ����Ϊ0ʱ����������,ֱ����Ϊ�ɹ� */
	if (_usWrLen == 0)
	{
		return 1;
	}

	/* ���ƫ�Ƶ�ַ����оƬ�������˳� */
	if (_uiWrAddr >= g_tSF.TotalSize)
	{
		return 0;
	}

	/* ������ݳ��ȴ����������������˳� */
	if (_usWrLen > g_tSF.SectorSize)
	{
		return 0;
	}

	/* ���FLASH�е�����û�б仯,��дFLASH */
	sf_ReadBuffer(s_spiBuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spiBuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}

	/* �ж��Ƿ���Ҫ�Ȳ������� */
	/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
	ucNeedErase = 0;
	if (sf_NeedErase(s_spiBuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

	if (_usWrLen == g_tSF.SectorSize)		/* ������������д */
	{
		for	(i = 0; i < g_tSF.SectorSize; i++)
		{
			s_spiBuf[i] = _ucpSrc[i];
		}
	}
	else						/* ��д�������� */
	{
		/* �Ƚ��������������ݶ��� */
		sf_ReadBuffer(s_spiBuf, uiFirstAddr, g_tSF.SectorSize);

		/* ���������ݸ��� */
		i = _uiWrAddr & (g_tSF.SectorSize - 1);
		memcpy(&s_spiBuf[i], _ucpSrc, _usWrLen);
	}

	/* д��֮�����У�飬�������ȷ����д�����3�� */
	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
		if (ucNeedErase == 1)
		{
			sf_EraseSector(uiFirstAddr);		/* ����1������ */
		}

		/* ���һ������ */
		sf_PageWrite(s_spiBuf, uiFirstAddr, g_tSF.SectorSize);

		if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
		{
			cRet = 1;
			break;
		}
		else
		{
			if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
			{
				cRet = 1;
				break;
			}

			/* ʧ�ܺ��ӳ�һ��ʱ�������� */
			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}

/**
 * @name   sf_WriteBuffer
 * @brief  д1��������У��,�������ȷ������д���Σ��������Զ���ɲ�������
 * @param  _pBuf : ����Դ������
 *		   _uiWrAddr ��Ŀ�������׵�ַ
 *		   _usSize �����ݸ����������С�������ܳ���оƬ����
 * @retval 1 : �ɹ��� 0 �� ʧ��
 */
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
	uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tSF.SectorSize;
	count = g_tSF.SectorSize - Addr;
	NumOfPage =  _usWriteSize / g_tSF.SectorSize;
	NumOfSingle = _usWriteSize % g_tSF.SectorSize;

	if (Addr == 0) /* ��ʼ��ַ�������׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��������С */
		{
			if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else 	/* ���ݳ��ȴ��ڵ���������С */
		{
			while (NumOfPage--)
			{
				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, g_tSF.SectorSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tSF.SectorSize;
				_pBuf += g_tSF.SectorSize;
			}
			if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
			{
				return 0;
			}
		}
	}
	else  /* ��ʼ��ַ���������׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��������С */
		{
			if (NumOfSingle > count)  /* (_usWriteSize + _uiWriteAddr) > SPI_FLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, count) == 0)
				{
					return 0;
				}

				_uiWriteAddr +=  count;
				_pBuf += count;

				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, temp) == 0)
				{
					return 0;
				}
			}
			else
			{
				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
				{
					return 0;
				}
			}
		}
		else	/* ���ݳ��ȴ��ڵ���������С */
		{
			_usWriteSize -= count;
			NumOfPage =  _usWriteSize / g_tSF.SectorSize;
			NumOfSingle = _usWriteSize % g_tSF.SectorSize;
			if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, count) == 0)
			{
				return 0;
			}

			_uiWriteAddr +=  count;
			_pBuf += count;

			while (NumOfPage--)
			{
				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, g_tSF.SectorSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tSF.SectorSize;
				_pBuf += g_tSF.SectorSize;
			}

			if (NumOfSingle != 0)
			{
				if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
				{
					return 0;
				}
			}
		}
	}
	return 1;	/* �ɹ� */
}

/**
 * @name   sf_ReadID
 * @brief  ��ȡ����ID
 * @param  ��
 * @retval 32bit������ID (���8bit��0����ЧIDλ��Ϊ24bit��
 */
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_SetCS(0);							/* ʹ��Ƭѡ */
	sf_SendByte(CMD_RDID);					/* ���Ͷ�ID���� */
	id1 = sf_SendByte(DUMMY_BYTE);			/* ��ID�ĵ�1���ֽ� */
	id2 = sf_SendByte(DUMMY_BYTE);			/* ��ID�ĵ�2���ֽ� */
	id3 = sf_SendByte(DUMMY_BYTE);			/* ��ID�ĵ�3���ֽ� */
	sf_SetCS(1);							/* ����Ƭѡ */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/**
 * @name   sf_ReadInfo
 * @brief  ��ȡ����ID,�������������
 * @param  ��
 * @retval ��
 */
void sf_ReadInfo(void)
{
	/* �Զ�ʶ����Flash�ͺ� */
	{
		g_tSF.ChipID = sf_ReadID();	/* оƬID */

		switch (g_tSF.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tSF.ChipName, "SST25VF016B");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tSF.ChipName, "MX25L1606E");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tSF.ChipName, "W25Q64");
				g_tSF.TotalSize = 8 * 1024 * 1024;	/* ������ = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;
			
			case W25Q128_ID:
				strcpy(g_tSF.ChipName, "W25Q128");
				g_tSF.TotalSize = 16 * 1024 * 1024;	/* ������ = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;			

			default:
				strcpy(g_tSF.ChipName, "Unknow Flash");
				g_tSF.TotalSize = 2 * 1024 * 1024;
				g_tSF.SectorSize = 4 * 1024;
				break;
		}
	}
}

/**
 * @name   sf_WriteEnable
 * @brief  ����������дʹ������
 * @param  ��
 * @retval ��
 */
static void sf_WriteEnable(void)
{
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_WREN);							/* �������� */
	sf_SetCS(1);									/* ����Ƭѡ */
}

/**
 * @name   sf_WriteStatu
 * @brief  д״̬�Ĵ���
 * @param  _ucValue : ״̬�Ĵ�����ֵ
 * @retval ��
 */
static void sf_WriteStatus(uint8_t _ucValue)
{

	if (g_tSF.ChipID == SST25VF016B_ID)
	{
		/* ��1������ʹ��д״̬�Ĵ��� */
		sf_SetCS(0);									/* ʹ��Ƭѡ */
		sf_SendByte(CMD_EWRSR);							/* ������� ����д״̬�Ĵ��� */
		sf_SetCS(1);									/* ����Ƭѡ */

		/* ��2������д״̬�Ĵ��� */
		sf_SetCS(0);									/* ʹ��Ƭѡ */
		sf_SendByte(CMD_WRSR);							/* ������� д״̬�Ĵ��� */
		sf_SendByte(_ucValue);							/* �������ݣ�״̬�Ĵ�����ֵ */
		sf_SetCS(1);									/* ����Ƭѡ */
	}
	else
	{
		sf_SetCS(0);									/* ʹ��Ƭѡ */
		sf_SendByte(CMD_WRSR);							/* ������� д״̬�Ĵ��� */
		sf_SendByte(_ucValue);							/* �������ݣ�״̬�Ĵ�����ֵ */
		sf_SetCS(1);									/* ����Ƭѡ */
	}
}

/**
 * @name   sf_WaitForWriteEnd
 * @brief  ����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
 * @param  ��
 * @retval ��
 */
static void sf_WaitForWriteEnd(void)
{
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_RDSR);							/* ������� ��״̬�Ĵ��� */
	while((sf_SendByte(DUMMY_BYTE) & WIP_FLAG) == SET);	/* �ж�״̬�Ĵ�����æ��־λ */
	sf_SetCS(1);									/* ����Ƭѡ */
}
