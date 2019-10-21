///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskTaskXMLSend.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：任务执行结果，上行发送类
///////////////////////////////////////////////////////////////////////////////////////////
#include "TaskXMLSend.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/PropManager.h"
#include "CommunicationMgr.h"
#include "ace/OS.h"
#include "XMLSend.h"

TaskXMLSend::TaskXMLSend()
{
	bFlag = false;
}

TaskXMLSend::~TaskXMLSend()
{

}

int TaskXMLSend::Init()
{
	//初始化
	bFlag = true;

	return 0;
}

int TaskXMLSend::open(void*)
{
	//启动线程
	this->activate();
	return 0;
}

int TaskXMLSend::svc()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 上行XML发送线程开始执行 !\n"));

	ACE_Message_Block *mb = 0;

	while (bFlag)
	{
		OSFunction::Sleep(0,50);
		try 
		{
			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (getq(mb,&OutTime) != -1 && mb != NULL)
			{
				if (mb->msg_type() == ACE_Message_Block::MB_BREAK)
				{
					mb->release();
					break;
				}
				//if (-1 == ProcessMessage(mb))
				//{
				//	std::string msg = string("回复XML被加入循环发送队列！");
			
				//	COMMUNICATIONMGR::instance()->AccessTaskXMLReSend()->putq(mb);//xml加入循环发送队列
				//	continue;
				//}
				ProcessMessage(mb);
				mb->release();
			}
		}
		catch (...)
		{
			std::string msg = string("发送任务对象XML文件异常！");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		}
	}

	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 上行XML发送线程停止执行 !\n"));

	return 0;
}

int TaskXMLSend::Stop()
{
	bFlag = false;
	ACE_Message_Block* pMsgBreak;
	ACE_NEW_NORETURN(pMsgBreak, ACE_Message_Block (0, ACE_Message_Block::MB_BREAK) );
	//清空队列，停止发送回复xml
	msg_queue()->flush();
	msg_queue()->enqueue_head(pMsgBreak);
	this->wait();

	return 0;
}

int TaskXMLSend::ProcessMessage(ACE_Message_Block* mb)
{
	std::string strXML ="";

	int xmllen = mb->length() + 100;
	char* buf = new char[xmllen];

	if (buf == NULL)
	{
		std::string msg = string("TaskXMLSend:内存空间不足！");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return -1;
	}

	memset(buf,0,xmllen);
	memcpy(buf,mb->rd_ptr(),mb->length());//xml拷贝至内存
	strXML = buf;
	if (strXML.empty())
	{
		std::string msg = string("任务对象返回XML为空！");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return 0;
	}
	
	std::string strUrl = GetURL(strXML);//获得发送的目标url
	std::cout<<"XMLSend Url: "<<strUrl<<std::endl;

	if (strUrl.empty())
	{
		std::string msg = string("任务对象返回XML无上报地址！");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return 0;
	}

	if(false == XMLSend::SendXML(strUrl,strXML))
	{
		std::string msg = string("发送任务对象XML文件失败！") + "URL:" + strUrl;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return -1;
	}

	delete[] buf;
	return 0;
}

std::string TaskXMLSend::GetURL(std::string& upXML)
{
	std::string dstCode,dstUrl,strRet,Type;

	XmlParser parser(upXML.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	//获取根元素的 "DstCODE" 属性值 
	parser.GetAttrNode(rootNode,"DstURL",dstUrl);
	parser.GetAttrNode( rootNode,"DstCode",dstCode);
	parser.GetAttrNode(rootNode,"Type",Type);
	
	//DstURL属性不为空，返回URL就为DstURL，否则根据DstCode从配置文件中获得URL
	//add by licf 注释：因为要配置回复CAS SMS的URL，这里根据回复任务类型进行了区分，并在配置文件中将CAS SMS的srccode分别设置
	//为CAS SMS，这样就可以根据任务返回类型从配置文件中获得上报CAS SMS的地址
	if(dstUrl.length()>0)
	{
		strRet=dstUrl;
	}
	else if (!(dstCode.empty()))
	{
		if (Type=="CASUp")
		{
#ifdef ZONG_JU_ZHAO_BIAO
			strRet = PROPMANAGER::instance()->GetUrlByType( "CAS" );
#else
			strRet = PROPMANAGER::instance()->GetUrl("CAS");//根据
#endif
			
		}
		else if (Type=="SMSUp")
		{
#ifdef ZONG_JU_ZHAO_BIAO
			strRet = PROPMANAGER::instance()->GetUrlByType( "SMS" );
#else
			strRet = PROPMANAGER::instance()->GetUrl("SMS");//根据目标码获得url
#endif
		}
		else
		{
		    strRet = PROPMANAGER::instance()->GetUrl(dstCode);//根据目标码获得url
		}
	}
	//去除upxml中msg节点的DstURL属性
	size_t pos=upXML.find("DstURL=");
	if(pos!=std::string::npos)
	{
		size_t pos1=upXML.find("\"",pos);
		size_t pos2=upXML.find("\"",pos1+1);
		upXML.erase(pos,pos2-pos+1);
	}
	return strRet;
}
