#include "bsp_spi_flash.h"
#include "bsp.h"

/*
	只适用于SPI查询方式，其他方式需要修改代码
*/

/* 串行Flash的片选GPIO端口， PF8  */
#define SF_CS_CLK_ENABLE() 			rcu_periph_clock_enable(RCU_GPIOB)
#define SF_CS_GPIO					GPIOB
#define SF_CS_PIN					GPIO_PIN_14

#define SF_CS_0()					GPIO_BOP(SF_CS_GPIO) = ((uint32_t)SF_CS_PIN << 16U) 
#define SF_CS_1()					GPIO_BOP(SF_CS_GPIO) = SF_CS_PIN
	
#define CMD_AAI       0xAD  	/* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* 禁止写, 退出AAI状态 */
#define CMD_EWRSR	  0x50		/* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
#define CMD_SE        0x20		/* 擦除扇区命令 */
#define CMD_BE        0xC7		/* 批量擦除命令 */
#define DUMMY_BYTE    0xA5		/* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG      0x01		/* 状态寄存器中的正在编程标志（WIP) */

SFLASH_T g_tSF;

#if 0
static void sf_WriteStatus(uint8_t _ucValue);
#endif

static void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

static uint8_t s_spiBuf[4*1024];	/* 用于写函数，先读出整个扇区，修改缓冲区后，再整个扇区回写 */

/**
 * @name   bsp_InitSFlash
 * @brief  串行falsh硬件初始化。 配置CS GPIO， 读取ID。
 * @param  无
 * @retval 无
 */
void bsp_InitSFlash(void)
{
	/* 配置CS GPIO */
	{
		/* 打开GPIO时钟 */
		SF_CS_CLK_ENABLE();
		
		gpio_mode_set(SF_CS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SF_CS_PIN);
		gpio_output_options_set(SF_CS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SF_CS_PIN);
		
	}
	
	/* 读取芯片ID, 自动识别芯片型号 */
	sf_ReadInfo();
}	

/**
 * @name   sf_SetCS
 * @brief  串行FALSH片选控制函数
 * @param  无
 * @retval 无
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
 * @brief  擦除指定的扇区
 * @param  _uiSectorAddr : 扇区地址
 * @retval 无
 */
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_SetCS(0);									/* 使能片选 */
	sf_SendByte(CMD_SE);							/* 发送擦除命令 */
	sf_SendByte((_uiSectorAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	sf_SendByte((_uiSectorAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	sf_SendByte(_uiSectorAddr & 0xFF);				/* 发送扇区地址低8bit */
	
	sf_SetCS(1);									/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}

/**
 * @name   sf_EraseChip
 * @brief  擦除整个芯片
 * @param  无
 * @retval 无
 */
void sf_EraseChip(void)
{	
	sf_WriteEnable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_SetCS(0);		/* 使能片选 */
	sf_SendByte(CMD_BE);							/* 发送整片擦除命令 */
	sf_SetCS(1);									/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}

/**
 * @name   sf_PageWrite
 * @brief  页编程
 * @param  _pBuf : 数据源缓冲区。
 *		   _uiWriteAddr ：目标区域首地址。
 *		   _usSize ：数据个数，页大小的整数倍（256字节的整数倍）
 * @retval 无
 */
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	if (g_tSF.ChipID == SST25VF016B_ID)
	{
		/* AAI指令要求传入的数据个数是偶数 */
		if ((_usSize < 2) && (_usSize % 2))
		{
			return ;
		}

		sf_WriteEnable();								/* 发送写使能命令 */

		sf_SetCS(0);									/* 使能片选 */
		sf_SendByte(CMD_AAI);							/* 发送AAI命令(地址自动增加编程) */
		sf_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
		sf_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
		sf_SendByte(_uiWriteAddr & 0xFF);				/* 发送扇区地址低8bit */
		sf_SendByte(*_pBuf++);							/* 发送第1个数据 */
		sf_SendByte(*_pBuf++);							/* 发送第2个数据 */
		sf_SetCS(1);									/* 禁能片选 */

		sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */

		_usSize -= 2;									/* 计算剩余字节数 */

		for (i = 0; i < _usSize / 2; i++)
		{
			sf_SetCS(0);	/* 使能片选 */	
			sf_SendByte(CMD_AAI);						/* 发送AAI命令(地址自动增加编程) */
			sf_SendByte(*_pBuf++);						/* 发送数据 */
			sf_SendByte(*_pBuf++);						/* 发送数据 */
			sf_SetCS(1);								/* 禁能片选 */
			sf_WaitForWriteEnd();						/* 等待串行Flash内部写操作完成 */
		}

		/* 进入写保护状态 */
		sf_SetCS(0);
		sf_SendByte(CMD_DISWR);
		sf_SetCS(1);

		sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
	}
	else	/* for MX25L1606E 、 W25Q64BV */
	{
		for (j = 0; j < _usSize / 256; j++)
		{
			sf_WriteEnable();								/* 发送写使能命令 */

			sf_SetCS(0);									/* 使能片选 */
			sf_SendByte(0x02);								/* 发送AAI命令(地址自动增加编程) */
			sf_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
			sf_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
			sf_SendByte(_uiWriteAddr & 0xFF);
			for (i = 0; i < 256; i++)
			{
				sf_SendByte(*_pBuf++);					/* 发送数据 */
			}
			sf_SetCS(1);								/* 禁止片选 */

			sf_WaitForWriteEnd();						/* 等待串行Flash内部写操作完成 */

			_uiWriteAddr += 256;
		}

		/* 进入写保护状态 */
		sf_SetCS(0);
		sf_SendByte(CMD_DISWR);
		sf_SetCS(1);

		sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
	}
}

/**
 * @name   sf_ReadBuffer
 * @brief  连续读取若干字节，字节个数不能超出芯片容量。
 * @param  _pBuf : 数据源缓冲区。
 *		   _uiReadAddr ：首地址
 *		   _usSize ：数据个数, 不能超出芯片总容量
 * @retval 无
 */
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
	{
		return;
	}

	/* 擦除扇区操作 */
	sf_SetCS(0);									/* 使能片选 */
	sf_SendByte(CMD_READ);							/* 发送读命令 */
	sf_SendByte((_uiReadAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	sf_SendByte((_uiReadAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	sf_SendByte(_uiReadAddr & 0xFF);				/* 发送扇区地址低8bit */
	while (_uiSize--)
	{
		*_pBuf++ = sf_SendByte(DUMMY_BYTE);			/* 读一个字节并存储到pBuf，读完后指针自加1 */
	}
	sf_SetCS(1);									/* 禁能片选 */
}

/**
 * @name   sf_CmpData
 * @brief  比较Flash的数据。
 * @param  _ucpTar : 数据缓冲区。
 *		   _uiSrcAddr ：Flash地址
 *		   _uiSize ：数据个数, 不能超出芯片总容量
 * @retval 0 = 相等, 1 = 不等
 */
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
	uint8_t ucValue;

	/* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
	if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	sf_SetCS(0);									/* 使能片选 */
	sf_SendByte(CMD_READ);							/* 发送读命令 */
	sf_SendByte((_uiSrcAddr & 0xFF0000) >> 16);		/* 发送扇区地址的高8bit */
	sf_SendByte((_uiSrcAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	sf_SendByte(_uiSrcAddr & 0xFF);					/* 发送扇区地址低8bit */
	while (_uiSize--)
	{
		/* 读一个字节 */
		ucValue = sf_SendByte(DUMMY_BYTE);
		if (*_ucpTar++ != ucValue)
		{
			sf_SetCS(1);
			return 1;
		}
	}
	sf_SetCS(1);
	return 0;		/* 相等 */
}

/**
 * @name   sf_NeedErase
 * @brief  判断写PAGE前是否需要先擦除。
 * @param  _ucpOldBuf ： 旧数据。
 *		   _ucpNewBuf ： 新数据
 *		   _uiLen ：数据个数
 * @retval 0 : 不需要擦除， 1 ：需要擦除
 */
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	算法第1步：old 求反, new 不变
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

	算法第2步: old 求反的结果与 new 位与
		  0010   old
	&	  0101   new
		 =0000

	算法第3步: 结果为0,则表示无需擦除. 否则表示需要擦除
	*/

	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;

		/* 注意错误的写法: if (ucOld & (*_ucpNewBuf++) != 0) */
		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * @name   sf_AutoWriteSector
 * @brief  写1个扇区并校验,如果不正确则再重写两次，本函数自动完成擦除操作
 * @param  _pBuf : 数据源缓冲区
 *		   _uiWriteAddr ：目标区域首地址
 *		   _usSize ：数据个数，不能超过扇区大小
 * @retval 0 : 错误， 1 ： 成功
 */
static uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;					/* 用于延时 */
	uint32_t uiFirstAddr;		/* 扇区首址 */
	uint8_t ucNeedErase;		/* 1表示需要擦除 */
	uint8_t cRet;

	/* 长度为0时不继续操作,直接认为成功 */
	if (_usWrLen == 0)
	{
		return 1;
	}

	/* 如果偏移地址超过芯片容量则退出 */
	if (_uiWrAddr >= g_tSF.TotalSize)
	{
		return 0;
	}

	/* 如果数据长度大于扇区容量，则退出 */
	if (_usWrLen > g_tSF.SectorSize)
	{
		return 0;
	}

	/* 如果FLASH中的数据没有变化,则不写FLASH */
	sf_ReadBuffer(s_spiBuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spiBuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}

	/* 判断是否需要先擦除扇区 */
	/* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
	ucNeedErase = 0;
	if (sf_NeedErase(s_spiBuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

	if (_usWrLen == g_tSF.SectorSize)		/* 整个扇区都改写 */
	{
		for	(i = 0; i < g_tSF.SectorSize; i++)
		{
			s_spiBuf[i] = _ucpSrc[i];
		}
	}
	else						/* 改写部分数据 */
	{
		/* 先将整个扇区的数据读出 */
		sf_ReadBuffer(s_spiBuf, uiFirstAddr, g_tSF.SectorSize);

		/* 再用新数据覆盖 */
		i = _uiWrAddr & (g_tSF.SectorSize - 1);
		memcpy(&s_spiBuf[i], _ucpSrc, _usWrLen);
	}

	/* 写完之后进行校验，如果不正确则重写，最多3次 */
	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		/* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
		if (ucNeedErase == 1)
		{
			sf_EraseSector(uiFirstAddr);		/* 擦除1个扇区 */
		}

		/* 编程一个扇区 */
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

			/* 失败后延迟一段时间再重试 */
			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}

/**
 * @name   sf_WriteBuffer
 * @brief  写1个扇区并校验,如果不正确则再重写两次，本函数自动完成擦除操作
 * @param  _pBuf : 数据源缓冲区
 *		   _uiWrAddr ：目标区域首地址
 *		   _usSize ：数据个数，任意大小，但不能超过芯片容量
 * @retval 1 : 成功， 0 ： 失败
 */
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
	uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tSF.SectorSize;
	count = g_tSF.SectorSize - Addr;
	NumOfPage =  _usWriteSize / g_tSF.SectorSize;
	NumOfSingle = _usWriteSize % g_tSF.SectorSize;

	if (Addr == 0) /* 起始地址是扇区首地址  */
	{
		if (NumOfPage == 0) /* 数据长度小于扇区大小 */
		{
			if (sf_AutoWriteSector(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else 	/* 数据长度大于等于扇区大小 */
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
	else  /* 起始地址不是扇区首地址  */
	{
		if (NumOfPage == 0) /* 数据长度小于扇区大小 */
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
		else	/* 数据长度大于等于扇区大小 */
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
	return 1;	/* 成功 */
}

/**
 * @name   sf_ReadID
 * @brief  读取器件ID
 * @param  无
 * @retval 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
 */
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_SetCS(0);							/* 使能片选 */
	sf_SendByte(CMD_RDID);					/* 发送读ID命令 */
	id1 = sf_SendByte(DUMMY_BYTE);			/* 读ID的第1个字节 */
	id2 = sf_SendByte(DUMMY_BYTE);			/* 读ID的第2个字节 */
	id3 = sf_SendByte(DUMMY_BYTE);			/* 读ID的第3个字节 */
	sf_SetCS(1);							/* 禁能片选 */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/**
 * @name   sf_ReadInfo
 * @brief  读取器件ID,并填充器件参数
 * @param  无
 * @retval 无
 */
void sf_ReadInfo(void)
{
	/* 自动识别串行Flash型号 */
	{
		g_tSF.ChipID = sf_ReadID();	/* 芯片ID */

		switch (g_tSF.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tSF.ChipName, "SST25VF016B");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tSF.ChipName, "MX25L1606E");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tSF.ChipName, "W25Q64");
				g_tSF.TotalSize = 8 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;
			
			case W25Q128_ID:
				strcpy(g_tSF.ChipName, "W25Q128");
				g_tSF.TotalSize = 16 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
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
 * @brief  向器件发送写使能命令
 * @param  无
 * @retval 无
 */
static void sf_WriteEnable(void)
{
	sf_SetCS(0);									/* 使能片选 */
	sf_SendByte(CMD_WREN);							/* 发送命令 */
	sf_SetCS(1);									/* 禁能片选 */
}

/**
 * @name   sf_WriteStatu
 * @brief  写状态寄存器
 * @param  _ucValue : 状态寄存器的值
 * @retval 无
 */
static void sf_WriteStatus(uint8_t _ucValue)
{

	if (g_tSF.ChipID == SST25VF016B_ID)
	{
		/* 第1步：先使能写状态寄存器 */
		sf_SetCS(0);									/* 使能片选 */
		sf_SendByte(CMD_EWRSR);							/* 发送命令， 允许写状态寄存器 */
		sf_SetCS(1);									/* 禁能片选 */

		/* 第2步：再写状态寄存器 */
		sf_SetCS(0);									/* 使能片选 */
		sf_SendByte(CMD_WRSR);							/* 发送命令， 写状态寄存器 */
		sf_SendByte(_ucValue);							/* 发送数据：状态寄存器的值 */
		sf_SetCS(1);									/* 禁能片选 */
	}
	else
	{
		sf_SetCS(0);									/* 使能片选 */
		sf_SendByte(CMD_WRSR);							/* 发送命令， 写状态寄存器 */
		sf_SendByte(_ucValue);							/* 发送数据：状态寄存器的值 */
		sf_SetCS(1);									/* 禁能片选 */
	}
}

/**
 * @name   sf_WaitForWriteEnd
 * @brief  采用循环查询的方式等待器件内部写操作完成
 * @param  无
 * @retval 无
 */
static void sf_WaitForWriteEnd(void)
{
	sf_SetCS(0);									/* 使能片选 */
	sf_SendByte(CMD_RDSR);							/* 发送命令， 读状态寄存器 */
	while((sf_SendByte(DUMMY_BYTE) & WIP_FLAG) == SET);	/* 判断状态寄存器的忙标志位 */
	sf_SetCS(1);									/* 禁能片选 */
}
