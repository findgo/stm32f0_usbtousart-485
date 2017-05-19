/**
  ******************************************************************************
  * @file   util_math.h
  * @author  
  * @version 
  * @date    
  * @brief  	高级数据算法
  ******************************************************************************
  * @attention 	20151110     v1.1   	jgb		重构,已测试	
  ******************************************************************************
  */
  
#include "util_math.h"


/**
  * @brief 求得number 平方根的倒数
  * @param  None
  * @note     
  * @retval None
  */
float invSqrt(float number)
{
	volatile long i;
	volatile float x,y;
	volatile const float threehalfs = 1.5F;

	x = number * 0.5F;
	y = number;
	i = *((long *) &y);
	i =  0x5f375a86 - (i >> 1);  //Lomont
	//i  = 0x5f3759df - ( i >> 1 );  //卡马克 what the fuck?
	y = *((float *) &i);
	y = y * (threehalfs - (x * y * y));

	return y;
}
/**
  * @brief 求得x 平方根
  * @param  None
  * @note     
  * @retval None
  */
float CarmSqrt(float x)
{
	union{
		int intPart;
		float floatPart;
	} convertor;
	union{
		int intPart;
		float floatPart;
	} convertor2;
	
	convertor.floatPart = x;
	convertor2.floatPart = x;
	convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
	convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
	
	return 0.5f * (convertor.floatPart + (x * convertor2.floatPart));
}






