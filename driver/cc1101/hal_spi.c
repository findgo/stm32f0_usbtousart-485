
/* 主机模式 */
//模拟SPI
#include "hal_spi.h"


/* 初始化函数 */
void halspiInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//HAL_SPI_SCK_OUTPUT_MODE();
	//HAL_SPI_MISO_INPUT_MODE();
	//HAL_SPI_MOSI_OUTPUT_MODE();
	//HAL_SPI_CS_OUTPUT_MODE();

	HAL_SPI_GPIO_CLOCK_EN();

	GPIO_InitStructure.GPIO_Pin = HAL_SPI_SCK_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HAL_SPI_SCK_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = HAL_SPI_CS_PIN;
	GPIO_Init(HAL_SPI_CS_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = HAL_SPI_MOSI_PIN;
	GPIO_Init(HAL_SPI_MOSI_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = HAL_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(HAL_SPI_MISO_PORT,&GPIO_InitStructure);

#if 0
	/*GDO0配置*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = HAL_CC1101_GDO0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HAL_CC1101_GDO0_PORT,&GPIO_InitStructure);

	/*GDO2配置*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = HAL_CC1101_GDO2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HAL_CC1101_GDO2_PORT,&GPIO_InitStructure);
#endif



// for spi init
	HAL_SPI_CS_ASSERT();	//选中
#if HAL_SPI_MODE == 0 || HAL_SPI_MODE == 1
	HAL_SPI_SCK_LOW();        
#elif HAL_SPI_MODE == 2 || HAL_SPI_MODE == 3
	HAL_SPI_SCK_HIGH();  
#endif
	HAL_SPI_CS_DEASSERT();	//不选中
}


#if (HAL_HARD_SPI_EN == 1)



#else

#if HAL_SPI_MODE == 0
/* 模式0: sck常态为低,高位先发送 */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	// HAL_SPI_SCK_LOW();
	for(i = 0; i < 8; i++) {
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();    //设置
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();  
		HAL_SPI_SCK_HIGH();       //上升沿采样
		HAL_SPI_TIME_CLOCK_WIDE();//MOSI发送，MISO接收

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_SCK_LOW(); //下降沿后可设置
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 1
/* 模式1: sck常态为低,高位先发送 */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;

	for(i = 0; i < 8; i++) {
		HAL_SPI_SCK_HIGH();        /* 起始沿 */
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();   /*等待适当的时间，以等待数据稳定*/   
		HAL_SPI_SCK_LOW();       //下降沿采样，MOSI发送，MISO接收

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 2
/* 模式0: sck常态为高,高位先发送 */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	for(i = 0; i < 8; i++) {
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW(); //设置
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();  
		HAL_SPI_SCK_LOW();    //下降沿采样，MOSI发送，MISO采亲
		HAL_SPI_TIME_CLOCK_WIDE();

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_SCK_HIGH(); //上升沿可设置
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 3
/* 模式3: SCK常态为高.高位先发送 */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	for(i = 0; i < 8; i++) {
		HAL_SPI_SCK_LOW();        /* 起始沿 */
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();//设置
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();   /*等待适当的时间，以等待数据稳定*/   
		HAL_SPI_SCK_HIGH();        //上升沿采样，MOSI发送，MOSI接收

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}
#endif
#endif
//延时
void Spi_delay(uint16_t dely)
{
	uint8_t count;
	
	while(dely--)
	{
		count = 48;
		while(count--)
		{
			;
		}
	}
}

//----------------------------------------------------------------------------------
//  void halSpiWrite(uint8 addr, const uint8 *buffer, uint16 length)
//
//  DESCRIPTION:
//    Write data to device, starting at internal device address "addr".
//    The device will increment the address internally for every new byte
//    that is written. For single byte write, set length to 1.
//----------------------------------------------------------------------------------
uint8_t halSpiWrite(uint8_t addr, uint8_t* data, uint8_t length)
{
	uint8_t i;
	uint8_t rc;

	HAL_SPI_BEGIN();
	rc = Spi_transmit_byte(addr);
	for (i = 0; i < length; i++){
		Spi_transmit_byte(data[i]);
	}
	HAL_SPI_END();
	return(rc);
}

//----------------------------------------------------------------------------------
//  uint8 halSpiRead(uint8 addr, uint8* data, uint16 length)
//
//  DESCRIPTION:
//    Read data from device, starting at internal device address "addr".
//    The device will increment the address internally for every new byte
//    that is read. Note that the master device needs to write a dummy byte
//    (in this case 0) for every new byte in order to generate the clock to
//    clock out the data. For single byte read, set length to 1.
//----------------------------------------------------------------------------------
uint8_t halSpiRead(uint8_t addr, uint8_t* data, uint8_t length)
{
	uint8_t i;
	uint8_t rc;

	HAL_SPI_BEGIN();
	rc = Spi_transmit_byte(addr);
	for (i = 0; i < length; i++){
	    data[i] = Spi_transmit_byte(0);
	}
	HAL_SPI_END();

	return(rc);
}
//----------------------------------------------------------------------------------
//  uint8_t halSpiStrobe(uint8_t cmd)
//
//  DESCRIPTION:
//    Special write function, writing only one byte (cmd) to the device.
//----------------------------------------------------------------------------------
/* 指令选通脉冲写命令 */
uint8_t halSpiStrobe(uint8_t cmd)
{
	uint8_t rc;

	HAL_SPI_BEGIN();
	rc = Spi_transmit_byte(cmd);
	HAL_SPI_END();

	return(rc);
}


