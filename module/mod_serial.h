/**
  ******************************************************************************
  * @file   serial.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

#ifndef __MOD_SERIAL_H_
#define __MOD_SERIAL_H_

#include "app_cfg.h"
#include <string.h>

#include "usart.h"


//宏 用于控制非阻塞方式下，设置发送，接收最大缓冲区
#define SERIAL_RX_MAX_SIZE   255
#define SERIAL_TX_MAX_SIZE   255


#define SerialPutByte(dat)  	(USART_SendData(USART_USING1, dat))
#define SerialGetByte()     	((uint8_t)USART_ReceiveData(USART_USING1))
#define SerialTxIEEnable()    	do{ USART_ITConfig(USART_USING1, USART_IT_TXE, ENABLE); }while(0)
#define SerialTxIEDisable()   	do{ USART_ITConfig(USART_USING1, USART_IT_TXE, DISABLE); }while(0)

bool Serial_WriteByte(uint8_t dat);
bool Serial_WriteBuf(uint8_t *buf,uint16_t len);
uint16_t Serial_Read(uint8_t *buf,uint16_t len);
uint16_t serialRxValidAvail(void);

void Serial_TXE_Isr_callback(void);
void Serial_TXC_Isr_callback(void);
void Serial_RX_Isr_callback(void);

#define Serial_WriteStr(str) Serial_WriteBuf((uint8_t *)str,strlen(str))

#endif

