/**
  ******************************************************************************
  * @file   mod_leds.h
  * @author  
  * @version 
  * @date    
  * @brief    coils模块，只有开和关两状态都可以归于这一类别
  ******************************************************************************
  * @attention 20151110     v1.1   	jgb		重构
  ******************************************************************************
  */
#include "mod_leds.h"


typedef struct ledseq_t{
	uint8_t		eve;
	uint8_t		todo; 	  	/* 眨现次数，如果设置为0xff，表明无限闪烁*/
	uint8_t		onPct;	  	/* 占空比*/
	uint16_t	period;	/*开/关总周期 */	
	uint16_t	next; 	 /* 下一次改变的时间/与流逝时间相关 ，与时基绑定*/
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
  * @brief	初始化
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
	HalLedOnOff( HAL_LED_ALL, false);// 所有灯关
}

/**
  * @brief	
  * @param	leds 哪个灯，位域法，可同时设置几个灯	
  * @note	  	eve 为 LEDSEQ_EVE_STOP，灯状态有mode 决定(当设置 LEDSEQ_MODE_NONE )
  * @note	  	eve 不为 LEDSEQ_EVE_STOP，忽略参数mode，一般设置为 LEDSEQ_MODE_NONE
  * @retval  None
  */

void ledseqset(uint8_t leds, ledseq_eve_t eve, ledseq_mode_t mode)
{
	uint8_t led;
	ledseq_t *sts;

	led = HAL_LED_1;			//从第一个灯开始查
	leds &= HAL_LED_ALL;		//屏蔽不在驱动内的灯
	sts = &ledseqControl[0];
	while (leds)	//遍历找出想要设置的灯
	{
		if (leds & led){
			if (eve == LEDSEQ_EVE_STOP){
				if(mode != LEDSEQ_MODE_TOGGLE){
					if(PreledsOnOffstatus & led){
						if(mode == LEDSEQ_MODE_OFF){
							PreledsOnOffstatus &= ~led;// 保存
							HalLedOnOff (led, false); //动作
						}
					}else{
						if(mode == LEDSEQ_MODE_ON){
							PreledsOnOffstatus |= led;//保存
							HalLedOnOff (led, true); //动作
						}
					}
				 }else{
				 	PreledsOnOffstatus ^= led; //取反
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
			leds ^= led;//这个参数已设置完,清除对应位掩码
		}
		led <<= 1;	//下一个灯
		sts++;		//下一个灯
	}
}


/**
  * @brief	led服务程序，大循环调用
  * @param	ElapseTime :流逝时间 (以时基为准)
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
	sts = &ledseqControl[0];//前端初始化
	while (leds)//遍历所有灯
	{
		if (leds & led){
	  		if(sts->eve != LEDSEQ_EVE_STOP){
	  			if(ElapseTime >= sts->next){//超时
	  				if(PreledsOnOffstatus & led){
	  					pct = 100 - sts->onPct;			/* Percentage of cycle for off */
						PreledsOnOffstatus &= ~led;		/* Say it's off */
						HalLedOnOff(led, false);		/* Turn it off */
						
						if (sts->todo != LED_SEQ_ALWAYS_TODO){	//不是无限闪烁，减少闪次数
				            sts->todo--;                        /* Not continuous, reduce count */
				        }
	  				}else{
	  					pct = sts->onPct;           /* Percentage of cycle for on */
          				PreledsOnOffstatus |= led;  /* Say it's on */
						HalLedOnOff (led, true);    /* Turn it on */
	  				}
					
					if( !sts->todo){//次数到
	  					sts->eve = LEDSEQ_EVE_STOP; //转到停止
	  				}
					sts->next = (uint16_t)(((uint32_t)pct * (uint32_t)sts->period) / 100);//取出下一次时间
	  			}else{//未超时，减去流逝时间
	  				sts->next -= ElapseTime;
	  			}
	  		}
			leds ^= led;//已处理完，对应位清零
		}
		led <<= 1;	//下一个灯
		sts++;		//下一个灯
	}
}

/**
  * @brief	leds开关控制 抽象低层
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




