#include<reg51.h>

sbit Pwm1=P1^5;
sbit Pwm2=P1^6;
sbit KeyOpen=P3^7;//门打开等于按键释放
sbit KeyClose=P3^6;//门关上按等于键按下
extern unsigned char Lock;
extern void operateDoor();