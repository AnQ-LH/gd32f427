#ifndef __BSP_EXMC_SRAM_H
#define __BSP_EXMC_SRAM_H

#include "gd32f4xx.h"

#define Bank0_SRAM2_ADDR            ((uint32_t)0x68000000)
#define BANK_SRAM_ADDR              Bank0_SRAM2_ADDR
#define BANK_SRAM_SIZE				(1 * 1024 * 1024)

void bsp_InitExtSRAM(void);
uint8_t bsp_TestExtSRAM(void);

void exmc_sram_writebuffer_16(uint16_t *pbuffer, uint32_t writeaddr, uint32_t num_halfword_write);
void exmc_sram_readbuffer_16(uint16_t *pbuffer, uint32_t readaddr, uint32_t num_halfword_read);

void exmc_sram_writebuffer_8(uint8_t *pbuffer, uint32_t writeaddr, uint32_t num_byte_write);
void exmc_sram_readbuffer_8(uint8_t *pbuffer, uint32_t readaddr, uint32_t num_byte_read);

void exmc_sram_writebuffer_32(uint32_t *pbuffer, uint32_t writeaddr, uint32_t num_word_write);
void exmc_sram_readbuffer_32(uint32_t *pbuffer, uint32_t readaddr, uint32_t num_word_read);

void fill_buffer_16(uint16_t *pbuffer, uint16_t buffer_lenght, uint16_t offset);

#endif	/* __BSP_EXMC_SRAM_H */