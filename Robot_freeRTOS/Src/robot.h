/*
		TIM2_CH1    PA0   ���
	    TIM2_CH2    PA1   ����
	    TIM2_CH3    PA2   �ҽ�
	    TIM2_CH4    PA3   ����

		����Ƕ���ʱ�估ռ�ձ�
		0.5ms   0��      500
		1.0ms   45��     1000
        1.5ms   90��     1500
        2.0ms   135��    2000
        2.5ms   180��    2500		
		
	  ��20000  20ms
	  1ms   1000
	  
	  ռ�ձ�=�Ƕ�x11.1+501
	  
	  �ҽ�̧�����
		���  79->98̧��
		�ҽ�  79->120���Ƶ�������
	  ������ǰ����
	    ����90->63��ǰת
		
		
	  
	  
	  
1.   angle_to_duty_cycle(unsigned char angle )//�Ƕ�תռ�ձ�
2.   robot_init()                                          //��ʼ����ʹ������PWM


*/

#ifndef _ROBOT_H
#define _ROBOT_H
#include "tim.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
unsigned int angle_to_duty_cycle(unsigned char angle )//�Ƕ�תռ�ձ�
{
	unsigned int Duty_Cycle;//ռ�ձ�
	if(angle<10)
	{
		Duty_Cycle=angle*11.1+500;
	}
	else if(angle>=10&&angle<100)
	{
		Duty_Cycle=angle*11.1+501;
	}
	else
	{
		Duty_Cycle=angle*11.1+502;
	}
	return Duty_Cycle;
}



void robot_init()
{
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);//PWMʹ��
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);//PWMʹ��
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);//PWMʹ��
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);//PWMʹ��
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);//PWMʹ��(��Ļ����)0-1000
	
	HAL_TIM_Base_Start_IT(&htim3);//��ʱ������ʱ�����ж�  ����
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	
	osDelay(500);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(70));//���
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(90));//���
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(79));//�ҽ�
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,angle_to_duty_cycle(95));//�ҿ�
	
	
}
/*
for(unsigned char i=79;i<=98;i++)//�ҽ�̧��
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
	osDelay(50);
}
for(unsigned char i=98;i>=79;i--)//�ҽŷ���
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
	osDelay(50);
}

for(unsigned char i=79;i<=120;i++)//�ҽ�����
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
	osDelay(10);
}
for(unsigned char i=120;i>=79;i--)//�ҽŸ�λ
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
	osDelay(10);
}

for(unsigned char i=90;i>=70;i--)//�����ǰת
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(i));
    osDelay(50);
}
		 




*/








#endif

	
	