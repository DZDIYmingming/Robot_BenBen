/*
		TIM2_CH1    PA0   左脚
	    TIM2_CH2    PA1   左腿
	    TIM2_CH3    PA2   右脚
	    TIM2_CH4    PA3   右腿

		舵机角度与时间及占空比
		0.5ms   0度      500
		1.0ms   45度     1000
        1.5ms   90度     1500
        2.0ms   135度    2000
        2.5ms   180度    2500		
		
	  总20000  20ms
	  1ms   1000
	  
	  占空比=角度x11.1+501
	  
	  右脚抬起过程
		左脚  79->98抬起
		右脚  79->120助推调整重心
	  右腿向前过程
	    左腿90->63向前转
		
		
	  
	  
	  
1.   angle_to_duty_cycle(unsigned char angle )//角度转占空比
2.   robot_init()                                          //初始化，使能所有PWM


*/

#ifndef _ROBOT_H
#define _ROBOT_H
#include "tim.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
unsigned int angle_to_duty_cycle(unsigned char angle )//角度转占空比
{
	unsigned int Duty_Cycle;//占空比
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
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);//PWM使能
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);//PWM使能
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);//PWM使能
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);//PWM使能
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);//PWM使能(屏幕背光)0-1000
	
	HAL_TIM_Base_Start_IT(&htim3);//定时器更新时产生中断  红外
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	
	osDelay(500);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(70));//左脚
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(90));//左跨
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(79));//右脚
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,angle_to_duty_cycle(95));//右跨
	
	
}
/*
for(unsigned char i=79;i<=98;i++)//右脚抬起
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
	osDelay(50);
}
for(unsigned char i=98;i>=79;i--)//右脚放下
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
	osDelay(50);
}

for(unsigned char i=79;i<=120;i++)//右脚助推
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
	osDelay(10);
}
for(unsigned char i=120;i>=79;i--)//右脚复位
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
	osDelay(10);
}

for(unsigned char i=90;i>=70;i--)//左跨向前转
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(i));
    osDelay(50);
}
		 




*/








#endif

	
	