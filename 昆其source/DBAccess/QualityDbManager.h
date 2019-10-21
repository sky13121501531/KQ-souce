#ifndef QualityDbManager_h__
#define QualityDbManager_h__


#include "ace/Task.h"
#include "ace/Singleton.h"
#include "../foundation/typedef.h"
#include "../foundation/applog.h"
#include "../foundation/strutil.h"
#include "./DBManager.h"


class QuaDelOperation
{
public:
	static void DirectDelete(std::string strCheckTime)
	{
		DBMANAGER::instance()->DelQualityInfo( strCheckTime );
		return;
	}

	static void BcpDelete(std::string strCheckTime)
	{
		std::string strBakFileName = "c:/vscttb/db/tmpdb.txt";
		std::string strOutFileName = "c:/vscttb/db/bcpout.txt";
		//把有效数据备份到文件中
		std::string strBcpCommand = "bcp \"select * from vsdvb.dbo.quality where checktime>='";
		strBcpCommand += strCheckTime;
		strBcpCommand += "'\" queryout ";
		strBcpCommand += strBakFileName;
		strBcpCommand += " -o ";
		strBcpCommand += strOutFileName;
		strBcpCommand += " -c -t \"|\" -r \\n -U sa -P";
		int nret = system( strBcpCommand.c_str() );
 		DWORD dwError = GetLastError();
 		//printf("errno is : %d", dwError );
// 		if (nret==0 && dwError==ENOENT)	//没有该操作符
// 		{
// 			std::string strBakMsg = "系统中没有bcp.exe !!";
// 			APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strBakMsg,LOG_OUTPUT_BOTH);
// 			return;
// 		}
		if (nret < 0)
		{
			std::string strBakMsg = "quality数据导出到文件失败，errornum : ";
			strBakMsg += StrUtil::Int2Str(dwError);
			APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strBakMsg,LOG_OUTPUT_BOTH);
			return;
		}
		std::string strBakMsg = "quality数据导出到文件成功!!";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strBakMsg,LOG_OUTPUT_BOTH);

		//truncate quality
		DBMANAGER::instance()->TruncateTable( std::string("quality") );

		//恢复quality数据
		strBcpCommand = "bcp vsdvb.dbo.quality in ";
		strBcpCommand += strBakFileName;
		strBcpCommand += " -o ";
		strBcpCommand += strOutFileName;
		strBcpCommand += " -b 5000 -c -t \"|\" -r \\n -U sa -P";
		system( strBcpCommand.c_str() );
		std::string strLoadMsg = "quality数据导入到数据库完成!!";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strLoadMsg,LOG_OUTPUT_BOTH);

		//delete file
		DeleteFile( strBakFileName.c_str() );

		return;
	};
};


class QualityDbManager : public ACE_Task<ACE_MT_SYNCH>
{
public:
	QualityDbManager();
	virtual ~QualityDbManager();

public:
	bool Start();
	int open(void*);
	virtual int svc();
	int Stop();

public:
	void AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality);
	bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime);

public:
	bool m_bStopFlag;
	bool m_bIsDeleting;

	std::string m_strSysCleanTime;
	std::string m_strLastCleanTime;

	std::vector<QualityDBInfo> m_vecQuaDbInfo;

	ACE_Thread_Mutex m_insertMutex;
	ACE_Thread_Mutex m_deletingMutex;
	
};


typedef ACE_Singleton<QualityDbManager,ACE_Mutex>  QUALITYDBMANAGER;














#endif // QualityDbManager_h__