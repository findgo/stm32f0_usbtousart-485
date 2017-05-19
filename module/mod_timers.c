/**
  ******************************************************************************
  * @file   mod_timers.h
  * @author  
  * @version 
  * @date    
  * @brief    ��ʱ��
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		�ع�
  ******************************************************************************
  */
#include "mod_timers.h"


/**
  * @brief  �趨����ʱ��
  * @param  t ʱ��ṹ�� period ʱ������
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_period_set( esoft_timer_t *t, uint32_t period)

{
	t->period = period;
	t->start = mcu_get_clocktime();//���ϵͳʱ��
}
/**
  * @brief  ��ʱ֮�����ڸ�λ
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
  * @brief  ��������
  * @param  
  * @note   
  * @note    
  * @note   
  * @retval 
  */
void timer_period_start( esoft_timer_t *t)
{
	t->start = mcu_get_clocktime();//���ϵͳʱ��
}

/**
  * @brief  ��ʱ
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
  * @brief  ��ʱ֮�����ڸ�λ
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
  * @brief  ������ʱ��
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
  * @brief interval��ʱ
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
  * @brief milliseconds��ʱ
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
  * @brief seconds��ʱ
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
  * @brief minutes��ʱ
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




