#include<reg51.h>

sbit Pwm1=P1^5;
sbit Pwm2=P1^6;
sbit KeyOpen=P3^7;//�Ŵ򿪵��ڰ����ͷ�
sbit KeyClose=P3^6;//�Ź��ϰ����ڼ�����
extern unsigned char Lock;
extern void operateDoor();