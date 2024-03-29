#ifndef _USER_DIRVER_UART_H_
#define _USER_DIRVER_UART_H_

#include "app_cfg.h"


#define USART_USING1   					USART1
#define USART_USING1_IRQ				USART1_IRQn	
#define USART_USING1_TX_PORT			GPIOB
#define USART_USING1_TX_PIN				GPIO_Pin_6
#define USART_USING1_TX_AF_PINSOURCE	GPIO_PinSource6
#define USART_USING1_TX_AF_SEL			GPIO_AF_0

#define USART_USING1_RX_PORT			GPIOB
#define USART_USING1_RX_PIN				GPIO_Pin_7
#define USART_USING1_RX_AF_PINSOURCE	GPIO_PinSource7
#define USART_USING1_RX_AF_SEL			GPIO_AF_0

#define USART_USING1_GPIO_PeriphClock_EN() do{ RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); }while(0)
#define USART_USING1_PeriphClock_EN()	do{ RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);}while(0)




#define USART_USING2   					USART2
#define USART_USING2_IRQ				USART2_IRQn	
#define USART_USING2_TX_PORT			GPIOA
#define USART_USING2_TX_PIN				GPIO_Pin_2
#define USART_USING2_TX_AF_PINSOURCE	GPIO_PinSource2
#define USART_USING2_TX_AF_SEL			GPIO_AF_1

#define USART_USING2_RX_PORT			GPIOA
#define USART_USING2_RX_PIN				GPIO_Pin_3
#define USART_USING2_RX_AF_PINSOURCE	GPIO_PinSource3
#define USART_USING2_RX_AF_SEL			GPIO_AF_1

#define USART_USING2_DE_PORT			GPIOA
#define USART_USING2_DE_PIN				GPIO_Pin_1
#define USART_USING2_DE_AF_PINSOURCE	GPIO_PinSource1
#define USART_USING2_DE_AF_SEL			GPIO_AF_1			

#define USART_USING2_GPIO_PeriphClock_EN() do{ RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); }while(0)
#define USART_USING2_PeriphClock_EN()	do{ RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); }while(0)




void Usart_Configuration(void);
void Usart_Vcp_Configuration(USART_InitTypeDef *USART_InitStruct, FunctionalState DEENABLE);

#endif

