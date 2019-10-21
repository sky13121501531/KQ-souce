
#include "TsSenderMgr.h"
#include "../Foundation/PropManager.h"
#include <list>
#include <iostream>

using namespace std;
int g_icodecchan = -1;
TsSenderMgr::TsSenderMgr()
{
}

TsSenderMgr::~TsSenderMgr()
{
}
int TsSenderMgr::Start()
{
	int idevnum = PROPMANAGER::instance()->GetCoderDevNum();
	g_icodecchan = idevnum;//用于1路转码
	idevnum = idevnum + 1;//用于1路转码
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		idevnum = 100;
	}
	//初始化TS数据获得线程
	for (int i = 0;i < idevnum;i++)
	{
		TsSender* tsSender = new TsSender(i);
		if (tsSender != NULL)
		{
			TsSenderMap.insert(make_pair(i,tsSender));
			tsSender->Start();
		}
	}
	//创建轮播通道
	//wz_0217
	std::list<int> virdevlist; //虚拟通道列表
	PROPMANAGER::instance()->GetVirDevList(virdevlist);
	if (virdevlist.empty())
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取的轮播虚拟通道为空 !\n"));
	}

	std::list<int>::iterator iter = virdevlist.begin();
	for(;iter != virdevlist.end();++iter)//每一个虚拟通道，创建一个tssender
	{
		int deviceId = *iter;
		TsSender* tsSender = new TsSender(deviceId);
		if (tsSender != NULL)
		{
			TsSenderMap.insert(make_pair(deviceId,tsSender));
			tsSender->Start();
		}
	}
	//wz_0217

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 视频数据发送线程池启动[%d]个线程 !\n",(int)TsSenderMap.size()));
	return 0;
}

int TsSenderMgr::Stop()
{	
	//停止所有的取数据线程
	std::map<int,TsSender*>::iterator ptr = TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->second != NULL)
		{
			ptr->second->Stop();
			delete ptr->second;
			ptr->second = NULL;
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 视频数据发送线程池停止[%d]个线程 !\n",(int)TsSenderMap.size()));
	TsSenderMap.clear();
	return 0;
}

void TsSenderMgr::SetSendSwitch(int deviceid,bool sendswitch)
{
	map<int,TsSender*>::iterator ptr= TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->first == deviceid && ptr->second != NULL)
		{
			(ptr->second)->SetSendSwitch(sendswitch);
			break;
		}
	}
}

bool TsSenderMgr::AddClient(int deviceid,sVedioUserInfo newclient)
{
	map<int,TsSender*>::iterator ptr= TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->first == deviceid && ptr->second != NULL)
		{
			(ptr->second)->AddClient(newclient);
			break;
		}
	}
	return true;
}

bool TsSenderMgr::HasClient(int deviceid)
{
	map<int,TsSender*>::iterator ptr= TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->first == deviceid && ptr->second != NULL)
		{
			return (ptr->second)->HasClient();
		}
	}
	return false;
}

bool TsSenderMgr::GetAllClient(std::vector<sVedioUserInfo>& uservec)
{
	map<int,TsSender*>::iterator ptr= TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->second != NULL)
		{
			(ptr->second)->GetAllClient(uservec);
		}
	}
	return true;
}

bool TsSenderMgr::StopAllClient(int deviceid)
{
	map<int,TsSender*>::iterator ptr= TsSenderMap.begin();
	for (;ptr!=TsSenderMap.end();++ptr)
	{
		if (ptr->first == deviceid && ptr->second != NULL)
		{
			(ptr->second)->StopAllClient();
		}
	}
	return true;
}


