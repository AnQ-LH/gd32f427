#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "gd32f4xx.h"

#define	SPI_BUFFER_SIZE		(10)

extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
extern uint32_t  g_spiTxLen, g_spiRxLen;

extern uint8_t g_spi_busy;

void bsp_InitSPIBus(void);
void bsp_InitSPIParam(uint32_t _BaudRatePrescaler, uint32_t _CLKPolarityAndPhase);

uint8_t sf_SendByte(uint8_t _ucValue);

void bsp_SpiBusEnter(void);
void bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);

#endif  /* __BSP_SPI_H */
