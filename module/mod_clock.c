/**
  ******************************************************************************
  * @file   mod_clock.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#include "mod_clock.h"


static volatile uint32_t Ms_ClockTimerCounter = 0; //time count

/**
  * @brief  获得Ms_ClockTimerCounter的值，
  * @param  None
  * @note   Ms_ClockTimerCounter为中断时基调用 
  * @retval Ms_ClockTimerCounter 当前值
  */
uint32_t mcu_get_clocktime(void)
{
	//halIntState_t intState;
	//uint32_t tmp;

	//ENTER_SAFE_ATOM_CODE(intState);
    //tmp = Ms_ClockTimerCounter;//获得系统时间
	//EXIT_SAFE_ATOM_CODE(intState);
	//return tmp;
	return Ms_ClockTimerCounter;
}
/**
  * @brief	获得流逝时间
  * @param	None
  * @note		每走到这里一次，流逝时间都不会超2^16个时基
  * @retval  流逝时间 最大2^32
  */
uint32_t mcu_elapse_clocktime(void)
{
	static volatile uint32_t wHisTimerCounter = 0;
    //halIntState_t intState;
	uint32_t elapsedMSec = 0;
	uint32_t tmp;
	
	//ENTER_SAFE_ATOM_CODE(intState);
    tmp = Ms_ClockTimerCounter;//获得系统时间
	//EXIT_SAFE_ATOM_CODE(intState);

	if(tmp != wHisTimerCounter){
		elapsedMSec = (uint32_t)((tmp - wHisTimerCounter)&0xffffffffu);
		wHisTimerCounter = tmp;
	}

	return elapsedMSec;
}

/**
  * @brief	用于系统时间，中断调用
  * @param	None
  * @note	
  * @retval  None
  */
void mcu_clocktime_isr_callback(void)
{
        Ms_ClockTimerCounter++;
}

