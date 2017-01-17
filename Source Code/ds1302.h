#ifndef __DS1302_H__
#define __DS1302_H__
//#define TIMING			//启用定时
//---包含头文件---//
#include<reg51.h>
//---定义ds1302使用的IO口---//
/*//开发板
sbit DSIO=P3^4;
sbit RST=P3^5;
sbit SCLK=P3^6;

//*///实际电路
sbit DSIO=P2^6;
sbit RST=P2^7;
sbit SCLK=P2^5;
//*///
//---全局函数声明---//
void Ds1302WriteTime();		//写入DS1302当前时间（将TIME前7个数组元素写入DS1302）
void Ds1302ReadTime();		//读取DS1302当前时间并写入TIME数组
#ifdef TIMING
void Ds1302WriteTimer();	//写入DS1302定时时间（将TIME后3个数组元素写入DS1302的RAM）
void Ds1302ReadTimer();		//读取DS1302定时时间并写入TIME数组
#endif
//---加入全局变量--//
extern unsigned char TIME[10];	//加入全局变量
//秒,分,时,日,月,周,年,分（定时）,时（定时）,定时控制器;
//定时控制器：最高位1时开启闹铃。后七位分别为周一到周日哪天开启，全为0时响铃一次
#endif