#include"key.h"

unsigned char KeyValue=0;//������Ŷ�ȡ���ļ�ֵ


/*******************************************************************************
* �� �� ��         : Delay10ms
* ��������		   : ��ʱ��������ʱ10ms
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Delay10ms(unsigned int c)   //��� 0us
{
    unsigned char a, b;

	//--c�Ѿ��ڴ��ݹ�����ʱ���Ѿ���ֵ�ˣ�������for����һ��Ͳ��ø�ֵ��--//
    for (;c>0;c--)
	{
		for (b=38;b>0;b--)
		{
			for (a=130;a>0;a--);
		}          
	}       
}
/*******************************************************************************
* �� �� ��         : KeyScan
* ��������		   : ����а������²���ȡ��ֵ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
#define KEY P1 //���������ӵ�I/O�ӿ��鶨��
/*******************************************************************************
* �� �� ��         : KeyScan
* ��������		   : ����а������²���ȡ��ֵ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void KeyScan()
{
	char a=0,key=0;
	GPIO_KEY=0X0F;
 	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		Delay10ms(1);//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{
			//������
			GPIO_KEY=0X0F;
			key|=GPIO_KEY;
			//������
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
			/*while((a<50) && (GPIO_KEY!=0xf0))	 //��ⰴ�����ּ��
			{
				Delay10ms(1);
				a++;
			}*/
		}
	}
	else  KeyValue=0;//����δ���¼�ֵΪ0
}
