#include "InfraredRemoteControlQueryTask.h"
#include "TranslateUpXML.h"
#include "../Foundation/C99IRsetSerial.h"
#include "../Foundation/TypeDef.h"

InfraredRemoteControlQuery::InfraredRemoteControlQuery()
{

}

InfraredRemoteControlQuery::InfraredRemoteControlQuery(std::string strXML) :
	DeviceIndependentTask(strXML)
{

}

InfraredRemoteControlQuery::~InfraredRemoteControlQuery()
{

}

void InfraredRemoteControlQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务执行 !\n"));

	bRun = true;
	SetRunning();


	XmlParser xml(strStandardXML.c_str());
	pXMLNODE queryNode = xml.GetNodeFromPath("Msg/InfraredRemoteControlQuery");
	string sVirtualKey;
	xml.GetAttrNode(queryNode,"VirtualKey",sVirtualKey);
	if (sVirtualKey == "")
	{
		SetRetValue(RUN_FAILED);
		SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
		SetFinised();//任务停止
		SetExpired();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务 虚拟键为空值 !\n"));
		return;
	}

	map<int, string> mapVirtualKeys;
	string strNumKey = sVirtualKey.substr(0, 3);
	int icount = 0;
	if (strNumKey == "NUM")
	{
		string str = sVirtualKey.substr(3, sVirtualKey.length());
		int iStrLen = str.length();
		while(str.length() > 0)
		{
			string str1 = strNumKey + str.substr(0, 1);
			mapVirtualKeys.insert(make_pair(icount, str1));
			icount++;
			str = str.substr(1, str.length());
		}
		icount = 0;
	}
	else
	{
		mapVirtualKeys.insert(make_pair(icount, sVirtualKey));
	}

	HDMIInfo* pHDMIInfo = PROPMANAGER::instance()->GetHDMIInfo();
	
	for (int i = 0; i < mapVirtualKeys.size(); i++)
	{
		map<int, string>::iterator ite = mapVirtualKeys.find(i);
		if (ite != mapVirtualKeys.end())
		{
			string strVirKey = (*ite).second;
			VirtualKeyInfo* pVirtualKeyInfo = PROPMANAGER::instance()->GetVirtualKeyInfo(strVirKey);
			if (pVirtualKeyInfo == NULL)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务失败 没有找到虚拟键[ %s ] !\n", strVirKey.c_str()));
				return;
			}

			C99IRsetSerial* pC99 = new C99IRsetSerial();;
			bool bRet = false;
			bRet = pC99->InitSerialDevice(pHDMIInfo->s_COMPort);
			if (!bRet)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务 串口打开失败 !\n"));
				return;
			}
			unsigned char cCmdCode[2] = {0};
			cCmdCode[0] = pHDMIInfo->c_IDCode;
			cCmdCode[1] = 0x02;		//0x02 单个红外发射指令
			int iContestsLen = 0;
			unsigned char* pContests = pC99->char2mem((unsigned char*)(pVirtualKeyInfo->s_Index.c_str()), 
				pVirtualKeyInfo->s_Index.length(), iContestsLen);
			unsigned char* pFrames = pC99->GetSendBuf(cCmdCode, pContests, iContestsLen);
			delete []pContests;
			pContests = NULL;
			int iFramesLen = pC99->ReadFrameLen(pFrames);

			unsigned char* pRecvFrames = NULL;
			int iRecvFramesLen = 0;
			bRet = false;
			bRet = pC99->SendSerial(pFrames, iFramesLen, pRecvFrames, iRecvFramesLen);
			pC99->CloseSerial();
			delete pC99;
			pC99 = NULL;
			delete []pFrames;
			pFrames = NULL;
			if (!bRet)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务 串口数据接收失败 !\n"));
				return;
			}
			if (pRecvFrames == NULL)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务失败 !\n"));
				return;
			}
			int ilen = pC99->ReadFrameLen(pRecvFrames);
			if (ilen != iRecvFramesLen)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务失败 !\n"));
				return;
			}
			unsigned char cStateCmd = *(pRecvFrames+6);
			delete []pRecvFrames;
			pRecvFrames = NULL;
			if (cStateCmd != 0x00)
			{
				SetRetValue(RUN_FAILED);
				SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
				SetFinised();//任务停止
				SetExpired();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务失败 !\n"));
				return;
			}
			Sleep(pHDMIInfo->i_IntervalTime+1);//红外命令发送间隔 毫秒级
		}
	}

	SetRetValue(RUN_SUCCESS);
	SendXML( TranslateUpXML::TranslateInfraredRemoteControlQuery(this) ); //发送到前端
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 红外遥控器 虚拟键任务停止 !\n"));
	return;
}

std::string InfraredRemoteControlQuery::GetObjectName()
{
	return std::string("InfraredRemoteControlQuery");
}

std::string InfraredRemoteControlQuery::GetTaskName()
{
	return std::string("红外遥控器 虚拟键任务");
}