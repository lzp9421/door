#include "nokia_5110.h"
#include "font_library.h"
/*******************************************************************************
*�� �� ��:	delay_1us
*��������:	1us��ʱ����
*��    ��:	��
*��    ��:	��
*******************************************************************************/
static void delay_1us()		//1us��ʱ����
{
	unsigned int i;
	for(i=0;i<1000;i++);
}
/*******************************************************************************
*�� �� ��:	Nokia5110Init
*��������:	Nokia5110��ʼ��
*��    ��:	��
*��    ��:	��
*******************************************************************************/

void Nokia5110Init()
{  
	Nokia5110_RST=0;			// ����һ����LCD��λ�ĵ͵�ƽ����
	delay_1us();
	Nokia5110_RST=1;			// �ر�LCD
	Nokia5110_CE=0;
	delay_1us();			// ʹ��LCD
	Nokia5110_CE=1;
	delay_1us();
	Nokia5110WriteByte(0x21,0);	// ʹ����չ��������LCDģʽ
	Nokia5110WriteByte(0xc8,0);	// ����ƫ�õ�ѹ
	Nokia5110WriteByte(0x06,0);	// �¶�У��
	Nokia5110WriteByte(0x13,0);	// 1:48
	Nokia5110WriteByte(0x20,0);	// ʹ�û�������
	Nokia5110Cls();				// ����
	Nokia5110WriteByte(0x0c,0);	// �趨��ʾģʽ��������ʾ
	Nokia5110_CE=0;			// �ر�LCD
  }

/*******************************************************************************
*�� �� ��:	Nokia5110Cls
*��������:	LCD��������
*��    ��:	��
*��    ��:	��
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
*�� �� ��:	Nokia5110SetXY
*��������:	����LCD���꺯��
*��    ��:	X:0��83;Y:0��5
*��    ��:	��
*******************************************************************************/

static void Nokia5110SetXY(unsigned char X, unsigned char Y)
{
	Nokia5110WriteByte(0x40|Y,0);		// column
	Nokia5110WriteByte(0x80|X,0);		// row
}

/*******************************************************************************
*�� �� ��:	Nokia5110WriteChar
*��������:	��ʾӢ���ַ�
*��    ��:	c:��ʾ���ַ�
*��    ��:	��
*******************************************************************************/

static void Nokia5110WriteChar(unsigned char c)
{
	unsigned char line;
	c-=32;
	for(line=0;line<6;line++)
	Nokia5110WriteByte(font6x8[c][line],1);
}

/*******************************************************************************
*�� �� ��:	Nokia5110WriteAChar
*��������:	��ָ��������ʾһ��Ӣ���ַ�
*��    ��:	X��Y:��ʾ�ַ�����λ��,x 0-83 ,y 0-5;c:��ʾ���ַ�
*��    ��:	��
*******************************************************************************/

void Nokia5110WriteAChar(unsigned char X,unsigned char Y,unsigned char c)
{
	Nokia5110SetXY(X,Y);
	Nokia5110WriteChar(c);
}
/*******************************************************************************
*�� �� ��:	Nokia5110WriteStrEN
*��������:	��LCD����ʾ����
*��    ��:	*s:Ӣ���ַ���ָ��;X��Y:��ʾ�ַ�����λ��,x 0-83 ,y 0-5;
*��    ��:	��			
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
*�� �� ��:	Nokia5110WriteStrCH
*��������:	��LCD����ʾ����
*��    ��:	X��Y:��ʾ���ֵ���ʼX��Y����;ch_with:���ֵ���Ŀ��;num:��ʾ���ֵĸ���;line:���ֵ��������е���ʼ����;row:������ʾ���м��
*��    ��:	��	
*******************************************************************************/                      
void Nokia5110WriteStrCH(unsigned char X, unsigned char Y, unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row)
{
	unsigned char i,n;	
	Nokia5110SetXY(X,Y);                             //���ó�ʼλ��
    	for(i=0;i<num;)
	{
		for(n=0;n<ch_with*2;n++)              //дһ������
		{ 
			if (n==ch_with)                      //д���ֵ��°벿��
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
*�� �� ��:	Nokia5110DrawBMP
*��������:	λͼ���ƺ���
*��    ��:	X��Y:λͼ���Ƶ���ʼX��Y����;*map:λͼ��������;Pix_x:λͼ���أ�����;Pix_y:λͼ���أ���
*��    ��:	��
*******************************************************************************/
/*
void Nokia5110DrawBMP(unsigned char X,unsigned char Y,unsigned char *map,unsigned char Pix_x,unsigned char Pix_y)
{
	unsigned int i,n;
	unsigned char row;
	if (Pix_y%8==0)row=Pix_y/8;      //����λͼ��ռ����
	else
	row=Pix_y/8+1;
	for (n=0;n<row;n++)
	{
		Nokia5110SetXY(X,Y);
		for(i=0;i<Pix_x;i++)
		{
		Nokia5110WriteByte(map[i+n*Pix_x],1);
		}
	Y++;                         //����
	}      
}
 */
/*******************************************************************************
*�� �� ��:	Nokia5110WriteByte
*��������:	ʹ��SPI�ӿ�д���ݵ�LCD
*��    ��:	data:д�������;command:д����/����ѡ��
*��    ��:	��
*******************************************************************************/

static void Nokia5110WriteByte(unsigned char dat,unsigned char cmd)
{
	unsigned char i;
	//PORTB&=~Nokia5110_CE ;		        // ʹ��LCD
	Nokia5110_CE=0;
	if (cmd==0)
	//PORTB&=~Nokia5110_DC ;	        // ��������
	Nokia5110_DC=0;
	else
	//PORTB|=Nokia5110_DC ;		        // ��������
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
	// SPDR=data;			// �������ݵ�SPI�Ĵ���
	//while((SPSR&0x80)==0);         // �ȴ����ݴ������
	//PORTB|=Nokia5110_CE;			// �ر�LCD
	Nokia5110_CE=1;
}

