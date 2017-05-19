/**
  ******************************************************************************
  * @file   mod_check.h
  * @author  
  * @version 
  * @date    
  * @brief    checkģ��
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		�ع� �Ѳ��� 20151113
  ******************************************************************************
  */

#ifndef __MOD_CHECK_H_
#define __MOD_CHECK_H_

#include "app_cfg.h"


typedef uint8_t (*IsDownFunc)(void);
typedef void (*CheckCallBackFunc)(void);

typedef struct check_mem_s
{
	uint8_t state;
	uint8_t filtertime;
	uint8_t filtercount;
	IsDownFunc pfuc;
	CheckCallBackFunc pdowncallfuc;
	CheckCallBackFunc pUpcallfuc;
	void *next;	
}cks_mem_t;




void checkregister(cks_mem_t *pcks, uint8_t filtetime, IsDownFunc pfuc, CheckCallBackFunc pdowncallback, CheckCallBackFunc pUpcallfuc);
void checkupdate(uint16_t ElapseTime);
#endif


