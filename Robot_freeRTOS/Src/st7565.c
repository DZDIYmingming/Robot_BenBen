
#include "st7565.h"
#include "Font.h"

/*写指令到LCD 模块*/
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

/*写数据到LCD 模块*/
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


/*LCD 模块初始化*/
void initial_lcd()
{
	RESET_L; /*低电平复位*/
	osDelay(20);
	RESET_H; /*复位完毕*/
	osDelay(20);
	transfer_command(0xe2); /*软复位*/
	osDelay(2);
	transfer_command(0x2c); /*升压步聚1*/
	osDelay(2);
	transfer_command(0x2e); /*升压步聚2*/
	osDelay(2);
	transfer_command(0x2f); /*升压步聚3*/
	osDelay(2);
	transfer_command(0x21); /*粗调对比度，可设置范围0x20～0x27*/
	transfer_command(0x81); /*微调对比度*/
	transfer_command(0x20); /*微调对比度的值，可设置范围0x00～0x3f*/
	transfer_command(0xa2); /*1/9 偏压比（bias）*/
	transfer_command(0xc8); /*行扫描顺序：从上到下 0xc8*/
	transfer_command(0xa0); /*列扫描顺序：从左到右 0xa0*/
	transfer_command(0x40); /*起始行：第一行开始*/
	transfer_command(0xaf); /*开显示*/
}

/*LCD 屏幕地址设置*/
void lcd_address(unsigned char page,unsigned char column)
{
  column=column-1; //设置第1列的位置。平常所说第1 列，在LCD 驱动IC里是第0列。所以在这里减1.
  page=page-1;
  transfer_command(0xb0+page); /*设置页地址。每页是8行。一个画面的64 行被分成8个页。我们平常所说的第1页，
	在LCD驱动IC 里是第0 页，所以在这里减去1*/
  transfer_command(((column>>4)&0x0f)+0x10); //设置列地址的高4 位
  transfer_command(column&0x0f); //设置列地址的低4 位
}
//-----------------------------------------------------------------
/*全屏清屏*/
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
/*显示32x32 点阵图像、汉字、生僻字或32x32 点阵的其他图标*/
void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp)
{
  unsigned char i,j;
  for(j=0;j<4;j++)
  {
    lcd_address(page+j,column);
    for (i=0;i<31;i++)
    {
      transfer_data(*dp); /*写数据到LCD,每写完一个8 位的数据后列地址自动加1*/
      dp++;
    }
  }
}
/*显示16x16 点阵图像、汉字、生僻字或16x16 点阵的其他图标*/
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
        transfer_data(*dp); /*写数据到LCD,每写完一个8 位的数据后列地址自动加1*/
      }
      else
      transfer_data(~*dp);
      dp++;
    }
  }
}
/*显示8x16 点阵图像、ASCII, 或8x16 点阵的自造字符、其他图标*/
void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp)
{
  unsigned char i,j;
  for(j=0;j<2;j++)
  {
    lcd_address(page+j,column);
    for (i=0;i<8;i++)
    {
      transfer_data(*dp); /*写数据到LCD,每写完一个8 位的数据后列地址自动加1*/
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
          transfer_data(ascii_table_8x16[j][k+8*n]);/*显示5x7的ASCII字到LCD 上，y 为页地址，x 为列地址，最后为数据*/
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
        transfer_data(ascii_table_5x7[j][k]);/*显示5x7的ASCII字到LCD上，y为页地址，x 为列地址，最后为数据*/
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

