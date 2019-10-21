
#include "RebootSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../DBAccess/DBManager.h"
#include "../Communications/DeviceServer.h"
#include <vector>
#include <iostream>
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <IPExport.h>
#include <iphlpapi.h>
#include "../Communications/XMLSend.h"
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"wsock32.lib")
using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO

RebootSetTask::RebootSetTask() : DeviceIndependentTask()
{

}

RebootSetTask::RebootSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RebootSetTask::~RebootSetTask()
{
}
DWORD RebootSetTask::DoReboot() 
{
	BOOL bResult = TRUE;
	HANDLE hToken = INVALID_HANDLE_VALUE;
	TOKEN_PRIVILEGES tkp;
	DWORD dwStatus = ERROR_SUCCESS;

	//Get a token for this process.

	bResult = OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken);

	if (!bResult) goto exit;

	bResult = LookupPrivilegeValue(
		NULL,
		SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	if (!bResult) goto exit;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	bResult = AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tkp,
		0,NULL,
		0);

	if (!bResult) goto exit;

	bResult = ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);

exit:

	if (!bResult)
	{
		dwStatus = GetLastError();
	}

	if (hToken != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hToken);
	}

	return dwStatus;

}

void RebootSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]前端重启任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	XmlParser parser( strStandardXML.c_str() );
	if (GetVersion() == "3.0")
	{
		string  MsgID;
		DBMANAGER::instance()->GetMsgID(MsgID);
		std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
		std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
		string Surl = DeviceServerIP+":"+DeviceServerPort;

		string rebootReport,Type, ChassisID, Position, ModuleID, Index,strRet; 
		pXMLNODE pRebootNode = parser.GetNodeFromPath("Msg");
		pXMLNODELIST pRebootList = parser.GetNodeList(pRebootNode);
		for (int i=0; i<pRebootList->Size(); i++)
		{
			pXMLNODE pInfoNode = parser.GetNextNode(pRebootList);
			parser.GetAttrNode(pInfoNode,"Type",Type);
			parser.GetAttrNode(pInfoNode,"Report",rebootReport);
			parser.GetAttrNode(pInfoNode,"ChassisID",ChassisID);
			parser.GetAttrNode(pInfoNode,"Position",Position);
			parser.GetAttrNode(pInfoNode,"ModuleID",ModuleID);
			parser.GetAttrNode(pInfoNode,"Index",Index);
			sDeviceInfo deviceInfo;
			PROPMANAGER::instance()->GetDeviceInfo(ChassisID,Position,ModuleID,deviceInfo);
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(ChassisID),deviceInfo.s_deviceip,StrUtil::Str2Int(deviceInfo.s_deviceport));
			if (Type == "1")
			{
				if (rebootReport == "RebootReport")
				{
					SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
					DeleteFile("C:/vscttb/RebootReport.xml");//删除文件 
				}
				else
				{
					pXMLNODE rootNode = parser.GetRootNode();
					parser.SetAttrNode("Type", string("MonDown"), rootNode);					
					parser.SetAttrNode("Type", string("1"), pInfoNode);
					parser.SetAttrNode("Report", string("RebootReport"), pInfoNode);
					parser.SaveAsFile("C:/vscttb/RebootReport.xml");
					OSFunction::ExitProcess( "执行前端重启任务..." );
				}
			}
			else if(Type == "2")
			{
				if (deviceInfo.ucharBoardType == 3 || deviceInfo.ucharBoardType == 204)
				{

					DeviceServerIP = DeviceServerIP.substr(DeviceServerIP.find(".")+1);
					string DeviceChassisID = DeviceServerIP.substr(0,DeviceServerIP.find("."));
					DeviceChassisID =  StrUtil::Int2Str((StrUtil::Str2Int(DeviceChassisID) - 16));
					DeviceServerIP = DeviceServerIP.substr(DeviceServerIP.find(".")+1);
					string DevicePosition = DeviceServerIP.substr(0,DeviceServerIP.find("."));
					//if (DeviceChassisID == ChassisID && DevicePosition == Position)
					{
						if (rebootReport == "RebootReport")
						{
							SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
							DeleteFile("C:/vscttb/RebootReport.xml");//删除文件 
						}
						else if (rebootReport == "")
						{
							pXMLNODE rootNode = parser.GetRootNode();
							parser.SetAttrNode("Type", string("MonDown"), rootNode);
							parser.SetAttrNode("Type", string("2"), pInfoNode);
							parser.SetAttrNode("Report", string("RebootReport"), pInfoNode);
							parser.SaveAsFile("C:/vscttb/RebootReport.xml");
							ACE_DEBUG ((LM_DEBUG,"(%T| %t) 服务器即将重启...\n"));
							DoReboot();
						}
					}
					/*else
					{
						SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));
						string strXML = TranslateUpXML::TranslateRebootSetToOther(this, strStandardXML);
						string strUrl = "http://10.1.105.129:5061";
						XMLSend::SendXML(strUrl,strXML);
					}*/
					
				}
				else
				{
					string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
					cmd += "<Msg Version=\"1.0\" MsgID=\"";
					cmd += MsgID;
					cmd += "\" SrcUrl=\"http://";
					cmd += Surl;
					cmd += "\">";
					cmd += "<Type>Restart</Type>";
					cmd += "<Channel>";
					cmd += "All";
					cmd += "</Channel>";
					cmd += "</Msg>";
					pDeviceAccess->SendTaskMsg(cmd,strRet);
					SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
					//while(TRUE)
					//{
					//	Sleep(1000);
					//	IPAddr DestIp ;
					//	IPAddr SrcIp = 0; 
					//	ULONG MacAddr[2];       
					//	ULONG PhysAddrLen=6; 
					//	DestIp=inet_addr(deviceInfo.s_deviceip.c_str());
					//	DWORD dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen); 
					//	if(dwRetVal == 0)
					//	{
					//		SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
					//		break;
					//	}
					//}
				}
			}
			else if (Type == "3")
			{
				string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
				cmd += "<Msg Version=\"1.0\" MsgID=\"";
				cmd += MsgID;
				cmd += "\" SrcUrl=\"http://";
				cmd += Surl;
				cmd += "\">";
				cmd += "<Type>Restart</Type>";
				cmd += "<Channel>";
				cmd += ModuleID;
				cmd += "</Channel>";
				cmd += "</Msg>";
				pDeviceAccess->SendTaskMsg(cmd,strRet);
				SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
				//while(TRUE)
				//{
				//	Sleep(1000);
				//	IPAddr DestIp ;
				//	IPAddr SrcIp = 0; 
				//	ULONG MacAddr[2];       
				//	ULONG PhysAddrLen=6; 
				//	DestIp=inet_addr(deviceInfo.s_deviceip.c_str());
				//	DWORD dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen); 
				//	if(dwRetVal == 0)
				//	{
				//		SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
				//		break;
				//	}
				//}
			}
			else if (Type == "4")
			{
				string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
				cmd += "<Msg Version=\"1.0\" MsgID=\"";
				cmd += MsgID;
				cmd += "\" SrcUrl=\"http://";
				cmd += Surl;
				cmd += "\">";
				cmd += "<Type>Restart</Type>";
				cmd += "<Channel>";
				cmd += Index;
				cmd += "</Channel>";
				cmd += "</Msg>";
				pDeviceAccess->SendTaskMsg(cmd,strRet);
				SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
				//while(TRUE)
				//{
				//	Sleep(1000);
				//	IPAddr DestIp ;
				//	IPAddr SrcIp = 0; 
				//	ULONG MacAddr[2];       
				//	ULONG PhysAddrLen=6; 
				//	DestIp=inet_addr(deviceInfo.s_deviceip.c_str());
				//	DWORD dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen); 
				//	if(dwRetVal == 0)
				//	{
				//		SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
				//		break;
				//	}
				//}
			}
			/*else
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 服务器即将重启...\n"));
				DoReboot();
			}*/
		}	
	}
	else
	{
		pXMLNODE setNode = parser.GetNodeFromPath("Msg/RebootSet");
		std::string strType, strIndex, rebootReport;
		parser.GetAttrNode(setNode, "Type", strType);
		parser.GetAttrNode(setNode, "Index", strIndex);
		parser.GetAttrNode(setNode, "Report", rebootReport);
		if (rebootReport == "RebootReport")
		{
			SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
			DeleteFile("C:/vscttb/RebootReport.xml");//删除文件 
		}
		else if (rebootReport == "")
		{
			pXMLNODE rootNode = parser.GetRootNode();
			parser.SetAttrNode("Type", string("MonDown"), rootNode);
			parser.SetAttrNode("Report", string("RebootReport"), setNode);
			parser.SaveAsFile("C:/vscttb/RebootReport.xml");
			if (strType == "6")		//硬件重启
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 服务器即将重启...\n"));
				//std::string cmd="shutdown -r -t 0";
				//system(cmd.c_str());
				DoReboot();
			}
			else	//重启前端程序
			{
				OSFunction::ExitProcess( "执行前端重启任务..." );
			}
		}
		else
		{
			SetRetValue(RUN_FAILED);
			SendXML(TranslateUpXML::TranslateRebootSet(this, strStandardXML));//发送xml
		}
	}

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]前端重启任务停止 !\n",DeviceID));
}

string RebootSetTask::GetTaskName()
{
	return std::string("前端重启任务");
}
std::string RebootSetTask::GetObjectName()
{
	return std::string("RebootSetTask");
}

#endif