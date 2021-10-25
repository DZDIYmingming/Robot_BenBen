/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "robot.h"
#include "usart.h"
#include "st7565.h"
#include "pic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/*****************************************************************************************任务调度变量**/

unsigned char Task02end=1;  //线程2完成标志位
unsigned char Task03end=1;  //线程3完成标志位
unsigned char Step=0;       //步骤

unsigned char delay_time=25;//舵机间隔时间
unsigned char buff='4';//3动

unsigned int light=900;//背光0-1000

unsigned char image_num=7;//LCD显示的表情  1-9

unsigned char flag=0;

/********************************************************************************************红外*/
uint32_t upcount=0;//定时器更新的

unsigned int value_up=0;//上升沿的值
unsigned int value_down=0;

unsigned char isUpCapt=1;//是不是上升沿

unsigned int width=0;//储存脉宽

unsigned int buffer[48]={0};//红外接收缓冲区
unsigned char buffer_id=0;  //储存位置，从0开始放

unsigned char rcvFlag=0;//接收完标志位


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim3)
	{
		upcount++;
	}
	
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim3)
	{
		if(isUpCapt)//如果是上升沿捕获
	{
	value_up=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//捕获上升沿的值
	isUpCapt=0;//设置为下降沿
	__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);//设置为下降沿捕获
	upcount=0;
	}
	else
	{
		value_down=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//捕获下降沿的值
		isUpCapt=1;//设置为上升沿
		__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);//设置为上升沿捕获
		width=value_down+upcount*65536-value_up;
		
		if(width>4400&&width<4600)//检测是否是同步码，4.5ms
		{
			buffer_id=0;
			buffer[buffer_id]=width;
			buffer_id++;
		}
		else if(buffer_id>0)//如果同步码收到了
		{
			buffer[buffer_id]=width;
			buffer_id++;
			if(buffer_id>32)
			{
				rcvFlag=1;
				buffer_id=0;//地址清零
			}
		}
	}
	}
	
	
}


uint32_t ir_value()
{
	uint32_t RCV_Value=0;//转换完后的
	if(rcvFlag==1)//如果收到了
	 {
		
		 for(unsigned char i=1;i<33;i++)
		 {
			 if(buffer[i]>500&&buffer[i]<700)//如果是低电平
			 {
				 
				 RCV_Value=RCV_Value<<1;
			 }
			 else//否则是高电平
			 {
				 RCV_Value=RCV_Value|0x00000001;
				 RCV_Value=RCV_Value<<1;
			 }
			 
		 }
		 RCV_Value=RCV_Value>>1;
		// printf("%X",RCV_Value);
		 for(unsigned char i=0;i<33;i++)//缓存区重置
		 {
			 buffer[i]=0;
		 }
		 
		 rcvFlag=0;
		 return RCV_Value;
	 }
	 else
	 {
		 return 0;
	 }
	  
	 
	

}
void ir_Handler(uint32_t data)//红外任务
{
	/*
	用户码：00FF
	
	
	1: A25D    2: 629D    3:  E21D    
	
	4: 22DD    5: 02FD    6:  C23D
	
	7: E01F    8: A857    9:  906F
	
 	*: 6897    0: 9867    #:  B04F
	
	          ↑：18E7

    ←：10EF  OK:38C7   →：5AA5
	
              ↓：4AB5	
	
	*/
	if(data==0xFF6897)//按下*开始走
	{
		buff='3';
	}
	if(data==0xFFB04F)//按下#停止
	{
		buff='4';
	}
	
	
	
	if(data==0xFFA25D)
	{
		image_num=1;
		
	}
	if(data==0xFF629D)
	{
		image_num=2;
	}
	if(data==0xFFE21D)
	{
		image_num=3;
	}
	if(data==0xFF22DD)
	{
		image_num=4;
	}
	if(data==0xFF02FD)
	{
		image_num=5;
	}
	if(data==0xFFC23D)
	{
		image_num=6;
	}
	if(data==0xFFE01F)
	{
		image_num=7;
	}
	if(data==0xFFA857)
	{
		image_num=8;
	}
	if(data==0xFF906F)
	{
		image_num=9;
	}
	
	/*HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);*/
	
}



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask05Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityNormal, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, StartTask04, osPriorityNormal, 0, 128);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadDef(myTask05, StartTask05, osPriorityNormal, 0, 128);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)//主线程，分配舵机任务
{
    
    
    
    
    robot_init();
	
	initial_lcd();
	
    

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	   __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,light);
	  
	  if(buff=='3')
	  {
	  
		Step=1;//运行步骤一
		Task02end=1; 
		Task03end=1; 
		while(Task02end==1||Task03end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=2;//运行步骤二
		Task02end=1; 
		Task03end=1; 
		while(Task02end==1||Task03end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=3;//运行步骤三
		Task02end=1; 
		while(Task02end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=4;//运行步骤四
		Task02end=1; 
		while(Task02end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=5;//运行步骤五
		Task02end=1; 
		Task03end=1;
		while(Task02end==1||Task03end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=6;//运行步骤六
		Task02end=1; 
		Task03end=1; 
		while(Task02end==1||Task03end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=7;//运行步骤七
		Task02end=1; 
		while(Task02end==1)//等待完成
		{
			osDelay(100);
		}
		
		Step=8;//运行步骤八
		Task02end=1; 
		while(Task02end==1)//等待完成
		{
			osDelay(100);
		}
		
	}
	  else
	  {
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(70));//左脚
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(90));//左跨
	      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(79));//右脚
	      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,angle_to_duty_cycle(95));//右跨
	  }
	

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)//舵机同时运动线程1
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		  	  
			if(Step==1&&Task02end==1)
			{
				osDelay(500);
				for(unsigned char i=70;i<=90;i++)//右脚抬起
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			}
			if(Step==2&&Task02end==1)
			{
				
				for(unsigned char i=90;i>=70;i--)//左跨向前转
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			}  
			
			if(Step==3&&Task02end==1)
			{
				for(unsigned char i=90;i>=70;i--)//右脚放下
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				
				Task02end=0;
			}  
			if(Step==4&&Task02end==1)
			{
				for(unsigned char i=70;i<=90;i++)//左跨复位
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			}  
			if(Step==5&&Task02end==1)
			{
				osDelay(500);
				for(unsigned char i=79;i>=59;i--)//左跨复位
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			} 
			if(Step==6&&Task02end==1)
			{
				for(unsigned char i=95;i<=115;i++)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			} 
			if(Step==7&&Task02end==1)
			{
				for(unsigned char i=59;i<=79;i++)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			} 
			if(Step==8&&Task02end==1)
			{
				for(unsigned char i=115;i>=95;i--)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,angle_to_duty_cycle(i));
					osDelay(delay_time);
				}
				Task02end=0;
			} 
	  
		 /* for(unsigned int i=0;i<1000;i++)
			{
				light=i;
				osDelay(2);
			}
			for(unsigned int i=1000;i>0;i--)
			{
				light=i;
				osDelay(2);
			}
	*/
	  
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)//舵机同时运动线程2
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
	  if(Step==1&&Task03end==1)
	  {
		  for(unsigned char i=79;i<=120;i++)//右脚助推
		  {
	         __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
	         osDelay(delay_time);
		  }
		  
		  Task03end=0;
	  }
	  
	  if(Step==2&&Task03end==1)
	  {
		  for(unsigned char i=120;i>=79;i--)//右脚复位
		  {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
			osDelay(delay_time);
		  }
		  /*for(unsigned char i=79;i<=120;i++)
		  {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,angle_to_duty_cycle(i));
			osDelay(delay_time);
		  }*/
		  
		  Task03end=0;
	  }
	  if(Step==5&&Task03end==1)
	  {
		  for(unsigned char i=70;i>=38;i--)
		  {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
			osDelay(delay_time);
		  }
		  
		  Task03end=0;
	  }
	  if(Step==6&&Task03end==1)
	  {
		  for(unsigned char i=38;i<=70;i++)
		  {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,angle_to_duty_cycle(i));
			osDelay(delay_time);
		  }
		  
		  Task03end=0;
	  }
	  
	  
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)//屏幕更新线程
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
	 flag=1;
		  switch(image_num)
		{
		  case 1:disp_grap(1,1,expression1);break;
		  case 2:disp_grap(1,1,expression2);break;
		  case 3:disp_grap(1,1,expression3);break;
		  case 4:disp_grap(1,1,expression4);break;
		  case 5:disp_grap(1,1,expression5);break;
		  case 6:disp_grap(1,1,expression6);break;
		  case 7:disp_grap(1,1,expression7);break;
		  case 8:disp_grap(1,1,expression8);break;
		  case 9:disp_grap(1,1,expression9);break;
		}
	  flag=0;
	  osDelay(400);

  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)//红外遥控读取线程
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
  for(;;)
  {
	  if(flag==0)
	{
		ir_Handler(ir_value());//红外任务
	}
	 
	  
	 osDelay(400);
	 
	  
		
  }
  /* USER CODE END StartTask05 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
