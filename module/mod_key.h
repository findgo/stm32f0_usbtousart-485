/**
  ******************************************************************************
  * @file   mod_key.h
  * @author  
  * @version 
  * @date    
  * @brief    key模块
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		重构 		已测试
  ******************************************************************************
  */
#ifndef __MOD_KEY_H_
#define __MOD_KEY_H_

#include "app_cfg.h"

// 配置支持短按 长按 连击和松键
#define KEY_PRESSED_EN			(1)
#define KEY_LONGPRESSED_EN		(1)
#define KEY_REPEATPRESSED_EN	(0)
#define KEY_UP_EN				(0)

//用于定义按键滤波，长击，连击时间  根据你key_decetor_task 调用的周期计算
#define KEY_FILTER_TIME     	2
#define KEY_LONG_TIME			80
#define KEY_REPEAT_TIME	        30

//缓冲区大小
#define KEY_QUEUE_MAX_SIZE		5

// you must define like this :  
//key_value_t key_scan_value(void)



/*按键值，用于拓展按键，需要修改*/
typedef enum
{
	KEY_NULL = 0,			/* 0 表示按键事件 */	
	KEY_PRESS_K1,			/* k1按键 */
	KEY_PRESS_K2,			/* k2按键*/
	KEY_PRESS_K3			/* k3按键 */
}key_value_t;

typedef void keycallfunc(key_value_t value);


//每隔一定周期做一次键扫 一般为10ms
void key_decetor_task(void);
void key_deal(void);
/*	------对应功能不使用，声明NULL-----	短按回调函数     			长按回调函数					连击回调函数			松键回调函数*/
void key_register_function(keycallfunc pshortprecal, keycallfunc plongprecal, keycallfunc preprecal, keycallfunc puprecal);

#endif

