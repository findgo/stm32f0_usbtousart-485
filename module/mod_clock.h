/**
  ******************************************************************************
  * @file   mod_clock.c
  * @author  
  * @version 
  * @date    
  * @brief   系统时基内置
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		重构		已测试 20151112
  ******************************************************************************
  */
#ifndef __MOD_CLOCK_H_
#define __MOD_CLOCK_H_

#include "app_cfg.h"

#define CLOCKTICK_RATE_HZ		1000   // 1000hz
#define CLOCKTICK_RATE_MS	(1000 / CLOCKTICK_RATE_HZ)

uint32_t mcu_get_clocktime(void);
uint32_t mcu_elapse_clocktime(void);
void mcu_clocktime_isr_callback(void);

#endif

