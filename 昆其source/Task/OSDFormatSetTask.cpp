#include "OSDFormatSetTask.h"
#include "./TranslateUpXML.h"
#include "../DBAccess/DBManager.h"
#include "../Task/TranslateXMLForDevice.h"
#include "../DeviceAccess/TsFetcherMgr.h"
std::vector<OSDFormat> gOSDFormatVec;//jsp2017.7.25
OSDFormatSetTask::OSDFormatSetTask(void) : DeviceIndependentTask()
{
}

OSDFormatSetTask::~OSDFormatSetTask(void)
{
}

OSDFormatSetTask::OSDFormatSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	//jsp2017.7.25
	gOSDFormatVec.clear();
	XmlParser taskParser( strXML.c_str() );
	pXMLNODE formatNode = taskParser.GetNodeFromPath("Msg");
	pXMLNODELIST pformatList = taskParser.GetNodeList(formatNode);
	for (int i=0; i<pformatList->Size(); i++)
	{
		pXMLNODE PosdNode = taskParser.GetNextNode(pformatList);
		taskParser.GetAttrNode(PosdNode, "Freq", Freq);
		pXMLNODELIST PosdNodeNodeList=taskParser.GetNodeList(PosdNode);

		for (int i=0;i!=PosdNodeNodeList->Size();++i)
		{
			OSDFormat osdf;
			osdf.Freq = Freq;
			pXMLNODE childNode=taskParser.GetNextNode(PosdNodeNodeList);
			taskParser.GetAttrNode(childNode,"ServiceID",osdf.ServiceID);
			taskParser.GetAttrNode(childNode,"FontSize",osdf.FontSize);
			taskParser.GetAttrNode(childNode,"Position",osdf.Position);
			taskParser.GetAttrNode(childNode,"AntiColor",osdf.AntiColor);
			taskParser.GetAttrNode(childNode,"Type",osdf.Type);
			taskParser.GetAttrNode(childNode,"Align",osdf.Align);
			bool bfind = false;
			for(int i=0;i<gOSDFormatVec.size();i++)
			{
				if((gOSDFormatVec[i].Freq == osdf.Freq)&&(gOSDFormatVec[i].ServiceID == osdf.ServiceID))
				{
					bfind = true;
					break;
				}
			}
			if(!bfind)
			{
				gOSDFormatVec.push_back(osdf);
			}
		}
	}
	DBMANAGER::instance()->OSDFormatSetInfo(DVBC,gOSDFormatVec);
}
void OSDFormatSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]OSD设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateOSDFormatSetQuery(this));//发送xml

	SetFinised(); 

	//jsp2017.7.25
	std::vector<std::string>  vctXML;
	bool rnt1 = DBMANAGER::instance()->QueryTask(DVBC,vctXML);

	for(int i= 0;i<vctXML.size();i++)
	{
		XmlParser parser;
		int deviceid = -1;
		parser.Set_xml(vctXML[i]);
		pXMLNODE autoNode=parser.GetNodeFromPath("Msg/AutoRecord/Record");
		parser.GetAttrNode(autoNode,"DeviceID",deviceid);
		std::string strDeviceXML = TranslateXMLForDevice::TranslateRecordTask(vctXML[i] );//自定义标准XML到硬件接口XML转换
		TSFETCHERMGR::instance()->SetTsDeviceXml(deviceid,strDeviceXML);
	}

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]OSD设置任务停止 !\n",DeviceID));

}
std::string OSDFormatSetTask::GetObjectName()
{
	return string("OSDFormatSetTask");
}
std::string OSDFormatSetTask::GetTaskName()
{
	return string("OSD设置任务");
}
