/*
 * stm32f401_spi_driver.h
 *
 *  Created on: 21 de dez de 2022
 *      Author: r.freitas
 */

#ifndef INC_STM32F401XX_SPI_DRIVER_H_
#define INC_STM32F401XX_SPI_DRIVER_H_

#include "stm32f401xx.h"

/*
 * Configuration structure for SPIx peripheral
 */
typedef struct{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
} SPI_Config_t;

/*typedef struct{
	uint8_t SPI_CPHA; //Clock phase
	uint8_t SPI_CPOL; //Clock polarity
	uint8_t SPI_DeviceMode; //Master selection
	uint8_t SPI_BusConfig[2]; //Baud rate control
	uint8_t SPI_SPE; //SPI enable
	uint8_t SPI_LSBFIRST; //Frame format
	uint8_t SPI_SSI; //Internal slave select
	uint8_t SPI_SSM; //Software slave management
	uint8_t SPI_RXONLY; //Receive only
	uint8_t SPI_DFF; //Data frame format
	uint8_t SPI_CRCNEXT; //CRC transfer next
	uint8_t SPI_CRCEN; //Hardware CRC calculation enable
	uint8_t SPI_BIDIOE; //Output enable in bidirectional mode
	uint8_t SPI_BIDIMODE; //Bidirectional data mode enable

} SPI_Config_t;*/


typedef struct{
	SPI_RegDef_t *pSPIx; // Holds base address of SPIx(x = 0,1,2) peripheral
	SPI_Config_t SPIConfig;
} SPI_Handle_t;


/*******************************************************
 * 			             Macros
 *******************************************************/
// @SPI_DeviceMode
#define SPI_DEVICE_MODE_MASTER	1
#define SPI_DEVICE_MODE_SLAVE	0			// default
// @SPI_BusConfig
#define SPI_BUS_CONFIG_FD 			1		// Full-Duplex - default
#define SPI_BUS_CONFIG_HD 			2		// Half-Duplex
#define SPI_BUS_CONFIG_S_TXONLY 	3		// Simplex TX Only
#define SPI_BUS_CONFIG_S_RXONLY		4		// Simplex RX Only
// @SPI_SclkSpeed
#define SPI_SCLK_SPEED_DIV2			0 		// fpclk/2
#define SPI_SCLK_SPEED_DIV4			1 		// fpclk/4
#define SPI_SCLK_SPEED_DIV8			2 		// fpclk/8
#define SPI_SCLK_SPEED_DIV16		3 		// fpclk/16
#define SPI_SCLK_SPEED_DIV32		4 		// fpclk/32
#define SPI_SCLK_SPEED_DIV64		5 		// fpclk/64
#define SPI_SCLK_SPEED_DIV128		6 		// fpclk/128
#define SPI_SCLK_SPEED_DIV256		7 		// fpclk/256
// @SPI_DFF
#define	SPI_DFF_8BITS			0		//8 -bit frame
#define	SPI_DFF_16BITS			1		//16-bit frame
// @SPI_CPOL
#define SPI_CPOL_LOW			0
#define SPI_CPOL_HIGH			1
// @SPI_CPHA
#define SPI_CPHA_LOW			0
#define SPI_CPHA_HIGH			1
// @SPI_SSM
#define SPI_SSM_DI				0		// Chip select controlled by SW
#define SPI_SSM_EN				1		// Chip select controlled by HW - default

/*
 * 	SPI related status flags definitions
 */
#define SPI_TXE_FLAG 	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG 	(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG 	(1 << SPI_SR_BSY)
// Create for all other flags in SR register

/*******************************************************
 * 			API supported by this driver
 *
 *******************************************************/

/*
 *  Peripheral Clock Setup
 */
void SPI_PeriClkCtrl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

/*
 *  Init and De-Init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 *  Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

/*
 *  IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 * 	Other Peripheral Control APIs
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

#endif /* INC_STM32F401XX_SPI_DRIVER_H_ */
