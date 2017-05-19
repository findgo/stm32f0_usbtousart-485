/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "usbd_cdc_core.h"
#include "usart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LINE_CODING linecoding =
{
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* nb. of bits 8*/
};


USART_InitTypeDef USART_InitStructure;

app_transfer_t app_comrx_utx;
app_transfer_t app_comtx_urx;
__IO uint32_t last_packet = 0;
uint8_t  USB_Tx_State = USB_TXOVER;


/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataTx   (void *pdev, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx   (void *pdev, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops = 
{
	VCP_Init,
	VCP_DeInit,
	VCP_Ctrl,
	VCP_DataTx,
	VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the operation (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{

  /* EVAL_COM1 default configuration */
  /* EVAL_COM1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Parity Odd
        - Hardware flow control disabled
        - Receive and transmit enabled
 */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure and enable the USART */
  Usart_Vcp_Configuration(&USART_InitStructure, ENABLE);

  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{

  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    /* Set the new configuration */
    VCP_COMConfig(OTHER_CONFIG);
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype;
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;

  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataTx (void *pdev, uint8_t* Buf, uint32_t Len)
{
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;
	halIntState_t bintstate;
	
	ENTER_SAFE_ATOM_CODE(bintstate);	  
	if (USB_Tx_State == USB_TXING)
	{
	  if (app_comrx_utx.length == 0) {
		  if (last_packet ==1){
			  last_packet = 0;
		  
			  /*Send zero-length packet*/
			  DCD_EP_Tx (pdev, CDC_IN_EP, 0, 0);
		  }else{
			  USB_Tx_State = USB_TXOVER;
		  }
	  }else {
		  USB_Tx_length = APP_RX_DATA_SIZE - app_comrx_utx.ptr_out;
		  if(USB_Tx_length > app_comrx_utx.length){
			  USB_Tx_length = app_comrx_utx.length;   
		  }
		  
		  
		  if (USB_Tx_length > CDC_DATA_IN_PACKET_SIZE){
			  USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;
		  }else{
			  if (USB_Tx_length == CDC_DATA_IN_PACKET_SIZE) 
				  last_packet = 1; //IBA	  
		  }
		  USB_Tx_ptr = app_comrx_utx.ptr_out;
		  app_comrx_utx.ptr_out += USB_Tx_length; 
		  app_comrx_utx.length -= USB_Tx_length;
		  
		  if(app_comrx_utx.ptr_out >= APP_RX_DATA_SIZE){
			  app_comrx_utx.ptr_out = 0;
		  }



		  /* Prepare the available data buffer to be sent on IN endpoint */
		  DCD_EP_Tx (pdev,
					 CDC_IN_EP,
					 (uint8_t*)&app_comrx_utx.buffer[USB_Tx_ptr],
					 USB_Tx_length);
	  }
	}  
	EXIT_SAFE_ATOM_CODE(bintstate);
	return USBD_OK;
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @note   This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataRx (void *pdev, uint8_t* Buf, uint32_t Len)
{
	uint32_t i;
	halIntState_t bintstate;

	ENTER_SAFE_ATOM_CODE(bintstate);
	for (i = 0; i < Len; i++){
		if(app_comtx_urx.length != APP_RX_DATA_SIZE){
			app_comtx_urx.buffer[app_comtx_urx.ptr_in] = *(Buf + i);
			app_comtx_urx.length++;
			if(++app_comtx_urx.ptr_in >= APP_RX_DATA_SIZE){
				app_comtx_urx.ptr_in = 0;
			}
		}	
	}
	USART_ITConfig(USART_USING2, USART_IT_TXE, ENABLE);
	EXIT_SAFE_ATOM_CODE(bintstate);

	return USBD_OK;
}

/**
  * @brief  VCP_COMConfig
  *         Configure the COM Port with default values or values received from host.
  * @param  Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG
  *         to set a configuration received from the host.
  * @retval None.
  */
static uint16_t VCP_COMConfig(uint8_t Conf)
{
	if (Conf == DEFAULT_CONFIG){
		/* EVAL_COM1 default configuration */
		/* EVAL_COM1 configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- Parity Odd
		- Hardware flow control disabled
		- Receive and transmit enabled
		*/
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		/* Configure and enable the USART */
		Usart_Vcp_Configuration(&USART_InitStructure, ENABLE);

		
	}else{
		/* set the Stop bit*/
		switch (linecoding.format){
			case 0:
				USART_InitStructure.USART_StopBits = USART_StopBits_1;
				break;
			case 1:
				USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
				break;
			case 2:
				USART_InitStructure.USART_StopBits = USART_StopBits_2;
				break;
			default :
				VCP_COMConfig(DEFAULT_CONFIG);
				return (USBD_FAIL);
		}

		/* set the parity bit*/
		switch (linecoding.paritytype)
		{
		case 0:
		  USART_InitStructure.USART_Parity = USART_Parity_No;
		  break;
		case 1:
		  USART_InitStructure.USART_Parity = USART_Parity_Even;
		  break;
		case 2:
		  USART_InitStructure.USART_Parity = USART_Parity_Odd;
		  break;
		default :
		  VCP_COMConfig(DEFAULT_CONFIG);
		  return (USBD_FAIL);
		}

		/*set the data type : only 8bits and 9bits is supported */
		switch (linecoding.datatype)
		{
		case 0x07:
		  /* With this configuration a parity (Even or Odd) should be set */
		  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		  break;
		case 0x08:
		  if (USART_InitStructure.USART_Parity == USART_Parity_No)
		  {
		    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		  }
		  else
		  {
		    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		  }

		  break;
		default :
		  VCP_COMConfig(DEFAULT_CONFIG);
		  return (USBD_FAIL);
		}

		USART_InitStructure.USART_BaudRate = linecoding.bitrate;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		/* Configure and enable the USART */
		Usart_Vcp_Configuration(&USART_InitStructure, ENABLE);
  }
	
	return USBD_OK;
}


static void VCP_Uart_Receive(void)
{
	uint8_t dat;
	halIntState_t bintstate;

	if (linecoding.datatype == 7){
		dat = USART_ReceiveData(USART_USING2) & 0x7F;
	}else if (linecoding.datatype == 8){
		dat = USART_ReceiveData(USART_USING2);
	}
	ENTER_SAFE_ATOM_CODE(bintstate)	
	if(app_comrx_utx.length != APP_RX_DATA_SIZE){
		app_comrx_utx.buffer[app_comrx_utx.ptr_in] = dat;
		app_comrx_utx.length++;
		/* To avoid buffer overflow */
		if(++app_comrx_utx.ptr_in >= APP_RX_DATA_SIZE){
	    	app_comrx_utx.ptr_in = 0;
		}
	}
	EXIT_SAFE_ATOM_CODE(bintstate);
}
/**
  * @brief  Handle_USBAsynchXfer
  *         Send data to USB
  * @param  pdev: instance
  * @retval None
  */
void Handle_USBAsynchXfer (void *pdev)
{
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;
	halIntState_t bintstate;

	ENTER_SAFE_ATOM_CODE(bintstate);
	if(USB_Tx_State == USB_TXOVER){ // usb send going on
		if(app_comrx_utx.length == 0){ // no data want to send
			USB_Tx_State = USB_TXOVER; 
		}else{
			USB_Tx_length = APP_RX_DATA_SIZE - app_comrx_utx.ptr_out;
		    if(USB_Tx_length > app_comrx_utx.length){
				USB_Tx_length = app_comrx_utx.length;   
		    }
		    
			if (USB_Tx_length > CDC_DATA_IN_PACKET_SIZE){
				USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;
		    }else{
				if (USB_Tx_length == CDC_DATA_IN_PACKET_SIZE) 
					last_packet = 1; //IBA		
		    }
			USB_Tx_ptr = app_comrx_utx.ptr_out;
			app_comrx_utx.ptr_out += USB_Tx_length;	
			app_comrx_utx.length -= USB_Tx_length;
			
			if(app_comrx_utx.ptr_out >= APP_RX_DATA_SIZE){
				app_comrx_utx.ptr_out = 0;
			}
			USB_Tx_State = USB_TXING; 


		    DCD_EP_Tx (pdev,
		               CDC_IN_EP,
		               (uint8_t*)&app_comrx_utx.buffer[USB_Tx_ptr],
		               USB_Tx_length);
		}
	}
	EXIT_SAFE_ATOM_CODE(bintstate); 
}

/**
  * @brief  EVAL_COM_IRQHandler
  * @param  None.
  * @retval None.
  */
void VCP_isr_callback(void)
{
  uint8_t dat;
  halIntState_t bintstate;


	if (USART_GetITStatus(USART_USING2, USART_IT_RXNE) != RESET){
		/* Send the received data to the PC Host*/
		VCP_Uart_Receive();
	}

	/* If overrun condition occurs, clear the ORE flag and recover communication */
	if (USART_GetFlagStatus(USART_USING2, USART_FLAG_ORE) != RESET)
	{
		(void)USART_ReceiveData(USART_USING2);
	}

	if(USART_GetFlagStatus(USART_USING2, USART_FLAG_TXE) != RESET)
	{
		ENTER_SAFE_ATOM_CODE(bintstate);
		if(app_comtx_urx.length){
			dat = app_comtx_urx.buffer[app_comtx_urx.ptr_out];
			app_comtx_urx.length--;
			if(++app_comtx_urx.ptr_out >= APP_RX_DATA_SIZE){
				app_comtx_urx.ptr_out = 0;
			}
			USART_SendData(USART_USING2, dat);
		}else{
			USART_ITConfig(USART_USING2, USART_IT_TXE, DISABLE);
		}
		EXIT_SAFE_ATOM_CODE(bintstate);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
