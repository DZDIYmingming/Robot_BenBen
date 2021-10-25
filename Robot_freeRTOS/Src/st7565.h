#ifndef  __LCD12864__H__
#define  __LCD12864__H__

#include "main.h"
#include "gpio.h"

//---------------------------------------------------
/*
	RESET     PA4   灰色
	RS        PA5   蓝色
	SCL       PB12  白色
	SDA       PB13  黄色
	背光            紫色

*/

//库函数操作

#define RESET_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1)
#define RESET_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0)

#define RS_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1)
#define RS_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0)

#define SCL_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,1)
#define SCL_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,0)

#define SDA_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,1)
#define SDA_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,0)


//-------------------------------------------------

void initial_lcd();//初始化
void clear_screen();//清屏


void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp);
//   display_graphic_32x32(1,32*1,cheng1); //在第1 页，第49 列显示单个汉字"成"

void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char reverse,unsigned char *dp);
//   display_graphic_16x16(5,(1+16*2+8),1,shi1); //在第5 页，第41 列显示单个汉字"使"

void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp);
//   display_graphic_8x16(5,(81+8*1),num0); //在第5 页，第97 列显示单个数字"0"

void display_string_5x7(unsigned int page,unsigned int column,unsigned char *text);
//   display_string_5x7(7,1,"JKLMNOPQRSTUVWXYZ[\\]^");
void display_string_8x16(unsigned char page,unsigned char column,unsigned char *text);
//   display_string_8x16(1,1,"0123456789abcdef");//在第1 页，第1 列显示字符串

void disp_grap(unsigned char page,unsigned char column,const unsigned char *dp);//刷全屏
//   disp_grap(1,1,expression1); //display a picture of 128*64 dots
void full_display(unsigned char data_left,unsigned char data_right);//同一变量刷屏
//   full_display(0xff,0xff);


#endif