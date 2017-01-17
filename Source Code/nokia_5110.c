#include "nokia_5110.h"
#include "font_library.h"
/*******************************************************************************
*函 数 名:	delay_1us
*函数功能:	1us延时函数
*输    入:	无
*输    出:	无
*******************************************************************************/
static void delay_1us()		//1us延时函数
{
	unsigned int i;
	for(i=0;i<1000;i++);
}
/*******************************************************************************
*函 数 名:	Nokia5110Init
*函数功能:	Nokia5110初始化
*输    入:	无
*输    出:	无
*******************************************************************************/

void Nokia5110Init()
{  
	Nokia5110_RST=0;			// 产生一个让LCD复位的低电平脉冲
	delay_1us();
	Nokia5110_RST=1;			// 关闭LCD
	Nokia5110_CE=0;
	delay_1us();			// 使能LCD
	Nokia5110_CE=1;
	delay_1us();
	Nokia5110WriteByte(0x21,0);	// 使用扩展命令设置LCD模式
	Nokia5110WriteByte(0xc8,0);	// 设置偏置电压
	Nokia5110WriteByte(0x06,0);	// 温度校正
	Nokia5110WriteByte(0x13,0);	// 1:48
	Nokia5110WriteByte(0x20,0);	// 使用基本命令
	Nokia5110Cls();				// 清屏
	Nokia5110WriteByte(0x0c,0);	// 设定显示模式，正常显示
	Nokia5110_CE=0;			// 关闭LCD
  }

/*******************************************************************************
*函 数 名:	Nokia5110Cls
*函数功能:	LCD清屏函数
*输    入:	无
*输    出:	无
*******************************************************************************/

void Nokia5110Cls()
{
	unsigned int i;
	Nokia5110WriteByte(0x0c,0);			
	Nokia5110WriteByte(0x80,0);			
	for (i=0;i<504;i++)
	Nokia5110WriteByte(0,1);			
}

/*******************************************************************************
*函 数 名:	Nokia5110SetXY
*函数功能:	设置LCD坐标函数
*输    入:	X:0－83;Y:0－5
*输    出:	无
*******************************************************************************/

static void Nokia5110SetXY(unsigned char X, unsigned char Y)
{
	Nokia5110WriteByte(0x40|Y,0);		// column
	Nokia5110WriteByte(0x80|X,0);		// row
}

/*******************************************************************************
*函 数 名:	Nokia5110WriteChar
*函数功能:	显示英文字符
*输    入:	c:显示的字符
*输    出:	无
*******************************************************************************/

static void Nokia5110WriteChar(unsigned char c)
{
	unsigned char line;
	c-=32;
	for(line=0;line<6;line++)
	Nokia5110WriteByte(font6x8[c][line],1);
}

/*******************************************************************************
*函 数 名:	Nokia5110WriteAChar
*函数功能:	在指定坐标显示一个英文字符
*输    入:	X、Y:显示字符串的位置,x 0-83 ,y 0-5;c:显示的字符
*输    出:	无
*******************************************************************************/

void Nokia5110WriteAChar(unsigned char X,unsigned char Y,unsigned char c)
{
	Nokia5110SetXY(X,Y);
	Nokia5110WriteChar(c);
}
/*******************************************************************************
*函 数 名:	Nokia5110WriteStrEN
*函数功能:	在LCD上显示汉字
*输    入:	*s:英文字符串指针;X、Y:显示字符串的位置,x 0-83 ,y 0-5;
*输    出:	无			
*******************************************************************************/

void Nokia5110WriteStrEN(unsigned char X,unsigned char Y,char *s)
{
	Nokia5110SetXY(X,Y);
	while(*s) 
	{
		Nokia5110WriteChar(*s);
		s++;
	}
}

/*******************************************************************************
*函 数 名:	Nokia5110WriteStrCH
*函数功能:	在LCD上显示汉字
*输    入:	X、Y:显示汉字的起始X、Y坐标;ch_with:汉字点阵的宽度;num:显示汉字的个数;line:汉字点阵数组中的起始行数;row:汉字显示的行间距
*输    出:	无	
*******************************************************************************/                      
void Nokia5110WriteStrCH(unsigned char X, unsigned char Y, unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row)
{
	unsigned char i,n;	
	Nokia5110SetXY(X,Y);                             //设置初始位置
    	for(i=0;i<num;)
	{
		for(n=0;n<ch_with*2;n++)              //写一个汉字
		{ 
			if (n==ch_with)                      //写汉字的下半部分
			{
				if (i==0)
					Nokia5110SetXY(X,Y+1);
				else
					Nokia5110SetXY((X+(ch_with+row)*i),Y+1);
			}
			Nokia5110WriteByte(write_chinese[line+i][n],1);
		}
		i++;
		Nokia5110SetXY((X+(ch_with+row)*i),Y);
	}
}

/*******************************************************************************
*函 数 名:	Nokia5110DrawBMP
*函数功能:	位图绘制函数
*输    入:	X、Y:位图绘制的起始X、Y坐标;*map:位图点阵数据;Pix_x:位图像素（长）;Pix_y:位图像素（宽）
*输    出:	无
*******************************************************************************/
/*
void Nokia5110DrawBMP(unsigned char X,unsigned char Y,unsigned char *map,unsigned char Pix_x,unsigned char Pix_y)
{
	unsigned int i,n;
	unsigned char row;
	if (Pix_y%8==0)row=Pix_y/8;      //计算位图所占行数
	else
	row=Pix_y/8+1;
	for (n=0;n<row;n++)
	{
		Nokia5110SetXY(X,Y);
		for(i=0;i<Pix_x;i++)
		{
		Nokia5110WriteByte(map[i+n*Pix_x],1);
		}
	Y++;                         //换行
	}      
}
 */
/*******************************************************************************
*函 数 名:	Nokia5110WriteByte
*函数功能:	使用SPI接口写数据到LCD
*输    入:	data:写入的数据;command:写数据/命令选择；
*输    出:	无
*******************************************************************************/

static void Nokia5110WriteByte(unsigned char dat,unsigned char cmd)
{
	unsigned char i;
	//PORTB&=~Nokia5110_CE ;		        // 使能LCD
	Nokia5110_CE=0;
	if (cmd==0)
	//PORTB&=~Nokia5110_DC ;	        // 传送命令
	Nokia5110_DC=0;
	else
	//PORTB|=Nokia5110_DC ;		        // 传送数据
	Nokia5110_DC=1;
		for(i=0;i<8;i++)
		{
			if(dat&0x80)
				Nokia5110_SDIN=1;
			else
				Nokia5110_SDIN=0;
			Nokia5110_SCLK=0;
			dat=dat<<1;
			Nokia5110_SCLK=1;
		}
	// SPDR=data;			// 传送数据到SPI寄存器
	//while((SPSR&0x80)==0);         // 等待数据传送完毕
	//PORTB|=Nokia5110_CE;			// 关闭LCD
	Nokia5110_CE=1;
}

