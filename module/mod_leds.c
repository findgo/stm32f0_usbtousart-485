/**
  ******************************************************************************
  * @file   mod_leds.h
  * @author  
  * @version 
  * @date    
  * @brief    coilsģ�飬ֻ�п��͹���״̬�����Թ�����һ���
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		�ع�
  ******************************************************************************
  */
#include "mod_leds.h"


typedef struct ledseq_t{
	uint8_t		eve;
	uint8_t		todo; 	  	/* գ�ִ������������Ϊ0xff������������˸*/
	uint8_t		onPct;	  	/* ռ�ձ�*/
	uint16_t	period;	/*��/�������� */	
	uint16_t	next; 	 /* ��һ�θı��ʱ��/������ʱ����� ����ʱ����*/
}ledseq_t;

static uint8_t PreledsOnOffstatus;
static ledseq_t ledseqControl[LED_SEQ_DEFAULT_MAX_LEDS];

const ledseq_t ledseqeveList[LEDSEQ_EVE_MAX]= {
	{LEDSEQ_EVE_STOP, 0, 0, 0, 0},
	{LEDSEQ_EVE_BLINK_ONCE, 	1, 					 LED_SEQ_DEFAULT_BLINK_DUTY_CYCLE, 		   LED_SEQ_DEFAULT_FLASH_TIME, 0},
	{LEDSEQ_EVE_FLASH_BLINK, 	LED_SEQ_ALWAYS_TODO, LED_SEQ_DEFAULT_BLINK_DUTY_CYCLE, 		   LED_SEQ_DEFAULT_FLASH_TIME, 0},
	{LEDSEQ_EVE_FLASH_QUARTER,	LED_SEQ_ALWAYS_TODO, LED_SEQ_DEFAULT_FLASH_QUARTER_DUTY_CYCLE, LED_SEQ_DEFAULT_FLASH_TIME, 0},
	{LEDSEQ_EVE_FLASH_HALF, 	LED_SEQ_ALWAYS_TODO, LED_SEQ_DEFAULT_FLASH_HALF_DUTY_CYCLE,	   LED_SEQ_DEFAULT_FLASH_TIME, 0},
	LEDSEQ_EVE_EXTERN_LSIT
};

static void HalLedOnOff (uint8_t leds, bool val);

/**
  * @brief	��ʼ��
  * @param	None
  * @note	
  * @retval  None
  */
void ledseqInit(void)
{
	uint8_t i;
	
	for(i = 0; i < LED_SEQ_DEFAULT_MAX_LEDS; i++){
		ledseqControl[i].eve = LEDSEQ_EVE_STOP;
		ledseqControl[i].onPct = 0;
		ledseqControl[i].todo = 0;
		ledseqControl[i].period = LED_SEQ_DEFAULT_FLASH_TIME;
		ledseqControl[i].next = 0;
	}
	PreledsOnOffstatus = 0x00;
	HalLedOnOff( HAL_LED_ALL, false);// ���еƹ�
}

/**
  * @brief	
  * @param	leds �ĸ��ƣ�λ�򷨣���ͬʱ���ü�����	
  * @note	  	eve Ϊ LEDSEQ_EVE_STOP����״̬��mode ����(������ LEDSEQ_MODE_NONE )
  * @note	  	eve ��Ϊ LEDSEQ_EVE_STOP�����Բ���mode��һ������Ϊ LEDSEQ_MODE_NONE
  * @retval  None
  */

void ledseqset(uint8_t leds, ledseq_eve_t eve, ledseq_mode_t mode)
{
	uint8_t led;
	ledseq_t *sts;

	led = HAL_LED_1;			//�ӵ�һ���ƿ�ʼ��
	leds &= HAL_LED_ALL;		//���β��������ڵĵ�
	sts = &ledseqControl[0];
	while (leds)	//�����ҳ���Ҫ���õĵ�
	{
		if (leds & led){
			if (eve == LEDSEQ_EVE_STOP){
				if(mode != LEDSEQ_MODE_TOGGLE){
					if(PreledsOnOffstatus & led){
						if(mode == LEDSEQ_MODE_OFF){
							PreledsOnOffstatus &= ~led;// ����
							HalLedOnOff (led, false); //����
						}
					}else{
						if(mode == LEDSEQ_MODE_ON){
							PreledsOnOffstatus |= led;//����
							HalLedOnOff (led, true); //����
						}
					}
				 }else{
				 	PreledsOnOffstatus ^= led; //ȡ��
				 	HalLedOnOff(led, PreledsOnOffstatus & led);// do it
				 }
				 sts->eve = LEDSEQ_EVE_STOP;
			}else{
				if(eve != sts->eve){//diferent from current
					sts->eve = eve;
					sts->onPct = ledseqeveList[eve].onPct;
					sts->todo = ledseqeveList[eve].todo;
					sts->period = ledseqeveList[eve].period;
					sts->next = ledseqeveList[eve].next;

					PreledsOnOffstatus &= ~led;// do it now
					HalLedOnOff(led, false); 
				}
			}
			leds ^= led;//���������������,�����Ӧλ����
		}
		led <<= 1;	//��һ����
		sts++;		//��һ����
	}
}


/**
  * @brief	led������򣬴�ѭ������
  * @param	ElapseTime :����ʱ�� (��ʱ��Ϊ׼)
  * @note	
  * @retval  None
  */
void ledsequpdate (uint16_t ElapseTime)
{
	uint8_t pct;
	uint8_t led;
	uint8_t leds;
	ledseq_t *sts;

	led  = HAL_LED_1;
	leds = HAL_LED_ALL;
	sts = &ledseqControl[0];//ǰ�˳�ʼ��
	while (leds)//�������е�
	{
		if (leds & led){
	  		if(sts->eve != LEDSEQ_EVE_STOP){
	  			if(ElapseTime >= sts->next){//��ʱ
	  				if(PreledsOnOffstatus & led){
	  					pct = 100 - sts->onPct;			/* Percentage of cycle for off */
						PreledsOnOffstatus &= ~led;		/* Say it's off */
						HalLedOnOff(led, false);		/* Turn it off */
						
						if (sts->todo != LED_SEQ_ALWAYS_TODO){	//����������˸������������
				            sts->todo--;                        /* Not continuous, reduce count */
				        }
	  				}else{
	  					pct = sts->onPct;           /* Percentage of cycle for on */
          				PreledsOnOffstatus |= led;  /* Say it's on */
						HalLedOnOff (led, true);    /* Turn it on */
	  				}
					
					if( !sts->todo){//������
	  					sts->eve = LEDSEQ_EVE_STOP; //ת��ֹͣ
	  				}
					sts->next = (uint16_t)(((uint32_t)pct * (uint32_t)sts->period) / 100);//ȡ����һ��ʱ��
	  			}else{//δ��ʱ����ȥ����ʱ��
	  				sts->next -= ElapseTime;
	  			}
	  		}
			leds ^= led;//�Ѵ����꣬��Ӧλ����
		}
		led <<= 1;	//��һ����
		sts++;		//��һ����
	}
}

/**
  * @brief	leds���ؿ��� ����Ͳ�
  * @param	leds: bitmask ,detaial for HAL_LED_XX ,bool
  * @note	
  * @note	 
  * @note	
  * @retval 
  */
static void HalLedOnOff (uint8_t leds, bool val)
{
	if (leds & HAL_LED_1){
		if(val)
			HAL_TURN_LED1_ON();
		else
			HAL_TURN_LED1_OFF();
	}
	
	if (leds & HAL_LED_2){
		if(val)
			HAL_TURN_LED2_ON();
		else
			HAL_TURN_LED2_OFF();
	}
	
	if (leds & HAL_LED_3){
		if(val)
			HAL_TURN_LED3_ON();
		else
			HAL_TURN_LED3_OFF();
	}
	
	if (leds & HAL_LED_4){
		if(val)
			HAL_TURN_LED4_ON();
		else
			HAL_TURN_LED4_OFF();
	}
	
	if (leds & HAL_LED_5){
		if(val)
			HAL_TURN_LED5_ON();
		else
			HAL_TURN_LED5_OFF();
	}
	
	if (leds & HAL_LED_6){
		if(val)
			HAL_TURN_LED6_ON();
		else
			HAL_TURN_LED6_OFF();
	}
	
	if (leds & HAL_LED_7){
		if(val)
			HAL_TURN_LED7_ON();
		else
			HAL_TURN_LED7_OFF();
	}
	if (leds & HAL_LED_8){
		if(val)
			HAL_TURN_LED8_ON();
		else
			HAL_TURN_LED8_OFF();
	}
}




