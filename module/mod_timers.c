/**
  ******************************************************************************
  * @file   mod_timers.h
  * @author  
  * @version 
  * @date    
  * @brief    软定时器
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		重构
  ******************************************************************************
  */
#include "mod_timers.h"


/**
  * @brief  设定周期时间
  * @param  t 时间结构体 period 时间周期
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_period_set( esoft_timer_t *t, uint32_t period)

{
	t->period = period;
	t->start = mcu_get_clocktime();//获得系统时间
}
/**
  * @brief  超时之后用于复位
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_period_reset( esoft_timer_t *t)
{
	t->start += t->period;
}
/**
  * @brief  重新启动
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_period_start( esoft_timer_t *t)
{
	t->start = mcu_get_clocktime();//获得系统时间
}

/**
  * @brief  超时
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
bool timer_period_expired( esoft_timer_t *t)
{
	uint32_t now = mcu_get_clocktime();

	
	//return (bool)(now - t->start >= t->period);
	return (bool)timer_after_eq(now, t->start + t->period);
}





/**
  * @brief  超时之后用于复位
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_interval_reset( isoft_timer_t *t)
{
	t->start = mcu_get_clocktime();
}
/**
  * @brief  启动定时器
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_interval_start( isoft_timer_t *t)
{
	t->start = mcu_get_clocktime();
}
/**
  * @brief interval超时
  * @note   
  * @note    
  * @note   
  * @retval 
  */
bool timer_interval_expired( isoft_timer_t *t, uint32_t interval)
{
	uint32_t now = mcu_get_clocktime();
	
	return (bool)timer_after_eq(now, t->start + interval);
}

/**
  * @brief milliseconds超时
  * @note   
  * @note    
  * @note   
  * @retval 
  */
bool timer_milliseconds_expired( isoft_timer_t *t, uint32_t milliseconds)
{
	uint32_t now = mcu_get_clocktime();
	
	return (bool)timer_after_eq(now, t->start + milliseconds / CLOCKTICK_RATE_MS);
}
/**
  * @brief seconds超时
  * @note   
  * @note    
  * @note   
  * @retval 
  */
bool timer_seconds_expired( isoft_timer_t *t, uint32_t seconds)
{
	uint32_t interval = (seconds * 1000L) / CLOCKTICK_RATE_MS;
	uint32_t now = mcu_get_clocktime();
	
	return (bool)timer_after_eq(now, t->start + interval);
}
/**
  * @brief minutes超时
  * @note   
  * @note    
  * @note   
  * @retval 
  */
bool timer_minutes_expired( isoft_timer_t *t, uint32_t minutes)
{
	uint32_t interval = (minutes * 60L * 1000L) / CLOCKTICK_RATE_MS;
	uint32_t now = mcu_get_clocktime();
	
	return (bool)timer_after_eq(now, t->start + interval);
}




