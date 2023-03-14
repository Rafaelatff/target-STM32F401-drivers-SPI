# target-STM32F401-drivers-SPI
This project uses STM32CubeIDE and it's a program created to practice my C habilities during the course 'Mastering Microcontroller and Embedded Driver Development' from FastBit Embedded Brain Academy. I am using a NUCLEO-F401RE board.

## general info about SPI

**Board connection - SPI, I2C.** / Equipment connection (car, machine) - CAN, Ethernet, RS232, RS485.


SPI -> working with shift register 

	MOSI - Master Out Slave In
	
	MISO - Master In Slave Out

By default -> Full-Duplex, can be Half-Duplex [MOSI - 1kOhm - MISO], both communicate. Can be Simples - MOSI always in transmit mode e Slave in receive mode OR 
MISO and MISO connected, for Slave in transceive mode. To config SPI: NSS = output = master configuration. NSS = input = it can be multi-master or slave. 
NSS can be configured by hw (directly connected to ground) or sw (SSI bit of SPIx_CR1 register for STM32 as slave).


CPHA - SCLK Phase (controls which clock edge of data should be sampled by slave)

	CPHA in 1 = trailing edge (2nd edge)
	
	CPHA in 0 = leading edge (1st edge)
	
	
CPOL - SCLK Polarity (controls the idle state value of clock)

	CPOL in reset = low-level idle state (no comm)
	
	CPOL in set = high-level idle state
	
DFF - Data Frame Format


CPHA e CPOL together selects the data capture clock edge

Driver da SPI - APIS de:
* SPI initialization and peripheral clock control
* SPI TX
* SPI RX
* SPI Interrupt config. & Handling
* Others

## drivers library creation

Create the header and source files for SPI driver.

![image](https://user-images.githubusercontent.com/58916022/208926521-3a78f4ed-1cb6-43dc-929b-7eebd5945d4a.png)

SPI 1 and 4 are controlled by APB2 bus. We have the following boundary addresses for SPI 1 and SPI 4 being: 

![image](https://user-images.githubusercontent.com/58916022/208922081-a46cb41c-179e-4cf4-984b-2e97e72d38b3.png)

And for SPI 2 and SPI 3, both are controlled by APB1 bus and their boundary addresses are:

![image](https://user-images.githubusercontent.com/58916022/208922208-48293d54-39bd-45a4-88d9-8edc6414ad27.png)

We already have add to the stm32f401xx.h file all the addresses for the peripherals hanging on APB1 and APB2 buses, so we don't need to create them.

In the stm32f401xx.h we need to add the peripheral definitions for SPI (just after the GPIO, RCC, EXTI and SYSCFG peripheral definitions, typecasted to xxx_RegDef_t).

```c
#define SPI1		((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2		((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3		((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4		((SPI_RegDef_t*)SPI4_BASEADDR)
```

The clock enable macros were already created, we don't need to do that again. Just to recap:

![image](https://user-images.githubusercontent.com/58916022/208925922-00635983-2ae9-4eab-9767-ed42668782a0.png)

And we also need to create the SPI_RegDef_t struct. To create this struct we followed the 'Table 93. SPI register map and reset values.

![image](https://user-images.githubusercontent.com/58916022/208932150-7618f4b4-7af5-40d9-bee6-05d022261ee8.png)

```c
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

```c
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

```c
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

After the compilation, a lot of errors appeared. The include for the drivers went moved to the bottom of the stm32f401xx.h file.

![image](https://user-images.githubusercontent.com/58916022/208954557-9252beed-448d-409a-b5d6-5cc0a23d3857.png)

Some macros were created to auxiliate the coding.

![image](https://user-images.githubusercontent.com/58916022/208963798-8d72b371-e6cd-48de-9c62-376d8224d6d0.png)

Now we are going some macros to auxiliate the reading of the code. Those will be placed in the stm32f401xx.h and are creating according to the bit fields of the SPI registers.

![image](https://user-images.githubusercontent.com/58916022/208970073-a783a446-2857-495e-99ae-dfeefbe516f1.png)

The macros:

![image](https://user-images.githubusercontent.com/58916022/208970554-ad988374-d2dc-4fc0-9ed5-d3303ee9e2b7.png)

The code (in spi_driver.c), with and without the macros:

```c
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
```
The same was created for all the other register's bit fields and MUST BE CREATED FOR THE SPI_CR2 REGISTER.

To create the function SPI_SendData we will follow the 'Figure 192. SPI block diagram' of RM0368 document. 

![image](https://user-images.githubusercontent.com/58916022/208975131-edc31c99-0e5d-46e8-8257-902e09d46a5b.png)

SPI_DR (Data Register) holds the value of data register (both transmitter and receiver). Firmware doesn't have directly access to RX and TX registers.

![image](https://user-images.githubusercontent.com/58916022/208975984-3718e7a4-7c3e-4bc6-9965-466c80c487b6.png)


## Let's test the APIs

For this exercise we are going to:

1 - Test the SPI_SendData API to send the string "Hello world":

2 - Using SPI2 Master mode

3 - Using SCLK max possible

4.a. - Using DFF = 0

4.b. - Using DFF = 1

Using 'Table 9. Alternate function mapping' from stm32f401re datasheet, we can find the SPI2 pins. They are allocated only in AF05 or AF06 (actually we only found AF allocated in AF05).

![image](https://user-images.githubusercontent.com/58916022/208984547-1d82e9e2-5e95-4958-9015-8601688a6b2b.png)

We need to find the pins MOSI, MISO (not required), SCLK and NSS (not required): 

| Pin | MOSI | MISO | SCLK | NSS | 
| :---: | :---: | :---: | :---: | :---: |
| PB9 | x | x | x | SPI2_NSS (AF05) |
| PB10 | x | x | SPI2_SCK (AF05) | x |
| PB12 | x | x | x | **SPI2_NSS (AF05)** |
| PB13 | x | x | **SPI2_SCK (AF05)** | x |
| PB14 | x | **SPI2_MISO (AF05)** | x | x |
| PB15 | **SPI2_MOSI (AF05)** | x | x | x |
| PC2 | x | SPI2_MISO (AF05) | x | x |
| PC3 | SPI2_MOSI (AF05) | x | x | x |
| PD3 | x | x | SPI2_SCK (AF05) | x |

The selected pins are in **bold**.

Note 1: when coding, when the following appear, remember to add the libraries:

![image](https://user-images.githubusercontent.com/58916022/208994142-ffe53526-c730-48e7-8440-436dc82bb18a.png)

Note 2: We add to the Init APIs (libraries) the line to automatically initialize the clock for the GPIO peripheral.

![image](https://user-images.githubusercontent.com/58916022/208996184-e60929c1-2b11-4f4d-b981-5c9bd89d111f.png)

And the same for SPI init peripheral clock.

![image](https://user-images.githubusercontent.com/58916022/208996412-647fcd80-7e4b-43cb-8b19-621dbd01bb01.png)


## testing

Logic analyzer must be connected to a USB 3.0 or the software may not collect all the samples properly.

PAREI 150 (testar com osc)

## debugging

The struct SPI2Handle is ok.

![image](https://user-images.githubusercontent.com/58916022/209125735-2c19439e-1e22-4d76-a8df-da414fd955da.png)

Clock for peripheral is ok.

![image](https://user-images.githubusercontent.com/58916022/209126462-66641023-6da1-4975-a127-47e138ae1814.png)

Something is happening with my 'tempreg' variable after the line 69 is executed: tempreg &= ~(SPI_CR1_BIDIMODE);

![image](https://user-images.githubusercontent.com/58916022/209127672-7ae1ad44-4380-4209-ba7f-3fc4694fb411.png)

Checking bit field we can se it is ok: #define SPI_CR1_BIDIMODE	15	//bit field for Bidirectional data mode enable 

I commented the line 69 and SPI remained in MASTER mode. Lets check the register after 98 line (pSPIHandle->pSPIx->CR1 = tempreg;):

![image](https://user-images.githubusercontent.com/58916022/209129414-b317dc4e-1d0b-4fc5-8682-cfd15bbcdb5e.png)

Now it should work!! Still no:

Solution, thanks to the help of my teammate França, the SSM configuration was missing.

![image](https://user-images.githubusercontent.com/58916022/209142532-397cde82-6710-4e0b-ae36-dc34d7bb50b0.png)

Here it was ok (already implemented):

![image](https://user-images.githubusercontent.com/58916022/209142724-f2993de6-946c-45a8-8430-80182cae0d25.png)

But in the .c library it was missing the config:

![image](https://user-images.githubusercontent.com/58916022/209142923-43d20718-78ef-46ab-88b7-12666e49994f.png)

## results:

![WhatsApp Image 2022-12-22 at 10 24 04](https://user-images.githubusercontent.com/58916022/209143821-3245d6ae-8b90-4d4b-b2ec-b40a86630df3.jpeg)

![WhatsApp Image 2022-12-22 at 10 24 03](https://user-images.githubusercontent.com/58916022/209143871-2ff81364-dc1f-4054-aa8c-ec96e942f9fe.jpeg)





