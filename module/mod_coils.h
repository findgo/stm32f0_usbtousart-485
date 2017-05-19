/**
  ******************************************************************************
  * @file   mod_coils.h
  * @author  
  * @version 
  * @date    
  * @brief    coils模块，只有开和关两状态都可以归于这一类别
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		重构			已测试20151113
  ******************************************************************************
  */
#ifndef __MOD_COILS_H_
#define __MOD_COILS_H_

#include "app_cfg.h"
/* COLIS - The COLIS number is the same as the bit position */
#define HAL_COILS_1     	((uint8_t)1 << 0)
#define HAL_COILS_2    		((uint8_t)1 << 1)
#define HAL_COILS_3     	((uint8_t)1 << 2)
#define HAL_COILS_4     	((uint8_t)1 << 3)
#define HAL_COILS_5     	((uint8_t)1 << 4)
#define HAL_COILS_6    		((uint8_t)1 << 5)
#define HAL_COILS_7     	((uint8_t)1 << 6)
#define HAL_COILS_8     	((uint8_t)1 << 7)

#define HAL_COILS_ALL   	( HAL_COILS_1 )

/* Modes */
typedef enum 
{
	HAL_COILS_MODE_OFF 		= 0x00,
	HAL_COILS_MODE_ON		= 0x01,
	HAL_COILS_MODE_TOGGLE  	= 0x02,
}Hal_Coils_Mode;

#define HAL_TURN_COILS1_ON()	
#define HAL_TURN_COILS1_OFF()	
#define HAL_TURN_COILS2_ON()	 
#define HAL_TURN_COILS2_OFF()	 
#define HAL_TURN_COILS3_ON()
#define HAL_TURN_COILS3_OFF()
#define HAL_TURN_COILS4_ON()
#define HAL_TURN_COILS4_OFF()
#define HAL_TURN_COILS5_ON()	
#define HAL_TURN_COILS5_OFF()	 
#define HAL_TURN_COILS6_ON()	 
#define HAL_TURN_COILS6_OFF()	 
#define HAL_TURN_COILS7_ON()
#define HAL_TURN_COILS7_OFF()
#define HAL_TURN_COILS8_ON()
#define HAL_TURN_COILS8_OFF()


void HalCoilsInit(void);
void HalCoilsSet (uint8_t Coils, Hal_Coils_Mode mode);
uint8_t HalCoilsGetStasus(uint8_t Coils);

#endif

