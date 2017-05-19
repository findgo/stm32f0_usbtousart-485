
/**
  ******************************************************************************
  * @file   mod_key.c
  * @author  
  * @version 
  * @date    
  * @brief    key模块
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		重构 	
  ******************************************************************************
  */
#include "mod_key.h"

//按键状态
typedef enum{
	KEY_NONE_PRESSED = 0,
	KEY_PRESSED,			        //!< key pressed
	KEY_LONG_PRESSED,               //!< key long pressed
	KEY_REPEAT_PRESSED,             //!< key repeat 
	KEY_UP							//!< key UP
}key_status_t;

typedef struct{
	key_value_t 	chKeyValue;   //!< key value and the value 0 is reserve
	key_status_t 	tStatus;      //!< key event type
}key_event_t;


typedef struct{																		
	uint8_t		tHead;
	uint8_t		tTail;
	uint8_t		tCounter;
	uint8_t		tSize;
	key_event_t *ptBuffer;
}key_fifo_t;


//定义缓冲区
static key_event_t s_tKeyDetectorBuffer[KEY_QUEUE_MAX_SIZE];
//定义一个对象实列
static key_fifo_t s_tKeyDetectorQueue = {0, 0, 0, KEY_QUEUE_MAX_SIZE, s_tKeyDetectorBuffer};
#if (KEY_PRESSED_EN == 1)
static keycallfunc *keyshortcallcall = NULL;
#endif
#if (KEY_LONGPRESSED_EN == 1)
static keycallfunc *keylongcallcall = NULL;
#endif
#if (KEY_REPEATPRESSED_EN == 1)
static keycallfunc *keyrepeatcallcall = NULL;
#endif
#if (KEY_UP_EN == 1)
static keycallfunc *keyupcallcall = NULL;
#endif

extern key_value_t key_scan_value(void);

/**
  * @brief	参数入队功能
  * @param	tObj 按键结构指针
  * @note	
  * @retval true or false
  * @note 
  */
static bool key_put(key_event_t *tObj)	
{
	key_fifo_t *ptQ = (key_fifo_t *)&s_tKeyDetectorQueue;

	if (ptQ->tCounter != ptQ->tSize) {
		ptQ->ptBuffer[ptQ->tTail] = *tObj;
		if (++ptQ->tTail >= ptQ->tSize){
			ptQ->tTail = 0;
		}		
		ptQ->tCounter++;
		return true;
	}
	
	return false;
}
/**
  * @brief	参数出队功能
  * @param	ptKey 按键指针
  * @note	
  * @retval true or false
  * @note 
  */
static bool key_pop(key_event_t *ptObj)
{ 
	key_fifo_t *ptQ = (key_fifo_t *)&s_tKeyDetectorQueue;

	if(ptQ->tCounter != 0) {
		*ptObj = ptQ->ptBuffer[ptQ->tHead];
		if(++ptQ->tHead >= ptQ->tSize){	
			ptQ->tHead = 0; 
		}		
		ptQ->tCounter--;
		return true;
	} 
	
	return false;
}

/**
  * @brief	按键扫描滤波
  * @param	keyevent 键事件结构指针
  * @note	
  * @retval 	none
  * @note		采用扫描式滤波,松键和按键都有滤波
  */
static void key_check(key_event_t *keyevent)
{
	static uint16_t s_hwCount			 = 0;				//计数值
	static key_value_t s_chHisKeyValue   = KEY_NULL;		//历史键值
	static key_value_t s_chLastKeyValue  = KEY_NULL;		//历史键值
	key_value_t s_chNowKeyValue  		 = KEY_NULL;		//当前键值
	
	s_chNowKeyValue = key_scan_value(); 		//获得新值
	if (s_chNowKeyValue == s_chHisKeyValue){ 	//键值比较 //与历史键值一致
		if (++s_hwCount  >= KEY_FILTER_TIME){
			if(s_chLastKeyValue != s_chNowKeyValue){				//与滤波后历史键值比较
				if (KEY_NULL != s_chLastKeyValue) {
					keyevent->tStatus = KEY_UP;						//表明有键抬起
					keyevent->chKeyValue = s_chLastKeyValue;		//键值
				}

				if (KEY_NULL != s_chNowKeyValue){
					keyevent->tStatus = KEY_PRESSED;				//表明有键按下
					keyevent->chKeyValue = s_chNowKeyValue;			//键值
				}
				s_chLastKeyValue = s_chNowKeyValue;				//存历史键值
			}
			s_hwCount = 0;
		}
	} else {			//当进行跳变时，进行跳变滤波
		s_chHisKeyValue = s_chNowKeyValue;		//现键赋给历史键值
		s_hwCount 		= 0;
	}
}
/**
  * @brief	 按键分析程序，短击，长击，连击
  * @param	none
  * @note	
  * @retval  none
  * @note 
  */
#define KEY_DECETOR_CHECK_START 		0
#define KEY_DECETOR_CHECK_DOWN			1
#define KEY_DECETOR_LONG_PRESS			2   
#define KEY_DECETOR_REPEAT_PRESS		3 
#define KEY_DECETOR_IS_UP	    		4
#define KEY_DECETOR_RESET()				do{s_u8KeyState = KEY_DECETOR_CHECK_START;}while(0)

void key_decetor_task(void)
{
	static uint8_t s_u8KeyState    = KEY_DECETOR_CHECK_START;	//初始状态为0
	static key_event_t s_u8LastKey = { KEY_NULL, KEY_NONE_PRESSED};
	static key_event_t s_u8NowKey  = { KEY_NULL, KEY_NONE_PRESSED};
	static uint16_t s_u8KeyTimeCount;//计时器

	//这里的键值是安全的
	key_check(&s_u8NowKey);//只有在跳变时，才改变值 

    switch(s_u8KeyState){
		case KEY_DECETOR_CHECK_START:
			//break;
			
		case KEY_DECETOR_CHECK_DOWN:	
			if(KEY_PRESSED == s_u8NowKey.tStatus){		//的确有健按下
				s_u8LastKey.chKeyValue = s_u8NowKey.chKeyValue;
				s_u8KeyTimeCount = 0;//计时器
				s_u8KeyState = KEY_DECETOR_LONG_PRESS;
			}else {
				KEY_DECETOR_RESET();//无效健值
			}
			break;
			
		case KEY_DECETOR_LONG_PRESS:
			if(s_u8LastKey.chKeyValue != s_u8NowKey.chKeyValue){ //一发现键值不一致，说明有乱键，但仍可以处理
#if (KEY_PRESSED_EN == 1)
				s_u8LastKey.tStatus = KEY_PRESSED;
				key_put(&s_u8LastKey);		//短按有效健值入队
#endif
				s_u8KeyState = KEY_DECETOR_IS_UP;
				return;
			}
			
			if(KEY_UP == s_u8NowKey.tStatus){ //发现键抬起---短按
#if (KEY_PRESSED_EN == 1)
				s_u8LastKey.tStatus = KEY_PRESSED;
				key_put(&s_u8LastKey);		//短按有效健值入队
#endif
				s_u8KeyState = KEY_DECETOR_IS_UP;
			}else{
#if ((KEY_LONGPRESSED_EN == 1) || (KEY_REPEATPRESSED_EN == 1))
				if(++s_u8KeyTimeCount >= KEY_LONG_TIME ){
#if (KEY_LONGPRESSED_EN == 1)
					s_u8LastKey.tStatus = KEY_LONG_PRESSED;
					key_put(&s_u8LastKey);	//长按有效健值入队
#endif
					s_u8KeyTimeCount = 0;
					s_u8KeyState = KEY_DECETOR_REPEAT_PRESS;//准备进入连击
				}
#endif
			}
			break;
		case KEY_DECETOR_REPEAT_PRESS:
			if(s_u8LastKey.chKeyValue != s_u8NowKey.chKeyValue){ //一发现键值不一致，说明有乱键，但仍可以处理
				s_u8KeyState = KEY_DECETOR_IS_UP;
				return;
			}
			
			if(KEY_UP == s_u8NowKey.tStatus){//发现键抬起
				s_u8KeyState = KEY_DECETOR_IS_UP;
#if (KEY_REPEATPRESSED_EN == 1)
			}else {	

				if(++s_u8KeyTimeCount >= KEY_REPEAT_TIME ){
					s_u8LastKey.tStatus = KEY_REPEAT_PRESSED;
					key_put(&s_u8LastKey);	//连击有效入队
					s_u8KeyTimeCount = 0;
				}
#endif
			}
			break;
			
		case KEY_DECETOR_IS_UP: 		//抬键处理
#if (KEY_UP_EN == 1)
			s_u8LastKey.tStatus = KEY_UP;
			key_put(&s_u8LastKey);					//键抬起有效入队
#endif
			//一波结束
			s_u8LastKey.chKeyValue  = KEY_NULL;
			s_u8LastKey.tStatus		= KEY_NONE_PRESSED; 
			s_u8NowKey.chKeyValue   = KEY_NULL;
			s_u8NowKey.tStatus		= KEY_NONE_PRESSED; 
			KEY_DECETOR_RESET();//抬健回初始状态
			break;
			
			default:
				break;
	}
}

/**
  * @brief	键处理函数
  * @param	
  * @note	
  * @retval  none
  * @note 
  */
void key_deal(void)
{
	key_event_t keyvalue;
	
	if(key_pop(&keyvalue)){
		switch(keyvalue.tStatus){
#if (KEY_PRESSED_EN == 1)
			case KEY_PRESSED:
				if(keyshortcallcall)
					keyshortcallcall(keyvalue.chKeyValue);
				break;
#endif
#if (KEY_LONGPRESSED_EN == 1)				
			case KEY_LONG_PRESSED:
				if(keylongcallcall)
					keylongcallcall(keyvalue.chKeyValue);
				break;
#endif

#if (KEY_REPEATPRESSED_EN == 1)
			case KEY_REPEAT_PRESSED:
				if(keyrepeatcallcall)
					keyrepeatcallcall(keyvalue.chKeyValue);
				break;
#endif
#if (KEY_UP_EN == 1)
			case KEY_UP:
				if(keyupcallcall)
					keyupcallcall(keyvalue.chKeyValue);
				break;
#endif				
			default:
				break;
		}
	}
}


void key_register_function(keycallfunc pshortprecal, keycallfunc plongprecal, keycallfunc preprecal, keycallfunc puprecal)
{
#if (KEY_PRESSED_EN == 1)
	keyshortcallcall = pshortprecal;
#endif
#if (KEY_LONGPRESSED_EN == 1)
	keylongcallcall = plongprecal;
#endif
#if (KEY_REPEATPRESSED_EN == 1)
	keyrepeatcallcall = preprecal;
#endif
#if (KEY_UP_EN == 1)
	keyupcallcall = puprecal;
#endif
}

