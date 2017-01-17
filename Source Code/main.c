#include<string.h>
#include"ds1302.h"
#include"nokia_5110.h"
#include"at24c02.h"
#include"key.h"
#include"at24c02.h"
#include"door.h"
#define uchar unsigned char
//uchar DisplayCount=0,DisplayStatus=0;
sbit KeyA=P2^0;
sbit KeyB=P2^1;
sbit IRIN=P3^3;
uchar IRTime;
uchar ComeIn=0;
uchar ComeOut=0;
uchar InOut=0;
uchar KeyIn=0xff;
uchar PasswordCount=0;//���������
uchar Temp1[15];//���������
uchar Temp2[15];//���������
uchar DisplayStatus=0;//0������ʾ��1�������룬2׼����״̬��3׼�������룬4�������룬5����ʱ��
//uchar Password[15]="12345678";
uchar TimeOut;//��ʱ�жϼ�����
uchar Count=0,Count2=0;
char PasswordERR=0; //����������
struct users{
	unsigned char id;
	unsigned char type;//0Ϊ��ͨ�û���1Ϊ�����˻���
	unsigned char password[15];
	//struct users *next;
}User={0,0,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};
//***********************************
void initINT0(){
	EA=1;	//�ж��ܿ���
	EX0=1; //�����ⲿ�ж�0�ж�
	IT0=1;//1�����ش���  0���͵�ƽ����
}
void initINT1(){
	EA=1;	//�ж��ܿ���
	EX1=1; //�����ⲿ�ж�1�ж�    
	IT1=1; //1�����ش���  0���͵�ƽ����
}
void initTC0(){
	TMOD=0x11; //��4λ����T/C1 [ GATE��C/T��M1��M0��GATE��C/T��M1��M0 ]
	EA=1;	//�ж��ܿ���
	TH0=0x3C; //16λ�����Ĵ���T0��8λ
	TL0=0xB0; //16λ�����Ĵ���T0��8λ��0x3CB0 = 50mS��ʱ��
	ET0=1; //T/C0�жϿ���
	TR0=1; //T/C0��������
}
void init()
{	
	Lock=0;//������
	Nokia5110Init();//��ʼ��Һ�� 
	initTC0();
	initINT0();
	initINT1();
}
/********************************************************************/
void refreshTime()
{
	Temp2[0]='2';
	Temp2[1]='0';
	Temp2[2]=(TIME[6]>>4)+48;
	Temp2[3]=(TIME[6]&0x0f)+48;
	Temp2[4]='-';
	Temp2[5]=(TIME[4]>>4)+48;
	Temp2[6]=(TIME[4]&0x0f)+48;
	Temp2[7]='-';
	Temp2[8]=(TIME[3]>>4)+48;
	Temp2[9]=(TIME[3]&0x0f)+48;
	Temp2[10]='\0';
	Nokia5110WriteStrEN(12,0,Temp2);

		Nokia5110WriteStrCH(16,1,12,2,0,8);//��ʾ�����ڡ�
		Nokia5110WriteStrCH(56,1,12,1,(TIME[5]&0x0f)+1,0);

	Temp2[0]=(TIME[2]>>4)+48;
	Temp2[1]=(TIME[2]&0x0f)+48;
	Temp2[2]=':';
	Temp2[3]=(TIME[1]>>4)+48;
	Temp2[4]=(TIME[1]&0x0f)+48;
	Temp2[5]=':';
	Temp2[6]=(TIME[0]>>4)+48;
	Temp2[7]=(TIME[0]&0x0f)+48;
	Temp2[8]='\0';
	Nokia5110WriteStrEN(18,3,Temp2);
}

uchar getLocation()//��λ
{
	switch(PasswordCount)
	{
		case 0:return 0x02;
		case 1:return 0x03;
		case 2:return 0x05;
		case 3:return 0x06;
		case 4:return 0x08;
		case 5:return 0x09;
		case 6:return 0x20;
		case 7:return 0x21;
		case 8:return 0x23;
		case 9:return 0x24;
	}
}

void refreshSetTime()
{
	if(KeyValue)Count=0;
	if(Count%20<10)
	{
		Nokia5110WriteStrEN(0,0,Temp1);
		Nokia5110WriteStrEN(0,2,Temp2);
	}
	else Nokia5110WriteStrEN(6*(getLocation()&0x0f),getLocation()>>4," ");

}
void refreshOtherUsers()
{
		uchar i,n=0;
		Nokia5110WriteStrEN(0,0,"1.Create User");
		Nokia5110WriteStrEN(0,1,"2.User Info");
		Nokia5110WriteStrEN(0,3,"Capacity:");
		for(i=0;i<15;i++)
			if(At24c02Read(17*i)<16&&At24c02Read(17*i)>0)n++;
		Temp2[0]=n/10+'0';	Temp2[1]=n%10+'0';	Temp2[2]='/';
		Temp2[3]='1';		Temp2[4]='5';		Temp2[5]='\0';
		Nokia5110WriteStrEN(54,3,Temp2);
}

void refreshSet(uchar*str)
{
	uchar h;
	Nokia5110Cls();//����
		
	Nokia5110WriteStrEN(0,0,"UserID: ");
	Nokia5110WriteAChar(48,0,User.id/10+'0');
	Nokia5110WriteAChar(54,0,User.id%10+'0');
	Nokia5110WriteStrEN(0,1,"1.ChangePWD");
	if(User.type==1)Nokia5110WriteStrEN(0,2,"2.SetTime");
	if(User.type==1)Nokia5110WriteStrEN(0,3,"3.OtherUsers");
	if(str==NULL){
		h=TIME[2]/0x10*10+TIME[2]%0x10;//ʱ
	if(TIME[5]%0x10!=6&&h>=6&&h<23||TIME[5]%0x10==6&&h>=5&&h<24){//ƽʱ6��00~23��00����5��00~24��00
		Nokia5110WriteStrEN(0,5,"     OPEN     ");//��ָ��������ʱ���ڿ�������ʾOPEN
		Lock=1;//����
		}
	else Nokia5110WriteStrEN(0,5,"   PADLOCK!   ");
	}
	else Nokia5110WriteStrEN(0,5,str);
}
void refreshSetPWD()
{
	Nokia5110Cls();
	Nokia5110WriteStrEN(0,0,"UserID:");
	Nokia5110WriteStrEN(0,1,"Password:");
	Nokia5110WriteStrEN(0,2,User.password);
	Nokia5110WriteStrCH(12,4,12,5,17,0);
	DisplayStatus=3;//��������
}

void Time2Str()
{
	Temp1[0]='2';
	Temp1[1]='0';
	Temp1[2]=(TIME[6]>>4)+48;
	Temp1[3]=(TIME[6]&0x0f)+48;
	Temp1[4]='-';
	Temp1[5]=(TIME[4]>>4)+48;
	Temp1[6]=(TIME[4]&0x0f)+48;
	Temp1[7]='-';
	Temp1[8]=(TIME[3]>>4)+48;
	Temp1[9]=(TIME[3]&0x0f)+48;
	Temp1[10]='\0';

	Temp2[0]=(TIME[2]>>4)+48;
	Temp2[1]=(TIME[2]&0x0f)+48;
	Temp2[2]=':';
	Temp2[3]=(TIME[1]>>4)+48;
	Temp2[4]=(TIME[1]&0x0f)+48;
	Temp2[5]='\0';
}

void clsPassword()
{
		Nokia5110WriteStrEN(0,4,"              ");//�����ʾ������4��
		Nokia5110WriteStrEN(0,5,"              ");//�����ʾ������5��
		KeyIn=0;//���ü�ֵ
		PasswordCount=0;//��ԭ���������
			
		//����ʱ��DisplayStatus��0���س�ʼ״̬
}
uchar isLeapyear(int year)
{
	return (year%4==0&&year%100)||year%400==0?1:0;
}
uchar DayInOneMonth(int year,int month)
{
	switch(month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:return 31;
		case 2:break;
		default:return 30;
	}
	return (isLeapyear(year)?29:28);
}

uchar setTimeGo(uchar(*CtrlFun)())
{
	uchar temp=0xff;
	if(KeyValue)
		KeyIn=KeyValue;
		if(KeyIn<='9'&&KeyIn>='0'&&PasswordCount<10)//��ֵΪ����
		{
			switch(PasswordCount)
			{
				case 0:					//��ʮλ
						Temp1[2]=KeyIn;
						PasswordCount++;
						break;
				case 1:					//���λ
						Temp1[3]=KeyIn;
						PasswordCount++;
						break;
				case 2:					//��ʮλ
						if(KeyIn!='0'&&KeyIn!='1')break;//����ֻ����0��1
						Temp1[5]=KeyIn;
						PasswordCount++;
						break;
				case 3:					//�¸�λ
						if(Temp1[5]!='0'&&KeyIn>'2')break;//��ʮλΪ1ʱ����ֻ��С�ڶ�
						Temp1[6]=KeyIn;
						PasswordCount++;
						break;
				case 4:					//��ʮλ
						if(DayInOneMonth(Temp1[2]*10+Temp1[3]+1472,Temp1[5]*10+Temp1[6]-528)/10+'0'<KeyIn)break;//����ʮλ���ܴ��ڵ�ǰ�·�������ʮλ
						Temp1[8]=KeyIn;
						PasswordCount++;
						break;
				case 5:					//�ո�λ
						if(DayInOneMonth(Temp1[2]*10+Temp1[3]+1472,Temp1[5]*10+Temp1[6]-528)<Temp1[8]*10+KeyIn-528)break;//������λ���ܴ��ڵ�ǰ�·������ĸ�λ
						Temp1[9]=KeyIn;
						PasswordCount++;
						break;
				case 6:					//Сʱʮλ
						if(KeyIn>'2')break;//����ֻ��С�ڵ���6
						Temp2[0]=KeyIn;
						PasswordCount++;
						break;
				case 7:					//Сʱ��λ
						if(Temp2[0]>='2'&&KeyIn>'3')break;//����ֻ����0��1
						Temp2[1]=KeyIn;
						PasswordCount++;
						break;
				case 8:					//����ʮλ
						if(KeyIn>'5')break;//����ֻ��С��6
						Temp2[3]=KeyIn;
						PasswordCount++;
						break;
				case 9:					//���Ӹ�λ
						Temp2[4]=KeyIn;
						PasswordCount++;
						break;
			}
		 	//��¼����
		}
		else if(KeyIn=='*')
		{
			if(PasswordCount<=1)//���벻��һλ������һ״̬
			{
				DisplayStatus=2;
				clsPassword();
				return(4);
			}
			PasswordCount--;
		}
		else if(KeyIn=='#'||PasswordCount==10)
		{
			clsPassword();
			temp=(*CtrlFun)();//����ȷ�ϼ�����������
		}
	KeyIn=0;
	while(KeyValue)KeyScan();;
	return(temp);
}

uchar saveTime()
{
	uchar i;
	int year,month,day,week;
	year=Temp1[2]*10+Temp1[3]+1472;
	month=Temp1[5]*10+Temp1[6]-528;
	day=Temp1[8]*10+Temp1[9]-528;
	week=(day+1+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;
	for(i=0;i<=6;i++)
		TIME[i]=0;
	TIME[6]=Temp1[2]-'0'<<4|Temp1[3]-'0';
	TIME[5]=week==0?7:week;
	TIME[4]=Temp1[5]-'0'<<4|Temp1[6]-'0';
	TIME[3]=Temp1[8]-'0'<<4|Temp1[9]-'0';
	TIME[2]=Temp2[0]-'0'<<4|Temp2[1]-'0';
	TIME[1]=Temp2[3]-'0'<<4|Temp2[4]-'0';
	Ds1302WriteTime();
	DisplayStatus=2;
	return 0;
}

uchar isBack()
{
	if(KeyIn)
		if(KeyValue==0){//���°���������
			if(KeyIn=='*')//��ֵΪ����
			{
			 return(1);
			}
			KeyIn=0;
			}
	return(0);	
}
uchar checkPassword()
{
	if(strlen(User.password)<15&&strcmp(Temp1,User.password))
	{
		PasswordERR++;
		Nokia5110WriteStrEN(0,5,"Password Error");
		PasswordCount=0;
		return(1);//��������ʧ��
	}
	else 
	{
		PasswordERR=0;
		DisplayStatus=2;//����׼����״̬
		return(0);//�������óɹ�
		//refreshSet();//ˢ�����ý���ͼ��
	}
}

void savePassword(uchar*password)
{
	uchar i=0;
	for(i=0;i<15;i++)
		At24c02Write(17*User.id-15+i,*(password+i));
}
uchar saveUser()
{
	At24c02Write(17*(User.id-1),User.id);
	At24c02Write(17*(User.id-1)+1,0);
	savePassword(Temp1);
	return 0;
}
uchar getUser(uchar id)//�齨һ���û�
{
	uchar i;
	if(id!=At24c02Read(17*(id-1)))return 1;//�û������ڷ���1
	else{
		User.id=id;
		User.type=At24c02Read(17*(id-1)+1);
		for(i=0;i<15;i++)
			User.password[i]=At24c02Read(17*(id-1)+2+i);//��24c02������û�ID����ʱ������û�
		if(strlen(User.password)>14)return 1;//���벻��û�н�����ʶ����ֹ�û�������
		}
	return 0;//�ɹ��齨�û�����0
}

uchar getKeyVal()
{
	if(KeyIn)
		if(KeyValue==0)return(KeyIn);
	return 0;
}
uchar saveUserID()
{
	uchar id,t=0;
	if(strlen(Temp1)==1)id=Temp1[0]-'0';
	else if(strlen(Temp1)==2)id=(Temp1[0]-'0')*10+Temp1[1]-'0';
	if(getUser(id))
	{
		User.id=id;
		Nokia5110Cls();
		return(1);//�û��������
	}
	else 
	{
		PasswordERR=0;
		Nokia5110Cls();
		Nokia5110WriteStrEN(0,5,"Already Exists");
		Nokia5110WriteStrEN(0,0,"Cover?");
		Nokia5110WriteStrEN(0,2,"1.YES    2.NO");
		while(!t)
		{
		KeyScan();
		if(KeyValue)KeyIn=KeyValue;
		t=getKeyVal();
		switch(t){//���°���������
			case '1':
						User.id=id;
						Nokia5110Cls();
						KeyIn=0;
						break;//��ֵΪ����
			case '2':
			case '*':
						KeyIn=0;
						User.id=1+'0';
						Nokia5110Cls();
						break;
			default:t=0;
			}
		}
		return(0);//�������óɹ�
		//refreshSet();//ˢ�����ý���ͼ��
	}
}
uchar checkUserID()
{
	uchar id;
	if(strlen(Temp1)==1)id=Temp1[0]-'0';
	else if(strlen(Temp1)==2)id=(Temp1[0]-'0')*10+Temp1[1]-'0';
	if(strlen(Temp1)>2||getUser(id))
	{
		PasswordERR++;
		Nokia5110WriteStrEN(0,5,"Nonentity");
		PasswordCount=0;
		return(1);//��������ʧ��
	}
	else 
	{
		PasswordERR=0;
		switch(DisplayStatus){
			case 1:
					DisplayStatus=0;
					break;//������������״̬
		}
		return(0);//�������óɹ�
		//refreshSet();//ˢ�����ý���ͼ��
	}
}

uchar checkTimeout(int time)
{
	if(KeyValue)TimeOut=0;//3���޲�������������ʾ״̬
	if(TimeOut>=time){DisplayStatus=0;PasswordERR=0;PasswordCount=0;Lock=0;User.id=0;return(1);}
	else return(0);
}

uchar getPassword(uchar(*CtrlFun)(),uchar ch,uchar size)
{	//Nokia5110WriteAChar(6,5,KeyIn);
	uchar temp=0xff;
	if(KeyIn)
		if(KeyValue==0){//���°���������
			if(KeyIn<='9'&&KeyIn>='0'&&PasswordCount<size)//��ֵΪ����
			{
				if(PasswordERR&&PasswordCount==0){Nokia5110WriteStrEN(0,4,"              ");Nokia5110WriteStrEN(0,5,"              ");}//�����ʾ������5��
				Nokia5110WriteAChar(PasswordCount*6,5,ch);//��ʾһ����*��
			 	Temp1[PasswordCount++]=KeyIn;//��¼����
			}
			
			else if(KeyIn=='*')
			{
				if(PasswordCount<=1)//���벻��һλ������һ״̬
				{
					DisplayStatus--;
					clsPassword();
					return(4);
				}
				Temp1[--PasswordCount]=0;//����ɾ����ɾ��һ������
				Nokia5110WriteAChar(PasswordCount*6,5,' ');//ɾȥһ����*��
			}
			else if(KeyIn=='#')
			{
				Temp1[PasswordCount]='\0';//����������
				clsPassword();
				temp=(*CtrlFun)();//����ȷ�ϼ�����������
			}
			KeyIn=0;
			}
	return(temp);
}


/******************************************************************************/
uchar changePassword()
{
	static uchar RePassword;
	if(RePassword==0)
	{
	if(strlen(Temp1)<6)
		{
			PasswordERR=1;
			Nokia5110WriteStrEN(0,5,"too short!");
			PasswordCount=0;
			RePassword=0;
			return(2);
		}
		else
		{
			PasswordERR=0xff;
			RePassword=1;//����׼����״̬
			strcpy(Temp2,Temp1);
			Nokia5110WriteStrCH(12,4,12,4,15,0);//��ʾ���ٴ����롱
		}
	}
	else if(RePassword==1)
	{
		RePassword=0;
		if(strcmp(Temp1,Temp2))
		{									
			PasswordERR=2;
			Nokia5110WriteStrEN(0,5,"Different");
			PasswordCount=0;
			return(3);//��������ʧ��
		}
		else 
		{
			PasswordERR=0;
			DisplayStatus=2;//��������׼����״̬
			savePassword(Temp1);
			return(0);//�������óɹ�
		}
	}
	return(0xfe);
}							  
void display()
{	
	uchar temp,t=0,i;
	if(KeyValue)KeyIn=KeyValue;
	if(DisplayStatus==0)//������ʾ״̬�µİ�������
	{
		refreshTime();
		if(KeyValue<='9'&&KeyValue>='0'){DisplayStatus=1;Nokia5110WriteStrEN(0,4,"              ");Nokia5110WriteStrEN(0,5,"              ");}//������������״̬����������������롱
		if(DisplayStatus==0)if(User.id)Nokia5110WriteStrCH(12,4,12,5,9,0);
							else{
								Nokia5110WriteStrCH(6,4,12,3,9,0);
								Nokia5110WriteStrCH(42,4,12,3,22,0);
								}//����״̬����ʾ���������û�ID��//����״̬����ʾ�����������롱	
		if(User.id&&checkTimeout(5))clsPassword();//��ʱ�жϲ��ڳ�ʱ������
	}
//----------------------------
	if(DisplayStatus==1)//����0~9����һ���������������״̬
	{																					 
		refreshTime();
		User.id?getPassword(checkPassword,'*',15):getPassword(checkUserID,KeyIn,2);
		if(checkTimeout(5))clsPassword();//��ʱ�ж�
		if(DisplayStatus==2)refreshSet(NULL);
	}
//-------------------------
	if(DisplayStatus==2)//׼����״̬
	{
		//��ȡ���ü�ֵ
		Nokia5110WriteStrEN(0,4,"IN:");Nokia5110WriteAChar(18,4,ComeIn/100+'0');Nokia5110WriteAChar(24,4,ComeIn%100/10+'0');Nokia5110WriteAChar(30,4,ComeIn%10+'0');
		Nokia5110WriteStrEN(42,4,"OUT:");Nokia5110WriteAChar(66,4,ComeOut/100+'0');Nokia5110WriteAChar(72,4,ComeOut%100/10+'0');Nokia5110WriteAChar(78,4,ComeOut%10+'0');
		switch(getKeyVal()){//���°���������
			case '1':	
						Nokia5110Cls();
						refreshSetPWD();
						KeyIn=0;
						break;//��ֵΪ����
			case '2':
						if(User.type==1){
							Nokia5110Cls();
							DisplayStatus=5;
							KeyIn=0;
							Time2Str();
							}break;//����ʱ��
			case '3':
						if(User.type==1){
							Nokia5110Cls();
							DisplayStatus=6;
							KeyIn=0;
							}break;//���������˻�
		//	default:KeyIn=0;
			}
		//if(isBack()==1){checkTimeout(0);Nokia5110Cls();}
		if(DisplayStatus==3)Nokia5110WriteStrCH(12,4,12,5,17,0);//��������״̬����ʾ�����������롱
		if(checkTimeout(10))Nokia5110Cls();//��ʱ�жϲ��ڳ�ʱ������
		if(DisplayStatus!=2)KeyIn=0;//
	}
//---------------------------
	if(DisplayStatus==3)//׼��������״̬
	{
		if(isBack()==1)DisplayStatus=2;
		if(KeyValue<='9'&&KeyValue>='0'){DisplayStatus=4;clsPassword();}//������������״̬����������������롱	
		if(checkTimeout(10))Nokia5110Cls();//��ʱ�жϲ��ڳ�ʱ������
		if(DisplayStatus==2)refreshSet(NULL);
	}
	if(DisplayStatus==4)//����0~9����һ���������������״̬
	{																					 
		if(checkTimeout(20)){
			clsPassword();
			Nokia5110Cls();
			}//��ʱ�ж�
		if(DisplayStatus==3)refreshSetPWD();
		if(getPassword(changePassword,'*',14)==0){
		clsPassword();
		refreshSet("Success");
		}
	}
	if(DisplayStatus==5)
	{
		refreshSetTime();
		if(checkTimeout(20)){
			Nokia5110Cls();
			clsPassword();
			DisplayStatus=0;
			}
		if(setTimeGo(saveTime)==0){
			clsPassword();
			refreshSet("Success");
			}
		if(DisplayStatus==2)refreshSet(NULL);
	}
	if(DisplayStatus==6)
	{
		refreshOtherUsers();
		//��ȡ���ü�ֵ
		switch(getKeyVal()){//���°���������
			case '1':
						DisplayStatus=7;
						User.id+='0';
						Nokia5110Cls();
						KeyIn=0;
						break;//�����û�
			case '2':
						DisplayStatus=8;
						Nokia5110Cls();
						KeyIn=0;
						break;//���������˻�
			case '*':
						DisplayStatus=2;
						KeyIn=0;
						break;//��ֵΪ����
			}									
		if(checkTimeout(20)){
			Nokia5110Cls();
			clsPassword();
			DisplayStatus=0;
			}
		if(DisplayStatus==2)refreshSet(NULL);
		if(DisplayStatus!=6)KeyIn=0;//
	}
	if(DisplayStatus==7)
	{
		if(User.id<=15)Nokia5110WriteStrCH(12,1,12,5,9,0);
		else{
		Nokia5110WriteStrCH(6,1,12,3,9,0);
		Nokia5110WriteStrCH(42,1,12,3,22,0);
		}//����״̬����ʾ���������û�ID��//����״̬����ʾ�����������롱	
		if(User.id<=15){
			if(getPassword(saveUser,KeyIn,15)==0){
				clsPassword();
				DisplayStatus=2;
				getUser(User.id);
				refreshSet("Success");
				}
			}
		else
			getPassword(saveUserID,KeyIn,2);									
		if(checkTimeout(20)){
			Nokia5110Cls();
			clsPassword();
			DisplayStatus=0;
			}
		if(DisplayStatus!=7)KeyIn=0;//
		if(DisplayStatus==6)Nokia5110Cls();		
	}
	if(DisplayStatus==8)
	{
		Nokia5110WriteStrCH(0,1,12,4,25,0);//Ҫ�鿴��
		Nokia5110WriteStrCH(48,1,12,3,22,0);//�û�ID
		temp=User.id;
		i=getPassword(checkUserID,KeyIn,2);
		if(i==0)//
		{
			if(User.id==1)
			{
				Nokia5110Cls();
				KeyIn=0;
				DisplayStatus=2;
				refreshSet(NULL);
				return;
			}
			Nokia5110Cls();
			Nokia5110WriteStrEN(0,0,"UserID:");
			Nokia5110WriteAChar(48,0,User.id/10+'0');
			Nokia5110WriteAChar(54,0,User.id%10+'0');
			Nokia5110WriteStrEN(0,1,"Password:");
			Nokia5110WriteStrEN(0,2,User.password);
			Nokia5110WriteStrEN(0,3,"1.Delete");
			Nokia5110WriteStrEN(0,4,"2.ChangePWD");
			Nokia5110WriteStrEN(0,5,"Back      GoTo");
			while(!t)
			{
				KeyScan();
				if(KeyValue)KeyIn=KeyValue;
				t=getKeyVal();
				switch(t){//���°���������
					case '1':	
								At24c02Write(17*(User.id-1),0);
								getUser(temp);
								Nokia5110Cls();
								KeyIn=0;
								DisplayStatus=2;
								refreshSet("Success");
								break;//ɾ���û�
					case '2':	
								DisplayStatus=3;
								Nokia5110Cls();
								refreshSetPWD();
								Nokia5110WriteStrCH(12,4,12,5,17,0);
								KeyIn=0;
								break;//��������
					case '*':
								KeyIn=0;
								getUser(temp);
								Nokia5110Cls();
								break;//����
					case '#':
								Nokia5110Cls();
								KeyIn=0;
								DisplayStatus=2;
								refreshSet(NULL);
								break;//������˻�
					default:t=0;
				}
			}
		}
		else if(i==1)//�û�������
		{
			Nokia5110Cls();
			Nokia5110WriteStrEN(0,0,"This User is");
			Nokia5110WriteStrEN(0,1,"Nonentity!");
			Nokia5110WriteStrEN(0,5,"Back    Create");
			while(!t)
			{
				KeyScan();
				if(KeyValue)KeyIn=KeyValue;
				t=getKeyVal();
				switch(t){//���°���������
					case '*':					//����
								KeyIn=0;
								getUser(temp);
								Nokia5110Cls();
								break;
					case '#':				    //����
								Nokia5110Cls();
								KeyIn=0;
								if(strlen(Temp1)==1)
									User.id=Temp1[0]-'0';
								else if(strlen(Temp1)==2)
									User.id=(Temp1[0]-'0')*10+Temp1[1]-'0';
								DisplayStatus=7;
								break;
					default:
								t=0;
				}
			}
		}
		if(i==4)
		{
			DisplayStatus=6;
			getUser(temp);
			Nokia5110Cls();
			}
	}
}
void checkInOut()
{
	if(KeyA!=0&&KeyB==0)InOut=1;
	if(KeyB!=0&&KeyA==0)InOut=2;
}
void main() 
{	   
	init();
	while(1)  
	{
		Ds1302ReadTime();//��ȡ������
		KeyScan();
		checkInOut();
		display();
//��һ��ʹ��ʱ��ʼ��24C08
//����һ��IDΪ1����Ϊ��123456���ĳ����û�
		At24c02Write(0,1);
		At24c02Write(1,1);
		At24c02Write(2,'1');
		At24c02Write(3,'2');
		At24c02Write(4,'3');
		At24c02Write(5,'4');
		At24c02Write(6,'5');
		At24c02Write(7,'6');
		At24c02Write(8,'\0');
				At24c02Write(0,1);
		At24c02Write(1,1);
		At24c02Write(2,'1');
		At24c02Write(3,'2');
		At24c02Write(4,'3');
		At24c02Write(5,'4');
		At24c02Write(6,'5');
		At24c02Write(7,'6');
		At24c02Write(8,'\0');
//*////
//Nokia5110WriteAChar(0,5,ComeIn+'0');//���ڳ������
//Nokia5110WriteAChar(20,5,ComeOut+'0');//���ڳ������
	}
}

void TC_0() interrupt 1 using 1{ //�л��Ĵ����鵽1
	TH0=0x3C; //16λ�����Ĵ���T0��8λ������д���ֵ��
	TL0=0xB0; //16λ�����Ĵ���T0��8λ��0x3CB0 = 50mS��ʱ��
	Count++;
	operateDoor();
	if(KeyValue)TimeOut=0;
	if(Count%20==0)TimeOut++;//ÿ���һ
	if(Count>=50000)Count=0;
	if(Count2!=0)Count2--;
}

void INT_0() interrupt 0 using 0{ //�л��Ĵ����鵽0
	EX0=0;
	if(KeyA==0&&KeyB!=0&&Count2==0&&InOut==1){ComeIn++;Count2=12;TimeOut=0;InOut=0;}
	if(KeyB==0&&KeyA!=0&&Count2==0&&InOut==2){ComeOut++;Count2=12;TimeOut=0;InOut=0;}
	EX0=1;
}
void INT_1() interrupt 2 using 2{ //�л��Ĵ����鵽2

}