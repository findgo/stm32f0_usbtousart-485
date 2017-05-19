/**
  ******************************************************************************
  * @file    usart.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "usart.h"


extern bool Serial_WriteByte(uint8_t dat);

static void Usart_DeConfiguration(void);

/**
  * @brief	usartÅäÖÃ
  * @param	None
  * @note	
  * @retval  None
  */
void Usart_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	USART_InitTypeDef USART_InitStruct;
	
	USART_USING1_GPIO_PeriphClock_EN();
	USART_USING1_PeriphClock_EN();

	GPIO_PinAFConfig(USART_USING1_TX_PORT, USART_USING1_TX_AF_PINSOURCE, USART_USING1_TX_AF_SEL);
	GPIO_PinAFConfig(USART_USING1_RX_PORT, USART_USING1_RX_AF_PINSOURCE, USART_USING1_RX_AF_SEL);
	//PB6 ï¿½ï¿½ï¿½ï¿½ÎªUSAR1 TX, PB7 ï¿½ï¿½ï¿½ï¿½ÎªUSART1 RX
	GPIO_InitStruct.GPIO_Pin   = USART_USING1_TX_PIN;//TX AF mode
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;  //ï¿½ï¿½Ò»ï¿½ï¿½È·ï¿½ï¿½ï¿½Äµï¿½Æ½?
	GPIO_Init(USART_USING1_TX_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = USART_USING1_RX_PIN;//RX AF mode
	GPIO_Init(USART_USING1_RX_PORT, &GPIO_InitStruct);

	
	NVIC_InitStruct.NVIC_IRQChannel = USART_USING1_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART_USING1, &USART_ClockInitStruct);

	USART_InitStruct.USART_BaudRate = 115200;//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//Ê¹ï¿½Ü·ï¿½ï¿½ÍºÍ½ï¿½ï¿½ï¿½
	USART_InitStruct.USART_HardwareFlowControl = DISABLE;
	USART_InitStruct.USART_Parity = USART_Parity_No;//ï¿½ï¿½ï¿½ï¿½Ð£ï¿½ï¿½
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//Ò»ï¿½ï¿½Í£Ö¹Î»
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8Î»ï¿½ï¿½ï¿½Î»
	USART_Init(USART_USING1, &USART_InitStruct);


	USART_ClearFlag(USART_USING1,    USART_FLAG_TC);
	USART_ITConfig(USART_USING1, USART_IT_TXE,  DISABLE);
	USART_ITConfig(USART_USING1, USART_IT_TC, DISABLE);
	USART_ITConfig(USART_USING1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART_USING1, ENABLE);//Ê¹ï¿½ï¿½USART1
}

void Usart_Vcp_Configuration(USART_InitTypeDef *USART_InitStruct, FunctionalState DEENABLE)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_ClockInitTypeDef USART_ClockInitStruct;
//	USART_InitTypeDef USART_InitStruct;
	
	USART_USING2_GPIO_PeriphClock_EN();
	USART_USING2_PeriphClock_EN();

	GPIO_PinAFConfig(USART_USING2_TX_PORT, USART_USING2_TX_AF_PINSOURCE, USART_USING2_TX_AF_SEL);
	GPIO_PinAFConfig(USART_USING2_RX_PORT, USART_USING2_RX_AF_PINSOURCE, USART_USING2_RX_AF_SEL);
	//PA2   USAR2 TX, PA3 USART2 RX
	GPIO_InitStruct.GPIO_Pin   = USART_USING2_TX_PIN;//TX AF mode
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP; 
	GPIO_Init(USART_USING2_TX_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = USART_USING2_RX_PIN;//RX AF mode
	GPIO_Init(USART_USING2_RX_PORT, &GPIO_InitStruct);

	if(DEENABLE == ENABLE){
		Usart_DeConfiguration();
	}

	NVIC_InitStruct.NVIC_IRQChannel = USART_USING2_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART_USING2, &USART_ClockInitStruct);

//	USART_InitStruct.USART_BaudRate = 115200;//baudrate
//	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//enable tx and rx
//	USART_InitStruct.USART_HardwareFlowControl = DISABLE;
//	USART_InitStruct.USART_Parity = USART_Parity_No;// ÎÞÐ§Ñé
//	USART_InitStruct.USART_StopBits = USART_StopBits_1;//Ò»Î»Í£Ö¹Î»
//	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8Î»³¤¶È
//	USART_Init(USART_USING2, &USART_InitStruct);
	USART_Init(USART_USING2, USART_InitStruct);
	
	USART_InvPinCmd(USART_USING2, USART_InvPin_Tx, ENABLE);

	USART_ClearFlag(USART_USING2, USART_FLAG_TC);
	USART_ITConfig(USART_USING2, USART_IT_TXE,  DISABLE);
	USART_ITConfig(USART_USING2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART_USING2, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART_USING2, ENABLE);//Ê¹ÄÜUSART2
}

static void Usart_DeConfiguration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_PinAFConfig(USART_USING2_DE_PORT, USART_USING2_DE_AF_PINSOURCE, USART_USING2_DE_AF_SEL);
	//PA2   USAR2 TX, PA3 USART2 RX
	GPIO_InitStruct.GPIO_Pin   = USART_USING2_DE_PIN;//TX AF mode
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP; 
	GPIO_Init(USART_USING2_DE_PORT, &GPIO_InitStruct);

	USART_DECmd(USART_USING2, ENABLE);
}

/* ï¿½Ø¶ï¿½ï¿½ï¿½fputc */ 
#ifdef DEBUG_MSG
int fputc(int ch, FILE *f)
{
	(void)Serial_WriteByte(ch);

    /* e.g. write a character to the USART */
	//USART_SendData(USART_USING1,ch);
    /* Loop until the end of transmission */
	//while(USART_GetFlagStatus(USART_USING1, USART_FLAG_TC) == RESET);
	
    return ch;
}
#endif
