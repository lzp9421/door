#include"door.h"
unsigned char Lock=0;
extern void operateDoor()
{
	if(Lock)
	{
		if(!KeyOpen){//门没完全开启，电机继续转动
			Pwm1=0;
			Pwm2=1;
		}
		else{		 //门已完全开启，电机停止转动　                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          　                                          
			Pwm1=0;
			Pwm2=0;
		}
	}
	else
	{
		if(KeyClose){//门没完全闭合，电机继续转动
			Pwm1=1;
			Pwm2=0;
		}
		else{
			Pwm1=0;//门已完全闭合，电机停止转动
			Pwm2=0;
		}
	}
}