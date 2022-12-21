/*
 * stm32f401_spi_driver.c
 *
 *  Created on: 21 de dez de 2022
 *      Author: r.freitas
 */
#include "stm32f401xx_spi_driver.h"

/*******************************************************
 * 			API supported by this driver
 *
 *******************************************************/


/*
 *	@name 	SPI_PeriClkCtrl
 *	@brief	Clock Control for SPIO peripheral
 *
 *	@param	*pSPIx -> pointer to SPIx address (structure SPI_RegDef_t type)
 *	@param	EnOrDi can be ENABLE or DISABLE
 *	@return void type
 */
void SPI_PeriClkCtrl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if (EnOrDi == ENABLE){
		if(pSPIx == SPI1){
			SPI1_PCLK_EN();
		} else if (pSPIx == SPI2){
			SPI2_PCLK_EN();
		} else if (pSPIx == SPI3){
			SPI3_PCLK_EN();
		} else if (pSPIx == SPI4){
			SPI4_PCLK_EN();
		}
	}else{
		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		} else if (pSPIx == SPI2){
			SPI2_PCLK_DI();
		} else if (pSPIx == SPI3){
			SPI3_PCLK_DI();
		} else if (pSPIx == SPI4){
			SPI4_PCLK_DI();
		}
	}
}

/*
 *	@name 	SPI_
 *	@brief
 *
 *	@param
 *	@return void type
 */
void SPI_Init(SPI_Handle_t *pSPIHandle){

	// automatically enables the peripheral clock
	SPI_PeriClkCtrl(pSPIHandle->pSPIx, ENABLE);

	// 1. Lets configure the SPI_CR1 register
	uint32_t tempreg = 0;

	// 1.a. Configure the device mode
	// NO MACROS: tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	// 1.b. Configure the bus configuration
	if(pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_FD){
		//Bi-direcional mode should be cleared
		tempreg &= ~(1 << 15);
	}else if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_HD){
		//Bi-direcional mode should be set
		tempreg |= (1 << 15);
	}else if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_S_RXONLY){
		//Bi-direcional mode should be cleared
		tempreg &= ~(1 << 15);
		//RX Only bit must be set
		tempreg |= (1 << 150);
	}else if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_S_TXONLY){

	}
	// 1.c. Configure the SPI serial clock speed (baud rate)
	// NO MACROS: tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << 3;
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	// 1.d. Configure the DFF
	// NO MACROS: tempreg |= pSPIHandle->SPIConfig.SPI_DFF << 11;
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	// 1.e. Configure the CPOL
	// NO MACROS: tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << 1;
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	// 1.f. Configure the CPHA
	// NO MACROS: tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << 0;
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	// Send values do SPI_CR1
	pSPIHandle->pSPIx->CR1 = tempreg;

	// 2. Lets configure the SPI_CR2 register
}

/*
 *	@name 	SPI_DeInit
 *	@brief
 *
 *	@param	*pSPIx -> pointer to SPIx address (structure SPI_RegDef_t type)
 *	@return void type
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx);


uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName){
	if(pSPIx->SR & FlagName){
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*
 *	@name 	SPI_SendData
 *	@brief
 *
 *	@param	*pSPIx -> pointer to SPIx address (structure SPI_RegDef_t type)
 *	@param	*pTxBuffer
 *	@param  Len
 *	@return void type
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len){
	//SPI_SR_TXE says if the Transmit buffer empty (1 = empty)
	//RXNE flag set when RX buffer is not empty
	while (Len>0){
		//1. Wait until TXE is set
		//ONE WAY: while(!(pSPIx->SR & (1<<1)));
		while (SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG) == FLAG_RESET); //OR ANOTHER
		// Here we are polling for the TXE flag to SET [NOT GOOD]
		// Later we will implement an interrupt so code won't get stuck in here

		//2. Check the DFF  bit in CR1
		if((pSPIx->CR1 & (1 << SPI_CR1_DFF))){
			// 16-bit DFF
			//2.a. Load the data in to the DR
			pSPIx-> DR = *((uint16_t*)pTxBuffer); //de-referenced value typecasted to 16 bit lenght
			Len--;
			Len--; // 2 bytes sent at time
			(uint16_t*)pTxBuffer++; // Increment the buffer
		}else{
			// 8-bit DFF
			pSPIx-> DR = *pTxBuffer;
			Len--;
			pTxBuffer++; // Increment the buffer
		}
	}
}

/*
 *	@name 	SPI_PeriClkCtrl
 *	@brief	Clock Control for SPIO peripheral
 *
 *	@param	*pSPIx -> pointer to SPIx address (structure SPI_RegDef_t type)
 *	@param	EnOrDi can be ENABLE or DISABLE
 *	@return void type
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

/*
 *  IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 *	@name 	SPI_PeripheralControl
 *	@brief	Enable or Disable the SPI. All configuration of SPI must occur with
 *			SPI disabled. After all configs are made, then (and only then) the
 *			SPI can be enable.
 *
 *	@param	*pSPIx -> pointer to SPIx address (structure SPI_RegDef_t type)
 *	@param	EnOrDi can be ENABLE or DISABLE
 *	@return void type
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if(EnOrDi == ENABLE){
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}

}
