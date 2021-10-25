#ifndef  __LCD12864__H__
#define  __LCD12864__H__

#include "main.h"
#include "gpio.h"

//---------------------------------------------------
/*
	RESET     PA4   ��ɫ
	RS        PA5   ��ɫ
	SCL       PB12  ��ɫ
	SDA       PB13  ��ɫ
	����            ��ɫ

*/

//�⺯������

#define RESET_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1)
#define RESET_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0)

#define RS_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1)
#define RS_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0)

#define SCL_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,1)
#define SCL_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,0)

#define SDA_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,1)
#define SDA_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,0)


//-------------------------------------------------

void initial_lcd();//��ʼ��
void clear_screen();//����


void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp);
//   display_graphic_32x32(1,32*1,cheng1); //�ڵ�1 ҳ����49 ����ʾ��������"��"

void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char reverse,unsigned char *dp);
//   display_graphic_16x16(5,(1+16*2+8),1,shi1); //�ڵ�5 ҳ����41 ����ʾ��������"ʹ"

void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp);
//   display_graphic_8x16(5,(81+8*1),num0); //�ڵ�5 ҳ����97 ����ʾ��������"0"

void display_string_5x7(unsigned int page,unsigned int column,unsigned char *text);
//   display_string_5x7(7,1,"JKLMNOPQRSTUVWXYZ[\\]^");
void display_string_8x16(unsigned char page,unsigned char column,unsigned char *text);
//   display_string_8x16(1,1,"0123456789abcdef");//�ڵ�1 ҳ����1 ����ʾ�ַ���

void disp_grap(unsigned char page,unsigned char column,const unsigned char *dp);//ˢȫ��
//   disp_grap(1,1,expression1); //display a picture of 128*64 dots
void full_display(unsigned char data_left,unsigned char data_right);//ͬһ����ˢ��
//   full_display(0xff,0xff);


#endif