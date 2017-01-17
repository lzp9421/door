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
uchar PasswordCount=0;//密码计数器
uchar Temp1[15];//输入的密码
uchar Temp2[15];//输入的密码
uchar DisplayStatus=0;//0正常显示，1输入密码，2准设置状态，3准更改密码，4更改密码，5更改时间
//uchar Password[15]="12345678";
uchar TimeOut;//超时判断计数器
uchar Count=0,Count2=0;
char PasswordERR=0; //密码错误次数
struct users{
	unsigned char id;
	unsigned char type;//0为普通用户，1为超级账户；
	unsigned char password[15];
	//struct users *next;
}User={0,0,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};
//***********************************
void initINT0(){
	EA=1;	//中断总开关
	EX0=1; //允许外部中断0中断
	IT0=1;//1：下沿触发  0：低电平触发
}
void initINT1(){
	EA=1;	//中断总开关
	EX1=1; //允许外部中断1中断    
	IT1=1; //1：下沿触发  0：低电平触发
}
void initTC0(){
	TMOD=0x11; //高4位控制T/C1 [ GATE，C/T，M1，M0，GATE，C/T，M1，M0 ]
	EA=1;	//中断总开关
	TH0=0x3C; //16位计数寄存器T0高8位
	TL0=0xB0; //16位计数寄存器T0低8位（0x3CB0 = 50mS延时）
	ET0=1; //T/C0中断开关
	TR0=1; //T/C0启动开关
}
void init()
{	
	Lock=0;//锁不打开
	Nokia5110Init();//初始化液晶 
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

		Nokia5110WriteStrCH(16,1,12,2,0,8);//显示“星期”
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

uchar getLocation()//定位
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
	Nokia5110Cls();//清屏
		
	Nokia5110WriteStrEN(0,0,"UserID: ");
	Nokia5110WriteAChar(48,0,User.id/10+'0');
	Nokia5110WriteAChar(54,0,User.id%10+'0');
	Nokia5110WriteStrEN(0,1,"1.ChangePWD");
	if(User.type==1)Nokia5110WriteStrEN(0,2,"2.SetTime");
	if(User.type==1)Nokia5110WriteStrEN(0,3,"3.OtherUsers");
	if(str==NULL){
		h=TIME[2]/0x10*10+TIME[2]%0x10;//时
	if(TIME[5]%0x10!=6&&h>=6&&h<23||TIME[5]%0x10==6&&h>=5&&h<24){//平时6：00~23：00周六5：00~24：00
		Nokia5110WriteStrEN(0,5,"     OPEN     ");//在指定允许开锁时间内开锁，提示OPEN
		Lock=1;//开锁
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
	DisplayStatus=3;//更改密码
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
		Nokia5110WriteStrEN(0,4,"              ");//清除显示缓冲区4行
		Nokia5110WriteStrEN(0,5,"              ");//清除显示缓冲区5行
		KeyIn=0;//重置键值
		PasswordCount=0;//还原密码计数器
			
		//超退时后DisplayStatus置0返回初始状态
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
		if(KeyIn<='9'&&KeyIn>='0'&&PasswordCount<10)//键值为数字
		{
			switch(PasswordCount)
			{
				case 0:					//年十位
						Temp1[2]=KeyIn;
						PasswordCount++;
						break;
				case 1:					//年个位
						Temp1[3]=KeyIn;
						PasswordCount++;
						break;
				case 2:					//月十位
						if(KeyIn!='0'&&KeyIn!='1')break;//输入只能是0或1
						Temp1[5]=KeyIn;
						PasswordCount++;
						break;
				case 3:					//月个位
						if(Temp1[5]!='0'&&KeyIn>'2')break;//月十位为1时输入只能小于二
						Temp1[6]=KeyIn;
						PasswordCount++;
						break;
				case 4:					//日十位
						if(DayInOneMonth(Temp1[2]*10+Temp1[3]+1472,Temp1[5]*10+Temp1[6]-528)/10+'0'<KeyIn)break;//天数十位不能大于当前月份天数的十位
						Temp1[8]=KeyIn;
						PasswordCount++;
						break;
				case 5:					//日个位
						if(DayInOneMonth(Temp1[2]*10+Temp1[3]+1472,Temp1[5]*10+Temp1[6]-528)<Temp1[8]*10+KeyIn-528)break;//天数个位不能大于当前月份天数的个位
						Temp1[9]=KeyIn;
						PasswordCount++;
						break;
				case 6:					//小时十位
						if(KeyIn>'2')break;//输入只能小于等于6
						Temp2[0]=KeyIn;
						PasswordCount++;
						break;
				case 7:					//小时个位
						if(Temp2[0]>='2'&&KeyIn>'3')break;//输入只能是0或1
						Temp2[1]=KeyIn;
						PasswordCount++;
						break;
				case 8:					//分钟十位
						if(KeyIn>'5')break;//输入只能小于6
						Temp2[3]=KeyIn;
						PasswordCount++;
						break;
				case 9:					//分钟个位
						Temp2[4]=KeyIn;
						PasswordCount++;
						break;
			}
		 	//记录数字
		}
		else if(KeyIn=='*')
		{
			if(PasswordCount<=1)//密码不足一位返回上一状态
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
			temp=(*CtrlFun)();//按下确认键进入密码检查
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
		if(KeyValue==0){//按下按键后松手
			if(KeyIn=='*')//键值为数字
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
		return(1);//密码设置失败
	}
	else 
	{
		PasswordERR=0;
		DisplayStatus=2;//进入准设置状态
		return(0);//密码设置成功
		//refreshSet();//刷新设置界面图像
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
uchar getUser(uchar id)//组建一个用户
{
	uchar i;
	if(id!=At24c02Read(17*(id-1)))return 1;//用户不存在返回1
	else{
		User.id=id;
		User.type=At24c02Read(17*(id-1)+1);
		for(i=0;i<15;i++)
			User.password[i]=At24c02Read(17*(id-1)+2+i);//在24c02储存的用户ID存在时读入该用户
		if(strlen(User.password)>14)return 1;//密码不能没有结束标识，防止用户名误判
		}
	return 0;//成功组建用户返回0
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
		return(1);//用户创建完成
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
		switch(t){//按下按键后松手
			case '1':
						User.id=id;
						Nokia5110Cls();
						KeyIn=0;
						break;//键值为数字
			case '2':
			case '*':
						KeyIn=0;
						User.id=1+'0';
						Nokia5110Cls();
						break;
			default:t=0;
			}
		}
		return(0);//密码设置成功
		//refreshSet();//刷新设置界面图像
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
		return(1);//密码设置失败
	}
	else 
	{
		PasswordERR=0;
		switch(DisplayStatus){
			case 1:
					DisplayStatus=0;
					break;//进入输入密码状态
		}
		return(0);//密码设置成功
		//refreshSet();//刷新设置界面图像
	}
}

uchar checkTimeout(int time)
{
	if(KeyValue)TimeOut=0;//3秒无操作返回正常显示状态
	if(TimeOut>=time){DisplayStatus=0;PasswordERR=0;PasswordCount=0;Lock=0;User.id=0;return(1);}
	else return(0);
}

uchar getPassword(uchar(*CtrlFun)(),uchar ch,uchar size)
{	//Nokia5110WriteAChar(6,5,KeyIn);
	uchar temp=0xff;
	if(KeyIn)
		if(KeyValue==0){//按下按键后松手
			if(KeyIn<='9'&&KeyIn>='0'&&PasswordCount<size)//键值为数字
			{
				if(PasswordERR&&PasswordCount==0){Nokia5110WriteStrEN(0,4,"              ");Nokia5110WriteStrEN(0,5,"              ");}//清除显示缓冲区5行
				Nokia5110WriteAChar(PasswordCount*6,5,ch);//显示一个“*”
			 	Temp1[PasswordCount++]=KeyIn;//记录数字
			}
			
			else if(KeyIn=='*')
			{
				if(PasswordCount<=1)//密码不足一位返回上一状态
				{
					DisplayStatus--;
					clsPassword();
					return(4);
				}
				Temp1[--PasswordCount]=0;//按下删除键删除一个数字
				Nokia5110WriteAChar(PasswordCount*6,5,' ');//删去一个“*”
			}
			else if(KeyIn=='#')
			{
				Temp1[PasswordCount]='\0';//密码结束标记
				clsPassword();
				temp=(*CtrlFun)();//按下确认键进入密码检查
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
			RePassword=1;//进入准设置状态
			strcpy(Temp2,Temp1);
			Nokia5110WriteStrCH(12,4,12,4,15,0);//显示“再次输入”
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
			return(3);//密码设置失败
		}
		else 
		{
			PasswordERR=0;
			DisplayStatus=2;//进入密码准设置状态
			savePassword(Temp1);
			return(0);//密码设置成功
		}
	}
	return(0xfe);
}							  
void display()
{	
	uchar temp,t=0,i;
	if(KeyValue)KeyIn=KeyValue;
	if(DisplayStatus==0)//正常显示状态下的按键操作
	{
		refreshTime();
		if(KeyValue<='9'&&KeyValue>='0'){DisplayStatus=1;Nokia5110WriteStrEN(0,4,"              ");Nokia5110WriteStrEN(0,5,"              ");}//进入密码输入状态，清除“请输入密码”
		if(DisplayStatus==0)if(User.id)Nokia5110WriteStrCH(12,4,12,5,9,0);
							else{
								Nokia5110WriteStrCH(6,4,12,3,9,0);
								Nokia5110WriteStrCH(42,4,12,3,22,0);
								}//正常状态下显示“请输入用户ID”//正常状态下显示“请输入密码”	
		if(User.id&&checkTimeout(5))clsPassword();//超时判断并在超时后清屏
	}
//----------------------------
	if(DisplayStatus==1)//按下0~9任意一键后进入密码输入状态
	{																					 
		refreshTime();
		User.id?getPassword(checkPassword,'*',15):getPassword(checkUserID,KeyIn,2);
		if(checkTimeout(5))clsPassword();//超时判断
		if(DisplayStatus==2)refreshSet(NULL);
	}
//-------------------------
	if(DisplayStatus==2)//准设置状态
	{
		//获取设置键值
		Nokia5110WriteStrEN(0,4,"IN:");Nokia5110WriteAChar(18,4,ComeIn/100+'0');Nokia5110WriteAChar(24,4,ComeIn%100/10+'0');Nokia5110WriteAChar(30,4,ComeIn%10+'0');
		Nokia5110WriteStrEN(42,4,"OUT:");Nokia5110WriteAChar(66,4,ComeOut/100+'0');Nokia5110WriteAChar(72,4,ComeOut%100/10+'0');Nokia5110WriteAChar(78,4,ComeOut%10+'0');
		switch(getKeyVal()){//按下按键后松手
			case '1':	
						Nokia5110Cls();
						refreshSetPWD();
						KeyIn=0;
						break;//键值为数字
			case '2':
						if(User.type==1){
							Nokia5110Cls();
							DisplayStatus=5;
							KeyIn=0;
							Time2Str();
							}break;//更改时间
			case '3':
						if(User.type==1){
							Nokia5110Cls();
							DisplayStatus=6;
							KeyIn=0;
							}break;//更改其他账户
		//	default:KeyIn=0;
			}
		//if(isBack()==1){checkTimeout(0);Nokia5110Cls();}
		if(DisplayStatus==3)Nokia5110WriteStrCH(12,4,12,5,17,0);//设置密码状态下显示“输入新密码”
		if(checkTimeout(10))Nokia5110Cls();//超时判断并在超时后清屏
		if(DisplayStatus!=2)KeyIn=0;//
	}
//---------------------------
	if(DisplayStatus==3)//准设置密码状态
	{
		if(isBack()==1)DisplayStatus=2;
		if(KeyValue<='9'&&KeyValue>='0'){DisplayStatus=4;clsPassword();}//进入密码输入状态，清除“请输入密码”	
		if(checkTimeout(10))Nokia5110Cls();//超时判断并在超时后清屏
		if(DisplayStatus==2)refreshSet(NULL);
	}
	if(DisplayStatus==4)//按下0~9任意一键后进入密码设置状态
	{																					 
		if(checkTimeout(20)){
			clsPassword();
			Nokia5110Cls();
			}//超时判断
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
		//获取设置键值
		switch(getKeyVal()){//按下按键后松手
			case '1':
						DisplayStatus=7;
						User.id+='0';
						Nokia5110Cls();
						KeyIn=0;
						break;//创建用户
			case '2':
						DisplayStatus=8;
						Nokia5110Cls();
						KeyIn=0;
						break;//更改其他账户
			case '*':
						DisplayStatus=2;
						KeyIn=0;
						break;//键值为数字
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
		}//正常状态下显示“请输入用户ID”//正常状态下显示“请输入密码”	
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
		Nokia5110WriteStrCH(0,1,12,4,25,0);//要查看的
		Nokia5110WriteStrCH(48,1,12,3,22,0);//用户ID
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
				switch(t){//按下按键后松手
					case '1':	
								At24c02Write(17*(User.id-1),0);
								getUser(temp);
								Nokia5110Cls();
								KeyIn=0;
								DisplayStatus=2;
								refreshSet("Success");
								break;//删除用户
					case '2':	
								DisplayStatus=3;
								Nokia5110Cls();
								refreshSetPWD();
								Nokia5110WriteStrCH(12,4,12,5,17,0);
								KeyIn=0;
								break;//更改密码
					case '*':
								KeyIn=0;
								getUser(temp);
								Nokia5110Cls();
								break;//返回
					case '#':
								Nokia5110Cls();
								KeyIn=0;
								DisplayStatus=2;
								refreshSet(NULL);
								break;//进入此账户
					default:t=0;
				}
			}
		}
		else if(i==1)//用户不存在
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
				switch(t){//按下按键后松手
					case '*':					//返回
								KeyIn=0;
								getUser(temp);
								Nokia5110Cls();
								break;
					case '#':				    //创建
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
		Ds1302ReadTime();//获取年月日
		KeyScan();
		checkInOut();
		display();
//第一次使用时初始化24C08
//建立一个ID为1密码为“123456”的超级用户
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
//Nokia5110WriteAChar(0,5,ComeIn+'0');//用于程序调试
//Nokia5110WriteAChar(20,5,ComeOut+'0');//用于程序调试
	}
}

void TC_0() interrupt 1 using 1{ //切换寄存器组到1
	TH0=0x3C; //16位计数寄存器T0高8位（重新写入初值）
	TL0=0xB0; //16位计数寄存器T0低8位（0x3CB0 = 50mS延时）
	Count++;
	operateDoor();
	if(KeyValue)TimeOut=0;
	if(Count%20==0)TimeOut++;//每秒加一
	if(Count>=50000)Count=0;
	if(Count2!=0)Count2--;
}

void INT_0() interrupt 0 using 0{ //切换寄存器组到0
	EX0=0;
	if(KeyA==0&&KeyB!=0&&Count2==0&&InOut==1){ComeIn++;Count2=12;TimeOut=0;InOut=0;}
	if(KeyB==0&&KeyA!=0&&Count2==0&&InOut==2){ComeOut++;Count2=12;TimeOut=0;InOut=0;}
	EX0=1;
}
void INT_1() interrupt 2 using 2{ //切换寄存器组到2

}