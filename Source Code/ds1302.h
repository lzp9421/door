#ifndef __DS1302_H__
#define __DS1302_H__
//#define TIMING			//���ö�ʱ
//---����ͷ�ļ�---//
#include<reg51.h>
//---����ds1302ʹ�õ�IO��---//
/*//������
sbit DSIO=P3^4;
sbit RST=P3^5;
sbit SCLK=P3^6;

//*///ʵ�ʵ�·
sbit DSIO=P2^6;
sbit RST=P2^7;
sbit SCLK=P2^5;
//*///
//---ȫ�ֺ�������---//
void Ds1302WriteTime();		//д��DS1302��ǰʱ�䣨��TIMEǰ7������Ԫ��д��DS1302��
void Ds1302ReadTime();		//��ȡDS1302��ǰʱ�䲢д��TIME����
#ifdef TIMING
void Ds1302WriteTimer();	//д��DS1302��ʱʱ�䣨��TIME��3������Ԫ��д��DS1302��RAM��
void Ds1302ReadTimer();		//��ȡDS1302��ʱʱ�䲢д��TIME����
#endif
//---����ȫ�ֱ���--//
extern unsigned char TIME[10];	//����ȫ�ֱ���
//��,��,ʱ,��,��,��,��,�֣���ʱ��,ʱ����ʱ��,��ʱ������;
//��ʱ�����������λ1ʱ�������塣����λ�ֱ�Ϊ��һ���������쿪����ȫΪ0ʱ����һ��
#endif