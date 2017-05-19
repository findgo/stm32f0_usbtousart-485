/**
  ******************************************************************************
  * @file   mod_key.h
  * @author  
  * @version 
  * @date    
  * @brief    keyģ��
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		�ع� 		�Ѳ���
  ******************************************************************************
  */
#ifndef __MOD_KEY_H_
#define __MOD_KEY_H_

#include "app_cfg.h"

// ����֧�ֶ̰� ���� �������ɼ�
#define KEY_PRESSED_EN			(1)
#define KEY_LONGPRESSED_EN		(1)
#define KEY_REPEATPRESSED_EN	(0)
#define KEY_UP_EN				(0)

//���ڶ��尴���˲�������������ʱ��  ������key_decetor_task ���õ����ڼ���
#define KEY_FILTER_TIME     	2
#define KEY_LONG_TIME			80
#define KEY_REPEAT_TIME	        30

//��������С
#define KEY_QUEUE_MAX_SIZE		5

// you must define like this :  
//key_value_t key_scan_value(void)



/*����ֵ��������չ��������Ҫ�޸�*/
typedef enum
{
	KEY_NULL = 0,			/* 0 ��ʾ�����¼� */	
	KEY_PRESS_K1,			/* k1���� */
	KEY_PRESS_K2,			/* k2����*/
	KEY_PRESS_K3			/* k3���� */
}key_value_t;

typedef void keycallfunc(key_value_t value);


//ÿ��һ��������һ�μ�ɨ һ��Ϊ10ms
void key_decetor_task(void);
void key_deal(void);
/*	------��Ӧ���ܲ�ʹ�ã�����NULL-----	�̰��ص�����     			�����ص�����					�����ص�����			�ɼ��ص�����*/
void key_register_function(keycallfunc pshortprecal, keycallfunc plongprecal, keycallfunc preprecal, keycallfunc puprecal);

#endif

