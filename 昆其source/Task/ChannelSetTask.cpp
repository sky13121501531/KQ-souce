
#include "ChannelSetTask.h"
#include "ace/Task.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "./TranslateDownXML.h"
#include "./TranslateUpXML.h"
#include <iostream>
using namespace std;

ChannelSetTask::ChannelSetTask() : DeviceIndependentTask()
{

}

ChannelSetTask::ChannelSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ChannelSetTask::~ChannelSetTask()
{

}
void ChannelSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	sChannelInfo channel;//频道信息
	vector< sChannelInfo > vecChannel;//频道id
	XmlParser parser( strStandardXML.c_str());
	pXMLNODE channelSetNode = parser.GetNodeFromPath("Msg/ChannelSet");//ChannelSet节点
	pXMLNODELIST channelNodeList = parser.GetNodeList(channelSetNode);//ChannelSet节点的子节点列表
	for (int i = 0;i!=channelNodeList->Size();i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);//channel节点
		
		if (DVBType==RADIO || DVBType==AM)
		{
			parser.GetAttrNode( channelNode,"Code",channel.ChannelCode );
			parser.GetAttrNode( channelNode,"Freq",channel.Freq );//
			parser.GetAttrNode( channelNode,"Freq",channel.ChannelID );//
		}
		else
		{
			parser.GetAttrNode( channelNode,"CODE",channel.ChannelCode );
			parser.GetAttrNode( channelNode,"VideoFreq",channel.Freq );//
			parser.GetAttrNode( channelNode,"VideoFreq",channel.ChannelID );//
		}
		
		parser.GetAttrNode( channelNode,"Name",channel.ProgramName);

		vecChannel.push_back( channel);//保存频道id
	}
	//更新数据库中特定DVBTYPE的CHANNELID信息
	if( CHANNELINFOMGR::instance()->UpdateChannelInfo(GetDVBType(),vecChannel)==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateChannelSet(this));//发送xml
	SetFinised();//更新任务状态为结束
	bRun = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道设置任务停止 !\n",DeviceID));
}
string ChannelSetTask::GetTaskName()
{
	return std::string("频道设置任务");
}
std::string ChannelSetTask::GetObjectName()
{
	return std::string("ChannelSetTask");
}

