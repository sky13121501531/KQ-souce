
#include "NvrStatusSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include <vector>
#include <iostream>
using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO

NvrStatusSetTask::NvrStatusSetTask() : DeviceIndependentTask()
{

}

NvrStatusSetTask::NvrStatusSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

NvrStatusSetTask::~NvrStatusSetTask()
{
}
void NvrStatusSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]板卡(通道)设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	XmlParser parser( strStandardXML.c_str() );
	pXMLNODE statusSetNode = parser.GetNodeFromPath("Msg/NvrStatusSet");

	int nSetCount = parser.GetChildCount( statusSetNode );
	pXMLNODELIST setRecordList = parser.GetNodeList( statusSetNode );
	for (int i=0; i<nSetCount; i++)
	{
		pXMLNODE setRecordNode = parser.GetNextNode( setRecordList );
		
		std::string strIndex, strIndexType;
		parser.GetAttrNode(setRecordNode, "Index", strIndex);
		parser.GetAttrNode(setRecordNode, "IndexType", strIndexType);
		int index = StrUtil::Str2Int(strIndex);
		if(index<500)
		{
			vector<int> vDeviceID;
			bool bRet = PROPMANAGER::instance()->GetCoderDevFromTunerDev(vDeviceID, index);
			if (bRet)
			{
				for (int i = 0; i < vDeviceID.size(); i++)
				{
					int iDeviceID = vDeviceID[i];
				}
			}
		}
		else
		{
			cout<<"停用卡设置失败,通道:"<<index<<endl;
		}
	}

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateNvrStatusSet(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]板卡(通道)设置任务停止 !\n",DeviceID));
}

string NvrStatusSetTask::GetTaskName()
{
	return std::string("板卡(通道)设置任务");
}
std::string NvrStatusSetTask::GetObjectName()
{
	return std::string("NvrStatusSetTask");
}


#endif