
/**
  ******************************************************************************
  * @file   mod_key.c
  * @author  
  * @version 
  * @date    
  * @brief    keyģ��
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		�ع� 	
  ******************************************************************************
  */
#include "mod_key.h"

//����״̬
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


//���建����
static key_event_t s_tKeyDetectorBuffer[KEY_QUEUE_MAX_SIZE];
//����һ������ʵ��
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
  * @brief	������ӹ���
  * @param	tObj �����ṹָ��
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
  * @brief	�������ӹ���
  * @param	ptKey ����ָ��
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
  * @brief	����ɨ���˲�
  * @param	keyevent ���¼��ṹָ��
  * @note	
  * @retval 	none
  * @note		����ɨ��ʽ�˲�,�ɼ��Ͱ��������˲�
  */
static void key_check(key_event_t *keyevent)
{
	static uint16_t s_hwCount			 = 0;				//����ֵ
	static key_value_t s_chHisKeyValue   = KEY_NULL;		//��ʷ��ֵ
	static key_value_t s_chLastKeyValue  = KEY_NULL;		//��ʷ��ֵ
	key_value_t s_chNowKeyValue  		 = KEY_NULL;		//��ǰ��ֵ
	
	s_chNowKeyValue = key_scan_value(); 		//�����ֵ
	if (s_chNowKeyValue == s_chHisKeyValue){ 	//��ֵ�Ƚ� //����ʷ��ֵһ��
		if (++s_hwCount  >= KEY_FILTER_TIME){
			if(s_chLastKeyValue != s_chNowKeyValue){				//���˲�����ʷ��ֵ�Ƚ�
				if (KEY_NULL != s_chLastKeyValue) {
					keyevent->tStatus = KEY_UP;						//�����м�̧��
					keyevent->chKeyValue = s_chLastKeyValue;		//��ֵ
				}

				if (KEY_NULL != s_chNowKeyValue){
					keyevent->tStatus = KEY_PRESSED;				//�����м�����
					keyevent->chKeyValue = s_chNowKeyValue;			//��ֵ
				}
				s_chLastKeyValue = s_chNowKeyValue;				//����ʷ��ֵ
			}
			s_hwCount = 0;
		}
	} else {			//����������ʱ�����������˲�
		s_chHisKeyValue = s_chNowKeyValue;		//�ּ�������ʷ��ֵ
		s_hwCount 		= 0;
	}
}
/**
  * @brief	 �����������򣬶̻�������������
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
	static uint8_t s_u8KeyState    = KEY_DECETOR_CHECK_START;	//��ʼ״̬Ϊ0
	static key_event_t s_u8LastKey = { KEY_NULL, KEY_NONE_PRESSED};
	static key_event_t s_u8NowKey  = { KEY_NULL, KEY_NONE_PRESSED};
	static uint16_t s_u8KeyTimeCount;//��ʱ��

	//����ļ�ֵ�ǰ�ȫ��
	key_check(&s_u8NowKey);//ֻ��������ʱ���Ÿı�ֵ 

    switch(s_u8KeyState){
		case KEY_DECETOR_CHECK_START:
			//break;
			
		case KEY_DECETOR_CHECK_DOWN:	
			if(KEY_PRESSED == s_u8NowKey.tStatus){		//��ȷ�н�����
				s_u8LastKey.chKeyValue = s_u8NowKey.chKeyValue;
				s_u8KeyTimeCount = 0;//��ʱ��
				s_u8KeyState = KEY_DECETOR_LONG_PRESS;
			}else {
				KEY_DECETOR_RESET();//��Ч��ֵ
			}
			break;
			
		case KEY_DECETOR_LONG_PRESS:
			if(s_u8LastKey.chKeyValue != s_u8NowKey.chKeyValue){ //һ���ּ�ֵ��һ�£�˵�����Ҽ������Կ��Դ���
#if (KEY_PRESSED_EN == 1)
				s_u8LastKey.tStatus = KEY_PRESSED;
				key_put(&s_u8LastKey);		//�̰���Ч��ֵ���
#endif
				s_u8KeyState = KEY_DECETOR_IS_UP;
				return;
			}
			
			if(KEY_UP == s_u8NowKey.tStatus){ //���ּ�̧��---�̰�
#if (KEY_PRESSED_EN == 1)
				s_u8LastKey.tStatus = KEY_PRESSED;
				key_put(&s_u8LastKey);		//�̰���Ч��ֵ���
#endif
				s_u8KeyState = KEY_DECETOR_IS_UP;
			}else{
#if ((KEY_LONGPRESSED_EN == 1) || (KEY_REPEATPRESSED_EN == 1))
				if(++s_u8KeyTimeCount >= KEY_LONG_TIME ){
#if (KEY_LONGPRESSED_EN == 1)
					s_u8LastKey.tStatus = KEY_LONG_PRESSED;
					key_put(&s_u8LastKey);	//������Ч��ֵ���
#endif
					s_u8KeyTimeCount = 0;
					s_u8KeyState = KEY_DECETOR_REPEAT_PRESS;//׼����������
				}
#endif
			}
			break;
		case KEY_DECETOR_REPEAT_PRESS:
			if(s_u8LastKey.chKeyValue != s_u8NowKey.chKeyValue){ //һ���ּ�ֵ��һ�£�˵�����Ҽ������Կ��Դ���
				s_u8KeyState = KEY_DECETOR_IS_UP;
				return;
			}
			
			if(KEY_UP == s_u8NowKey.tStatus){//���ּ�̧��
				s_u8KeyState = KEY_DECETOR_IS_UP;
#if (KEY_REPEATPRESSED_EN == 1)
			}else {	

				if(++s_u8KeyTimeCount >= KEY_REPEAT_TIME ){
					s_u8LastKey.tStatus = KEY_REPEAT_PRESSED;
					key_put(&s_u8LastKey);	//������Ч���
					s_u8KeyTimeCount = 0;
				}
#endif
			}
			break;
			
		case KEY_DECETOR_IS_UP: 		//̧������
#if (KEY_UP_EN == 1)
			s_u8LastKey.tStatus = KEY_UP;
			key_put(&s_u8LastKey);					//��̧����Ч���
#endif
			//һ������
			s_u8LastKey.chKeyValue  = KEY_NULL;
			s_u8LastKey.tStatus		= KEY_NONE_PRESSED; 
			s_u8NowKey.chKeyValue   = KEY_NULL;
			s_u8NowKey.tStatus		= KEY_NONE_PRESSED; 
			KEY_DECETOR_RESET();//̧���س�ʼ״̬
			break;
			
			default:
				break;
	}
}

/**
  * @brief	��������
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

