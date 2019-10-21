//////////////////////////////////////////////////////////////////////////////////////////
// 文件名：HardwareMonitor.h
// 创建者：wuyy	
// 创建时间：2014-6-25
// 内容描述：硬件监测模块
///////////////////////////////////////////////////////////////////////////////////////////
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "HardwareMonitor.h"
#include "../Foundation/PropManager.h"
#include <IPExport.h>

#include <iphlpapi.h>
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"wsock32.lib")

#include <iostream>
#include <string>
#include <list>
//1.获取设备通道信息，2.设置通道用途，3.处理解调通道和转码的关系
HardwareMonitor::HardwareMonitor(void)
{
	harddisk = true;
	casepower =true;
	board = true;
	fan =true;
}

HardwareMonitor::~HardwareMonitor(void)
{
}
void HardwareMonitor::BoardMonitor()
{
	PropManager p_manager;
	p_manager.IpVec;
	DWORD dwRetVal;
	IPAddr DestIp ;
	IPAddr SrcIp = 0; 
	ULONG MacAddr[2];       
	ULONG PhysAddrLen=6;  

	for (map<string,int>::iterator iter=p_manager.IpVec.begin();iter!=p_manager.IpVec.end();iter++)
	{
		DestIp=inet_addr(iter->first.c_str());
		dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen); //ping板卡通的话返回0
		PhysAddrLen=6; 
		if (dwRetVal!=0)
		{
			if (iter->second>0&&iter->second<17)
			{
				O_brokeBoardVec.push_back(iter->first.c_str());
			}
			else
			{
				T_brokeBoardVec.push_back(iter->first.c_str());
			}
		}	
	}
	if (O_brokeBoardVec.size()>0||T_brokeBoardVec.size()>0)
	{
		cout<<"板卡不通"<<endl;
	}
}
void HardwareMonitor::CasePower()
{
	string cmd = "taskkill /IM tftpd32.exe";
	string str;
	if (O_brokeBoardVec.size() == 16||T_brokeBoardVec.size()==16)
	{
		system(cmd.c_str());
		Sleep(20000);

	}

	if (O_brokeBoardVec.size()==16&&T_brokeBoardVec.size()==16)
	{	
		str = "01号机箱故障和02号机箱故障";
		BrokeCasePower.push_back(str);
	}
	else if(O_brokeBoardVec.size()==16)
	{
		str = "01号机箱故障";
		BrokeCasePower.push_back(str);
	}
	else if(T_brokeBoardVec.size()==16)
	{
		str = "02号机箱故障";
		BrokeCasePower.push_back(str);
	}
}
void HardwareMonitor::FanMonitor()
{
	//风扇无法判断默认为好
}
void HardwareMonitor::HarddiskMonitor()
{
	//通过是否能创建文件夹来判断各分区硬盘是否正常
	std::string bharddisk;
	std::string Testpath = "m:/dtest.txt";
	if (!CreateDirectory(Testpath.c_str(),NULL))
	{
		//硬盘故障
		bharddisk = "d盘分区故障";
		BrokeHarddiskVec.push_back(bharddisk);
		harddisk = false;
	}
	else
	{
		//硬盘正常   删除刚才创建的测试文档
		RemoveDirectory(Testpath.c_str());
	}

}