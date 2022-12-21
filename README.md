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

![image](https://user-images.githubusercontent.com/58916022/208925004-d0b87c6b-0190-4a9d-a22f-9506f305a33b.png)

The clock enable macros were already created, we don't need to do that again. Just to recap:

![image](https://user-images.githubusercontent.com/58916022/208925922-00635983-2ae9-4eab-9767-ed42668782a0.png)

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
