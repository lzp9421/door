#include"door.h"
unsigned char Lock=0;
extern void operateDoor()
{
	if(Lock)
	{
		if(!KeyOpen){//��û��ȫ�������������ת��
			Pwm1=0;
			Pwm2=1;
		}
		else{		 //������ȫ���������ֹͣת����                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ��                                          
			Pwm1=0;
			Pwm2=0;
		}
	}
	else
	{
		if(KeyClose){//��û��ȫ�պϣ��������ת��
			Pwm1=1;
			Pwm2=0;
		}
		else{
			Pwm1=0;//������ȫ�պϣ����ֹͣת��
			Pwm2=0;
		}
	}
}