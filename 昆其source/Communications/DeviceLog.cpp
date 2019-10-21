#include "DeviceLog.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/AppLog.h"

DeviceLog::DeviceLog(void)
{
	bFlag=false;
	
	std::string LogIP = PROPMANAGER::instance()->GetLogIP();
	unsigned int LogPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetLogPort());
	ACE_INET_Addr local_addr(LogPort,LogIP.c_str());

	//sock.open(local_addr);
}

DeviceLog::~DeviceLog(void)
{
}

int DeviceLog::open( void* )
{
	activate();
	return 0;
}

int DeviceLog::svc()
{
	bFlag=true;
	
	ACE_INET_Addr remote_addr;
	ACE_Time_Value TimeOut(0,100);
	
	char buf[1024*4]={0};
	while(bFlag)
	{
		OSFunction::Sleep(0,100);
		
		//std::string recv_info;
		//int byte_count=sock.recv(buf,1024*4,remote_addr,0,&TimeOut);
		//if(byte_count!=-1)
		//{
		//	recv_info+=std::string(buf);
		//	if(recv_info.length()>0)
		//	{
		//		std::string remote_addr_info=std::string(remote_addr.get_host_addr());
		//		remote_addr_info=remote_addr_info+std::string("\xd\xa");
		//		recv_info.insert(0,remote_addr_info);//插入远程计算机地址信息并换行

		//		string msg = string("板卡日志信息:") + recv_info;
		//		SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSTINFO,DEVICE,LOG_EVENT_DEBUG,false,true);
		//	}
		//	memset(buf,0,1024*4);
		//	recv_info.clear();
		//}
		
	}
	return 0;
}

int DeviceLog::Stop()
{
	bFlag=false;
	this->wait();
	return 0;
}