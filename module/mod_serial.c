/**
  ******************************************************************************
  * @file   mod_serial.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention	 20151110     v1.1   	jgb		重构
  ******************************************************************************
  */
  
#include "mod_serial.h"


typedef struct {
	uint8_t *ptxbuf;
#if SERIAL_TX_MAX_SIZE < 256
	uint8_t txHead;
	uint8_t txTail;
	uint8_t txcount;
	uint8_t txsize;
#else
	uint16_t txHead;	//发送缓冲区头
	uint16_t txTail;	//发送缓冲区尾
	uint16_t txcount;	//发送缓冲有效个数
	uint16_t txsize;	//发送缓冲区大小
#endif

	uint8_t *prxbuf;
#if SERIAL_RX_MAX_SIZE < 256
	uint8_t rxHead;
	uint8_t rxTail;
	uint8_t rxcount;
	uint8_t rxsize;
#else
	uint16_t rxHead;	//接收缓冲区头
	uint16_t rxTail;	//接收缓冲区尾
	uint16_t rxcount;	//接收缓冲有效个数
	uint16_t rxsize;	//接收缓冲区大小
#endif
}serialcfg_t;

//发送缓冲区空闲字节数
#define SERIAL_TX_IDLE_AVAIL	(serialcfg.txsize - serialcfg.txcount)
//发送缓冲区有效节数
#define SERIAL_TX_VALID_AVAIL	(serialcfg.txcount)
//发送缓冲区是否有可读数据
#define IS_SERIAL_TX_VALID()	(serialcfg.txcount > 0)

//接收缓冲区空闲字节数
#define SERIAL_RX_IDLE_AVAIL	(serialcfg.rxsize - serialcfg.rxcount)
//接收缓冲区有效字节数   
#define SERIAL_RX_VALID_AVAIL	(serialcfg.rxcount)
//接收缓冲区是否有可读数据
#define IS_SERIAL_RX_VALID()	(serialcfg.rxcount > 0)

//private 
static uint8_t serialtxBuf[SERIAL_TX_MAX_SIZE];	//发送缓冲区
static uint8_t serialrxBuf[SERIAL_RX_MAX_SIZE];	//接收缓冲区
static serialcfg_t serialcfg = {&serialtxBuf[0], 0, 0, 0, SERIAL_TX_MAX_SIZE, &serialrxBuf[0], 0, 0, 0, SERIAL_RX_MAX_SIZE};	// serial对象实例


static bool serialtxgoingflag = false;//用于表明当前是否正在发送中


/**
  * @brief	向发送缓冲区存多个数据
  * @param	buf: 要存入缓冲区的数据 Len: 存入缓冲区的数据长度 
  * @note	
  * @retval true or false
  */
static bool SerialTxBufEnqueue(uint8_t *buf, uint16_t len)
{
	uint16_t i;
	bool bresult = false;

	if (SERIAL_TX_IDLE_AVAIL >= len) {   // 判断发送缓存是否有足够空间
		for(i = 0; i < len; i++){
			serialcfg.ptxbuf[serialcfg.txTail] = *buf++;
			serialcfg.txcount++;
			if (++serialcfg.txTail >= serialcfg.txsize){
				serialcfg.txTail = 0;
			} 
		}
		bresult = true;
	}

	return bresult;
}
/**
  * @brief	向发送缓冲区存一个数据
  * @param	dat: 要存入缓冲区的数据
  * @note	
  * @retval true or false
  */
static bool SerialTxByteEnqueue(uint8_t dat)
{
	bool bresult = false;

	if (SERIAL_TX_IDLE_AVAIL >= 1){  // 判断发送缓存是否有足够空间  
		serialcfg.ptxbuf[serialcfg.txTail] = dat;
		serialcfg.txcount++;
		if (++serialcfg.txTail >= serialcfg.txsize){
			serialcfg.txTail = 0;
		} 
		bresult = true;
	}

  return bresult;
}

/**
  * @brief	发送缓冲区取出一个字节数据
  * @param	dat: 要取缓冲区的数据地址
  * @note	
  * @retval	返回取出的字节数
  * @note		中断发送取数据时需要调用
  */
uint16_t SerialTxByteDequeue(uint8_t *dat)
{
	if(IS_SERIAL_TX_VALID()){//发送缓冲区是否有数据
		*dat = serialcfg.ptxbuf[serialcfg.txHead];
		serialcfg.txcount--;
		if(++serialcfg.txHead >= serialcfg.txsize){
			serialcfg.txHead = 0;
		}
		return 1; 
	}

	return 0;
}
/**
  * @brief	读发送缓冲区有效字节数
  * @param	
  * @note	
  * @retval	返回发送缓冲区有效字节数
  * @note		
  */
uint16_t serialTxValidAvail(void)
{
	return SERIAL_TX_VALID_AVAIL;
}



/**
  * @brief	向接收缓冲区存入一个数据
  * @param	dat:存入的数据
  * @note	
  * @retval  	ture or false
  * @note		中断接收存数据时中断调用
  */
bool SerialRxByteEnqueue(uint8_t dat)
{
	if(SERIAL_RX_IDLE_AVAIL > 0){
		serialcfg.prxbuf[serialcfg.rxTail] = dat;
		serialcfg.rxcount++;
		if(++serialcfg.rxTail >= serialcfg.rxsize){
			serialcfg.rxTail = 0;
		} 
		return true;
	}

	return false;
}

/**
  * @brief	从接收缓冲区取出len个数据
  * @param	buf:取出数据的缓冲区,len: 数据长度
  * @note	
  * @retval  返回取出数据的实际个数
  */
static uint16_t SerialRxBufDequeue(uint8_t *buf, uint16_t len)
{
	uint16_t cnt = 0;

    while(IS_SERIAL_RX_VALID() && (cnt < len))//接收缓存区有数据
    {
		*buf++ = serialcfg.prxbuf[serialcfg.rxHead];//从接收缓冲区提取数据
		serialcfg.rxcount--;
		if(++serialcfg.rxHead >= serialcfg.rxsize){
			serialcfg.rxHead = 0;
		}
		cnt++;
    }

	return cnt;
}
/**
  * @brief	从接收缓冲区取出len个数据
  * @param	buf:取出数据的缓冲区,len: 数据长度
  * @note	
  * @retval  返回取出数据的实际个数
  */
uint16_t serialRxValidAvail(void)
{
	return SERIAL_RX_VALID_AVAIL;
}

/**
  * @brief	TX输出长度为len的数据
  * @param	buf:输出数据缓冲区,len:输出数据长度
  * @note	
  * @retval  success or fail
  */
bool Serial_WriteBuf(uint8_t *buf,uint16_t len)
{
//	uint8_t temp;
	bool bresult = false;
	halIntState_t bintstate;
	
	ENTER_SAFE_ATOM_CODE(bintstate);
	bresult = SerialTxBufEnqueue(buf, len);
	EXIT_SAFE_ATOM_CODE(bintstate);
	if(!serialtxgoingflag){//只有当前不在发送进行时，才启动
	
		//采用发送完成中断 has some bug
//		if(SerialTxByteDequeue(&temp))
//		{
//			SerialPutByte(temp);
//		}
		
		//采用发送完成空中断
		if(IS_SERIAL_TX_VALID())//有数据
		{
			//使能发送中断
			SerialTxIEEnable();
		}
	}
	
	return bresult;
}
bool Serial_WriteByte(uint8_t dat)
{
//	uint8_t temp;
	bool bresult = false;
	halIntState_t bintstate;
	
	ENTER_SAFE_ATOM_CODE(bintstate);
	bresult = SerialTxByteEnqueue(dat);
	EXIT_SAFE_ATOM_CODE(bintstate);
	if(!serialtxgoingflag){//只有当前不在发送进行时，才启动
		//采用发送完成中断 has some bug
//		if(SerialTxByteDequeue(&temp)){
//			SerialPutByte(temp);
//			SerialTxIEEnable();
//		}
		
		//采用发送完成空中断
		if(IS_SERIAL_TX_VALID()){//有数据
			//使能发送中断
			SerialTxIEEnable();
		}
	}

	return bresult;
}

/**
  * @brief	从接收取出len个数据
  * @param	buf:读入数据缓冲区,len:读入数据长度
  * @note	
  * @retval  返回取到数据的实际个数
  */
uint16_t Serial_Read(uint8_t *buf, uint16_t len)
{
	uint16_t length;
	halIntState_t bintstate;

	ENTER_SAFE_ATOM_CODE(bintstate);
	length = SerialRxBufDequeue(buf, len);
	EXIT_SAFE_ATOM_CODE(bintstate);
	
	return length;
}

/***********only for Isr callback***********/
/**
  * @brief	发送空中断调用函数
  * @param	None
  * @note	
  * @note	 
  * @note	
  * @retval  None
  */
void Serial_TXE_Isr_callback(void)
{
	uint8_t temp;

	if(SerialTxByteDequeue(&temp)){
		serialtxgoingflag = true;
		SerialPutByte(temp);
	}else{
		serialtxgoingflag = false;//无数据了，表明不在发送中了
		SerialTxIEDisable();
	}
}
/**
  * @brief	发送完成中断回调函数
  * @param	None
  * @note	
  * @retval  None
  */
void Serial_TXC_Isr_callback(void)
{


}
/**
  * @brief	接收中断回调函数
  * @param	None
  * @note	
  * @retval  None
  */
void Serial_RX_Isr_callback(void)
{
	uint8_t temp = SerialGetByte();
	
	SerialRxByteEnqueue(temp);
}

