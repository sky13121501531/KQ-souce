
#include "QualityQueryTask.h"
#include "TranslateDownXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/TimeUtil.h"
#include "../DBAccess/QualityDbManager.h"
#include "../Foundation/StrUtil.h"
#include "TranslateUpXML.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
QualityQueryTask::QualityQueryTask() : DeviceIndependentTask()
{

}

QualityQueryTask::QualityQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

QualityQueryTask::~QualityQueryTask()
{
}

void QualityQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标查询任务执行 !\n",DeviceID));
	SetRunning();
	XmlParser psr;
	psr.Set_xml(strStandardXML);
    RetValue = RUN_SUCCESS;
	std::string taskid,freq,startdatetime,enddatetime,reporttime,channelcode;
	std::string STD;
	std::map< string,vector<eQualityInfo> > FreqQualityInfo;

	std::string qualitypath="Msg/QualityQuery";
	pXMLNODE node = NULL;
	pXMLNODELIST childlist;
	try
	{
		node=psr.GetNodeFromPath((char*)qualitypath.c_str());
		childlist=psr.GetNodeList(node);//获取QualityQuery的子节点列表
	}
	catch(...)
	{
		RetValue = RUN_FAILED;
		string error = "获取QualityQuery节点失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		SendXML(TranslateUpXML::TranslateQualityQuery(this,FreqQualityInfo));
		SetFinised();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标查询任务停止 !\n",DeviceID));
		return;
	}
	for (int i=0;i!=childlist->Size();++i)
	{
		pXMLNODE childnode=psr.GetNextNode(childlist);
		try
		{
			psr.GetAttrNode(childnode,string("TaskID"),taskid);                //获取Quality节点的TaskID属性
			psr.GetAttrNode(childnode,string("Freq"),freq);                    //获取Quality节点的Freq属性
			psr.GetAttrNode(childnode,string("STD"),STD);                      //获取Quality节点的STD属性
			psr.GetAttrNode(childnode,string("StartDateTime"),startdatetime);  //获取Quality节点的StartDateTime属性
			psr.GetAttrNode(childnode,string("EndDateTime"),enddatetime);      //获取Quality节点的EndDateTime属性
			psr.GetAttrNode(childnode,string("ReportTime"),reporttime);        //获取Quality节点的ReportTime属性
			psr.GetAttrNode(childnode,"ChannelCode",channelcode);
			if(channelcode!="")
				CHANNELINFOMGR::instance()->GetFreqByChannelCode(DVBType,channelcode,freq);
		}
		catch(...)
		{
			string error = "获取Quality节点属性失败";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
			continue;
		}
		vector<eQualityInfo> vecQualityInfo;
		//查询指标测量结果信息
		if( QUALITYDBMANAGER::instance()->QueryQualityInfo(DVBType,freq,taskid,vecQualityInfo,startdatetime,enddatetime) ==false )
		{
			RetValue = DATABASEERROR;
			continue;
		}
	    if (vecQualityInfo.empty())
	    {
            eQualityInfo quality;
			quality.checktime ="";
			quality.STD = STD;
			quality.desc ="";
			quality.type ="";
			quality.valu = "";
			vecQualityInfo.push_back(quality);
	    }
		else
		{
			for (size_t j=0;j!=vecQualityInfo.size();++j)
			{
				vecQualityInfo[j].STD=STD;
			}
		}

		//对新安播下发all的情况作特殊处理：把根据taskid查出来的指标信息，按照频点分类，插入到map中
		if (channelcode=="All" || channelcode=="all")	
		{
			vector<eQualityInfo>::iterator quaInfoIter = vecQualityInfo.begin();
			for (; quaInfoIter!=vecQualityInfo.end(); quaInfoIter++)
			{
				string strTempFreq = quaInfoIter->freq;
				if (FreqQualityInfo.find(strTempFreq) != FreqQualityInfo.end())	//map中有这个频点了，就直接插入到相应的vector中
				{
					FreqQualityInfo[strTempFreq].push_back( *quaInfoIter );
				}
				else	//map中没有这个频点
				{
					std::vector<eQualityInfo> vecTempQuaInfo;
					vecTempQuaInfo.push_back( *quaInfoIter );
					FreqQualityInfo.insert( make_pair(strTempFreq, vecTempQuaInfo) );
				}
			}	
		}
		else
			FreqQualityInfo.insert(make_pair(freq,vecQualityInfo)); 
		if (reporttime!="")
		{
			//主动上报处理
		}
	}
	//将结果发送到前端系统
	SendXML(TranslateUpXML::TranslateQualityQuery(this,FreqQualityInfo));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标查询任务停止 !\n",DeviceID));
}

std::string QualityQueryTask::GetTaskName()
{
	return "指标查询任务";
}

std::string QualityQueryTask::GetObjectName()
{
	return std::string("QualityQueryTask");
}