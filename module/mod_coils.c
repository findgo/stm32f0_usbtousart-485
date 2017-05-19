






#include "mod_coils.h"

/* 用于记录coil当前的工作状态 */
static uint8_t PreCoilOnOffStatus = 0x00;

static void HalCoilsOnOff(uint8_t Coils, Hal_Coils_Mode mode);

/***********************************************************
* 函数名称: 	HalCoilsInit(void)
* 函数功能: 	外部调用，初始化
* 入口参数: 	None
* 出口参数: 	无		
* 返回参数: 	无
* 功能描述:	Coils 采用位掩码
* 备 注: 
************************************************************/
void HalCoilsInit(void)
{
	HalCoilsOnOff(HAL_COILS_ALL, HAL_COILS_MODE_OFF);
	PreCoilOnOffStatus = 0x00;
}


/***********************************************************
* 函数名称: 	HalCoilsSet
* 函数功能: 	外部调用，设置coils模式,位掩码开发
* 入口参数: 	Coils--哪个coil,  mode  模式
* 出口参数: 	无		
* 返回参数: 	无
* 功能描述:	Coils 采用位掩码
* 备 注: 
************************************************************/
void HalCoilsSet (uint8_t Coils, Hal_Coils_Mode mode)
{
	uint8_t Coil;
	Hal_Coils_Mode uSMode;
	
	Coil  = HAL_COILS_1;		//从第一个灯开始查
	Coils &= HAL_COILS_ALL;	  	//屏蔽不在驱动内的coils
	
	while (Coils)  //遍历找出想要设置的coils
	{
	  if (Coils & Coil)
	  {
		if (mode != HAL_COILS_MODE_TOGGLE)//非切换模式
		{
		    if(PreCoilOnOffStatus & Coil)//查看当前coil状态
            {//on,check it want to set off?
              if(mode == HAL_COILS_MODE_OFF)
              {
   			    HalCoilsOnOff(Coil, HAL_COILS_MODE_OFF); //动作
   			    PreCoilOnOffStatus &= ~Coil;//save current mode
              }
            }
			else
			{//off,check it want to set on?
			  if(mode == HAL_COILS_MODE_ON)
              {
   			    HalCoilsOnOff(Coil, HAL_COILS_MODE_ON); //动作
   			    PreCoilOnOffStatus |= Coil;
              }
			}
		}
		else		   /* Toggle mode*///是切换模式
		{
			if(PreCoilOnOffStatus & Coil)//查看当前coil状态
			{//on, Toggle to off
			    PreCoilOnOffStatus &= ~Coil;
				uSMode = HAL_COILS_MODE_OFF;  /* Toggle */
			}
		    else
		    {//off, Toggle to on
		        PreCoilOnOffStatus |= Coil;
		    	uSMode = HAL_COILS_MODE_ON;
		    }
			HalCoilsOnOff(Coil, uSMode);	  //动作
		}
		Coils ^= Coil;//这个参数已设置完,清除对应位掩码
	  }
	  Coil <<= 1;  //下一个coils
	}
}
/***********************************************************
* 函数名称: 	HalCoilsGetStasus(void)
* 函数功能: 	外部调用，获得这个coils模式,位掩码开发
* 入口参数: 	Coils--哪个coil, 
* 出口参数: 	无		
* 返回参数: 	无
* 功能描述:	Coils 采用位掩码
* 备 注: 
************************************************************/
uint8_t HalCoilsGetStasus(uint8_t Coils)
{
	return (PreCoilOnOffStatus & Coils);
}

/**
  * @brief		Coils开关控制
  * @param	Coils --哪个coils, mode---模式
  * @note	
  * @note	 
  * @note	
  * @retval None
  */
static void HalCoilsOnOff(uint8_t Coils, Hal_Coils_Mode mode)
{
	if (Coils & HAL_COILS_1)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS1_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS1_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_2)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS2_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS2_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_3)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS3_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS3_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_4)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS4_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS4_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_5)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS5_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS5_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_6)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS6_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS6_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_7)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS7_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS7_OFF();
	  }
	}
	
	if (Coils & HAL_COILS_8)
	{
	  if (mode == HAL_COILS_MODE_ON)
	  {
	  	HAL_TURN_COILS8_ON();
	  }
	  else
	  {
	  	HAL_TURN_COILS8_OFF();
	  }
	}
}

