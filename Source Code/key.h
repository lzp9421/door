#ifndef __KEY_H__
#define __KEY_H__
#include<reg51.h>

//--����ʹ�õ�IO��--//
//������	
//#define GPIO_KEY P1

//ʵ�ʵ�·
#define GPIO_KEY P0
//---����ȫ�ֱ���--//
extern unsigned char KeyValue;	//����ȫ�ֱ���

//--����ȫ�ֺ���--//
void KeyScan();
#endif