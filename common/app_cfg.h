/**
  ******************************************************************************
  * @file   app_cfg.h
  * @author  
  * @version 
  * @date    
  * @brief    ͨ�ÿ�API���ļ�
  ******************************************************************************
  * @attention 	20151110     v1.1   	jgb		�ع�
  ******************************************************************************
  */

#ifndef __APP_CFG_H_
#define __APP_CFG_H_

//!< common
#include "common_global.h"
#include "common_def.h"
#include "common_type.h"
#include "common_bits.h"
#include "common_signal.h"

//!< util
#include "util_bits.h"

//!< ��׼��
#include "stdio.h"


#ifdef DEBUG_MSG
//#define DBG_LOG(fmt,args...) printf(fmt,__VA_ARGS__)
#define DBG_LOG(fmt,...)	printf(fmt,##__VA_ARGS__)//printf(fmt,##args)
#else
//#define DBG_LOG(fmt,args...)
#define DBG_LOG(fmt,args...) 
#endif




#endif
