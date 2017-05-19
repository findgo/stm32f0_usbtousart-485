






#include "mod_coils.h"

/* ���ڼ�¼coil��ǰ�Ĺ���״̬ */
static uint8_t PreCoilOnOffStatus = 0x00;

static void HalCoilsOnOff(uint8_t Coils, Hal_Coils_Mode mode);

/***********************************************************
* ��������: 	HalCoilsInit(void)
* ��������: 	�ⲿ���ã���ʼ��
* ��ڲ���: 	None
* ���ڲ���: 	��		
* ���ز���: 	��
* ��������:	Coils ����λ����
* �� ע: 
************************************************************/
void HalCoilsInit(void)
{
	HalCoilsOnOff(HAL_COILS_ALL, HAL_COILS_MODE_OFF);
	PreCoilOnOffStatus = 0x00;
}


/***********************************************************
* ��������: 	HalCoilsSet
* ��������: 	�ⲿ���ã�����coilsģʽ,λ���뿪��
* ��ڲ���: 	Coils--�ĸ�coil,  mode  ģʽ
* ���ڲ���: 	��		
* ���ز���: 	��
* ��������:	Coils ����λ����
* �� ע: 
************************************************************/
void HalCoilsSet (uint8_t Coils, Hal_Coils_Mode mode)
{
	uint8_t Coil;
	Hal_Coils_Mode uSMode;
	
	Coil  = HAL_COILS_1;		//�ӵ�һ���ƿ�ʼ��
	Coils &= HAL_COILS_ALL;	  	//���β��������ڵ�coils
	
	while (Coils)  //�����ҳ���Ҫ���õ�coils
	{
	  if (Coils & Coil)
	  {
		if (mode != HAL_COILS_MODE_TOGGLE)//���л�ģʽ
		{
		    if(PreCoilOnOffStatus & Coil)//�鿴��ǰcoil״̬
            {//on,check it want to set off?
              if(mode == HAL_COILS_MODE_OFF)
              {
   			    HalCoilsOnOff(Coil, HAL_COILS_MODE_OFF); //����
   			    PreCoilOnOffStatus &= ~Coil;//save current mode
              }
            }
			else
			{//off,check it want to set on?
			  if(mode == HAL_COILS_MODE_ON)
              {
   			    HalCoilsOnOff(Coil, HAL_COILS_MODE_ON); //����
   			    PreCoilOnOffStatus |= Coil;
              }
			}
		}
		else		   /* Toggle mode*///���л�ģʽ
		{
			if(PreCoilOnOffStatus & Coil)//�鿴��ǰcoil״̬
			{//on, Toggle to off
			    PreCoilOnOffStatus &= ~Coil;
				uSMode = HAL_COILS_MODE_OFF;  /* Toggle */
			}
		    else
		    {//off, Toggle to on
		        PreCoilOnOffStatus |= Coil;
		    	uSMode = HAL_COILS_MODE_ON;
		    }
			HalCoilsOnOff(Coil, uSMode);	  //����
		}
		Coils ^= Coil;//���������������,�����Ӧλ����
	  }
	  Coil <<= 1;  //��һ��coils
	}
}
/***********************************************************
* ��������: 	HalCoilsGetStasus(void)
* ��������: 	�ⲿ���ã�������coilsģʽ,λ���뿪��
* ��ڲ���: 	Coils--�ĸ�coil, 
* ���ڲ���: 	��		
* ���ز���: 	��
* ��������:	Coils ����λ����
* �� ע: 
************************************************************/
uint8_t HalCoilsGetStasus(uint8_t Coils)
{
	return (PreCoilOnOffStatus & Coils);
}

/**
  * @brief		Coils���ؿ���
  * @param	Coils --�ĸ�coils, mode---ģʽ
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

