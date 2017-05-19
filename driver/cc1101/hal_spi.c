
/* ����ģʽ */
//ģ��SPI
#include "hal_spi.h"


/* ��ʼ������ */
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
	/*GDO0����*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = HAL_CC1101_GDO0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HAL_CC1101_GDO0_PORT,&GPIO_InitStructure);

	/*GDO2����*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = HAL_CC1101_GDO2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HAL_CC1101_GDO2_PORT,&GPIO_InitStructure);
#endif



// for spi init
	HAL_SPI_CS_ASSERT();	//ѡ��
#if HAL_SPI_MODE == 0 || HAL_SPI_MODE == 1
	HAL_SPI_SCK_LOW();        
#elif HAL_SPI_MODE == 2 || HAL_SPI_MODE == 3
	HAL_SPI_SCK_HIGH();  
#endif
	HAL_SPI_CS_DEASSERT();	//��ѡ��
}


#if (HAL_HARD_SPI_EN == 1)



#else

#if HAL_SPI_MODE == 0
/* ģʽ0: sck��̬Ϊ��,��λ�ȷ��� */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	// HAL_SPI_SCK_LOW();
	for(i = 0; i < 8; i++) {
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();    //����
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();  
		HAL_SPI_SCK_HIGH();       //�����ز���
		HAL_SPI_TIME_CLOCK_WIDE();//MOSI���ͣ�MISO����

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_SCK_LOW(); //�½��غ������
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 1
/* ģʽ1: sck��̬Ϊ��,��λ�ȷ��� */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;

	for(i = 0; i < 8; i++) {
		HAL_SPI_SCK_HIGH();        /* ��ʼ�� */
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();   /*�ȴ��ʵ���ʱ�䣬�Եȴ������ȶ�*/   
		HAL_SPI_SCK_LOW();       //�½��ز�����MOSI���ͣ�MISO����

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 2
/* ģʽ0: sck��̬Ϊ��,��λ�ȷ��� */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	for(i = 0; i < 8; i++) {
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW(); //����
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();  
		HAL_SPI_SCK_LOW();    //�½��ز�����MOSI���ͣ�MISO����
		HAL_SPI_TIME_CLOCK_WIDE();

		ret <<= 1;
		if (HAL_SPI_MISO_READ){
			ret |= 0x01;
		}
		HAL_SPI_SCK_HIGH(); //�����ؿ�����
		HAL_SPI_TIME_CLOCK_WIDE();
	}

	return (ret);
}


#elif HAL_SPI_MODE == 3
/* ģʽ3: SCK��̬Ϊ��.��λ�ȷ��� */
uint8_t Spi_transmit_byte(uint8_t dat)
{
	uint8_t ret = 0;
	uint8_t i;
	
	for(i = 0; i < 8; i++) {
		HAL_SPI_SCK_LOW();        /* ��ʼ�� */
		if (dat & 0x80){
			HAL_SPI_MOSI_HIGH();
		}else{
			HAL_SPI_MOSI_LOW();//����
		}
		dat <<= 1;
		HAL_SPI_TIME_CLOCK_WIDE();   /*�ȴ��ʵ���ʱ�䣬�Եȴ������ȶ�*/   
		HAL_SPI_SCK_HIGH();        //�����ز�����MOSI���ͣ�MOSI����

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
//��ʱ
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
/* ָ��ѡͨ����д���� */
uint8_t halSpiStrobe(uint8_t cmd)
{
	uint8_t rc;

	HAL_SPI_BEGIN();
	rc = Spi_transmit_byte(cmd);
	HAL_SPI_END();

	return(rc);
}


