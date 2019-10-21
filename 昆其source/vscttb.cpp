#include "cmdLine.h"
#include "./Communications/CommunicationMgr.h"
#include "./BusinessProcess/BusinessIterfaceMgr.h"
#include "./BusinessProcess/BusinessLayoutMgr.h"
#include "./Communications/TsSenderMgr.h"
#include "./Communications/TSServer.h"
#include "./DeviceAccess/TsFetcherMgr.h"
#include "./DeviceAccess/ChanScanThread.h"
#include "./DeviceAccess/UdpRecvAlarmThreadMgr.h"
#include "./Foundation/OSFunction.h"
#include "./Foundation/StrUtil.h"
#include "./Foundation/PropManager.h"
#include "./Foundation/AppLog.h"
#include "./DBAccess/QualityDbManager.h"//指标数据存库
#include "./Alarm/AlarmMgr.h"
#include "ace/OS.h"
#include <ace/Thread.h>
#include <fstream>
#include <Windows.h>
#include "./Foundation/TimeUtil.h"
#include "./DeviceAccess/AllSetTime.h"
/******************山西*******************/
void DestroyMyWinClosedForCommand()
{
	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, false);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	DestroyMenu(hmenu);
	ReleaseDC(hwnd, NULL);
}
void Start()
{
	std::string RunInfo = std::string("程序启动 ") + VSCTTB_PROG_VERSION;
	std::string SysTitel = string("title ")+VSCTTB_PROG_VERSION;
	//读取xml文件中的内容
	std::vector<std::string>  vctXML;
	ifstream rebootRFile("C:/vscttb/RebootReport.xml");
	string strRebootReport = "";
	string str = "";
	while (getline(rebootRFile,str))
	{
		strRebootReport += str;
	}
	rebootRFile.close();
	//
	if(strRebootReport.size()>0)
	{
		vctXML.push_back(strRebootReport);
	}
	for (size_t i = 0;i<vctXML.size();i++)
	{
		std::string strXML = vctXML.at(i);
		ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
		memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
		mbXML->wr_ptr(strXML.length());

		if (mbXML != NULL)
		{
			ACE_Time_Value OutTime(ACE_OS::time(0)+1);
			COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mbXML,&OutTime);
			OSFunction::Sleep(0,500);
		}
	}
	//
    string SysRebootTimeFlag = string("T_Reboot:") + TimeUtil::GetCurDateTime();
    APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,SysRebootTimeFlag,LOG_OUTPUT_FILE);
	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("开始从数据库初始化配置信息!!"),LOG_OUTPUT_FILE);
	PROPMANAGER::instance()->InitInfoFromDB();
	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("从数据库初始化配置信息成功!!"),LOG_OUTPUT_FILE);

	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("开始检查需要的文件夹是否创建!!"),LOG_OUTPUT_FILE);
	CheckDirectoryExsit();	//检测必须文件夹是否存在，不存在，就创建它.
	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("检查文件夹完毕!!"),LOG_OUTPUT_FILE);
	//
	system(SysTitel.c_str());
	//
	PROPMANAGER::instance()->GetDeviceInfo();
	//
	TSFETCHERMGR::instance()->Start();
	TSSENDERMGR::instance()->Start();
	if (COMMUNICATIONMGR::instance()->Start() != 0)	//只判断网络相关的初始化，其他默认成功
	{
		OSFunction::ExitProcess("系统初始化(网络)失败");
	}
	BUSINESSITERFACEMGR::instance()->Start();
	BUSINESSLAYOUTMGR::instance()->Start();
	ALARMMGR::instance()->Start();	
	QUALITYDBMANAGER::instance()->Start();
	CHANSCANTHREAD::instance()->Start();
	TSSERVERMGR::instance()->CreateTsServer((char*)PROPMANAGER::instance()->GetRtspVideoIp().c_str(),StrUtil::Str2Int(PROPMANAGER::instance()->GetRtspVideoPort()));
	OSFunction::Sleep(0,50);
	cout<<endl<<"\n\t[GD-C1000]-:multiRF Wacth and Measure System Run."<<endl<<endl;
	LoadXMLTask();
	UDPRECVALARMTHREADMGR::instance()->Start();
	ALLSETTIME::instance()->Start();
}

void Stop()
{
	TSFETCHERMGR::instance()->Stop();
	TSSENDERMGR::instance()->Stop();
	COMMUNICATIONMGR::instance()->Stop();
	BUSINESSITERFACEMGR::instance()->Stop();
	BUSINESSLAYOUTMGR::instance()->Stop();
	CHANSCANTHREAD::instance()->Stop();
	UDPRECVALARMTHREADMGR::instance()->Stop();
	ALARMMGR::instance()->Stop();
	QUALITYDBMANAGER::instance()->Stop();
	ALLSETTIME::instance()->Stop();
	OSFunction::Sleep(0,500);
	cout<<endl<<"\t[GD-C1000]-:multiRF Wacth and Measure System is stopped."<<endl<<endl;
}

int main(int argc, char *argv[])
{

	//////////////////////////////////
    
	/////////////////////////////////
	CheckSoleProcess();
	//去除程序窗体关闭按钮---统一到命令关闭
	//DestroyMyWinClosedForCommand();
	string cmd;
	bool bRunning = false;

	PrintTitle();

	cout<<"-GD-C1000_CMD->"<<endl;

	OSFunction::Sleep(0,10);
	Start();
	bRunning = true;

	while(true)
	{
		cmd.clear();
		cin>>cmd;

		if(cmd == "run" && bRunning == true)
		{
			cout<<"\t[GD-C1000]-:System is running."<<endl;
			continue;
		}
		else if(cmd == "run" && bRunning == false)
		{
			Start();
			bRunning = true;
			continue;
		}
		else if(cmd == "stop" && bRunning == true)
		{
			Stop();
			bRunning = false;
			cout<<"-GD-C1000_CMD->";
			continue;
		}
		else if(cmd == "stop" && bRunning == false)
		{
			cout<<"\t[GD-C1000]-:System is stopped."<<endl;
			cout<<"-GD-C1000_CMD->";
			continue;
		}
		else if(cmd == "quit" && bRunning == true)
		{
			Stop();
			break; 
		}
		else if(cmd == "quit" && bRunning == false)
		{
			break;
		}
		else if(cmd == ">") 
		{
			cout<<"-GD-C1000_CMD->";
			continue;
		}
		else if(cmd == "help")
		{
			PrintUsage(); 
			cout<<"-GD-C1000_CMD->"; 
			continue;
		}
		else
		{
			cout<<"\t[Cmd_Tip]-:invalid command!"<<endl; 
			cout<<"-GD-C1000_CMD->"; 
			continue;
		}
	}
	return 0;
}