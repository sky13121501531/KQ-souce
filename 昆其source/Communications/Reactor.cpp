
#include "Reactor.h"
#include "XMLReceiverServer.h"
#include "RealTimeVideoServer.h"
#include "RealTimeQualityServer.h"
#include "DeviceServer.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "ace/Synch.h"

MyReactor::MyReactor(void)
{
	pXMLReceiverServer = NULL;
	pRealTimeVideoServer = NULL;
	pRealTimeQualityServer = NULL;
	pDeviceServer = NULL;
}

MyReactor::~MyReactor(void)
{

}

int MyReactor::Init()
{
	if (NULL == pXMLReceiverServer)
		pXMLReceiverServer = new XMLReceiverServer();

	if(NULL == pRealTimeVideoServer)
		pRealTimeVideoServer = new RealTimeVideoServer();

	if (NULL == pRealTimeQualityServer)
		pRealTimeQualityServer = new RealTimeQualityServer();

	if (NULL == pDeviceServer)
		pDeviceServer = new DeviceServer();

	//XML命令监听IP、Port
	std::string XMLServerIP = PROPMANAGER::instance()->GetXmlServerIP();
	unsigned int  XMLServerPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetXmlServerPort());
	ACE_INET_Addr  XMLServerAddr(XMLServerPort,XMLServerIP.c_str());
	cout<<XMLServerIP<<":"<<PROPMANAGER::instance()->GetXmlServerPort()<<endl;

	////视频监听IP,Port
	std::string videoServerIP = PROPMANAGER::instance()->GetHttpVideoIp();
	unsigned int videoServerPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetHttpVideoPort());
	ACE_INET_Addr videoServerAddr(videoServerPort/*,videoServerIP.c_str()*/);

	//实时指标监听IP,Port
	std::string QualityServerIP = PROPMANAGER::instance()->GetQualityIp();
	unsigned int QualityServerPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetQualityPort());
	ACE_INET_Addr QualityServerAddr(QualityServerPort,QualityServerIP.c_str());

	//板卡信息监听IP,Port
	std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
	unsigned int DeviceServerPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetDevicePort());
	ACE_INET_Addr DeviceServerAddr(DeviceServerPort,DeviceServerIP.c_str());

	//初始化
	if (pXMLReceiverServer->Open(XMLServerAddr) == -1 ||
		pRealTimeVideoServer->Open(videoServerAddr) == -1 ||
		pRealTimeQualityServer->Open(QualityServerAddr) == -1 ||
		pDeviceServer->Open(DeviceServerAddr) == -1)
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t)视频/指标/板卡服务端地址初始化错误!\n"));
		return -1;
	}

	//向反应器注册事件
	if (ACE_Reactor::instance()->register_handler(pXMLReceiverServer,ACE_Event_Handler::ACCEPT_MASK) == -1 ||
		ACE_Reactor::instance()->register_handler(pRealTimeVideoServer,ACE_Event_Handler::ACCEPT_MASK) == -1 ||
		ACE_Reactor::instance()->register_handler(pRealTimeQualityServer,ACE_Event_Handler::ACCEPT_MASK) == -1 ||
		ACE_Reactor::instance()->register_handler(pDeviceServer,ACE_Event_Handler::ACCEPT_MASK) == -1)
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t)视频/指标/板卡服务端事件注册失败!\n"));
		return -1;
	}

	return 0;
}

int MyReactor::open(void*)
{	
	//启动反应器
	ACE_DEBUG((LM_DEBUG,"(%T| %t) 反应器开始运行(视频/指标/板卡服务)!\n"));
	this->activate();

	return 0;
}

int MyReactor::svc()
{
	ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;
}

int MyReactor::Stop()
{
	ACE_Reactor::instance()->end_event_loop();

	pXMLReceiverServer->Stop();
	pRealTimeVideoServer->Stop();
	pRealTimeQualityServer->Stop();
	pDeviceServer->Stop();

	ACE_DEBUG((LM_DEBUG,"(%T| %t) 反应器停止运(视频/指标/板卡服务)!\n"));

	return 0;
}

ACE_Reactor* MyReactor::GetReactor()
{
	return ACE_Reactor::instance();
}