#include "FetchQualityLevel.h"
#include "../Foundation/StrUtil.h"

ACE_Thread_Mutex FetchQualityLevel::DVBCLevelMutex;
ACE_Thread_Mutex FetchQualityLevel::DVBSLevelMutex;
ACE_Thread_Mutex FetchQualityLevel::CTTBLevelMutex;

FetchQualityLevel::FetchQualityLevel( enumDVBType dvbtype,std::string strIP,int nPort)
{
	DvbType=dvbtype;
	strIPAddress=strIP;
	port=nPort;
	server.set(nPort,strIP.c_str());
	//server.set_address(strIPAddress.c_str(),strIPAddress.length());
	//server.set_port_number(port);
}
FetchQualityLevel::FetchQualityLevel(void)
{
	
}
FetchQualityLevel:: ~FetchQualityLevel()
{

}
int FetchQualityLevel::GetLevelFromCard(int Freq)
{
	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[200] = {0};
	//信息头
	int msg_type = MSG_GET_QUA;
	PkgHeader_Obj   ph;
	ph.header   = 0x51;
	ph.msg_type = msg_type;;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//信息内容
	memcpy(sendbuf+pkgLen, (void*)&Freq, sizeof(Freq));
	pkgLen	+= sizeof(Freq);

	ACE_Time_Value timeout(10);
	if(DvbType==DVBC)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(DVBCLevelMutex);
		if (connector.connect (stream, server) == -1)//连接硬件服务器
		{
			string msg = string("连接[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBC,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -1;
		}

		if(stream.send((char*)sendbuf, 200,&timeout) <= 0)
		{
			string msg = string("发送板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]指令失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBC,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -2;
		}
		//接收返回数据
		int lenth;
		if((lenth=stream.recv(( char *)&RetMsg,sizeof(TVQuaRetMessage_Obj),&timeout)) <= 0)
		{
			string msg = string("接收板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]数据失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBC,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			OSFunction::Sleep(0,500);
			return -3;
		}

		TVQuaRetMessage_Handle rm = (TVQuaRetMessage_Handle)(RetMsg);
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		if(rm->ph.header!=0x52 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
		{
			string msg = string("板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]返回数据类型错误");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBC,VS_MSG_SYSALARM);
			return -4;
		}
		int level=rm->level;
		return level;
	}
	else if(DvbType==CTTB)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(CTTBLevelMutex);
		if (connector.connect (stream, server) == -1)//连接硬件服务器
		{
			string msg = string("连接[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,CTTB,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -1;
		}

		if(stream.send((char*)sendbuf, 200,&timeout) <= 0)
		{
			string msg = string("发送板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]指令失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,CTTB,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -2;
		}
		//接收返回数据
		int lenth;
		if((lenth=stream.recv(( char *)&RetMsg,sizeof(TVQuaRetMessage_Obj),&timeout)) <= 0)
		{
			string msg = string("接收板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]数据失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,CTTB,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			OSFunction::Sleep(0,500);
			return -3;
		}
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		TVQuaRetMessage_Handle rm = (TVQuaRetMessage_Handle)(RetMsg);
		if(rm->ph.header!=0x52 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
		{
			string msg = string("板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]返回数据类型错误");
			//SYSMSGSENDER::instance()->SendMsg(msg,CTTB,VS_MSG_SYSALARM);
			return -4;
		}
		int level=rm->level;
		return level;
	}
	if(DvbType==DVBS)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(DVBSLevelMutex);
		if (connector.connect (stream, server) == -1)//连接硬件服务器
		{
			string msg = string("连接[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBS,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -1;
		}

		if(stream.send((char*)sendbuf, 200,&timeout) <= 0)
		{
			string msg = string("发送板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]指令失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBS,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			return -2;
		}
		//接收返回数据
		int lenth;
		if((lenth=stream.recv(( char *)&RetMsg,sizeof(TVQuaRetMessage_Obj),&timeout)) <= 0)
		{
			string msg = string("接收板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]数据失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBS,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			OSFunction::Sleep(0,500);
			return -3;
		}

		TVQuaRetMessage_Handle rm = (TVQuaRetMessage_Handle)(RetMsg);
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		if(rm->ph.header!=0x52 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
		{
			string msg = string("板卡[") + strIPAddress + string(":") + StrUtil::Int2Str(server.get_port_number()) + string("]返回数据类型错误");
			//SYSMSGSENDER::instance()->SendMsg(msg,DVBS,VS_MSG_SYSALARM);
			return -4;
		}
		int level=rm->level;
		return level;
	}
	else
	{
		return 0;
	}
}