
#include "HDDefAudioParamSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"

#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/ParamSetManager.h"
#endif

#include <vector>
#include <iostream>

using namespace std;

HDDefAudioParamSetTask::HDDefAudioParamSetTask() : DeviceIndependentTask()
{

}

HDDefAudioParamSetTask::HDDefAudioParamSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

HDDefAudioParamSetTask::~HDDefAudioParamSetTask()
{
}
void HDDefAudioParamSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]高清音频默认参数设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

#ifdef ZONG_JU_ZHAO_BIAO
	XmlParser parser( strStandardXML.c_str() );
	pXMLNODE HDAudioPSetNode = parser.GetNodeFromPath("Msg/HDDefAudioParamSet");

	std::string setRight,setLeft,setLeftS,setRightS,setCenter,setSubwoofer,setAudioEnc;
	parser.GetAttrNode(HDAudioPSetNode, "Right",		setRight);
	parser.GetAttrNode(HDAudioPSetNode, "Left",			setLeft);
	parser.GetAttrNode(HDAudioPSetNode, "LeftSurround", setLeftS);
	parser.GetAttrNode(HDAudioPSetNode, "RightSurround", setRightS);
	parser.GetAttrNode(HDAudioPSetNode, "Center",		setCenter);
	parser.GetAttrNode(HDAudioPSetNode, "Subwoofer",	setSubwoofer);
	parser.GetAttrNode(HDAudioPSetNode, "AudioEncode",	setAudioEnc);

	AudioParam tempAudioParam;
	tempAudioParam._right = setRight;
	tempAudioParam.left = setLeft;
	tempAudioParam.leftsurround = setLeftS;
	tempAudioParam.rightsurround = setRightS;
	tempAudioParam.center = setCenter;
	tempAudioParam.subwoofer = setSubwoofer;
	tempAudioParam.audioencode = setAudioEnc;

	PARAMSETMANAGER::instance()->SetHDAudioParam( tempAudioParam );
#endif

	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateHDDefAudioParamSet(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]高清音频默认参数设置任务停止 !\n",DeviceID));
}

string HDDefAudioParamSetTask::GetTaskName()
{
	return std::string("高清音频默认参数设置任务");
}
std::string HDDefAudioParamSetTask::GetObjectName()
{
	return std::string("HDDefAudioParamSetTask");
}

