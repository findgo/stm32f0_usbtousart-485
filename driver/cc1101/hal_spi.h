


#ifndef _DRIVER_HAL_SPI_H_
#define _DRIVER_HAL_SPI_H_

#include "app_cfg.h"

#define HAL_HARD_SPI_EN 	(0)
#define HAL_SPI_MODE 		(0)

/*定义硬件IO*/
#define HAL_SPI_SCK_PORT 		GPIOA
#define HAL_SPI_SCK_PIN	 		GPIO_Pin_5
#define HAL_SPI_MISO_PORT 		GPIOA
#define HAL_SPI_MISO_PIN	 	GPIO_Pin_6
#define HAL_SPI_MOSI_PORT 		GPIOA
#define HAL_SPI_MOSI_PIN	 	GPIO_Pin_7
#define HAL_SPI_CS_PORT 		GPIOA
#define HAL_SPI_CS_PIN			GPIO_Pin_4
#define HAL_SPI_GPIO_CLOCK_EN()	do{RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);}while(0)
	

/*SPI宏*/
#define HAL_SPI_TIME_CLOCK_WIDE()       	//delay_us(1)//延时间隔100ns以上

#define HAL_SPI_SCK_OUTPUT_MODE()           /* 设置为output模式 */ 
#define HAL_SPI_SCK_LOW()                   GPIO_ResetBits(HAL_SPI_SCK_PORT, HAL_SPI_SCK_PIN)              
#define HAL_SPI_SCK_HIGH()                 	GPIO_SetBits(HAL_SPI_SCK_PORT, HAL_SPI_SCK_PIN)               

#define HAL_SPI_MISO_INPUT_MODE()            /* 设置为input模式 */ 
#define HAL_SPI_MISO_READ           		GPIO_ReadInputDataBit(HAL_SPI_MISO_PORT,HAL_SPI_MISO_PIN)

#define HAL_SPI_MOSI_OUTPUT_MODE()           /* 设置为output模式 */ 
#define HAL_SPI_MOSI_LOW()                  GPIO_ResetBits(HAL_SPI_MOSI_PORT,HAL_SPI_MOSI_PIN)	  
#define HAL_SPI_MOSI_HIGH()                 GPIO_SetBits(HAL_SPI_MOSI_PORT,HAL_SPI_MOSI_PIN)      

#define HAL_SPI_CS_OUTPUT_MODE()			/* 设置为output模式 */ 
#define HAL_SPI_CS_DEASSERT()     			GPIO_SetBits(HAL_SPI_CS_PORT,HAL_SPI_CS_PIN)
#define HAL_SPI_CS_ASSERT()       			GPIO_ResetBits(HAL_SPI_CS_PORT,HAL_SPI_CS_PIN)

#define HAL_SPI_BEGIN()        do{ HAL_SPI_CS_ASSERT();while(HAL_SPI_MISO_READ);}while(0)
#define HAL_SPI_END()          do{ HAL_SPI_CS_DEASSERT();}while(0)


//GDO0
#define HAL_CC1101_GDO0_PORT		GPIOB 
#define HAL_CC1101_GDO0_PIN			GPIO_Pin_1	 

#define HAL_CC1101_GDO0_READ		GPIO_ReadInputDataBit(HAL_CC1101_GDO0_PORT, HAL_CC1101_GDO0_PIN)
//GDO2
#define HAL_CC1101_GDO2_PORT		GPIOB
#define HAL_CC1101_GDO2_PIN			GPIO_Pin_0

#define HAL_CC1101_GDO2_READ	GPIO_ReadInputDataBit(HAL_CC1101_GDO2_PORT, HAL_CC1101_GDO2_PIN)

void halspiInit(void);
uint8_t Spi_transmit_byte(uint8_t dat);
void Spi_delay(uint16_t dely);
uint8_t halSpiRead(uint8_t addr, uint8_t* data, uint8_t length);
uint8_t halSpiWrite(uint8_t addr, uint8_t* data, uint8_t length);
uint8_t halSpiStrobe(uint8_t cmd);


#endif


