#ifndef __BSP_GT1151_H
#define __BSP_GT1151_H

#include "gd32f4xx.h"

#define GT1151_I2C_WR	0		/* 写控制bit */
#define GT1151_I2C_RD	1		/* 读控制bit */

/* I2C读写命令 */
#define GT1151_CMD_WR 		0X28    	/* 写命令 */
#define GT1151_CMD_RD 		0X29		/* 读命令 */
 
#define GT1151_CTRL_REG 	0X8040   	/* GT1151控制寄存器 */
#define GT1151_CFGS_REG 	0X8050   	/* GT1151地址寄存器 */
#define GT1151_CHECK_REG 	0X813C   	/* GT1151验和寄存器 */
#define GT1151_PID_REG 		0X8140   	/* GT1151产品ID寄存器 */

#define GT1151_GSTID_REG 	0X814E   	/* GT1151检测到的触摸情况 */
#define GT1151_TP0_REG 		0X8150  	/* 第一个触摸点数据地址 */
#define GT1151_TP1_REG 		0X8158		/* 第二个触摸点数据地址 */
#define GT1151_TP2_REG 		0X8160		/* 第三个触摸点数据地址 */
#define GT1151_TP3_REG 		0X8168		/* 第四个触摸点数据地址 */
#define GT1151_TP4_REG 		0X8170		/* 第五个触摸点数据地址 */

typedef struct
{
	uint8_t Enable;
	uint8_t TimerCount;
	uint8_t i2c_addr;
	
	uint8_t TouchpointFlag;
	uint8_t Touchkeystate;

	uint16_t X0;
	uint16_t Y0;
	uint16_t P0;

	uint16_t X1;
	uint16_t Y1;
	uint16_t P1;

	uint16_t X2;
	uint16_t Y2;
	uint16_t P2;

	uint16_t X3;
	uint16_t Y3;
	uint16_t P3;

	uint16_t X4;
	uint16_t Y4;
	uint16_t P4;
}GT1151_T;

extern GT1151_T g_GT1151;

void GT1151_I2CInit(void);
void GT1151_InitHard(void);
uint32_t GT1151_ReadID(void);
void GT1151_Timer1ms(void);
void GT1151_Scan(void);

void GT1151_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen);
void GT1151_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen);


void gt1151_i2c_Start(void);
void gt1151_i2c_Stop(void);
void gt1151_i2c_SendByte(uint8_t _ucByte);
uint8_t gt1151_i2c_ReadByte(uint8_t ack);
uint8_t gt1151_i2c_WaitAck(void);
void gt1151_i2c_Ack(void);
void gt1151_i2c_NAck(void);
uint8_t gt1151_i2c_CheckDevice(uint8_t _Address);

#endif	/* __BSP_GT1151_H */

