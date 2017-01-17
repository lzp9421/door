#ifndef __NOKIA5110_H__
#define __NOKIA5110_H__
#include <reg51.h>

// pin define for n5110lcd_8key board
// change this if your hardware changed!
/*/开发板
sbit Nokia5110_SCLK = P2^6;		// pin 2	 header	5
sbit Nokia5110_SDIN = P2^5;		// pin 3	 header	4
sbit Nokia5110_DC = P2^4;		// pin 4	 header	3
sbit Nokia5110_CE = P2^3;		// pin 5	 header	2
sbit Nokia5110_RST = P2^2;	// pin 9	 header	1

//*///实际电路
sbit Nokia5110_SCLK = P1^4;		// pin 2	 header	5
sbit Nokia5110_SDIN = P1^3;		// pin 3	 header	4
sbit Nokia5110_DC = P1^2;		// pin 4	 header	3
sbit Nokia5110_CE = P1^1;		// pin 5	 header	2
sbit Nokia5110_RST = P1^0;	// pin 9	 header	1
//*///
void Nokia5110Init();
void Nokia5110Cls();
void Nokia5110WriteStrEN(unsigned char X,unsigned char Y,char *s);
void Nokia5110WriteStrCH(unsigned char X, unsigned char Y,unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row);
void Nokia5110WriteChar(unsigned char c);
void Nokia5110WriteAChar(unsigned char X,unsigned char Y,unsigned char c);
void Nokia5110WriteByte(unsigned char dat, unsigned char dc);
void Nokia5110SetXY(unsigned char X, unsigned char Y);
void delay_1us();                 
#endif