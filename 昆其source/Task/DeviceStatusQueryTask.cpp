
#include "DeviceStatusQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"

DeviceStatusQueryTask::DeviceStatusQueryTask() : DeviceIndependentTask()
{

}

DeviceStatusQueryTask::DeviceStatusQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

DeviceStatusQueryTask::~DeviceStatusQueryTask()
{

}

void DeviceStatusQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]通道状态查询任务执行 !\n",DeviceID));
	SetRunning();

	RetValue = RUN_SUCCESS;
	XmlParser psr;
	psr.Set_xml(strStandardXML);

	pXMLNODE rootnode=psr.GetRootNode();

	//获得要查询的通道号
	pXMLNODE node=psr.GetNodeFromPath("Msg/DeviceStatusQuery");
	pXMLNODELIST nodelist=psr.GetNodeList(node);
	std::vector<sDeviceStatus> VecDeviceStatus;
	eDVBType dvbtype = UNKNOWN;
	
	try
	{
		for(int i=0;i<nodelist->Size();++i)
		{
			node=psr.GetNextNode(nodelist);
			std::string LogicDeviceId,DeviceId;
			psr.GetAttrNode(node,"DeviceID",LogicDeviceId);
			sDeviceStatus curDeviceStatus;
			std::string desc;
			int status=-1;
			PROPMANAGER::instance()->GetDeviceIndex(DeviceId,LogicDeviceId,OSFunction::GetStrDVBType(GetDVBType()));
			PROPMANAGER::instance()->GetDeviceType(atoi(DeviceId.c_str()),dvbtype);
			if (PROPMANAGER::instance()->IsDeviceAvaiable(StrUtil::Str2Int(DeviceId)) == false|| dvbtype != GetDVBType())  //通道不合法、监测类型不对
			{ 
				curDeviceStatus.DevicID=StrUtil::Str2Int(LogicDeviceId);
				curDeviceStatus.Statuc=1;
				curDeviceStatus.Desc=string("无板卡");
				VecDeviceStatus.push_back(curDeviceStatus);
			}
			else
			{
				BUSINESSLAYOUTMGR::instance()->DeviceStatusQuery(StrUtil::Str2Int(DeviceId),status,desc);//获得通道状态

				curDeviceStatus.DevicID=StrUtil::Str2Int(LogicDeviceId);
				curDeviceStatus.Statuc=status;
				curDeviceStatus.Desc=desc;
				VecDeviceStatus.push_back(curDeviceStatus);
			}
		}
	}
	catch(...)
	{
		RetValue = RUN_FAILED;
	}

	SendXML(TranslateUpXML::TranslateDeviceStatusQuery(this,VecDeviceStatus));//发送xml

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]通道状态查询任务停止 !\n",DeviceID));
}

std::string DeviceStatusQueryTask::GetTaskName()
{
	return "通道状态查询任务";
}

std::string DeviceStatusQueryTask::GetObjectName()
{
	return std::string("DeviceStatusQueryTask");
}