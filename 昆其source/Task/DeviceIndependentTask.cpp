///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceIndependentTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：与设备操作无关的任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "DeviceIndependentTask.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Time_value.h"

DeviceIndependentTask::DeviceIndependentTask() : XMLTask()
{

}

DeviceIndependentTask::DeviceIndependentTask(std::string strXML) : XMLTask(strXML)
{
	DeviceID = -100;//与设备无关的DeviceID为-100；各个子类无需再次设置。
}

DeviceIndependentTask::~DeviceIndependentTask()
{

}
bool DeviceIndependentTask::SendCASXML(const std::string& dstUrl,const std::string& upXML)
{
	std::string hostip,content;;
	int port = 0;
	size_t pos1,pos2,i,temp;
	bool bEnd = false;
	//如果upxml没有xml头，则添加xml头
	std::string tempXML = upXML;
	if(tempXML.find("encoding=\"GB2312\"") == -1)
	{
		int ppos = (int)tempXML.find("standalone=\"yes\"");
		if(ppos != -1)
			tempXML.insert(ppos-1,string(" encoding=\"GB2312\""));
	}

	pos1 = 0;
	pos2 = 0;
	//获得发送目标的地址和端口号
	char *http = "http://"; 
	size_t httpLen = strlen(http);

	std::string url = dstUrl;

	if(url.find(http) == 0)
	{
		temp = 0;
		for(i=0;i<url.length();i++)
		{
			if( url[i] == ':')
				temp++;
			if (temp == 2) {
				pos1 = i; 
				break;
			}
		}

		temp = 0;
		for(i=0;i<url.length();i++)
		{
			if( url[i] == '/')
				temp++;
			if (temp == 3) {
				pos2 = i; 
				break;
			}
		}

		if (pos1 == 0)//如果没有第二个‘:’
		{
			port = 80;

			if (pos2 == 0)//如果没有后面的‘/’
			{
				temp = url.length() - httpLen;
				hostip = url.substr(httpLen,temp);
			}else//如果有后面的‘/’
			{
				temp = pos2 - httpLen;
				hostip = url.substr(httpLen,temp);
			}
		}else//如果有第二个‘:’
		{
			if (pos2 == 0)//如果没有后面的‘/’
			{
				port = atol(url.substr(pos1 + 1,url.length() - pos1).c_str());

				hostip = url.substr(httpLen,pos1 - httpLen); 
			}else//如果有后面的‘/’
			{
				port = atol(url.substr(pos1+1,(pos2-pos1-1)).c_str());

				temp = pos1 - httpLen;
				hostip = url.substr(httpLen,temp);
			}
		}

		std::string errorMsg = "发送XML文件失败！\n",errorPos("");

		try
		{
			ACE_SOCK_Stream clientStream;
			ACE_INET_Addr remoteAddr(port,hostip.c_str());//目的地址
			ACE_SOCK_Connector connector;
			int nResult = -1;

			errorPos = "connect";
			if (connector.connect (clientStream, remoteAddr) == -1)//连接服务器
			{
				clientStream.close_reader();
				clientStream.close_writer();
				clientStream.dump();
				clientStream.close();
				return false;
				ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","XML发送任务连接服务器失败"),false);
			}
			else
			{
				;//ACE_DEBUG((LM_DEBUG,"(%T| %t) 连接到 %s\n",remoteAddr.get_host_name ()));
			}

			std::string tmp = dstUrl;
			content = "POST " + tmp + " HTTP/1.1\r\nUser-Agent: Mozila\r\nConnection: close\r\nHost: " + hostip;
			content = content + ":" + StrUtil::Int2Str(port) + "\r\nContent-Length: " + StrUtil::Int2Str((int)tempXML.length()) + "\r\n\r\n" + tempXML;//post形式的http请求头

			for(int j=0;j<PROPMANAGER::instance()->GetXmlSendTimes();j++)
			{
				nResult = clientStream.send(content.c_str(),(int)content.length(),0);
				cout<<content<<endl<<nResult<<endl;
				if(nResult > 0)//发送请求
				{
					ACE_Time_Value overTime(PROPMANAGER::instance()->GetXmlOvertime(),0);
					char buf[1024*16]={0};
					int ret = clientStream.recv(buf,1024*16,0,&overTime);//接收服务器响应
					cout<<buf<<endl;
					if (ret > 0)
					{
						//判断收到的服务器回复文件，是否含有200，说明服务器响应正常
						for(int i = 0;i < ret;i++)
						{
							if(buf[i]=='2')
								if(i+1<ret&&buf[i+1]=='0')
									if(i+2<ret&&buf[i+2]=='0')
										if(i+3<ret&&buf[i+3]==' ')
										{
											bEnd = true;
											break;
										}
						}
						if(bEnd)
							break;
					}
				}
			}

			clientStream.close();
		}
		catch (...)
		{
			bEnd = false;
			errorMsg += errorPos;
			//SYSMSGSENDER::instance()->SendMsg(errorMsg,UNKNOWN,VS_MSG_SYSALARM);
		}
	}

	return bEnd;
}

