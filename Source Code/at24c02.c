#include"at24c02.h"
#include"i2c.h"
/*******************************************************************************
* 函 数 名         : Delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

static void Delay10ms(unsigned int c)   //误差 0us
{
    unsigned char a, b;

	//--c已经在传递过来的时候已经赋值了，所以在for语句第一句就不用赋值了--//
    for (;c>0;c--)
	{
		for (b=38;b>0;b--)
		{
			for (a=130;a>0;a--);
		}          
	}       
}
/*******************************************************************************
* 函 数 名         : void At24c02Write(unsigned char addr,unsigned char dat)
* 函数功能		   : 往24c02的一个地址写入一个数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2C_Start();
	I2C_SendByte(0xa0, 1);//发送写器件地址
	I2C_SendByte(addr, 1);//发送要写入内存地址
	I2C_SendByte(dat, 0);	//发送数据
	I2C_Stop();
	Delay10ms(1);			//写入周期，等待数据固化
}
/*******************************************************************************
* 函 数 名         : unsigned char At24c02Read(unsigned char addr)
* 函数功能		   : 读取24c02的一个地址的一个数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2C_Start();
	I2C_SendByte(0xa0, 1); //发送写器件地址
	I2C_SendByte(addr, 1); //发送要读取的地址
	I2C_Start();
	I2C_SendByte(0xa1, 1); //发送读器件地址
	num=I2C_ReadByte(); //读取数据
	I2C_Stop();
	return num;	
}
