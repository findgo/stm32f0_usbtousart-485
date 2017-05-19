/**
  ******************************************************************************
  * @file   mod_leds.h
  * @author  
  * @version 
  * @date    
  * @brief    ledsģ��
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		�ع� 	�Ѳ��� 20151112
  ******************************************************************************
  */


#ifndef __MOD_LEDS_H_
#define __MOD_LEDS_H_

#include "app_cfg.h"

#include "hal_led.h"

/* leds - The leds number is the same as the bit position */
#define HAL_LED_1     	((uint8_t)1 << 0)
#define HAL_LED_2    	((uint8_t)1 << 1)
#define HAL_LED_3     	((uint8_t)1 << 2)
#define HAL_LED_4     	((uint8_t)1 << 3)
#define HAL_LED_5     	((uint8_t)1 << 4)
#define HAL_LED_6    	((uint8_t)1 << 5)
#define HAL_LED_7     	((uint8_t)1 << 6)
#define HAL_LED_8     	((uint8_t)1 << 7)

#define HAL_LED_ALL   	( HAL_LED_1 | HAL_LED_2)

/* Defaults */
#define LED_SEQ_DEFAULT_MAX_LEDS		 2	

#define LED_SEQ_ALWAYS_TODO						 0xff
#define LED_SEQ_DEFAULT_BLINK_COUNT      		 1    //!< Ĭ��գ�ִ���
#define LED_SEQ_DEFAULT_BLINK_DUTY_CYCLE 		 5    //!< Ĭ��գ��ռ�ձ�
#define LED_SEQ_DEFAULT_FLASH_QUARTER_DUTY_CYCLE 25   //!< �ٷֶ�ʮ��ռ�ձ�
#define LED_SEQ_DEFAULT_FLASH_HALF_DUTY_CYCLE 	 50   //!< �ٷ���ʮռ�ձ�
#define LED_SEQ_DEFAULT_FLASH_TIME       		 1000 //!< Ĭ�����ڣ�(��ʱ��Ϊ׼)

/* Modes */
typedef enum 
{
	LEDSEQ_MODE_OFF 		= 0x00,
	LEDSEQ_MODE_ON			= 0x01,
	LEDSEQ_MODE_TOGGLE  	= 0x02,
	LEDSEQ_MODE_NONE		= 0x04
}ledseq_mode_t;

/* event */
typedef enum 
{
	LEDSEQ_EVE_STOP = 0,
	LEDSEQ_EVE_BLINK_ONCE = 1,		// գ��һ��
	LEDSEQ_EVE_FLASH_BLINK = 2,		//����գ��
	LEDSEQ_EVE_FLASH_QUARTER = 3,	
	LEDSEQ_EVE_FLASH_HALF = 4,
	LEDSEQ_EVE_MAX = LEDSEQ_EVE_FLASH_HALF + 1
}ledseq_eve_t;

/* �ⲿ���¼����ԵǼ������� */
/*	eve  todo  onpct    period   next	*/
#define LEDSEQ_EVE_EXTERN_LSIT 


#define HAL_TURN_LED1_ON()		st(GPIO_SetBits(HAL_REDLED_PORT, HAL_REDLED_PIN);)
#define HAL_TURN_LED1_OFF()		st(GPIO_ResetBits(HAL_REDLED_PORT, HAL_REDLED_PIN);)
#define HAL_TURN_LED2_ON()	 	st(GPIO_SetBits(HAL_GREENLED_PORT, HAL_GREENLED_PIN);)
#define HAL_TURN_LED2_OFF()	 	st(GPIO_ResetBits(HAL_GREENLED_PORT, HAL_GREENLED_PIN);)
#define HAL_TURN_LED3_ON()		st(;)
#define HAL_TURN_LED3_OFF()		st(;)
#define HAL_TURN_LED4_ON()		st(;)
#define HAL_TURN_LED4_OFF()		st(;)
#define HAL_TURN_LED5_ON()		st(;)
#define HAL_TURN_LED5_OFF()	 	st(;)
#define HAL_TURN_LED6_ON()		st(;) 
#define HAL_TURN_LED6_OFF()	 	st(;)
#define HAL_TURN_LED7_ON()		st(;)
#define HAL_TURN_LED7_OFF()		st(;)
#define HAL_TURN_LED8_ON()		st(;)
#define HAL_TURN_LED8_OFF()		st(;)

void ledseqInit(void);
void ledseqset(uint8_t leds, ledseq_eve_t eve, ledseq_mode_t mode);
void ledsequpdate (uint16_t ElapseTime);

#endif


