
#include "ClientInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "ace/Synch.h"
#include "ace/OS.h"
#include <string>
#include "../Foundation/XmlParser.h"
#include "../Foundation/PropManager.h"

ClientInfoMgr::ClientInfoMgr()
{
	bFlag = false;
}

ClientInfoMgr::~ClientInfoMgr()
{

}
int ClientInfoMgr::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int ClientInfoMgr::open(void*)
{
	bFlag = true;
	activate();

	return 0;
}

int ClientInfoMgr::svc()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 音视频用户管理线程开始执行 !\n"));
	
	string keeplive = "ok!";
	while (bFlag)
	{
		OSFunction::Sleep(0,500);
		ACE_Time_Value TimeOut(1);
		ACE_Guard<ACE_Thread_Mutex> guard(ClientMutex);
		try 
		{
			std::vector<sVedioUserInfo>::iterator ptr = ClientVec.begin();
			for (;ptr!=ClientVec.end();++ptr)
			{
				ssize_t RecvCount = (*ptr).client.send(keeplive.c_str(),keeplive.length(),&TimeOut);
				if(RecvCount == SOCKET_ERROR)
				{
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) 音视频用户退出[%s:%s]!\n",(*ptr).Name.c_str(),(*ptr).IP.c_str()));
					ClientVec.erase(ptr);
					break;
				}
			}			
		}
		catch (...)
		{
		}
	}
	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 音视频用户管理线程停止 !\n"));
	return 0;
}

bool ClientInfoMgr::AddUser(ACE_SOCK_Stream client,string& xml)
{
	sVedioUserInfo tempUserInfo;
	tempUserInfo.client = client;

	XmlParser parser(xml.c_str());
	std::string DvbType,DstCode;

	pXMLNODE rootNode = parser.GetRootNode();
	parser.GetAttrNode(rootNode,"Type",DvbType);
	parser.GetAttrNode(rootNode,"DstCode",DstCode);
	pXMLNODE ClientNode=parser.GetNodeFromPath("Msg/ClientInfo/Client");
	parser.GetAttrNode(ClientNode,"URL",tempUserInfo.URL);
	parser.GetAttrNode(ClientNode,"IP",tempUserInfo.IP);
	parser.GetAttrNode(ClientNode,"UserName",tempUserInfo.Name);
	parser.GetAttrNode(ClientNode,"Priority",tempUserInfo.Priority);
	parser.GetAttrNode(ClientNode,"CenterCode",tempUserInfo.CenterCode);

	string URL =tempUserInfo.URL;
	size_t pos=URL.find_last_of("/");
	string deviceid;
	if (pos!=std::string::npos)
	{
		deviceid=URL.substr(pos);
	}
	if (deviceid!="")
	{
		tempUserInfo.DeviceID=deviceid.substr(1);
	}
	else
	{
		return false;
	}
	if(DvbType=="TVMonUp"&&(DstCode==PROPMANAGER::instance()->GetDefSrcCode(ATV)))
	{
		tempUserInfo.DvbType = ATV;
	}
	else if(DvbType=="TVMonUp"&&(DstCode==PROPMANAGER::instance()->GetDefSrcCode(CTV)))
	{
		tempUserInfo.DvbType = CTV;
	}
	else if(DvbType=="RadioUp")
	{
		tempUserInfo.DvbType = RADIO;
	}
	else if(DvbType=="AMUp")
	{
		tempUserInfo.DvbType = AM;
	}
	else if (DvbType=="MonUp")
	{
		tempUserInfo.DvbType = DVBC;
	}

	ACE_Guard<ACE_Thread_Mutex> guard(ClientMutex);

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 音视频用户连接[%s:%s]!\n",tempUserInfo.Name.c_str(),tempUserInfo.IP.c_str()));
	ClientVec.push_back(tempUserInfo);
	return 0;
}

bool ClientInfoMgr::GetUser(enumDVBType DvbType,std::vector<sVedioUserInfo>& uservec)
{
	ACE_Guard<ACE_Thread_Mutex> guard(ClientMutex);
	std::vector<sVedioUserInfo>::iterator ptr = ClientVec.begin();
	for (;ptr!=ClientVec.end();++ptr)
	{
		if ((*ptr).DvbType == DvbType)
		{
			uservec.push_back(*ptr);
		}
	}
	return true;
}
bool ClientInfoMgr::StopUser(int deviceid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(ClientMutex);
	std::vector<sVedioUserInfo>::iterator ptr = ClientVec.begin();
	for (;ptr!=ClientVec.end();)
	{
		if ((*ptr).DeviceID == StrUtil::Int2Str(deviceid))
		{
			ClientVec.erase(ptr++);
			ptr = ClientVec.begin();
			continue;
		}
		ptr++;
	}
	return true;
}
int ClientInfoMgr::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}