
#include "AlarmSender.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Communications/XMLSend.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/StrUtil.h"
#include "ace/OS.h"

AlarmSender::AlarmSender()
{

}

AlarmSender::~AlarmSender()
{

}

int AlarmSender::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 报警检查/发送任务线程开始执行 !\n"));
	this->activate();
	return 0;
}

//通过从xml中解析出来的不同类型发送到不同的监测类型报警服务器
int AlarmSender::svc()
{
	bFlag = true;
	ACE_Message_Block *mb = 0;
	while (bFlag)
	{
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
				if (-1 == ProcessMessage(mb))
				{
					mb->release();
					break;
				}
				mb->release();
			}
			OSFunction::Sleep(0,200);
		}
		catch(...)
		{
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 报警检查/发送任务线程停止执行 !\n"));

	return 0;
}

int AlarmSender::ProcessMessage(ACE_Message_Block* mb)
{
	std::string strXML ="";
	char buf[1024*100] = {0};
	
	memcpy(buf,mb->rd_ptr(),mb->length());//xml拷贝至内存
	strXML = buf;
#ifdef ZONG_JU_ZHAO_BIAO
#ifdef _DEBUG
	std::cout<<"====================="<<std::endl;
	std::cout<<strXML<<std::endl;
	std::cout<<"====================="<<std::endl;
#endif
#endif
	XmlParser parser(strXML.c_str());
	pXMLNODE rootNode=parser.GetRootNode();
	string dvbtype,srccode;
	eDVBType type;
	parser.GetAttrNode(rootNode,"Type",dvbtype);
	UpAlarmInfo alarminfo;
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	if(dvbtype=="DVBCUp")
	{
		string url = PROPMANAGER::instance()->GetUrlByType("DVBC_ALARMSENDUP");
		alarminfo.alarmurl = url;
		type = DVBC;
	}
	else if(dvbtype=="DVBSUp")
	{
		string url = PROPMANAGER::instance()->GetUrlByType("DVBS_ALARMSENDUP");
		alarminfo.alarmurl = url;
		type = DVBS;
	}
	else if(dvbtype=="DTMBUp")
	{
		string url = PROPMANAGER::instance()->GetUrlByType("DTMB_ALARMSENDUP");
		alarminfo.alarmurl = url;
		type = CTTB;
	}
	else if(dvbtype=="FMUp")
	{
		string url = PROPMANAGER::instance()->GetUrlByType("FM_ALARMSENDUP");
		alarminfo.alarmurl = url;
		type = RADIO;
	}
	else if(dvbtype=="AMUp")
	{
		string url = PROPMANAGER::instance()->GetUrlByType("AM_ALARMSENDUP");
		alarminfo.alarmurl = url;
		type = AM;
	}


	if ((strXML.empty()) || (alarminfo.alarmurl.empty()))
	{
		return 0;
	}
	if(alarminfo.type=="0")
	{
		return 0;
	}
	if(false == XMLSend::SendXML(alarminfo.alarmurl,strXML))
	{
		std::string msg = string("发送任务对象XML文件失败！") + "URL:" + alarminfo.alarmurl;
		//SYSMSGSENDER::instance()->SendMsg(msg,type,VS_MSG_SYSALARM);
		string ParentDir="C:/AlarmUp/";
		if(ACE_OS::opendir(ParentDir.c_str())==NULL)   
		{
			mkdir(ParentDir.c_str());
		} 
		string ChildDir=OSFunction::GetStrDVBType(type);
		string dir=ParentDir+ChildDir+string("/"); 
		if(ACE_OS::opendir(dir.c_str())==NULL)   
		{
			mkdir(dir.c_str());
		} 
		static int index=1;
		if(index>1000)
			index=1;
		string FileName=dir+TimeUtil::DateTimeToString(time(0))+string("_")+StrUtil::Int2Str(index)+string(".xml");

		parser.SaveAsFile(FileName.c_str());
		index++;
	}

	return 0;
}

int AlarmSender::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
