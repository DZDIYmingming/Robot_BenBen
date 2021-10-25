
#include "st7565.h"
#include "Font.h"

/*дָ�LCD ģ��*/
void transfer_command(unsigned char data1)
{
  char i;
  RS_L;
  for(i=0;i<8;i++)
  {
    SCL_L;
    if(data1&0x80) SDA_H;
    else SDA_L;
    SCL_H;
    data1=data1<<=1;
  }
  
}

/*д���ݵ�LCD ģ��*/
void transfer_data(unsigned char data1)
{
  char i;
  
  RS_H;
  for(i=0;i<8;i++)
  {
    SCL_L;
    if(data1&0x80) SDA_H;
    else SDA_L;
    SCL_H;
    data1=data1<<=1;
  }
 
}


/*LCD ģ���ʼ��*/
void initial_lcd()
{
	RESET_L; /*�͵�ƽ��λ*/
	osDelay(20);
	RESET_H; /*��λ���*/
	osDelay(20);
	transfer_command(0xe2); /*��λ*/
	osDelay(2);
	transfer_command(0x2c); /*��ѹ����1*/
	osDelay(2);
	transfer_command(0x2e); /*��ѹ����2*/
	osDelay(2);
	transfer_command(0x2f); /*��ѹ����3*/
	osDelay(2);
	transfer_command(0x21); /*�ֵ��Աȶȣ������÷�Χ0x20��0x27*/
	transfer_command(0x81); /*΢���Աȶ�*/
	transfer_command(0x20); /*΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f*/
	transfer_command(0xa2); /*1/9 ƫѹ�ȣ�bias��*/
	transfer_command(0xc8); /*��ɨ��˳�򣺴��ϵ��� 0xc8*/
	transfer_command(0xa0); /*��ɨ��˳�򣺴����� 0xa0*/
	transfer_command(0x40); /*��ʼ�У���һ�п�ʼ*/
	transfer_command(0xaf); /*����ʾ*/
}

/*LCD ��Ļ��ַ����*/
void lcd_address(unsigned char page,unsigned char column)
{
  column=column-1; //���õ�1�е�λ�á�ƽ����˵��1 �У���LCD ����IC���ǵ�0�С������������1.
  page=page-1;
  transfer_command(0xb0+page); /*����ҳ��ַ��ÿҳ��8�С�һ�������64 �б��ֳ�8��ҳ������ƽ����˵�ĵ�1ҳ��
	��LCD����IC ���ǵ�0 ҳ�������������ȥ1*/
  transfer_command(((column>>4)&0x0f)+0x10); //�����е�ַ�ĸ�4 λ
  transfer_command(column&0x0f); //�����е�ַ�ĵ�4 λ
}
//-----------------------------------------------------------------
/*ȫ������*/
void clear_screen()
{
  unsigned char i,j;
  for(i=0;i<9;i++)
  {
    lcd_address(1+i,1);
    for(j=0;j<132;j++)
    {
      transfer_data(0x00);
    }
  }
}
//==================display a piture of 128*64 dots================
void full_display(unsigned char data_left,unsigned char data_right)
{
  int i,j;
  for(i=0;i<8;i++)
  {
    
    lcd_address(i+1,1);
    for(j=0;j<64;j++)
    {
      transfer_data(data_left);
      transfer_data(data_right);
    }
  }
}
/*��ʾ32x32 ����ͼ�񡢺��֡���Ƨ�ֻ�32x32 ���������ͼ��*/
void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp)
{
  unsigned char i,j;
  for(j=0;j<4;j++)
  {
    lcd_address(page+j,column);
    for (i=0;i<31;i++)
    {
      transfer_data(*dp); /*д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1*/
      dp++;
    }
  }
}
/*��ʾ16x16 ����ͼ�񡢺��֡���Ƨ�ֻ�16x16 ���������ͼ��*/
void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char reverse,unsigned char *dp)
{
  unsigned char i,j;
  for(j=0;j<2;j++)
  {
    lcd_address(page+j,column);
    for (i=0;i<16;i++)
    {
      if(reverse==1)
      {
        transfer_data(*dp); /*д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1*/
      }
      else
      transfer_data(~*dp);
      dp++;
    }
  }
}
/*��ʾ8x16 ����ͼ��ASCII, ��8x16 ����������ַ�������ͼ��*/
void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp)
{
  unsigned char i,j;
  for(j=0;j<2;j++)
  {
    lcd_address(page+j,column);
    for (i=0;i<8;i++)
    {
      transfer_data(*dp); /*д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1*/
      dp++;
    }
  }
}
void display_string_8x16(unsigned char page,unsigned char column,unsigned char *text)
{
  unsigned int i=0,j,k,n;
  while(text[i]>0x00)
  {
    if((text[i]>=0x20)&&(text[i]<=0x7e))
    {
      j=text[i]-0x20;
      for(n=0;n<2;n++)
      {
        lcd_address(page+n,column);
        for(k=0;k<8;k++)
        {
          transfer_data(ascii_table_8x16[j][k+8*n]);/*��ʾ5x7��ASCII�ֵ�LCD �ϣ�y Ϊҳ��ַ��x Ϊ�е�ַ�����Ϊ����*/
        }
      }
      i++;
      column+=8;
    }
    else
    i++;
  }
}
void display_string_5x7(unsigned int page,unsigned int column,unsigned char *text)
{
   unsigned int i=0,j,k;
  while(text[i]>0x00)
  {
    if((text[i]>=0x20)&&(text[i]<0x7e))
    {
      j=text[i]-0x20;
      lcd_address(page,column);
      for(k=0;k<5;k++)
      {
        transfer_data(ascii_table_5x7[j][k]);/*��ʾ5x7��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��x Ϊ�е�ַ�����Ϊ����*/
      }
      i++;
      column+=6;
    }
    else
    i++;
  }
}
//==================display a piture of 128*64 dots================
void disp_grap(unsigned char page,unsigned char column,const unsigned char *dp)
{
  int i,j;
  for(i=0;i<8;i++)
  {
    lcd_address(page+i,column);
    for(j=0;j<128;j++)
    {
      transfer_data(*dp);
      dp++;
    }
  }
}

