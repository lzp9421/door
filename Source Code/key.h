#ifndef __KEY_H__
#define __KEY_H__
#include<reg51.h>

//--定义使用的IO口--//
//开发板	
//#define GPIO_KEY P1

//实际电路
#define GPIO_KEY P0
//---加入全局变量--//
extern unsigned char KeyValue;	//加入全局变量

//--声明全局函数--//
void KeyScan();
#endif