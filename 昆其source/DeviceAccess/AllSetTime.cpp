#include "AllSetTime.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
bool isSendTime = false;
AllSetTime::AllSetTime()
{
}
AllSetTime::~AllSetTime()
{

}
int AllSetTime::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int AllSetTime::open( void* )
{
	bFlag = true;
	activate();
	return 0;
}

int AllSetTime::svc()
{
	while(bFlag)
	{
		Sleep(100);
		TimeSet(40,600);
	}
	return 0;
}

int AllSetTime::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}

void AllSetTime::TimeSet(int First_time,int msec_time)
{
	if (msec_time < 0)
	{
		msec_time = 300;
	}
	time_t start;
	start = time(0);
	double totaltime = 0;
	time_t m_SetCardStartTime = time(0);
	while (1)
	{
		if ((time(0) - start > msec_time)||((time(0)-m_SetCardStartTime > First_time)&&!isSendTime))
		{
			if(!isSendTime)
			{
				isSendTime = true;
			}
			SendTime();
			break;
		}
	}
}
void AllSetTime::SendTime()
{
	int Monnum = PROPMANAGER::instance()->GetMonitorDevNum();
	for(int i=0;i<Monnum;i++)
	{
		sDeviceInfo Devinfo;
		PROPMANAGER::instance()->GetDevMonitorInfo(i,Devinfo);
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(Devinfo.s_deviceindex),Devinfo.s_deviceip,StrUtil::Str2Int(Devinfo.s_deviceport));
		string strSetTimeCMD = "POST /device/info  HTTP/1.1  \r\n\r\n";
		strSetTimeCMD+="time=";
		strSetTimeCMD+=StrUtil::Long2Str(time(0) + 1);
		string strSetTimeRet;
		pDeviceAccess->SendTaskMsg(strSetTimeCMD,strSetTimeRet);
		if(strSetTimeRet!="")
		{
			cout <<"/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/-Set Time Success ip = ["<<Devinfo.s_deviceip<<"]"<<endl;
		}
		delete pDeviceAccess;
	}
}

