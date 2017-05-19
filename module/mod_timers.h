/**
  ******************************************************************************
  * @file   mod_timers.h
  * @author  
  * @version 
  * @date    
  * @brief    软定时器 依赖库 mod_clock.h
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		重构		已测试 20151112
  ******************************************************************************
  */

#ifndef __MOD_TIMERS_H_
#define __MOD_TIMERS_H_

#include "app_cfg.h"
#include "mod_clock.h"



//!< 转换成时钟心跳
#define tm_ms(__x)		((__x) / CLOCKTICK_RATE_MS)
#define tm_s(__x)		tm_ms((__x) * 1000L)
#define tm_min(__x)  	tm_s((__x) * 60L)

typedef struct esoft_timer_s 
{
	uint32_t start;
	uint32_t period;
}esoft_timer_t;

typedef struct isoft_timer_s 
{
	uint32_t start;
}isoft_timer_t;


void timer_period_set( esoft_timer_t *t, uint32_t period);
void timer_period_reset( esoft_timer_t *t);
void timer_period_start( esoft_timer_t *t);
bool timer_period_expired( esoft_timer_t *t);




void timer_interval_reset( isoft_timer_t *t);
void timer_interval_start( isoft_timer_t *t);
bool timer_interval_expired( isoft_timer_t *t, uint32_t interval);
bool timer_milliseconds_expired( isoft_timer_t *t, uint32_t milliseconds);
bool timer_seconds_expired( isoft_timer_t *t, uint32_t seconds);
bool timer_minutes_expired( isoft_timer_t *t, uint32_t minutes);



#endif

