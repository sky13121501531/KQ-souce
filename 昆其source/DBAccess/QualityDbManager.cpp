


#include "QualityDbManager.h"
#include "../foundation/strutil.h"
#include "../foundation/propmanager.h"
#include "../foundation/osfunction.h"




QualityDbManager::QualityDbManager():m_bStopFlag(false),m_bIsDeleting(false)
{
	
	m_strSysCleanTime = PROPMANAGER::instance()->GetSystemCleanTime();//获得每天的清理时间
}

QualityDbManager::~QualityDbManager()
{

}

bool QualityDbManager::Start()
{
	this->open(0);
	return true;
}

int QualityDbManager::open( void* )
{
	this->activate();
	return 0;
}

int QualityDbManager::svc()
{
	while (!m_bStopFlag)
	{
		OSFunction::Sleep(0,20);

		if (!m_strSysCleanTime.empty())
		{
			time_t curTm = time(0);//当前时间
			std::string strCurDateTime = TimeUtil::DateTimeToStr( curTm );
			std::string strCleanDateTime = TimeUtil::GetCurDate();
			strCleanDateTime += " ";
			strCleanDateTime += m_strSysCleanTime;

			if (abs(TimeUtil::DiffMinute(strCurDateTime,strCleanDateTime)) < 3)//与清理时间上下不超过3分钟
			{
				if (m_strLastCleanTime.compare(TimeUtil::GetCurDate()) != 0)
				{
					m_deletingMutex.acquire();
					m_bIsDeleting = true;
					m_deletingMutex.release();

					std::string strMsg = "开始删除过期指标数据!!";
					APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_BOTH);
					strCurDateTime = TimeUtil::CalDay(strCurDateTime,-7);
					QuaDelOperation::BcpDelete( strCurDateTime );
					strMsg = "删除过期指标数据完成!!";
					APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_BOTH);

					m_deletingMutex.acquire();
					m_bIsDeleting = false;
					m_deletingMutex.release();
				}
				m_strLastCleanTime= TimeUtil::GetCurDate();//更新上次清理时间
			}

		}
	}
	m_bStopFlag = true;
	return 0;
}

int QualityDbManager::Stop()
{
	m_bStopFlag = true;

	this->wait();
	return 0;
}

void QualityDbManager::AddQualityInfo( eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality)
{
	m_insertMutex.acquire();

	QualityDBInfo quaDbInfo;
	quaDbInfo.s_quaInfo = quality;
	quaDbInfo.s_dvbtype = dvbtype;
	quaDbInfo.s_strTaskId = taskid;
	m_vecQuaDbInfo.push_back( quaDbInfo );

	m_deletingMutex.acquire();
	if (m_vecQuaDbInfo.size()>=3/*00*/ && !m_bIsDeleting)	//指标条数>1000条，同时没有在删除指标数据
	{
		std::string strInsertSql= "insert into quality(dvbtype,freq,taskid,type,des,valu,checktime) ";
		std::vector<QualityDBInfo>::iterator quaIter = m_vecQuaDbInfo.begin();
		while (quaIter != m_vecQuaDbInfo.end())
		{
			string tmpSql="select '"+StrUtil::Int2Str((*quaIter).s_dvbtype)+"','";
			tmpSql+=(*quaIter).s_quaInfo.freq+"','";
			tmpSql+=(*quaIter).s_strTaskId+"','";
			tmpSql+=(*quaIter).s_quaInfo.type+"','";
			tmpSql+=(*quaIter).s_quaInfo.desc+"','";
			tmpSql+=(*quaIter).s_quaInfo.valu+"','";
			tmpSql+=(*quaIter).s_quaInfo.checktime+"'";
			strInsertSql += tmpSql;

			quaIter++;
			if (quaIter != m_vecQuaDbInfo.end())
			{
				strInsertSql += " union all ";
			}
		}

		bool ret = DBMANAGER::instance()->AddQualityInfo( strInsertSql );
		if (ret)
		{
			//printf("向数据库中插入1000条指标数据成功!!\n\n");
			
			m_vecQuaDbInfo.clear();
		}
		
	}
	m_deletingMutex.release();

	m_insertMutex.release();

	return;
}


bool QualityDbManager::QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,
		std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime)
{
	bool ret = false;
	
	m_insertMutex.acquire();
	std::vector<QualityDBInfo>::iterator quaIter = m_vecQuaDbInfo.begin();
	for (; quaIter!=m_vecQuaDbInfo.end(); quaIter++)
	{
		QualityDBInfo tmpQuaDbInfo = *quaIter;
		if (tmpQuaDbInfo.s_dvbtype == dvbtype &&
			tmpQuaDbInfo.s_strTaskId == taskid &&
			tmpQuaDbInfo.s_quaInfo.freq == freq &&
			TimeUtil::DiffSecond(tmpQuaDbInfo.s_quaInfo.checktime, startdatetime)>=0 &&
			TimeUtil::DiffSecond(tmpQuaDbInfo.s_quaInfo.checktime, enddatetime)<0
			)
		{
			vecQuality.push_back( tmpQuaDbInfo.s_quaInfo );
		}
	}
	m_insertMutex.release();

	ret = DBMANAGER::instance()->QueryQualityInfo(dvbtype,freq,taskid,vecQuality,startdatetime,enddatetime);
	return ret;
}