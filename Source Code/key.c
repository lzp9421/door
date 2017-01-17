#include"key.h"

unsigned char KeyValue=0;//用来存放读取到的键值


/*******************************************************************************
* 函 数 名         : Delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Delay10ms(unsigned int c)   //误差 0us
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
* 函 数 名         : KeyScan
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
#define KEY P1 //键盘所连接的I/O接口组定义
/*******************************************************************************
* 函 数 名         : KeyScan
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void KeyScan()
{
	char a=0,key=0;
	GPIO_KEY=0X0F;
 	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		Delay10ms(1);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{
			//测试列
			GPIO_KEY=0X0F;
			key|=GPIO_KEY;
			//测试行
			GPIO_KEY=0XF0;
			key|=GPIO_KEY;
			switch(key)
			{
			/*//
				case(0Xee):KeyValue='1';break;
				case(0Xde):KeyValue='2';break;
				case(0Xbe):KeyValue='3';break;

				case(0Xed):KeyValue='4';break;
				case(0Xdd):KeyValue='5';break;
				case(0Xbd):KeyValue='6';break;

				case(0Xeb):KeyValue='7';break;
				case(0Xdb):KeyValue='8';break;
				case(0Xbb):KeyValue='9';break;

				case(0Xe7):KeyValue='*';break;
				case(0Xd7):KeyValue='0';break;
				case(0Xb7):KeyValue='#';break;
			//*///
			//
				case(0Xb7):KeyValue='1';break;
				case(0Xd7):KeyValue='2';break;
				case(0Xe7):KeyValue='3';break;

				case(0Xbb):KeyValue='4';break;
				case(0Xdb):KeyValue='5';break;
				case(0Xeb):KeyValue='6';break;

				case(0Xbd):KeyValue='7';break;
				case(0Xdd):KeyValue='8';break;
				case(0Xed):KeyValue='9';break;

				case(0Xbe):KeyValue='*';break;
				case(0Xde):KeyValue='0';break;
				case(0Xee):KeyValue='#';break;
//*///			
			}
			/*while((a<50) && (GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				Delay10ms(1);
				a++;
			}*/
		}
	}
	else  KeyValue=0;//按键未按下键值为0
}
