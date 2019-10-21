
#include "AlarmParamSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "./TranslateDownXML.h"
#include "../Foundation/AlarmPropManager.h"
using namespace std;

AlarmParamSetTask::AlarmParamSetTask() : DeviceIndependentTask()
{

}

AlarmParamSetTask::AlarmParamSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmParamSetTask::~AlarmParamSetTask()
{

}
void AlarmParamSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警参数设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE paramSetNode=parser.GetNodeFromPath("Msg/AlarmParamSet");
	std::string STD,Freq,SymbolRate;
	parser.GetAttrNode(paramSetNode,string("STD"),STD);//获得STD属性
	parser.GetAttrNode(paramSetNode,string("Freq"),Freq); //获得Freq属性
	parser.GetAttrNode(paramSetNode,string("SymbolRate"),SymbolRate); //获得SymbolRate属性
	parser.GetAttrNode(paramSetNode,string("ReturnTaskName"),m_strObjectName); //获得SymbolRate属性
	pXMLNODELIST paramSetNodeList=parser.GetNodeList(paramSetNode);//报警参数节点列表
	vector<sAlarmParam> vevAlarmParam;
	for (int i=0;i!=paramSetNodeList->Size();++i)
	{
		sAlarmParam tempAlarmParam;
		tempAlarmParam.STD=STD;
		tempAlarmParam.Freq=Freq;
		tempAlarmParam.SymbolRate=SymbolRate;
		tempAlarmParam.DVBType = GetDVBType();//监测任务类型
		pXMLNODE childNode=paramSetNodeList->GetNextNode();//子节点
		std::string TypeID;
		parser.GetAttrNode(childNode,string("Type"),TypeID);//获得Type属性
		tempAlarmParam.TypeID=TypeID;
		int alarmType=atoi(TypeID.c_str());

		if(alarmType<20)
		{
			tempAlarmParam.AlarmType=ALARM_FREQ;
		}
		else if(alarmType<50)
		{
			tempAlarmParam.AlarmType=ALARM_TR101_290;
		}
		else
		{
			tempAlarmParam.AlarmType=ALARM_PROGRAM;
		}
		if(tempAlarmParam.Freq.length()<1)
		{
			parser.GetAttrNode(childNode,string("Freq"),tempAlarmParam.Freq);
		}
		parser.GetAttrNode(childNode,string("Desc"),tempAlarmParam.TypeDesc);//获得Desc属性	
		parser.GetAttrNode(childNode,string("ChannelID"),tempAlarmParam.ChannelID);
		parser.GetAttrNode(childNode,string("Duration"),tempAlarmParam.Duration);//错误次数
		parser.GetAttrNode(childNode,string("TimeInterval"),tempAlarmParam.TimeInterval);//时间间隔
		
		parser.GetAttrNode(childNode,string("Num"),tempAlarmParam.Num);//下限
		parser.GetAttrNode(childNode,string("DownThreshold"),tempAlarmParam.DownThreshold);//下限
		parser.GetAttrNode(childNode,string("UpThreshold"),tempAlarmParam.UpThreshold);//上限
		
		parser.GetAttrNode(childNode,string("Switch"),tempAlarmParam.Switch);
		
		if (DVBType == THREED)
		{
			if (tempAlarmParam.Switch!="" )
			{
				m_strObjectName = "AlarmProgramSwitchSet";
			}
			else 
			{
				m_strObjectName = "AlarmProgramThresholdSet";
			}
		}
		
		vevAlarmParam.push_back(tempAlarmParam);

		if((DVBType==ATV&&TypeID=="10")||(DVBType==RADIO&&TypeID=="23")||(DVBType==AM&&TypeID=="23")||(DVBType==CTV&&TypeID=="10"))
		{
			string AlarmThreshold;
			parser.GetAttrNode(childNode,string("AlarmThreshold"),AlarmThreshold);
			ALARMPROPMANAGER::instance()->SetAlarmThreShold(DVBType,tempAlarmParam.Freq,AlarmThreshold);
		}
    }
	
	//更新报警参数信息
	if(( ALARMPARAMINFOMGR::instance()->UpdateAlarmParam(vevAlarmParam)&&ALARMPARAMINFOMGR::instance()->UpdateAllAlarmParam(vevAlarmParam))==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateAlarmParamSet(this));//发送回复xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警参数设置任务停止 !\n",DeviceID));
}
string AlarmParamSetTask::GetTaskName()
{
	return "报警参数设置任务";
}
std::string AlarmParamSetTask::GetObjectName()
{
	return m_strObjectName/*std::string("AlarmParamSetTask")*/;
}

