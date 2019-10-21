///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TSRecorder.cpp
// 创建者：lichunfeng
// 创建时间：2009-06-04
// 内容描述：保存录像文件类
///////////////////////////////////////////////////////////////////////////////////////////
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "TSRecorder.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../DBAccess/DBManager.h"
#include "DiskOperation.h"
#include "../foundation/applog.h"
TSRecorder::TSRecorder(int deviceid,std::string taskxml,std::string taskid)
{
	mbIsUpdateDbEndtime = true;
	mUpdateDbEndtime = time(0);
	//
	mIsCreate=false;
	mCurTime = 0;
	TaskXml = taskxml;
	DeviceID = deviceid;//通道号
	TaskID=taskid;

	XmlParser parser( TaskXml.c_str() );
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"DVBType",downType );
	parser.GetAttrNode( rootNode,"Version",m_Version );
	pXMLNODE recordNode=NULL;
	if(TaskXml.find("AutoRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/AutoRecord/Record");
	}
	else if(TaskXml.find("TaskRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TaskRecord/Record");
	}
	else if(TaskXml.find("TSQuery")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TSQuery/TS");
	}
	//获得视频流的信息
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode(recordNode,"Code",Code );
		parser.GetAttrNode(recordNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(recordNode,"TsID",TsID);
		parser.GetAttrNode(recordNode,"ServiceID",ServiceID);
		parser.GetAttrNode(recordNode,"VideoPID",VideoPID);
		parser.GetAttrNode(recordNode,"AudioPID",AudioPID);
		//
		if(m_Version == "3.0")
		{
			parser.GetAttrNode(recordNode,"CChassisID",m_strCChassisID);
			parser.GetAttrNode(recordNode,"CPosition",m_strCPosition);
			parser.GetAttrNode(recordNode,"CModuleID",m_strCModuleID);
			parser.GetAttrNode(recordNode,"CTranscode",m_strCTranscode);
		}
		//
#ifdef ZONG_JU_ZHAO_BIAO
		parser.GetAttrNode(recordNode,"Lifecycle",m_strExpireDays);	//2.5接口
#else
		parser.GetAttrNode(recordNode,"ExpireDays",m_strExpireDays);
#endif
		if (m_strExpireDays!="")
		{
			mRecordInfo.expiredays = m_strExpireDays;
		}

	}

	//属性赋值给mRecordInfo结构体
	if (TaskID.length()==0)
	{
		TaskID="0";
	}
	mRecordInfo.dvbtype = OSFunction::GetEnumDVBType(downType);	//监测类型

	CHANNELINFOMGR::instance()->GetChanIdByFreqServId(mRecordInfo.dvbtype, Freq, ServiceID, mRecordInfo.channelID);
	//CHANNELINFOMGR::instance()->GetChannelID(mRecordInfo.dvbtype,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,mRecordInfo.channelID);//获得频道id
	if(mRecordInfo.channelID.length()<1)
		mRecordInfo.channelID=Freq;
	
	mRecordInfo.taskid=TaskID;
}

TSRecorder::~TSRecorder(void)
{
	DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
}

bool TSRecorder::CreateRecFile()
{
	bool ret=false;
	std::string path = "";

	while(1)
	{
		std::string storetype = PROPMANAGER::instance()->GetStoreType();	//获取录像存储方式：分散 还是 顺序
		if (storetype == "0")
			DISKOPERATION::instance()->CheckDiskIsFullForRecord(path);	//顺序，deviceid按默认值传递
		else if (storetype == "1")
			DISKOPERATION::instance()->CheckDiskIsFullForRecord(path, DeviceID);  //分散

		//检查录像存储磁盘空间大小
		path = path  + StrUtil::Int2Str(DeviceID) + string("/");
		CreateDirectory(path.c_str(), NULL);
		if (path.length()==0 || path.length()<1)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t)获取录像存储路径失败!\n"));
			
			std::string strMsg = "通道：";
			strMsg += StrUtil::Int2Str(DeviceID);
			strMsg += "获取录像存储路径失败!";
			
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
			OSFunction::Sleep(0,50);
			continue;
		}
		break;
	}	

	//获得文件名：通道号_日期
	std::string datetime=TimeUtil::DateTimeToString(time(0));
	datetime=datetime.substr(4, datetime.size()- 4);
	mFileName=path + StrUtil::Int2Str(DeviceID) + string("_")  + TaskID + string("_") + datetime;
	
	mFileName+=".ts";
	
	//创建文件
	mStartTime=TimeUtil::GetCurDateTime();
	ret=mPFile.CreateNewFile(mFileName.c_str());
	if (!ret)
	{
		std::string strMsg = mFileName;
		strMsg += "创建文件失败!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	else
	{
		std::string strMsg = mFileName;
		strMsg += "创建文件成功!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	mCurTime = time(0);												//更新开始时间
	ret = Save2DB();														//录像文件记录存入数据库
	if (!ret)
	{
		std::string strMsg = mFileName;
		strMsg += "入库失败!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		strMsg = mFileName;
		strMsg += "文件删除!";
		DeleteFile(mFileName.c_str());//注意这里删除文件对硬盘的影响
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		Sleep(10000);
	}
	else
	{
		std::string strMsg = mFileName;
		strMsg += "入库成功!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 创建[%s]!\n",mFileName.c_str()));
	

	mIsCreate=ret;	
	return ret;
}
bool TSRecorder::SaveFile( unsigned char* buf,size_t num )
{
	UpAlarmInfo info;
	PROPMANAGER::instance()->GetUpAlarmInfo(mRecordInfo.dvbtype,info);

	if(info.runplanrecord=="1" && mRecordInfo.taskid=="0" && RUNPLANINFOMGR::instance()->InRunPlan(mRecordInfo.dvbtype,mRecordInfo.channelID)==false)
	{
		if(mIsCreate)
		{
			DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
			CloseFile();//关闭文件
		}
		return false;
	}
	bool ret=false;
	if (!mIsCreate)//文件是否创建
	{
		CreateRecFile();
	}

	if (time(0)-mCurTime>StrUtil::Str2Int(PROPMANAGER::instance()->GetRecordPeriod())*60/*单位：秒；从配置文件中获得*/)
	{
		DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
		CloseFile();//关闭文件
		CreateRecFile();//创建新文件
		mbIsUpdateDbEndtime = false;
	}
	//
	if(mbIsUpdateDbEndtime)
	{
		if(time(0)-mUpdateDbEndtime >= 10)
		{
			DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
			mUpdateDbEndtime = time(0);
		}
	}
	//
	if (mIsCreate)
	{
		int numWrite=mPFile.WriteBuf((void*)buf,(int)num,true);//数据写入文件
		if (numWrite>0)
			ret=true;
		//DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());//没写一次文件就保存一次结束时间 严重影响程序性能
	}
	return ret;
}

void TSRecorder::CloseFile( void )
{
	mIsCreate=false;
	mPFile.Close();
}

bool TSRecorder::Save2DB()
{
	if(m_Version == "3.0")
	{
		mRecordInfo.DeviceID=m_strCChassisID+"_"+m_strCPosition+"_"+m_strCModuleID+"_"+m_strCTranscode;
	}
	else
	{
		mRecordInfo.DeviceID=StrUtil::Int2Str(DeviceID);
	}
	mRecordInfo.filename=mFileName;
	mRecordInfo.starttime=mStartTime;
	
	string endtime=TimeUtil::CalMinutes(mStartTime,StrUtil::Str2Int(PROPMANAGER::instance()->GetRecordPeriod()));
	mRecordInfo.endtime=endtime;
	
#ifdef ZONG_JU_ZHAO_BIAO
	//if ( !m_strExpireDays.empty() )
	//{
	//	mRecordInfo.expiredays = StrUtil::Long2Str(TimeUtil::DiffDay(m_strExpireDays, mRecordInfo.starttime));
	//}
#endif
	if (mRecordInfo.expiredays.length()==0)//如果下发的录像任务没有过期天数就读取配置文件中的过期天数
	{
		mRecordInfo.expiredays=PROPMANAGER::instance()->GetRecordExpiredays(mRecordInfo.dvbtype);//从配置文件中获得过期天数
	}
	if (mFileName.length()==0 || mStartTime.length()==0 || PROPMANAGER::instance()->GetRecordExpiredays(mRecordInfo.dvbtype).length()==0)
	{
		return false;
	}
	else
	{
		if(m_Version == "3.0")
		{
			return DBMANAGER::instance()->AddRecord30V(mRecordInfo);
		}
		else
		{
			return DBMANAGER::instance()->AddRecord(mRecordInfo);
		}
	}
	
}

bool TSRecorder::SaveManualRecordFile( unsigned char* buf,size_t num )
{
	bool ret=false;
	if (!mIsCreate)//文件是否创建
	{
		CreateRecFile();
	}
	if (mIsCreate)
	{
		int numWrite=mPFile.WriteBuf((void*)buf,(int)num,true);//数据写入文件
		if (numWrite>0)
			ret=true;
	}
	return ret;
}