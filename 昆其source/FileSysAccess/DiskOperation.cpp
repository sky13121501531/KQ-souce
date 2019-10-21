///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DiskOperation.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-18
// 内容描述：磁盘文件操作类
///////////////////////////////////////////////////////////////////////////////////////////
#include "DiskOperation.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/FileOperater.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"


#if defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#include <sys/vfs.h> 
#endif
#include <time.h>  

DiskOperation::DiskOperation()
{
	Init();
}

DiskOperation::~DiskOperation()
{

}

void DiskOperation::Init()
{
	mPreTime = time(0);
	
	mDirNameVec.clear();//gxd 20110331 多个录像存储路径
	PROPMANAGER::instance()->GetRecFileLocVec(mDirNameVec);
	mLowestSpace=StrUtil::Str2Int(PROPMANAGER::instance()->GetMaxAvailableSize());//获得录像文件所在磁盘的最小空闲值
}

long DiskOperation::CalculateFreeSpace(const std::string& dirName)
{
#if defined(WIN32) || defined(__WIN32__)
	unsigned __int64 FreeBytesAvailableToCaller = 0;
	std::string dPath = dirName.substr(0,dirName.find_first_of(":/",0));//录像文件存储的盘符
	dPath.append(":\\");
	::GetDiskFreeSpaceEx(dPath.c_str(),
		(ULARGE_INTEGER *)&FreeBytesAvailableToCaller,NULL,NULL);//获得磁盘可用空间

	return (long)(FreeBytesAvailableToCaller/(1024*1024*1024));
#else
	long long FreeBytesAvailableToCaller = 0;
	struct statfs buf;
	memset(&buf,0,sizeof(buf));
	if (-1 != statfs(dirName.c_str(), &buf))
		FreeBytesAvailableToCaller = (long long)(buf.f_bsize * buf.f_bavail)/(1024*1024);
	return FreeBytesAvailableToCaller;
#endif
}

bool DiskOperation::DeleteOldestFileInPath(unsigned short delcount, const std::string& recPath)
{
	bool bRet = false;

	std::vector<std::string> vecfilename;
	DBMANAGER::instance()->QueryRecordTopInPath(delcount, recPath, vecfilename);
	//如果该路径在数据库中已经没有录像记录了，但是仍然空间不足，
	//就认为该文件夹下现有所有文件为垃圾文件，删除文件夹下所有文件
	if ( vecfilename.empty() )	
	{
		std::string strMsg = recPath;
		strMsg += "在数据库中已经没有记录，但是仍然空间不足,所以删除该文件夹下所有文件。";
		std::cout<<"===================="<<std::endl;
		std::cout<<strMsg<<std::endl;
		std::cout<<"===================="<<std::endl;
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);

		std::string strDelCmd = "del ";
		strDelCmd += recPath;
		strDelCmd += " \\Q";

		system( strDelCmd.c_str() );
	}

	std::vector<std::string>::iterator iter=vecfilename.begin();
	for (;iter!=vecfilename.end(); ++iter)
	{
		if(DeleteRecFile(*iter))								//删除磁盘文件
			DBMANAGER::instance()->DeleteRecord(*iter);			//删除数据库

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 删除[%s]!\n",(*iter).c_str()));
		OSFunction::Sleep(0,10);
	}
	bRet = true;
	return bRet;
}


bool DiskOperation::DeleteOldestFileByDays(unsigned short delnum)
{
	bool bRet = false;	

	std::vector<std::string> filenames;
	DBMANAGER::instance()->QueryRecordTop(delnum,filenames);

	for (std::vector<std::string>::iterator iter=filenames.begin();iter!=filenames.end(); ++iter)
	{
		if(DeleteRecFile(*iter))								//删除磁盘文件
			DBMANAGER::instance()->DeleteRecord(*iter);			//删除数据库

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 删除123[%s]!\n",(*iter).c_str()));
		OSFunction::Sleep(0,10);
	}
	bRet = true;
	return bRet;
}

bool DiskOperation::CheckDiskIsFullForRegular(int nTimeValue)
{
	bool bRet = false;

	if (nTimeValue > 0)//检测时间间隔
	{
		time_t currentTime = time(0);
		if ((currentTime - mPreTime) >= nTimeValue)
		{
			mPreTime = currentTime;								//更新上次检测时间
		}
		else
		{
			return bRet;
		}
	}

	std::string storetype = PROPMANAGER::instance()->GetStoreType();	//获取录像存储方式：分散 还是 顺序)
	
	if (storetype == "0" || storetype.length()<1)	//顺序存储或者未定义，磁盘全满时做删除逻辑
	{
		for(int i=0;i<100;i++)//while改为for防止删除无效导致无止境删除
		{
			std::vector<std::string>::iterator diriter=mDirNameVec.begin();
			for (;diriter!=mDirNameVec.end(); ++diriter)
			{
				long dirSpace = CalculateFreeSpace(*diriter);		//检查每个路径的剩余空间
				if(dirSpace > mLowestSpace+2)							//某个盘符空间符合要求，则无需删除
				{
					break;
				}
				OSFunction::Sleep(0,10);
			}
			if (diriter == mDirNameVec.end())						//如果所有磁盘空间都不符合要求
			{
				cout<<"磁盘空间不足开始删除文件storetype == 0 || storetype==.............."<<endl;
				DeleteOldestFileByDays(100);						//定时检查时删除100条历史记录
				cout<<"结束删除文件storetype == 0 || storetype==.............."<<endl;
				Sleep(100);
				continue;
			}
			break;													//如果任何一个磁盘空间符合要求则退出本次检查
		}
	}
	else if (storetype == "1")				//分散存储，每个不足空间的磁盘都需要删除
	{
		std::vector<std::string>::iterator diriter = mDirNameVec.begin();
		long dirSpace = 0;
		for (;diriter!=mDirNameVec.end(); ++diriter)
		{
			long cursp = dirSpace;
			dirSpace = CalculateFreeSpace(*diriter);		//检查每个路径的剩余空间
			if(dirSpace <= mLowestSpace+5)						//某个盘符空间不足
			{
				cout<<"磁盘空间不足开始删除文件storetype == 1.............."<<endl;
				DeleteOldestFileInPath(100,*diriter);			//定时检查时删除100条历史记录
				cout<<"结束删除文件storetype == 1.............."<<endl;
				diriter = mDirNameVec.begin();					//重新检查
				OSFunction::Sleep(0,10);
				if(dirSpace <= cursp)//写文件的速度大于删除的速度是不太可能的,应该是删除无效,退出.
				{
					cout<<"结束删除文件,删除文件无效退出.............."<<endl;
					break;
				}
				continue;		
			}
			OSFunction::Sleep(0,10);
		}									
	}
	return true;
}
long DiskOperation::GetTotalFreeSpace()
{
	long TotalSpace = 0;
	std::vector<std::string>::iterator diriter=mDirNameVec.begin();
	for (;diriter!=mDirNameVec.end(); ++diriter)
	{
		TotalSpace += CalculateFreeSpace(*diriter);
		OSFunction::Sleep(0,5);
	}
	return TotalSpace;
}
bool DiskOperation::CheckDiskIsFullForRecord(std::string& validpath,int deviceid)
{
	bool ret = false;
	int CheckTimes = 50;	//检测次数
	if (deviceid == -1)		//顺序
	{
		while(1)
		{
			std::vector<std::string>::iterator diriter=mDirNameVec.begin();
			for (;diriter!=mDirNameVec.end(); ++diriter)
			{
				long dirSpace = CalculateFreeSpace(*diriter);		//检查每个路径的剩余空间
				if(dirSpace > mLowestSpace)							//某个盘符空间符合要求
				{
					validpath = *diriter;
					break;
				}
				OSFunction::Sleep(0,5);
			}
			if (diriter == mDirNameVec.end() && --CheckTimes)		//如果所有磁盘空间都不符合要求
			{
				DeleteOldestFileByDays(50);							//录制检查时删除50条历史记录
				OSFunction::Sleep(0,10);
				continue;
			}
			break;													//如果任何一个磁盘空间符合要求则退出本次检查
		}
	}
	else		//分散
	{
		std::string recPath = PROPMANAGER::instance()->GetRecPathByDevId(deviceid);		//通过通道号获取存储路径
		if ( recPath.empty() )
			return false;

		while (1)
		{
			long lDirSpace = CalculateFreeSpace(recPath);		//获取空闲空间
			if (lDirSpace > mLowestSpace)
			{
				validpath = recPath;
				break;
			}
			else if( --CheckTimes)
			{
				DeleteOldestFileInPath(50, recPath);		//删除recpath下50条历史记录
				OSFunction::Sleep(0,10);
				continue;
			}
			break;
		}	
	}
	
	return true;	//调用函数会判断validpath是否为空
}

bool DiskOperation::ProcessInprireFiles()
{
	bool bRet = false;
	std::vector<std::string> filenames;

	/* 处理过期录像文件 */
	DBMANAGER::instance()->QueryRecordExpire(filenames);		//查询过期录像录音文件
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 开始删除录音录像文件,共[%d]个!\n",(int)filenames.size()));
	for (std::vector<std::string>::iterator iter=filenames.begin(); iter!=filenames.end(); ++iter)
	{
		DeleteRecFile(*iter);									//删除过期录音录像文件
		DBMANAGER::instance()->DeleteRecord(*iter);				//删除数据库中的录音录像记录
		//std::cout<<"删除录音录像文件："<<*iter<<std::endl;
		OSFunction::Sleep(0,500);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 结束删除录音录像文件,共[%d]个!\n",(int)filenames.size()));

	filenames.clear();

	/* 处理过期临时文件 */
	DBMANAGER::instance()->QueryTempFileExpire(filenames);		//查询过期临时文件
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 开始删除临时文件,共[%d]个!\n",(int)filenames.size()));
	for (std::vector<std::string>::iterator iter=filenames.begin(); iter!=filenames.end(); ++iter)
	{
		DeleteTempFile(*iter);									//删除过期临时文件
		DBMANAGER::instance()->DeleteTempFile(*iter);			//删除数据库中的临时文件记录
		OSFunction::Sleep(0,500);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 结束删除临时文件,共[%d]个!\n",(int)filenames.size()));

	/* 处理过期日志文件 */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 开始删除过期日志文件!\n"));
	DeleteExpireLog();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 结束删除过期日志文件!\n"));

	/* 处理垃圾录像文件 */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 开始删除垃圾录像文件!\n"));
	DeleteTrashFile();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 结束删除垃圾录像文件!\n"));

	/* 处理7天前报警数据 */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 开始删除数据库记录!\n"));
	DeleteDbData();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 结束删除数据库记录!\n"));

	bRet = true;
	return bRet;
}

bool DiskOperation::DeleteRecFile( std::string& filename )
{
	ACE_Guard<ACE_Thread_Mutex> guard(mDelMutex);
	bool bRet = true;

#if defined(WIN32) || defined(__WIN32__)
	 if( (_access( filename.c_str(),F_OK )) == -1 )		
		 return true/*false*/;		//如果文件不存在了，也认为是已经删除了
#endif

	if (!filename.empty())
	{
#if defined(WIN32) || defined(__WIN32__)
		if (0== DeleteFile(filename.c_str()))
		{
			string msg = "删除录像文件出错:"+filename;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			bRet=false;
		}
#else
		if (-1 == remove(filename.c_str()))
		{
			bRet=false;
		}
#endif
	}

	return bRet;
}


bool DiskOperation::DeleteTempFile(std::string& filename)
{
	ACE_Guard<ACE_Thread_Mutex> guard(mDelMutex);
	bool bRet = true;

#if defined(WIN32) || defined(__WIN32__)
	if( (_access( filename.c_str(),F_OK )) == -1 )
		return false;
#endif

	if (!filename.empty())
	{
#if defined(WIN32) || defined(__WIN32__)
		if (0 == DeleteFile(filename.c_str()))
		{
			bRet=false;
		}
#else
		if (-1== remove(filename.c_str()))
		{
			bRet=false;
		}
#endif
	}

	return bRet;
}

//删除过期日志文件
bool DiskOperation::DeleteExpireLog()			
{
	string logpath = PROPMANAGER::instance()->GetLogPath();		//c:/vscttb/log/
	logpath += "*.*";

	string logExpire = PROPMANAGER::instance()->GetLogExpire();
	//通过以下方法取负值
	int expire = (0 - StrUtil::Str2Int(logExpire) );

	//计算过期日期
	string CurDateTime = TimeUtil::GetCurDateTime();
	string logExpireDateTime = TimeUtil::CalDay(CurDateTime, expire);
	string logExpireDate = TimeUtil::GetDateFromDatetime(logExpireDateTime);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(logpath.c_str(), &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				//获取log的不同类型的文件夹名称
				string logtype = string(FindFileData.cFileName);
				string templogpath = logpath;
				templogpath.erase(15);
				//生成文件夹路径
				string DirName = templogpath + logtype + string("/")+string("*.*");

				WIN32_FIND_DATA ChildFindFileData;
				HANDLE ChildhFind;

				//获取具体日志文件
				ChildhFind = FindFirstFile(DirName.c_str(), &ChildFindFileData);
				while(ChildhFind !=INVALID_HANDLE_VALUE)
				{
					if (strcmp(ChildFindFileData.cFileName, ".") != 0 && strcmp(ChildFindFileData.cFileName, "..") != 0)
					{
						//实际文件名，格式为：xxxx-xx-xx.txt
						string FileNameTemp = string(ChildFindFileData.cFileName);

						//取出文件名中的日期信息，用于比较
						string fileDate = FileNameTemp.substr(0, 10);
						if (TimeUtil::DiffDate(logExpireDate, fileDate) > 0)	//如果过期
						{
							string tempDirName = DirName;
							tempDirName.erase( tempDirName.find_last_of("/")+1 );
							string fullFileName = tempDirName + FileNameTemp;	//deletefile函数参数必须包含实际路径

							int ret = DeleteFile( fullFileName.c_str() );	//删除
							if ( !ret )
							{
								string msg = "删除日志文件失败，文件名为" + FileNameTemp;
								APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
								//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
							}

							//写日志
							std::string msg = string("删除日志文件成功，文件名为：") + fullFileName;
							APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
							//SYSMSGSENDER::instance()->SendMsg(msg);

						}
					}
					if (!FindNextFile(ChildhFind, &ChildFindFileData))
					{
						break;
					}
				}
				FindClose(ChildhFind);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);

	return true;
}

void DiskOperation::DeleteTrashFile()
{
	//获取录像文件存储路径容器
	std::vector<std::string> vecStrFilePath;
	PROPMANAGER::instance()->GetRecFileLocVec( vecStrFilePath );	

	//遍历各个文件存储路径，删除垃圾文件
	std::vector<std::string>::iterator iterPath = vecStrFilePath.begin();
	for(; iterPath!=vecStrFilePath.end(); iterPath++)
	{
		//获取该路径下、数据库中最早的录像记录的开始时间
		std::string strLastDateTime ;
		bool ret = DBMANAGER::instance()->QueryLastRecordDateTime(*iterPath, strLastDateTime);
		if (!ret)
		{
			std::string strError = "获取数据库中路径：";
			strError += *iterPath;
			strError += "下最早的录像记录的开始时间失败!!";
			APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strError,LOG_OUTPUT_FILE);
			return;
		}
		std::string strMsg = "获取到数据库中路径：";
		strMsg += *iterPath;
		strMsg += "下最早的录像记录的开始时间: ";
		strMsg += strLastDateTime;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		
		if (strLastDateTime.length()<1)
		{
			strLastDateTime = TimeUtil::GetCurDateTime();
		}

		//最后一天的日期
		std::string strLastDate = TimeUtil::GetDateFromDatetime(strLastDateTime);

		int nDeleteCount = 5000;		//每个路径删除5000个文件
		std::string strFindPath = *iterPath + "*.*";
		std::string strLastDelFileName = "";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile( strFindPath.c_str(), &FindFileData);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				if (nDeleteCount <= 0)	//只删除nDeleteCount条，防止占用太多时间
				{
					break;
				}

				//获取文件的创建时间
				FILETIME fileTime;
				memset(&fileTime, 0, sizeof(fileTime) );
				fileTime = FindFileData.ftCreationTime;
				
				//将FILETIME转化为SYSTEMTIME
				SYSTEMTIME sysTime;
				memset(&sysTime, 0, sizeof(sysTime) );
				FileTimeToSystemTime(&fileTime, &sysTime);
				

				//SYSTEMTIME转化为string
				std::string strTempFileDate;
				char now[64];	
				sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, 
					sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
				strTempFileDate = now;

				//将时间转化为北京时间
				strTempFileDate = TimeUtil::CalHour(strTempFileDate, 8);
				strTempFileDate = TimeUtil::GetDateFromDatetime(strTempFileDate);

				//如果文件创建时间早于数据库最早记录的开始时间，就删除文件
				std::string strCurFileName = FindFileData.cFileName;
				if ( TimeUtil::DiffDate(strTempFileDate,strLastDate) < 0 )
				{
					strLastDelFileName = strCurFileName;
					strCurFileName = *iterPath + strCurFileName;
					DeleteRecFile(strCurFileName);
					nDeleteCount --;	//不论删除与否，都将nDeleteCount减一	
				}
				
				OSFunction::Sleep(0, 10);
			}
			
			if (!FindNextFile(hFind, &FindFileData))
			{
				break;
			}
		}
		FindClose(hFind);

		std::string strDelMsg = "路径：";
		strDelMsg += *iterPath;
		strDelMsg += "下最后删除的垃圾录像文件名: ";
		strDelMsg += strLastDelFileName;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strDelMsg,LOG_OUTPUT_FILE);
	}
	
	return ;
}

void DiskOperation::DeleteDbData()
{
	std::string strCurDateTime = TimeUtil::GetCurDateTime();
	std::string strAlarmTime = TimeUtil::CalDay(strCurDateTime, -7);
	
	DBMANAGER::instance()->DeleteAlarmInfo( strAlarmTime );

	return;
}