# target-STM32F401-drivers-SPI
This project uses STM32CubeIDE and it's a program created to practice my C habilities during the course 'Mastering Microcontroller and Embedded Driver Development' from FastBit Embedded Brain Academy. I am using a NUCLEO-F401RE board.

## drivers library creation

Create the header and source files for SPI driver.

![image](https://user-images.githubusercontent.com/58916022/208926521-3a78f4ed-1cb6-43dc-929b-7eebd5945d4a.png)

SPI 1 and 4 are controlled by APB2 bus. We have the following boundary addresses for SPI 1 and SPI 4 being: 

![image](https://user-images.githubusercontent.com/58916022/208922081-a46cb41c-179e-4cf4-984b-2e97e72d38b3.png)

And for SPI 2 and SPI 3, both are controlled by APB1 bus and their boundary addresses are:

![image](https://user-images.githubusercontent.com/58916022/208922208-48293d54-39bd-45a4-88d9-8edc6414ad27.png)

We already have add to the stm32f401xx.h file all the addresses for the peripherals hanging on APB1 and APB2 buses, so we don't need to create them.

In the stm32f401xx.h we need to add the peripheral definitions for SPI (just after the GPIO, RCC, EXTI and SYSCFG peripheral definitions, typecasted to xxx_RegDef_t).

```
#define SPI1		((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2		((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3		((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4		((SPI_RegDef_t*)SPI4_BASEADDR)
```

The clock enable macros were already created, we don't need to do that again. Just to recap:

![image](https://user-images.githubusercontent.com/58916022/208925922-00635983-2ae9-4eab-9767-ed42668782a0.png)

And we also need to create the SPI_RegDef_t struct. To create this struct we followed the 'Table 93. SPI register map and reset values.

![image](https://user-images.githubusercontent.com/58916022/208932150-7618f4b4-7af5-40d9-bee6-05d022261ee8.png)

```
typedef struct {
	volatile uint32_t SPI_CR1; // SPI control register 1 - Address offset: 0x00
	volatile uint32_t SPI_CR2; // SPI control register 2 - Address offset: 0x04
	volatile uint32_t SPI_SR; // SPI status register - Address offset: 0x08
	volatile uint32_t SPI_DR; // SPI data register - Address offset: 0x0C
	volatile uint32_t SPI_CRCPR; // SPI CRC polynomial register - Address offset: 0x10
	volatile uint32_t SPI_RXCRCR; // SPI RX CRC register - Address offset: 0x14
	volatile uint32_t SPI_TXCRCR; // SPI TX CRC register - Address offset: 0x18
	volatile uint32_t SPI_I2SCFGR; // SPI I2S configuration register - Address offset: 0x1C
	volatile uint32_t SPI_I2SPR; // SPI I2S prescaler register - Address offset: 0x20
} SPI_RegDef_t;
```

Let's them start working on the stm32f401xx_spi_driver.h file by creating the structs for SPI_Config_t and SPI_Handle_t. The code is showed below.

```
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

/*
 * Configuration structure for SPIx peripheral
 */
typedef struct{
	SPI_RegDef_t *pSPIx; // Holds base address of SPIx(x = 0,1,2) peripheral
	SPI_Config_t SPIConfig/
} SPI_Handle_t;
```

And them we create the SPI prototypes in the .h file.

```
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

```

And in the stm32f401xx_spi_driver.c we copy them to start implementing the APIs.
