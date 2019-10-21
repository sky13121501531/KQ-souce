#include "SqlServerDBConnection.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/SqlServerStrUtil.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TypeDef.h"
SqlServerDBConnection::SqlServerDBConnection(void)
{
	DBCon=new CADODatabase;
	BandDBOK = false;
	BandDBCon=new CADODatabase;
}

SqlServerDBConnection::~SqlServerDBConnection(void)
{
	if (DBCon!=NULL)
	{
		delete DBCon;
		DBCon=NULL;
	}
	//
	if (BandDBCon!=NULL)
	{
		delete BandDBCon;
		BandDBCon=NULL;
	}
}

bool SqlServerDBConnection::InitConnect( void )
{
	bool ret = false;
	int count = 0;

 	string path = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=" + PROPMANAGER::instance()->GetDbUsername()+";Password="+PROPMANAGER::instance()->GetDbPwd()
 		+ ";Initial Catalog=" + PROPMANAGER::instance()->GetDbName() + ";Data Source=" + PROPMANAGER::instance()->GetDbIp();//连接sqlserver字符串，oledb方式
	//string path = "provider=SQLNCLI;Server=localhost;Database=vsdvb;Trusted_Connection=yes";
	try
	{
		while(!DBCon->IsOpen() && count < 3)
		{
			DBCon->Open(_bstr_t(path.c_str()));//连接数据库
			count ++;
		}
		if(DBCon->IsOpen())
			ret = true;
	}
	catch (...) 
	{
		APPLOG::instance()->WriteLog(DEVICE, LOG_EVENT_ERROR, "打开数据库出错");
		return false;
	}
	//
	if((PROPMANAGER::instance()->GetBandDbIp()).size()>0)
	{
 		string bandpath = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=" + PROPMANAGER::instance()->GetDbUsername()+";Password="+PROPMANAGER::instance()->GetDbPwd()
 			+ ";Initial Catalog=" + PROPMANAGER::instance()->GetDbName() + ";Data Source=" + PROPMANAGER::instance()->GetBandDbIp();//连接sqlserver字符串，oledb方式
		try
		{
			while(!BandDBCon->IsOpen() && count < 3)
			{
				BandDBCon->Open(_bstr_t(bandpath.c_str()));//连接数据库
				count ++;
			}
			if(BandDBCon->IsOpen())
			{
				BandDBOK = true;
			}
		}
		catch (...) 
		{
			APPLOG::instance()->WriteLog(DEVICE, LOG_EVENT_ERROR, "打开备机数据库出错");
		}
	}
	return ret;
}

bool SqlServerDBConnection::DisConnect( void )
{
	if (DBCon->IsOpen())
	{
		 DBCon->Close();
	}
	//
	if (BandDBCon->IsOpen())
	{
		 BandDBCon->Close();
	}
	return true;
}

bool SqlServerDBConnection::AddXmlTask( eDVBType type,std::string xml )
{
	int id=type;
	bool ret=false;
	//DeleteTask(type,xml);
	//std::string sql="insert into task(dvbtype,taskinfo) values('"+StrUtil::Int2Str(id)+"','"+xml+"')";
	
	std::string strDelsql="";
	if(type==UNKNOWN)
	{
		strDelsql="delete from task where dvbtype='"+StrUtil::Int2Str(id)+"' ";
	}
	else
	{
		strDelsql="delete from task where dvbtype='"+StrUtil::Int2Str(id)+"' and taskinfo='"+xml+"' ";
	}

	std::string strInsertSql="insert into task(dvbtype,taskinfo) values('"+StrUtil::Int2Str(id)+"','"+xml+"') ";

	std::string strTransSql = "begin tran ";
	strTransSql += strDelsql;
	strTransSql += strInsertSql;
	strTransSql += "if @@error=0 commit else rollback";
	try
	{
		ret=DBCon->Execute(strTransSql.c_str());//执行添加任务sql

		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strTransSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加任务出错",LOG_OUTPUT_FILE);
			}
		}
		//
		std::string strTaskMsg = "数据库添加任务事务执行";
		strTaskMsg += ret?"成功!! \xd\xa":"失败!! \xd\xa";
		strTaskMsg += strTransSql;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strTaskMsg,LOG_OUTPUT_FILE);
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向数据库中添加任务出错",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteTask( eDVBType type,std::string xml )
{
	int id=type;
	bool ret=false;
	std::string sql="";
	if(type==UNKNOWN)
	{
		sql="delete from task where dvbtype='"+StrUtil::Int2Str(id)+"'";
	}
	else
	{
		sql="delete from task where dvbtype='"+StrUtil::Int2Str(id)+"' and taskinfo='"+xml+"'";
	}
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行删除任务sql
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从备机数据库中删除任务出错",LOG_OUTPUT_FILE);
			}
		}
		//

		std::string strTaskMsg = "从数据库中删除任务";
		strTaskMsg += ret?"成功!! \xd\xa":"失败!! \xd\xa";
		strTaskMsg += xml;
		
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strTaskMsg,LOG_OUTPUT_FILE);
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从数据库中删除任务出错",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryTask( eDVBType type,std::vector<std::string>& vecXml )
{
	int id=type;
	bool ret=false;
	std::string sql="select taskinfo from task where dvbtype='"+StrUtil::Int2Str(id)+"'";
	try
	{

		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行查询任务sql
		{
			std::vector<std::string> TaskXMLVec;
			std::string str;
			while(!record.IsEof())
			{
				record.GetFieldValue("taskinfo",str);//获得任务xml
				TaskXMLVec.push_back(str);
				record.MoveNext();//移动到下一条记录
			}
			ret=true;
			//*****************录像任务按通道排序************************
			int chanindex[500];
			memset(chanindex,-1,sizeof(int)*500);
			std::vector<int> deviceidVec;
			std::vector<std::string> vecTaskNotRecord;//非录像任务
			std::vector<std::string> vecTaskRecord;//录像任务
			for (int h=0; h< TaskXMLVec.size();h++)
			{
				XmlParser tempsr;
				int deviceid = -1;
				tempsr.Set_xml(TaskXMLVec[h]);
				pXMLNODE reNode;
				if(TaskXMLVec[h].find("AutoRecord")!=string::npos)
				{
					reNode=tempsr.GetNodeFromPath("Msg/AutoRecord/Record");
				}
				else if (TaskXMLVec[h].find("TaskRecord")!=string::npos)
				{
					reNode=tempsr.GetNodeFromPath("Msg/TaskRecord/Record");
				}
				else
				{
					vecTaskNotRecord.push_back(TaskXMLVec[h]);
					continue;
				}
				vecTaskRecord.push_back(TaskXMLVec[h]);
				tempsr.GetAttrNode(reNode,"DeviceID",deviceid);
				if(deviceid>=0)
				{
					deviceidVec.push_back(deviceid);
				}
				else
				{
					deviceidVec.push_back(1000);
				}
				//
			}
			for (int m=0; m< deviceidVec.size();m++)
			{
				int minval = 1000;
				int mindex = -1;
				for(int n=0; n< deviceidVec.size();n++)
				{
					if(deviceidVec[n]<minval)
					{
						minval = deviceidVec[n];
						mindex = n;
					}
				}
				chanindex[m] = mindex;
				deviceidVec[mindex] = 1000;
				//
			}
			for (int n=0; n< vecTaskRecord.size();n++)
			{
				if(chanindex[n]>=0)
				{
					vecXml.push_back(vecTaskRecord[chanindex[n]]);
				}
			}
			int iNotRecordSize = vecTaskNotRecord.size();
			if (iNotRecordSize > 0)
			{
				for(int i = 0; i < iNotRecordSize; i++)
				{
					vecXml.push_back(vecTaskNotRecord[i]);
				}
			}
			//***********************************************
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryTask( std::vector<std::string>& vecXml )
{
	bool ret=false;
	std::string sql="select taskinfo from task";
	try
	{

		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行查询任务sql
		{
			std::string str;
			while(!record.IsEof())
			{
				record.GetFieldValue("taskinfo",str);//获得任务xml
				vecXml.push_back(str);
				record.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddChannel( eDVBType type,std::string channelid,std::string xml )
{
	int id=type;
	bool ret=false;
	//std::string delsql="delete from channel where dvbtype='"+StrUtil::Int2Str(id)+"' and channelid='"+channelid+"'";//删除sql
	std::string addsql="insert into channel(dvbtype,channelid,channelinfo) values('"+StrUtil::Int2Str(id)+"','"+channelid+"','"+xml+"')";
	try
	{
		ret=DBCon->Execute(addsql.c_str());//执行删除频道，再执行添加频道信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(addsql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加频道扫描结果出错",LOG_OUTPUT_FILE);
			}
		}
		//

		std::string strTaskMsg = "向数据库中添加频道：dvbtype==";
		strTaskMsg += StrUtil::Int2Str( type );
		strTaskMsg += "  channelId==";
		strTaskMsg += channelid;
		strTaskMsg += " channelinfo : \xd\xa";
		strTaskMsg += xml;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strTaskMsg,LOG_OUTPUT_FILE);
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向数据库中添加频道扫描结果出错",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteChannel( eDVBType type,std::string channelid)
{
	int id=type;
	bool ret=false;
	std::string sql="delete from channel where dvbtype='"+StrUtil::Int2Str(id)+"' and channelid='"+channelid+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行删除频道信息sql
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从备机数据库中删除频道扫描结果出错",LOG_OUTPUT_FILE);
			}
		}
		//

		std::string strTaskMsg = "从数据库中删除频道：dvbtype==";
		strTaskMsg += StrUtil::Int2Str( type );
		strTaskMsg += "  channelId==";
		strTaskMsg += channelid;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strTaskMsg,LOG_OUTPUT_FILE);
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从数据库中删除频道扫描结果出错",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteChannelByType( eDVBType type)
{
	int id=type;
	bool ret=false;
	std::string sql="delete from channel where dvbtype='"+StrUtil::Int2Str(id)+"' ";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行删除频道信息sql
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从备机数据库中删除频道扫描结果出错",LOG_OUTPUT_FILE);
			}
		}
		//

		std::string strTaskMsg = "从数据库中删除监测类型为 : ";
		strTaskMsg += StrUtil::Int2Str(type);
		strTaskMsg += " 的所有频道!!";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strTaskMsg,LOG_OUTPUT_FILE);
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"从数据库中删除频道扫描结果出错",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryChannel( eDVBType type,std::string channelid,std::string& xml )
{
	int id=type;
	bool ret=false;
	std::string sql="select channelinfo from channel where dvbtype='"+StrUtil::Int2Str(id)+"' and channelid='"+channelid+"'";
	try
	{

		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行查询频道sql
		{
			while(!record.IsEof())
			{
				record.GetFieldValue("channelinfo",xml);//获得频道信息的xml
				record.MoveNext();//移动下一条记录到
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryAllChannel( std::vector<std::string>&vecXml )
{
	bool ret=false;
	std::string sql="select channelinfo from channel";
	try
	{
		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行频道查询sql
		{
			while(!record.IsEof())
			{
				string xml;
				record.GetFieldValue("channelinfo",xml);//获得频道信息xml
				vecXml.push_back(xml);
				record.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryTypeChannel( eDVBType type,std::vector<std::string>&vecXml )
{
	bool ret=false;
	int id=type;
	std::string sql="select channelinfo from channel where dvbtype='"+StrUtil::Int2Str(id)+"'";
	try
	{
		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行频道查询sql
		{
			while(!record.IsEof())
			{
				string xml;
				record.GetFieldValue("channelinfo",xml);//获得频道信息xml
				vecXml.push_back(xml);
				record.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddRecord30V( sRecordInfo record )
{
	bool ret=false;
	//***
	string strexpiredays = record.expiredays;
	if(record.expiredays.size() < 9)
	{
		long expiresecond = StrUtil::Str2Int(record.expiredays)*24*3600;
		strexpiredays = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(record.starttime)+expiresecond);
	}

	//***
	int type=record.dvbtype;
	string sql="insert into record(dvbtype,deviceid,channelid,taskid,filename,starttime,endtime,expiredays) values('";
	sql+=StrUtil::Int2Str(type)+"','";
	sql+=record.DeviceID+"','";
	sql+=record.channelID+"','";
	sql+=record.taskid+"','";
	sql+=record.filename+"','";
	sql+=record.starttime+"','";
	sql+=record.endtime+"','";
	sql+=strexpiredays+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行添加录像信息xml
		//
		//if(BandDBOK)
		//{
		//	try
		//	{
		//		BandDBCon->Execute(sql.c_str());
		//	}
		//	catch (...)
		//	{
		//		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加录像信息出错",LOG_OUTPUT_FILE);
		//	}
		//}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddRecord( sRecordInfo record )
{
	bool ret=false;
	//***
	string strexpiredays = record.expiredays;
	if(record.expiredays.size() < 9)
	{
		long expiresecond = StrUtil::Str2Int(record.expiredays)*24*3600;
		strexpiredays = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(record.starttime)+expiresecond);
	}

	//***
	int type=record.dvbtype;
	string sql="insert into record(dvbtype,channelid,taskid,filename,starttime,endtime,expiredays) values('";
	sql+=StrUtil::Int2Str(type)+"','";
	sql+=record.channelID+"','";
	sql+=record.taskid+"','";
	sql+=record.filename+"','";
	sql+=record.starttime+"','";
	sql+=record.endtime+"','";
	sql+=strexpiredays+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行添加录像信息xml
		//
		//if(BandDBOK)
		//{
		//	try
		//	{
		//		BandDBCon->Execute(sql.c_str());
		//	}
		//	catch (...)
		//	{
		//		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加录像信息出错",LOG_OUTPUT_FILE);
		//	}
		//}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordByTaskid( eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,string StartTime,string EndTime)
{
	int id=type;
	bool ret=false;
	string sql=string("select * from record where dvbtype='") + StrUtil::Int2Str(id) + string("' and taskid='") + taskId + string("'");

	if (StartTime != "" && EndTime != "")
	{
		sql += string(" and StartTime <='") + EndTime + string(" '");
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	else if (StartTime != "" && EndTime.length()<1)
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
	}
	else if (StartTime.length()<1 && EndTime != "")
	{
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	sql+=" order by starttime asc";

	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string strexpiredays;
				sRecordInfo record;
				recordset.GetFieldValue("filename",record.filename);//获得录像文件名
				recordset.GetFieldValue("starttime",record.starttime);//录像文件开始时间
				recordset.GetFieldValue("endtime",record.endtime);//录像文件结束时间
				recordset.GetFieldValue("ChannelID",record.channelID);//新安播要求获取channelId
				recordset.GetFieldValue("expiredays",strexpiredays);
				if(strexpiredays.size() > 8)
				{
					if(time(0)<TimeUtil::StrToDateTime(strexpiredays))
					{
						vecRecordInfo.push_back(record);
					}
				}
				else
				{
					vecRecordInfo.push_back(record);
				}
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
		
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordByChannelID( eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,string StartTime,string EndTime,std::string DeviceID)
{
	int id=type;
	bool ret=false;
	string sql=string("select * from record where dvbtype='") + StrUtil::Int2Str(id)+ string("' and channelid='") + channelID+"' and taskid='"+taskID+"'";

	if(DeviceID != "")
	{
		sql += string(" and DeviceID ='") + DeviceID + string("'");
	}
	if (StartTime != "" && EndTime != "")
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	else if (StartTime != "" && EndTime.length()<1)
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
	}
	else if (StartTime.length()<1 && EndTime != "")
	{
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	sql+=" order by starttime asc";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				sRecordInfo record;
				recordset.GetFieldValue("filename",record.filename);//获得录像文件名
				recordset.GetFieldValue("starttime",record.starttime);//录像文件开始时间
				recordset.GetFieldValue("endtime",record.endtime);//录像文件结束时间
				recordset.GetFieldValue("ChannelID",record.channelID);//新安播要求获取channelId
				vecRecordInfo.push_back(record);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
		
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::QueryLostRecordByChannelID( eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,string StartTime,string EndTime,std::string DeviceID)
{
	int id=type;
	bool ret=false;
	string sql=string("select * from record where dvbtype='") + StrUtil::Int2Str(id)+ string("' and channelid='") + channelID+"'";
	if(DeviceID != "")
	{
		sql += string(" and DeviceID ='") + DeviceID + string("'");
	}
	if (StartTime != "" && EndTime != "")
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	else if (StartTime != "" && EndTime.length()<1)
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
	}
	else if (StartTime.length()<1 && EndTime != "")
	{
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	sql+=" order by starttime asc";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			time_t sttime = 0;
			time_t entime = 0;
			if(StartTime.size() > 0)
			{
				sttime = TimeUtil::StrToDateTime(StartTime);
			}
			if(EndTime.size() > 0)
			{
				entime = TimeUtil::StrToDateTime(EndTime);
			}
			time_t lasttime = sttime;
			bool bfinish = false;
			while (!recordset.IsEof())
			{
				sRecordInfo record;
				string strsttime;
				string strentime;
				recordset.GetFieldValue("starttime",strsttime);//录像文件开始时间
				recordset.GetFieldValue("endtime",strentime);//录像文件结束时间
				time_t st = TimeUtil::StrToDateTime(strsttime);
				if(st > entime)
				{
					bfinish = true;
					st = entime;
				}
				if(st-lasttime > 4)
				{
					record.starttime = TimeUtil::DateTimeToStr(lasttime);
					if(bfinish)
					{
						record.endtime = TimeUtil::DateTimeToStr(st);
					}
					else
					{
						record.endtime = strsttime;
					}
					recordset.GetFieldValue("ChannelID",record.channelID);//新安播要求获取channelId
					vecRecordInfo.push_back(record);
					lasttime = TimeUtil::StrToDateTime(strentime);
				}
				if((TimeUtil::StrToDateTime(strentime) >= entime)||(bfinish))
				{
					bfinish = true;
					break;
				}
				lasttime = TimeUtil::StrToDateTime(strentime);
				recordset.MoveNext();//移动到下一条记录
			}
			if(!bfinish)
			{
				if((StartTime.size() > 0)&&(EndTime.size() > 0))
				{
					sRecordInfo record;
					record.starttime = TimeUtil::DateTimeToStr(lasttime);
					record.endtime = EndTime;
					vecRecordInfo.push_back(record);
				}
			}
			ret=true;
			
		}
		
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string& EarliestStartTime)
{
	int id=type;
	bool ret=false;
	string sql=string("select top 1 * from record where dvbtype='") + StrUtil::Int2Str(id)+ string("' and channelid='") + channelID+"'";
	sql += string(" and DeviceID ='") + DeviceID + string("'");
	sql+=" order by starttime asc";
	try  
	{  
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{	
			recordset.GetFieldValue("starttime",EarliestStartTime);//获得对应deviceid和channelid的最早时间
			if(EarliestStartTime=="")
			{
				ret = false;
			}
			else
				ret = true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::QueryRecordByDeviceID(eDVBType type,std::string taskID,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime,std::string EndTime)
{
	int id=type;      
	bool ret=false;  
	string sql=string("select * from record where dvbtype='") + StrUtil::Int2Str(id)+ string("'")/* + channelID+"'"*/;
	//sql += string(string("' and taskid='")+taskID);
	
	//if(StrUtil::Str2Int(DeviceID) <= 0) return false; modify by hjw 2017-10-30 for V3.0 can use 0 dev
	
	//sql += string(string("' and filename like '%/")+ DeviceID + "[_]%'");
	if(DeviceID != "")
	{
		sql += string(" and DeviceID ='") + DeviceID + string("'");
	}
	if (StartTime != "" && EndTime != "")
	{
		sql += string(" and StartTime <='") +  EndTime + string("'");
		sql += string(" and EndTime >='") + StartTime + string("'");

	}
	else if (StartTime != "" && EndTime.length()<1)
	{
		sql += string(" and StartTime <='") + EndTime + string("'");
	}
	else if (StartTime.length()<1 && EndTime != "")
	{
		sql += string(" and EndTime >='") + StartTime + string("'");
	}
	sql+=" order by starttime asc";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while(!recordset.IsEof())
			{
				sRecordInfo record;
				recordset.GetFieldValue("filename",record.filename);//获得录像文件名
				recordset.GetFieldValue("starttime",record.starttime);//录像文件开始时间
				recordset.GetFieldValue("endtime",record.endtime);//录像文件结束时间
				recordset.GetFieldValue("ChannelID",record.channelID);//新安播要求获取channelId
				vecRecordInfo.push_back(record);
				recordset.MoveNext();//移动到下一条记录
			}
			ret = true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteRecord( std::string filename )
{
	bool ret=false;
	string sql="delete from record where filename='"+filename+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行删除录像sql
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中删除录像信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"删除录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordByDays( unsigned int days,std::vector<std::string>& vecFilename )
{
	return true;		//该函数有问题，暂时忽略//gxd 2011-03-31
	/************************************************************************/

	bool ret=false;
	string sql="select filename from record where expiredays='"+StrUtil::Int2Str(days)+"'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("filename",file);//获得录像文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
		
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename)
{
	bool ret = false;

	string sql = "select top "+ StrUtil::Int2Str(count) +" * from record where filename like \'%"+recPath
		+"%\' order by expiredays,starttime";

	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("filename",file);//获得录像文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordTop(unsigned int count, std::vector<std::string>& vecFilename )
{
	bool ret=false;
	string sql = "select top "+ StrUtil::Int2Str(count) +" * from record order by expiredays,starttime";
	cout << "111"<<endl;
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("filename",file);//获得录像文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecordExpire( std::vector<std::string>& vecFilename )
{
	bool ret=false;
	string sql = "select  * from record where expiredays <='";
	sql = sql + TimeUtil::GetCurDateTime() + "'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("filename",file);//获得录像文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::IsConnected( void )
{
	return DBCon->IsOpen();
}

bool SqlServerDBConnection::QueryChannelID( eDVBType type,std::vector<std::string>&vecChannelID )
{
	bool ret=false;
	int id=type;
	std::string sql="select channelid from channel where dvbtype='"+StrUtil::Int2Str(id)+"'";
	try
	{

		CADORecordset record;
		if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行查询sql
		{
			while(!record.IsEof())
			{
				string channelid;
				record.GetFieldValue("channelid",channelid);//获得频道id
				vecChannelID.push_back(channelid);
				record.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询任务出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateAlarmParam(sAlarmParam& alarmparam)
{
	bool ret = false;

	string insertsql = "insert into AlarmParam(DVBType,DeviceID,AlarmType,TypeID,TypeDesc,STD,Freq,SymbolRate,";

	insertsql += "ChannelID,Duration,Num,TimeInterval,DownThreshold,UpThreshold,Switch) values('";
	insertsql += StrUtil::Int2Str(alarmparam.DVBType)+string("','")+alarmparam.DeviceID+string("','")+StrUtil::Int2Str(alarmparam.AlarmType)+string("','");
	insertsql += alarmparam.TypeID+string("','")+alarmparam.TypeDesc+string("','")+alarmparam.STD+string("','")+alarmparam.Freq+string("','");
	insertsql += alarmparam.SymbolRate+string("','")+alarmparam.ChannelID+string("','")+alarmparam.Duration+string("','");
	insertsql += alarmparam.Num+string("','")+alarmparam.TimeInterval+string("','")+alarmparam.DownThreshold+string("','");
	insertsql += alarmparam.UpThreshold+string("','")+alarmparam.Switch+string("')");

	string selsql = string("select * from AlarmParam  ") ;
	string wheresql=string("where DVBType = '") +  StrUtil::Int2Str(alarmparam.DVBType)+  string("' and DeviceID = '") + alarmparam.DeviceID+
		string("' and AlarmType = '") + StrUtil::Int2Str(alarmparam.AlarmType) + string("' and TypeID = '") + alarmparam.TypeID+"'";

	if (alarmparam.AlarmType == ALARM_PROGRAM)
	{
		wheresql += string((" and Freq = '")+alarmparam.Freq + string("' and SymbolRate = '") + alarmparam.SymbolRate+string("'  and ChannelID = '")+alarmparam.ChannelID)+"'";
	}
	else if(alarmparam.AlarmType == ALARM_FREQ || alarmparam.AlarmType == ALARM_TR101_290)
	{
		wheresql += string((" and Freq = '")+alarmparam.Freq + string("' and SymbolRate = '") + alarmparam.SymbolRate)+"'";
	}
	else if (alarmparam.AlarmType == ALARM_ENVIRONMENT)
	{
		;
	}
	else
	{
		return false;
	}
	selsql+=wheresql;
	string updatesql="";
	if(alarmparam.Switch!="")
	{
		updatesql="update AlarmParam set Switch = '"+alarmparam.Switch+"' ";
	}
	if(alarmparam.Duration!=""||alarmparam.TimeInterval!=""||alarmparam.Num!=""||alarmparam.DownThreshold!=""||alarmparam.UpThreshold!="")
	{
		updatesql="update AlarmParam set Duration = '"+alarmparam.Duration+"' ,Num = '"+alarmparam.Num+"' ,TimeInterval = '"+alarmparam.TimeInterval+"' ,DownThreshold = '"+alarmparam.DownThreshold+"' ,UpThreshold = '"+alarmparam.UpThreshold+"' " ;
		if(alarmparam.Switch!="")
		{
			updatesql+=",Switch = '"+alarmparam.Switch+"' ";
		}
	}
	updatesql+=wheresql;
	try
	{
		CADORecordset record;
		int rowcount=0;
		if (record.Open(DBCon->GetActiveConnection(),selsql.c_str()))//执行查询任务sql
		{
			rowcount=record.GetRecordCount();
		}
		if(rowcount==0)
		{
			ret=DBCon->Execute(insertsql.c_str());//添加报警参数信息
			//
			if(BandDBOK)
			{
				try
				{
					BandDBCon->Execute(insertsql.c_str());
				}
				catch (...)
				{
					APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库更新报警参数出错",LOG_OUTPUT_FILE);
				}
			}
			//
		}
		else
		{
			ret=DBCon->Execute(updatesql.c_str());
			//
			if(BandDBOK)
			{
				try
				{
					BandDBCon->Execute(updatesql.c_str());
				}
				catch (...)
				{
					APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新报警参数出错",LOG_OUTPUT_FILE);
				}
			}
			//
		}

	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新报警参数出错！",LOG_OUTPUT_FILE);
		return false;

	}
	return ret;
}
bool SqlServerDBConnection::QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec)
{
	bool ret=false;
	std::string sql="select * from AlarmParam where DVBType = ";
	sql += StrUtil::Int2Str(type);
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行报警参数查询sql
		{
			while (!recordset.IsEof())
			{
				sAlarmParam param;
				param.DVBType=type;
				string alarmtype;
				//获得报警参数信息
				recordset.GetFieldValue("AlarmType",alarmtype);
				switch(StrUtil::Str2Int(alarmtype))
				{
				case 0:
					param.AlarmType=ALARM_NONE;
					break;
				case 1:
					param.AlarmType=ALARM_ENVIRONMENT;
					break;
				case 2:
					param.AlarmType=ALARM_FREQ;
					break;
				case 3:
					param.AlarmType=ALARM_PROGRAM;
					break;
				case 4:
					param.AlarmType=ALARM_TR101_290;
					break;
				default:
					param.AlarmType=ALARM_NONE;
					break;
				}
				recordset.GetFieldValue("TypeID",param.TypeID);
				recordset.GetFieldValue("TypeDesc",param.TypeDesc);
				recordset.GetFieldValue("STD",param.STD);
				recordset.GetFieldValue("Freq",param.Freq);
				recordset.GetFieldValue("SymbolRate",param.SymbolRate);
				recordset.GetFieldValue("DeviceID",param.DeviceID);
				recordset.GetFieldValue("ChannelID",param.ChannelID);
				recordset.GetFieldValue("Duration",param.Duration);
				recordset.GetFieldValue("Num",param.Num);
				recordset.GetFieldValue("TimeInterval",param.TimeInterval);
				recordset.GetFieldValue("DownThreshold",param.DownThreshold);
				recordset.GetFieldValue("UpThreshold",param.UpThreshold);
				recordset.GetFieldValue("Switch",param.Switch);

				AlarmParamVec.push_back(param);
				recordset.MoveNext();
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警参数出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec)
{
	bool ret=false;
	std::string sql="select * from AlarmParam";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行报警参数查询sql
		{
			while (!recordset.IsEof())
			{
				sAlarmParam param;
				string dvbtype;
				recordset.GetFieldValue("dvbtype",dvbtype);
				switch(StrUtil::Str2Int(dvbtype))
				{
				case 1:
					param.DVBType=CTTB;
					break;
				case 2:
					param.DVBType=CMMB;
					break;
				case 3:
					param.DVBType=DVBC;
					break;
				case 4:
					param.DVBType=RADIO;
					break;
				case 5:
					param.DVBType=ATV;
					break;
				case 6:
					param.DVBType=AM;
					break;
				case 7:
					param.DVBType=DVBS;
					break;
				case 8:
					param.DVBType=CTV;
					break;
				default:
					param.DVBType=UNKNOWN;
					break;
				}
				string alarmtype;
				//获得报警参数信息
				recordset.GetFieldValue("AlarmType",alarmtype);
				switch(StrUtil::Str2Int(alarmtype))
				{
				case 0:
					param.AlarmType=ALARM_NONE;
					break;
				case 1:
					param.AlarmType=ALARM_ENVIRONMENT;
					break;
				case 2:
					param.AlarmType=ALARM_FREQ;
					break;
				case 3:
					param.AlarmType=ALARM_PROGRAM;
					break;
				case 4:
					param.AlarmType=ALARM_TR101_290;
					break;
				default:
					param.AlarmType=ALARM_NONE;
					break;
				}
				recordset.GetFieldValue("TypeID",param.TypeID);
				recordset.GetFieldValue("TypeDesc",param.TypeDesc);
				recordset.GetFieldValue("STD",param.STD);
				recordset.GetFieldValue("Freq",param.Freq);
				recordset.GetFieldValue("SymbolRate",param.SymbolRate);
				recordset.GetFieldValue("DeviceID",param.DeviceID);
				recordset.GetFieldValue("ChannelID",param.ChannelID);
				recordset.GetFieldValue("Duration",param.Duration);
				recordset.GetFieldValue("Num",param.Num);
				recordset.GetFieldValue("TimeInterval",param.TimeInterval);
				recordset.GetFieldValue("DownThreshold",param.DownThreshold);
				recordset.GetFieldValue("UpThreshold",param.UpThreshold);
				recordset.GetFieldValue("Switch",param.Switch);

				AlarmParamVec.push_back(param);
				recordset.MoveNext();
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警参数出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddRunPlanParam( sRunPlanParam runPlanParam )
{
	bool ret=false;
	std::string sql="insert into runplanparam(dvbtype,channelid,type,dayofweek,mmonth,dayofmonth,starttime,endtime,validstartdatetime,validenddatetime,startdatetime,enddatetime) values('"+StrUtil::Int2Str(runPlanParam.dvbtype)+"','";
	sql+=runPlanParam.ChannelID+"','";
	sql+=StrUtil::Int2Str(runPlanParam.Type)+"','";
	sql+=runPlanParam.DayOfWeek+"','";
	sql+=runPlanParam.Month+"','";
	sql+=runPlanParam.DayOfMonth+"','";
	sql+=runPlanParam.StartTime+"','";
	sql+=runPlanParam.EndTime+"','";
	sql+=runPlanParam.ValidStartDateTime+"','";
	sql+=runPlanParam.ValidEndDateTime+"','";
	sql+=runPlanParam.StartDateTime+"','";
	sql+=runPlanParam.EndDateTime+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//添加运行图参数
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加运行图参数出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加运行图参数出错！",LOG_OUTPUT_FILE);
		return false;

	}
	return ret;
}

bool SqlServerDBConnection::QueryRunPlanParam( eDVBType type,std::vector<sRunPlanParam>& runPlanParam )
{
	bool ret=false;
	std::string sql="select * from runplanparam where dvbtype='"+StrUtil::Int2Str(type)+"' order by dvbtype,channelid";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询运行图参数信息
		{
			while (!recordset.IsEof())
			{
				//生成运行图参数
				sRunPlanParam param;
				param.dvbtype=type;
				recordset.GetFieldValue("channelid",param.ChannelID);
				recordset.GetFieldValue("type",param.Type);
				recordset.GetFieldValue("dayofweek",param.DayOfWeek);
				recordset.GetFieldValue("mmonth",param.Month);
				recordset.GetFieldValue("dayofmonth",param.DayOfMonth);
				recordset.GetFieldValue("starttime",param.StartTime);
				recordset.GetFieldValue("endtime",param.EndTime);
				recordset.GetFieldValue("validstartdatetime",param.ValidStartDateTime);
				recordset.GetFieldValue("validenddatetime",param.ValidEndDateTime);
				recordset.GetFieldValue("startdatetime",param.StartDateTime);
				recordset.GetFieldValue("enddatetime",param.EndDateTime);
				runPlanParam.push_back(param);
				recordset.MoveNext();
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询运行图参数出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return true;
}

bool SqlServerDBConnection::QueryAllRunPlanParam( std::vector<sRunPlanParam>& runPlanParam )
{
	bool ret=false;
	std::string sql="select * from runplanparam order by dvbtype,channelid";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询运行图参数
		{
			while (!recordset.IsEof())
			{
				sRunPlanParam param;
				string dvbtype;
				recordset.GetFieldValue("dvbtype",dvbtype);
				switch(StrUtil::Str2Int(dvbtype))
				{
				case 1:
					param.dvbtype=CTTB;
					break;
				case 2:
					param.dvbtype=CMMB;
					break;
				case 3:
					param.dvbtype=DVBC;
					break;
				case 4:
					param.dvbtype=RADIO;
					break;
				case 5:
					param.dvbtype=ATV;
					break;
				case 6:
					param.dvbtype=AM;
					break;
				case 7:
					param.dvbtype=DVBS;
					break;
				case 8:
					param.dvbtype=CTV;
					break;
				default:
					param.dvbtype=UNKNOWN;
					break;
				}
				//获得运行图参数信息
				recordset.GetFieldValue("channelid",param.ChannelID);
				recordset.GetFieldValue("type",param.Type);
				recordset.GetFieldValue("dayofweek",param.DayOfWeek);
				recordset.GetFieldValue("mmonth",param.Month);
				recordset.GetFieldValue("dayofmonth",param.DayOfMonth);
				recordset.GetFieldValue("starttime",param.StartTime);
				recordset.GetFieldValue("endtime",param.EndTime);
				recordset.GetFieldValue("validstartdatetime",param.ValidStartDateTime);
				recordset.GetFieldValue("validenddatetime",param.ValidEndDateTime);
				recordset.GetFieldValue("startdatetime",param.StartDateTime);
				recordset.GetFieldValue("enddatetime",param.EndDateTime);
				runPlanParam.push_back(param);
				recordset.MoveNext();
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询运行图参数出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return true;
}

/**	channelid不会用到了
*/
bool SqlServerDBConnection::DeleteRunPlanParam(eDVBType type, std::string channelid )
{
	bool ret=false;
	std::string sql="delete from runplanparam where dvbtype='"+StrUtil::Int2Str(type)+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());//删除运行图参数
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中删除运行图参数出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"删除运行图参数出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam )
{
	if (runPlanParam.size()>0)
	     DeleteRunPlanParam(dvbtype,channelid);//删除运行图参数

	for (size_t i=0;i!=runPlanParam.size();++i)
	{
		AddRunPlanParam(runPlanParam[i]);//添加运行图参数
	}
	return true;
}

bool SqlServerDBConnection::AddQualityInfo( eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality)
{
	bool ret=false;
	string sql="insert into quality(dvbtype,freq,taskid,type,des,valu,checktime) values('"+StrUtil::Int2Str(dvbtype)+"','";
	sql+=freq+"','";
	sql+=taskid+"','";
	sql+=quality.type+"','";
	sql+=quality.desc+"','";
	sql+=quality.valu+"','";
	sql+=quality.checktime+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//添加指标信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加指标信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加指标信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddQualityInfo(const std::string &strInsertSql)
{
	bool ret=false;
	
	try
	{
		ret=DBCon->Execute( strInsertSql.c_str() );//添加指标信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strInsertSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加指标信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加指标信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryQualityInfo( eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime )
{
	bool ret=false;
	string sql;
	if(freq!="")
	{
		sql="select * from quality where dvbtype='"+StrUtil::Int2Str(dvbtype)+"' and freq='"+freq;
	}
	else
	{
		sql="select * from quality where dvbtype='"+StrUtil::Int2Str(dvbtype);
	}
	sql+="' and taskid='";
	sql+=taskid+"' and checktime>='";
	sql+=startdatetime+"' and checktime<='";
	sql+=enddatetime+"'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					//获得指标信息
					eQualityInfo qulity;
					recordset.GetFieldValue("type",qulity.type);
					recordset.GetFieldValue("des",qulity.desc);
					recordset.GetFieldValue("valu",qulity.valu);
					recordset.GetFieldValue("checktime",qulity.checktime);
					recordset.GetFieldValue("freq",qulity.freq);
					recordset.MoveNext();
					vecQuality.push_back(qulity);
				}
				ret=true;
			}

		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询指标出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DelQualityInfo(std::string strCheckTime)
{
	bool ret=false;
	std::string strDelSql = "select * into quality_tmp from quality where checktime>='";
	strDelSql += strCheckTime;
	strDelSql += "' truncate table quality drop table quality exec sp_rename 'quality_tmp','quality'";
	try
	{
		ret=DBCon->Execute( strDelSql.c_str() );//添加指标信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strDelSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中删除7天前指标数据出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,"删除7天前指标数据出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::GetMsgID( std::string &MsgID )
{
	bool ret=false;
	string sql="select msgid from MsgID ";
	
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("msgid",MsgID);//获得msgid
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询MsgID出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateMsgID( std::string MsgID )
{
	bool ret=false;
	string sql="update Msgid set msgid='"+MsgID+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());//更新MsgID信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新MsgID信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新MsgID信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryCompensationValu( std::vector<sQualityCompensation>& qualityCmpVec)
{
	bool ret=false;
	string sql="select * from qualitycompensation";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					sQualityCompensation quality;
					string dvbtype;
					recordset.GetFieldValue("dvbtype",dvbtype);
					switch(StrUtil::Str2Int(dvbtype))
					{
					case 1:
						quality.dvbtype=CTTB;
						break;
					case 2:
						quality.dvbtype=CMMB;
						break;
					case 3:
						quality.dvbtype=DVBC;
						break;
					case 4:
						quality.dvbtype=RADIO;
						break;
					case 5:
						quality.dvbtype=ATV;
						break;
					case 6:
						quality.dvbtype=AM;
						break;
					case 7:
						quality.dvbtype=DVBS;
						break;
					case 8:
						quality.dvbtype=CTV;
						break;
					default:
						quality.dvbtype=UNKNOWN;
						break;
					}
					recordset.GetFieldValue("deviceid",quality.deviceid);
					recordset.GetFieldValue("type",quality.type);
					recordset.GetFieldValue("valu",quality.valu);
					recordset.GetFieldValue("Freq",quality.freq);
					recordset.GetFieldValue("Lev",quality.level);
					qualityCmpVec.push_back(quality);
					recordset.MoveNext();
				}
				ret=true;
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询指标补偿值出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateCompensationValu(sQualityCompensation& qualityCmp)
{
	return DelCompensationValu(qualityCmp)&&AddCompensationValu(qualityCmp);
}

bool SqlServerDBConnection::AddCompensationValu(sQualityCompensation& qualityCmp)
{
    bool ret=false;
	string sql="insert into qualitycompensation(dvbtype,deviceid,type,valu,freq,lev) values('";
	sql+=StrUtil::Int2Str(qualityCmp.dvbtype)+"','";
	sql+=qualityCmp.deviceid+"','";
	sql+=qualityCmp.type+"','";
	sql+=qualityCmp.valu+"','";
	sql+=qualityCmp.freq+"','";
	sql+=qualityCmp.level+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加指标补偿信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加指标补偿信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DelCompensationValu(sQualityCompensation& qualityCmp)
{
	bool ret=false;
	string sql="delete from qualitycompensation where dvbtype='";
	sql+=string(StrUtil::Int2Str(qualityCmp.dvbtype)+"' and deviceid='");
	sql+=string(qualityCmp.deviceid+"' and type='");
	sql+=string(qualityCmp.type+"' and freq='");
	sql+=string(qualityCmp.freq+"' and lev='");
	sql+=string(qualityCmp.level+"'");
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中删除指标补偿信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{ 
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"删除指标补偿信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddAlarmInfo( sCheckParam& alarminfo,string mode )
{
	bool ret=false;
	string sql="insert into alarminfo(dvbtype,alarmtype,std,freq,symbolrate,deviceid,channelid,typeid,typedesc,alarmvalue,starttime,alarmid,mode) values('";
	sql+=string(StrUtil::Int2Str(alarminfo.DVBType)+"','");
	sql+=StrUtil::Int2Str(alarminfo.AlarmType)+"','";
	sql+=string(alarminfo.STD+"','");
	sql+=string(alarminfo.Freq+"','");
	sql+=string(alarminfo.SymbolRate+"','");
	sql+=string(alarminfo.DeviceID+"','");
	sql+=string(alarminfo.ChannelID+"','");
	sql+=string(alarminfo.TypeID+"','");
	sql+=string(alarminfo.TypeDesc+"','");
	sql+=string(alarminfo.TypedValue+"','");
	sql+=TimeUtil::DateTimeToStr(alarminfo.CheckTime)+"','";
	sql+=string(alarminfo.AlarmID+"','");
	sql+=string(mode+"')");
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		//if(BandDBOK)
		//{
		//	try
		//	{
		//		BandDBCon->Execute(sql.c_str());
		//	}
		//	catch (...)
		//	{
		//		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加报警结果出错",LOG_OUTPUT_FILE);
		//	}
		//}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加报警结果出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo)
{
	bool ret=false;
	string sql="select * from alarminfo where AlarmType='3' and deviceid='";
	sql+=deviceid;
	sql+="' and channelid='";
	sql+=channelid+"' and StartTime>='";
	sql+=sttime+/*"' and EndTime<='";//会导致解除报警没有详细信息
	sql+=entime+*/"'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					sCheckParam alarm;
					recordset.GetFieldValue("typeid",alarm.TypeID);
					recordset.GetFieldValue("typedesc",alarm.TypeDesc);
					recordset.GetFieldValue("StartTime",alarm.StartTime);
					recordset.GetFieldValue("EndTime",alarm.EndTime);
					if((alarm.TypeID == "31")||(alarm.TypeID == "32")||(alarm.TypeID == "33"))
					{
						vecAlarmInfo.push_back(alarm);
					}
					recordset.MoveNext();
				}
				ret=true;
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警结果信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryAlarmInfo( string mode,std::vector<sCheckParam>& vecAlarmInfo )
{
	bool ret=false;
	string sql="select * from alarminfo where mode='";
	sql+=mode;
	sql+="'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					sCheckParam alarm;
					string dvbtype;
					recordset.GetFieldValue("dvbtype",dvbtype);
					alarm.DVBType=(enumDVBType)StrUtil::Str2Int(dvbtype);
					recordset.GetFieldValue("std",alarm.STD);
					recordset.GetFieldValue("freq",alarm.Freq);
					recordset.GetFieldValue("symbolrate",alarm.SymbolRate);
					recordset.GetFieldValue("channelid",alarm.ChannelID);
					recordset.GetFieldValue("typeid",alarm.TypeID);
					recordset.GetFieldValue("typedesc",alarm.TypeDesc);
					recordset.GetFieldValue("alarmvalue",alarm.TypedValue);
					//string checktime;
					//recordset.GetFieldValue("starttime",checktime);
					//alarm.CheckTime=TimeUtil::StrToDateTime(checktime);
					alarm.CheckTime = time(0);
					recordset.GetFieldValue("alarmid",alarm.AlarmID);
					recordset.GetFieldValue("deviceid",alarm.DeviceID);
					string alarmtype;
					//获得报警参数信息
					recordset.GetFieldValue("AlarmType",alarmtype);
					switch(StrUtil::Str2Int(alarmtype))
					{
					case 0:
						alarm.AlarmType=ALARM_NONE;
						break;
					case 1:
						alarm.AlarmType=ALARM_ENVIRONMENT;
						break;
					case 2:
						alarm.AlarmType=ALARM_FREQ;
						break;
					case 3:
						alarm.AlarmType=ALARM_PROGRAM;
						break;
					case 4:
						alarm.AlarmType=ALARM_TR101_290;
						break;
					case 5:
						alarm.AlarmType=ALARM_EQUIPMENT;
						break;
					default:
						alarm.AlarmType=ALARM_NONE;
						break;
					}
					vecAlarmInfo.push_back(alarm);
					recordset.MoveNext();
				}
				ret=true;
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警结果信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateAlarmInfo( eDVBType dvbtype,std::string Alarmid,string mode )
{
	bool ret=false;

	string sql="update alarminfo set endtime='" + TimeUtil::DateTimeToStr(time(0)-1)+ "' , mode='" + mode; //V2.6
	sql+="' where dvbtype='"+StrUtil::Int2Str(dvbtype);
	sql+="' and alarmid='"+Alarmid;
	sql+="'";
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新报警出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;

}
bool SqlServerDBConnection::UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode)
{
	bool ret=false;
	string sql="update alarminfo set endtime='" + Alarmtime + "' , mode='" + mode;
	sql+="' where dvbtype='"+StrUtil::Int2Str(type);
	sql+="' and alarmid='"+Alarmid;
	sql+="'";
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新报警出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;

}
bool SqlServerDBConnection::QueryFreqAlarmID( eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid )
{
	bool ret=false;
	string sql="select alarmid from alarminfo where dvbtype='";
	sql+=StrUtil::Int2Str(dvbtype)+"' and deviceid='";
	sql+=deviceid+"' and freq='";
	sql+=freq+"' and typeid='";
	sql+=typeID+"' and mode='";
	sql+="0'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("alarmid",alarmid);
					recordset.MoveNext();
				}
				ret=true;
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警结果信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryProgramAlarmID( eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid )
{
	bool ret=false;
	string sql="select alarmid from alarminfo where dvbtype='";
	sql+=StrUtil::Int2Str(dvbtype)+"' and deviceid='";
	sql+=deviceid+"' and channelid='";
	sql+=channelid+"' and typeid='";
	sql+=typeID+"'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("alarmid",alarmid);
					recordset.MoveNext();
				}
				ret=true;
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警结果信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::UpdateFileEndTime( std::string filename,std::string endtime )
{
	bool ret=false;
	string sql="update record set endtime='"+endtime;
	sql+="' where filename='";
	sql+=filename;
	sql+="'";
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新文件结束时间出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::AddSpectrumInfo( eDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec ,std::string checktime)
{
	bool ret=false;
	string sql="insert into spec(dvbtype,freq,taskid,type,des,valu,status,checktime) values('"+StrUtil::Int2Str(dvbtype)+"','";
	sql+=freq+"','";
	sql+=taskid+"','";
	sql+=spec.type+"','";
	sql+=spec.desc+"','";
	sql+=spec.valu+"','";
	sql+=spec.status+"','";
	sql+=checktime+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//添加指标信息
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加频谱信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加频谱信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QuerySpecInfo( eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecSpec,std::string startdatetime,std::string enddatetime )
{
	bool ret=false;
	string sql="select * from spec where dvbtype='"+StrUtil::Int2Str(dvbtype);
	sql+="' and taskid='";
	sql+=taskid+"' and checktime>='";
	sql+=startdatetime+"' and checktime<='";
	sql+=enddatetime+"'";
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					//获得指标信息
					eSpecInfo spec;
					recordset.GetFieldValue("type",spec.type);
					recordset.GetFieldValue("des",spec.desc);
					recordset.GetFieldValue("valu",spec.valu);
					recordset.GetFieldValue("checktime",spec.checktime);
					recordset.GetFieldValue("freq",spec.freq);
					recordset.GetFieldValue("status",spec.status);
					recordset.MoveNext();
					vecSpec.push_back(spec);
				}
				ret=true;
			}

		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询频谱出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

//wz_101229
bool SqlServerDBConnection::AddTempFile(string filename, string createtime, string expiredays)
{

	bool ret=false;
	string sql="insert into TempFile(FileName,CreateTime,ExpireDays) values('";
	sql += string(filename + "','");
	sql += string(createtime + "','");
	sql += string(expiredays + "')");
	//cout<<sql<<endl;
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		//if(BandDBOK)
		//{
		//	try
		//	{
		//		BandDBCon->Execute(sql.c_str());
		//	}
		//	catch (...)
		//	{
		//		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中添加临时文件信息出错",LOG_OUTPUT_FILE);
		//	}
		//}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加临时文件信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

//wz_101229
bool SqlServerDBConnection::QueryTempFilesExpire( std::vector<std::string>& vecFilename )
{
	bool ret=false;
	string sql = "select  * from TempFile where dateadd(day,ExpireDays,CreateTime)<='";
	sql = sql + TimeUtil::GetCurDateTime() + "'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行录像文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("FileName",file);//获得临时文件的文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询临时文件信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

//wz_101229
bool SqlServerDBConnection::DeleteTempFile( std::string filename )
{
	bool ret=false;
	string sql="delete from TempFile where FileName='"+filename+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行删除临时文件sql
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中删除临时文件信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"删除临时文件信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

//wz_101229
bool SqlServerDBConnection::QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename)
{
	bool ret=false;
	string sql="select filename from TempFile where ExpireDays='"+StrUtil::Int2Str(days)+"'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行临时文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("FileName",file);//获得临时文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}

	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询临时文件信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;

}

//wz_101229
bool SqlServerDBConnection::QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename)
{
	bool ret=false;
	string sql = "select top "+StrUtil::Int2Str(count)+" * from TempFile order by CreateTime";

	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//执行临时文件查询sql
		{
			while (!recordset.IsEof())
			{
				string file;
				recordset.GetFieldValue("FileName",file);//获得临时文件名
				vecFilename.push_back(file);
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询临时信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

//报警查询 2011-03-19
bool SqlServerDBConnection::AlarmInfoQuery(vector<sCheckParam> &vecAlarm)
{
	vector<sCheckParam>::iterator itor=vecAlarm.begin();
	for(;itor!=vecAlarm.end();itor++)
	{
		bool ret=false;
		string sql = "select starttime,endtime,mode from alarminfo where DVBType='"+StrUtil::Int2Str(itor->DVBType)+"' and AlarmType='"+StrUtil::Int2Str(itor->AlarmType);
		sql+="' and Freq='"+itor->Freq+"' and TypeID='"+itor->TypeID+"' and ChannelID='"+itor->ChannelID+"'";

		try
		{
			CADORecordset recordset;
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))
			{
				int rowcount=0;
				rowcount=recordset.GetRecordCount();
				if(rowcount==0)
				{
					itor->StrCheckTime=TimeUtil::DateTimeToStr(time(0));
					itor->mode="0";
				}
				while (!recordset.IsEof())
				{
					string mode,starttime,endtime;
					recordset.GetFieldValue("starttime",starttime);
					recordset.GetFieldValue("endtime",endtime);
					recordset.GetFieldValue("mode",mode);
					if(mode.substr(0,1)=="0")
					{
						itor->StrCheckTime=starttime;
						itor->mode="1";
					}
					else if(mode.substr(0,1)=="1")
					{
						itor->StrCheckTime=endtime;
						itor->mode="2";
					}
					recordset.MoveNext();
				}
			}
		}
		catch (...)
		{
			APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询临时信息出错！",LOG_OUTPUT_FILE);
			continue;
		}
	}
	return true;
}

bool SqlServerDBConnection::AlarmStateClear(string Freq)
{
	bool ret=false;
	string sql="";
	if(Freq=="ALL")
		sql="delete from alarminfo where Freq<>''";
	else
		sql="delete from alarminfo where Freq='"+Freq+"'";
	try
	{
		ret=DBCon->Execute(sql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(sql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中报警状态删除信息出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"报警状态删除信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return true;
}

bool SqlServerDBConnection::UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid)
{
	bool ret=false;
	string sql="select channelid,starttime,endtime from alarminfo where alarmid='"+Alarmid+"'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询sql
		{
			while (!recordset.IsEof())
			{
				string channelid,starttime,endtime;
				recordset.GetFieldValue("ChannelID",channelid);
				recordset.GetFieldValue("StartTime",starttime);
				recordset.GetFieldValue("EndTime",endtime);

				string days=PROPMANAGER::instance()->GetAlarmRecordExpire(dvbtype);
				long expiresecond = StrUtil::Str2Int(days)*24*3600;
				days = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(starttime)+expiresecond);
				string updatesql,selectForeSql,selectBackSql;

				//获取异态录像存储配置信息
				sAlarmRecordStorageCfg alarmRecCfgInfo;
				PROPMANAGER::instance()->GetAlarmRecStorageCfg(dvbtype, alarmRecCfgInfo);
				if (alarmRecCfgInfo._switch == "1")		//开关打开
				{
					string whereSql = "where dvbtype='" + StrUtil::Int2Str(dvbtype);
					whereSql += "' and channelid='" + channelid;
					whereSql += "' and starttime<='" + endtime;
					whereSql += "' and endtime>='" + starttime + "'";

					string totalSelectSql = "select * from record ";
					totalSelectSql += whereSql;
					try
					{
						CADORecordset tempRecSet;
						if (tempRecSet.Open(DBCon->GetActiveConnection(),totalSelectSql.c_str()))//查询sql
						{
							int count = tempRecSet.GetRecordCount();	//获取异态录像文件个数
							if (count > 2*StrUtil::Str2Int(alarmRecCfgInfo._filenum))	//如果异态录像文件个数 > 2*filenum
							{
								updatesql = "update record set expiredays='" + days + "' where filename in ";
								selectForeSql = "(select top " + alarmRecCfgInfo._filenum + " filename from record " + whereSql + " order by starttime)";
								selectBackSql = "(select top " + alarmRecCfgInfo._filenum + " filename from record " + whereSql + "order by starttime desc)";

								try
								{
									string execSql = updatesql + selectForeSql;
									ret=DBCon->Execute(execSql.c_str());
									//
									if(BandDBOK)
									{
										try
										{
											BandDBCon->Execute(execSql.c_str());
										}
										catch (...)
										{
											APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"execSql向备机数据库中更新异态录像过期时间出错",LOG_OUTPUT_FILE);
										}
									}
									//
									execSql = updatesql + selectBackSql;
									ret=DBCon->Execute(execSql.c_str());
									//
									if(BandDBOK)
									{
										try
										{
											BandDBCon->Execute(execSql.c_str());
										}
										catch (...)
										{
											APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中更新异态录像过期时间出错",LOG_OUTPUT_FILE);
										}
									}
									//
								}
								catch (...)
								{
									APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新异态录像过期时间出错！",LOG_OUTPUT_FILE);
								}
								recordset.MoveNext();//移动到下一条记录
								continue;
							}
						}
					}//try
					catch (...)
					{
						APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询异态录像出错！",LOG_OUTPUT_FILE);
					}	
				}

				updatesql="update record set expiredays='"+days+"' where dvbtype='"+StrUtil::Int2Str(dvbtype);
				updatesql+="' and channelid='"+channelid;
				updatesql+="' and starttime<='"+endtime;
				updatesql+="' and endtime>='"+starttime+"'";

				try
				{
					ret=DBCon->Execute(updatesql.c_str());
					//
					if(BandDBOK)
					{
						try
						{
							BandDBCon->Execute(updatesql.c_str());
						}
						catch (...)
						{
							APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"updatesql向备机数据库中更新异态录像过期时间出错",LOG_OUTPUT_FILE);
						}
					}
					//
				}
				catch (...)
				{
					APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"更新异态录像过期时间出错！",LOG_OUTPUT_FILE);
				}
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询异态报警信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;

}
bool SqlServerDBConnection::QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue)
{
	bool ret=false;
	string sql="";
	
		sql="select ConfigValue from Config where dvbtype='"+StrUtil::Int2Str(dvbtype);
		sql+="' and ConfigName='"+NodeName+"'";
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询sql
		{
			while (!recordset.IsEof())
			{
				recordset.GetFieldValue("ConfigValue",NodeValue);			
				recordset.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询配置信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return true;
}


bool SqlServerDBConnection::QuerySystemConfig(eDVBType type, sSysConfigParam &config)
{
	bool bFind = false;
	string sql = string("declare @sql varchar(8000)\
				set @sql = 'select DVBType '\
				select @sql = @sql + ', max(case ConfigName when ''' + ConfigName + ''' then ConfigValue else STR(NULL) end) [' + ConfigName + ']'\
				from (select distinct ConfigName from Config where DVBType='") 
				+ StrUtil::Int2Str(type)
				+ string("') as a\
				set @sql = @sql + ' from Config group by DVBType'\
				exec(@sql)");
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))
		{
			while(!recordset.IsEof())
			{
				string type_;
				recordset.GetFieldValue("DVBType",type_);
				if(StrUtil::Str2Int(type_) != type)
				{
					recordset.MoveNext();
					continue;
				}
				bFind = true;
				recordset.GetFieldValue("SystemIp",config.SystemIp);
				recordset.GetFieldValue("VideoHttpServerIp",config.VideoHttpServerIp);
				recordset.GetFieldValue("VideoHttpPort",config.VideoHttpPort);
				recordset.GetFieldValue("VideoHttpMaxnum",config.VideoHttpMaxnum);
				recordset.GetFieldValue("VideoRtspServerIp",config.VideoRtspServerIp);
				recordset.GetFieldValue("VideoRtspPort",config.VideoRtspPort);
				recordset.GetFieldValue("VideoRtspMaxnum",config.VideoRtspMaxnum);
				recordset.GetFieldValue("QualityServerIp",config.QualityServerIp);
				recordset.GetFieldValue("QualityPort",config.QualityPort);
				recordset.GetFieldValue("QualityMaxnum",config.QualityMaxnum);
				recordset.GetFieldValue("DeviceServerIp",config.DeviceServerIp);
				recordset.GetFieldValue("DevicePort",config.DevicePort);
				recordset.GetFieldValue("DeviceMaxnum",config.DeviceMaxnum);
				recordset.GetFieldValue("LogServerIp",config.LogServerIp);
				recordset.GetFieldValue("LogPort",config.LogPort);
				recordset.GetFieldValue("LogMaxnum",config.LogMaxnum);
				recordset.GetFieldValue("XmlServerIp",config.XmlServerIp);
				recordset.GetFieldValue("XmlPort",config.XmlPort);
				recordset.GetFieldValue("HttpServerIp",config.HttpServerIp);
				recordset.GetFieldValue("HttpPort",config.HttpPort);
				recordset.GetFieldValue("FtpServerIp",config.FtpServerIp);
				recordset.GetFieldValue("FtpPort",config.FtpPort);
				recordset.GetFieldValue("RecordQualityServer",config.RecordQualityIp);
				recordset.GetFieldValue("RecordQualityPort",config.RecordQualityPort);
				recordset.GetFieldValue("RecordQualityMaxnum",config.RecordQualityMaxnum);
				recordset.GetFieldValue("Record_FileLocation",config.Record_FileLocation);
				recordset.GetFieldValue("Record_FileSharePath",config.Record_FileSharePath);
				recordset.GetFieldValue("Record_DeviceId",config.Record_DeviceId);
				recordset.GetFieldValue("Record_Period",config.Record_Period);
				recordset.GetFieldValue("Record_MaxAvailableSize",config.Record_MaxAvailableSize);
				recordset.GetFieldValue("Record_DBCleanInterval",config.Record_DBCleanInterval);
				recordset.GetFieldValue("Record_SystemCleanTime",config.Record_SystemCleanTime);
				recordset.GetFieldValue("Record_StoreType",config.Record_StoreType);
				recordset.GetFieldValue("Log_Path",config.Log_Path);
			//	recordset.GetFieldValue("SI_LogPath",config.SI_LogPath);
				recordset.GetFieldValue("Log_Expire",config.Log_Expire);
				recordset.GetFieldValue("Log_Type",config.Log_Type);
				recordset.GetFieldValue("Log_outputToFile",config.Log_outputToFile);
				recordset.GetFieldValue("LogPathType_Analyser",config.LogPathType_Analyser);
				recordset.GetFieldValue("LogPathType_Record",config.LogPathType_Record);
				recordset.GetFieldValue("LogPathType_Video",config.LogPathType_Video);
				recordset.GetFieldValue("LogPathType_Device",config.LogPathType_Device);
				recordset.GetFieldValue("LogPathType_Other",config.LogPathType_Other);
				recordset.GetFieldValue("LogPathType_Default",config.LogPathType_Default);
				/*recordset.GetFieldValue("Center_SrcCode",config.Center_SrcCode);
				recordset.GetFieldValue("Center_Url",config.Center_Url);
				recordset.GetFieldValue("SubCenter_SrcCode",config.SubCenter_SrcCode);
				recordset.GetFieldValue("SubCenter_Url",config.SubCenter_Url);*/
				recordset.GetFieldValue("XmlSendTime",config.XmlSendTime);
				recordset.GetFieldValue("XmlOverTime",config.XmlOverTime);
				recordset.GetFieldValue("TempFileLocation",config.TempFileLocation);
				recordset.GetFieldValue("TempSharePath",config.TempSharePath);
				recordset.GetFieldValue("TempFileExpireTime",config.TempFileExpireTime);

				recordset.GetFieldValue("DeviceSchedulerTask_Type",config.DeviceSchedulerTask_Type);
				recordset.GetFieldValue("DeviceSchedulerTask_WeekDay",config.DeviceSchedulerTask_WeekDay);
				recordset.GetFieldValue("DeviceSchedulerTask_Date",config.DeviceSchedulerTask_Date);
				recordset.GetFieldValue("DeviceSchedulerTask_Time",config.DeviceSchedulerTask_Time);

				recordset.GetFieldValue("PsisiSchedulerTask_Type",config.PsisiSchedulerTask_Type);
				recordset.GetFieldValue("PsisiSchedulerTask_WeekDay",config.PsisiSchedulerTask_WeekDay);
				recordset.GetFieldValue("PsisiSchedulerTask_Date",config.PsisiSchedulerTask_Date);
				recordset.GetFieldValue("PsisiSchedulerTask_Time",config.PsisiSchedulerTask_Time);

				break;
			}
			if(!bFind)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"没有查到相关系统配置信息！",LOG_OUTPUT_FILE);
				return false;
			}
			return true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询系统配置出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return false;
}

bool SqlServerDBConnection::QueryDvbConfig( eDVBType type,vector<sDvbConfigParam> &VecConfig)
{
	bool bFind = false;
	string sql="select * from Config where dvbtype='"+StrUtil::Int2Str(type)+"'";
	sDvbConfigParam config;
	config.type = StrUtil::Int2Str(type);
	try
	{
		CADORecordset recordset;
		if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))
		{
			while(!recordset.IsBof())
			{
				string ConfigName,ConfigValue;
				if(!(recordset.GetFieldValue("DVBType",config.type)))
				{
					break;
				}
				if(StrUtil::Str2Int(config.type) !=type)//0 为系统配置，其他为监测类型相关配置
				{
					recordset.MoveNext();
					continue;
				}
				recordset.GetFieldValue("ConfigName",ConfigName);
				recordset.GetFieldValue("ConfigValue",ConfigValue);
				bFind = true;
				if(ConfigName == "CenterFreq")
				{
					config.CenterFreq = ConfigValue;
				}
				else if(ConfigName == "Symbolrate")
				{
					config.Symbolrate = ConfigValue;
				}
				else if(ConfigName == "Qam")
				{
					config.Qam = ConfigValue;
				}
				else if(ConfigName == "TablePath")
				{
					config.TablePath = ConfigValue;
				}
				else if(ConfigName == "SharePath")
				{
					config.SharePath = ConfigValue;
				}
				else if(ConfigName == "HDStreamBps")
				{
					config.HDRealStreamBps = ConfigValue;
				}
				else if(ConfigName == "HDStreamWidth")
				{
					config.HDRealStreamWidth = ConfigValue;
				}
				else if(ConfigName == "HDStreamHeight")
				{
					config.HDRealStreamHeight = ConfigValue;
				}
				else if(ConfigName == "SDStreamBps")
				{
					config.SDRealStreamBps = ConfigValue;
				}
				else if(ConfigName == "SDStreamWidth")
				{
					config.SDRealStreamWidth = ConfigValue;
				}
				else if(ConfigName == "SDStreamHeight")
				{
					config.SDRealStreamHeight = ConfigValue;
				}
				else if(ConfigName == "HDRecordBps")
				{
					config.HDRecordBps = ConfigValue;
				}
				else if(ConfigName == "HDRecordWidth")
				{
					config.HDRecordWidth = ConfigValue;
				}
				else if(ConfigName == "HDRecordHeight")
				{
					config.HDRecordHeight = ConfigValue;
				}
				else if(ConfigName == "SDRecordBps")
				{
					config.SDRecordBps = ConfigValue;
				}
				else if(ConfigName == "SDRecordWidth")
				{
					config.SDRecordWidth = ConfigValue;
				}
				else if(ConfigName == "SDRecordHeight")
				{
					config.SDRecordHeight = ConfigValue;
				}
				else if(ConfigName == "ChannelScanType")
				{
					config.ChannelScanType = ConfigValue;
				}
				else if(ConfigName == "Osd_fontsize")
				{
					config.Osd_fontsize = ConfigValue;
				}
				else if(ConfigName == "Osd_infoosd")
				{
					config.Osd_infoosd = ConfigValue;
				}
				else if(ConfigName == "Osd_infoosdx")
				{
					config.Osd_infoosdx = ConfigValue;
				}
				else if(ConfigName == "Osd_infoosdy")
				{
					config.Osd_infoosdy = ConfigValue;
				}
				else if(ConfigName == "Osd_timeosdtype")
				{
					config.Osd_timeosdtype = ConfigValue;
				}
				else if(ConfigName == "Osd_timeosdx")
				{
					config.Osd_timeosdx = ConfigValue;
				}
				else if(ConfigName == "Osd_timeosdy")
				{
					config.Osd_timeosdy = ConfigValue;
				}
				else if(ConfigName == "Osd_programx")
				{
					config.Osd_programx = ConfigValue;
				}
				else if(ConfigName == "Osd_programy")
				{
					config.Osd_programy = ConfigValue;
				}
				else if(ConfigName == "AlarmType")
				{
					config.AlarmType = ConfigValue;
				}
				else if(ConfigName == "AlarmInterval")
				{
					config.AlarmInterval = ConfigValue;
				}
				else if(ConfigName == "AlarmOnceDay")
				{
					config.OnceDay = ConfigValue;
				}
				else if(ConfigName == "AlarmRunPlanRecord")
				{
					config.RunplanRecord = ConfigValue;
				}
				else if(ConfigName == "AlarmServer")
				{
					config.AlarmServer = ConfigValue;
				}
				else if(ConfigName == "StreamRoundUrl")
				{
					config.StreamRoundUrl = ConfigValue;
				}
				else if(ConfigName == "LogPath_VIDEO")
				{
					config.LogPath_VIDEO = ConfigValue;
				}
				else if(ConfigName == "LogPath_SI")
				{
					config.LogPath_SI = ConfigValue;
				}
				else if(ConfigName == "LogPath_SMS")
				{
					config.LogPath_SMS = ConfigValue;
				}
				else if(ConfigName == "LogPath_CAS")
				{
					config.LogPath_CAS = ConfigValue;
				}
				else if(ConfigName == "SmsDir")
				{
					config.SmsDir = ConfigValue;
				}
				else if(ConfigName == "CasDir")
				{
					config.CasDir = ConfigValue;
				}
				else if(ConfigName == "SIDir")
				{
					config.SIDir = ConfigValue;
				}
				else if(ConfigName == "VideoStreamProtocol")
				{
					config.VideoStreamProtocol = ConfigValue;
				}
				else if(ConfigName == "VideoFileProtocol")
				{
					config.VideoFileProtocol = ConfigValue;
				}
				else if(ConfigName == "VideoFileUrlType")
				{
					config.VideoFileUrlType = ConfigValue;
				}
				else if(ConfigName == "VideoFileOffSet")
				{
					config.VideoFileOffSet = ConfigValue;
				}
				else if(ConfigName == "RecordParam_recordexpire")
				{
					config.RecordParam_recordexpire = ConfigValue;
				}
				else if(ConfigName == "RecordParam_alarmrecordexpire")
				{
					config.RecordParam_alarmrecordexpire = ConfigValue;
				}
				else if(ConfigName == "RecordParam_alarmheadoffset")
				{
					config.RecordParam_alarmheadoffset = ConfigValue;
				}
				else if(ConfigName == "RecordParam_alarmtailoffset")
				{
					config.RecordParam_alarmtailoffset = ConfigValue;
				}
				else if(ConfigName == "RecordParam_alarmrecordmode")
				{
					config.RecordParam_alarmrecordmode = ConfigValue;
				}
				else if(ConfigName == "RecordDown_protocol")
				{
					config.RecordDown_protocol = ConfigValue;
				}
				else if(ConfigName == "RecordDown_urltype")
				{
					config.RecordDown_urltype = ConfigValue;
				}
				else if(ConfigName == "RecordDown_offset")
				{
					config.RecordDown_offset = ConfigValue;
				}
				else if(ConfigName == "GeneralDestCode")
				{
					config.GeneralDestCode = ConfigValue;
				}
				else if(ConfigName == "GeneralSrcCode")
				{
					config.GeneralSrcCode = ConfigValue;
				}
				else if(ConfigName == "RealtimeStreamUseRecordId")
				{
					config.RealTimeFromRecord = ConfigValue;
				}
				else if(ConfigName == "AlarmRecordSwitch")
				{
					config.AlarmStorage_switch = ConfigValue;
				}
				else if(ConfigName == "AlarmRecordFilenum")
				{
					config.AlarmStorage_filenum = ConfigValue;
				}
				recordset.MoveNext();
			}
			if(!bFind)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"没有查到监测类型相关配置信息！",LOG_OUTPUT_FILE);
				return false;
			}
			else
			{
				VecConfig.push_back(config);
			}
			return true;
		}
	}
	catch (...)
	{
		if(!bFind)
		{
			APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"没有查到监测类型相关配置信息！",LOG_OUTPUT_FILE);
			return false;
		}
		else
		{
			VecConfig.push_back(config);
		}
		return true;
	}
	return false;
}

bool SqlServerDBConnection::QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold)
{
	std::string sql="select * from LevelThresholdConfig ";
	bool ret=false;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询报警电平门限信息
			{
				while(!recordset.IsEof())
				{
					ThresHold threshold;
					string dvbtype;
					recordset.GetFieldValue("DVBType",dvbtype);
					recordset.GetFieldValue("Freq",threshold.freq);
					recordset.GetFieldValue("DownThreshold",threshold.downthreshod);
					threshold.dvbtype = OSFunction::GetEnumDVBType(dvbtype);
					recordset.MoveNext();
					VecThresHold.push_back(threshold);
				}
				ret=true;
			}

		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警电平门限出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold)
{
	bool ret=false;
	string sql  ="update LevelThresholdConfig set DownThreshold='"; 
		   sql += string(alarmthreshold+"' ");
	       sql += string("where dvbtype='"+StrUtil::Int2Str(dvbtype));
	       sql += string("' and freq='"+freq+"'");
	string delsql  ="delete from LevelThresholdConfig "; 
	       delsql += string("where dvbtype='"+StrUtil::Int2Str(dvbtype));
	       delsql += string("' and freq='"+freq+"'");

	string insertsql  ="insert into LevelThresholdConfig values('"+StrUtil::Int2Str(dvbtype)+"','";
		   insertsql += freq+"','";
		   insertsql += alarmthreshold+"')";
	try
	{
		ret=DBCon->Execute(delsql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(delsql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中设置无载波报警门限出错",LOG_OUTPUT_FILE);
			}
		}
		//
		ret=DBCon->Execute(insertsql.c_str());
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(insertsql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"insertsql向备机数据库中设置无载波报警门限出错",LOG_OUTPUT_FILE);
			}
		}
		//
		
		/*
		ret=DBCon->Execute(sql.c_str());
		if(DBCon->GetRecordsAffected()==0)
		{
			string insertsql="insert into LevelThresholdConfig values('"+StrUtil::Int2Str(dvbtype)+"','";
			insertsql += freq+"','";
			insertsql += alarmthreshold+"')";
			ret=DBCon->Execute(insertsql.c_str());
		}*/
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"设置无载波报警门限出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority)
{
	std::string sql="select * from AlarmLevelConfig ";
	bool ret=false;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询报警电平门限信息
			{
				while(!recordset.IsEof())
				{
					sAlarmPriority alarmpriority;
					string dvbtype,alarmtype,val,TypeID;
					recordset.GetFieldValue("DVBType",dvbtype);
					recordset.GetFieldValue("AlarmType",alarmtype);
					recordset.GetFieldValue("Value",val);
					alarmpriority.dvbtype = OSFunction::GetEnumDVBType(dvbtype);
					alarmpriority.priority = val;
					alarmpriority.type = OSFunction::GetAlarmTypeID(OSFunction::GetEnumDVBType(dvbtype),alarmtype);
					recordset.MoveNext();
					vecAlarmPriority.push_back(alarmpriority);
				}
				ret=true;
			}

		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警电平门限出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::QueryScheduleAlarmInfo( string taskid )
{
	bool ret=true;
	string sql="select * from alarminfo where TypeID='";
	sql+=taskid;
	sql+="'";
	try
	{
		CADORecordset record;
		{
			if (record.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				int rowcount=0;
				{
					rowcount=record.GetRecordCount();
				}
				if(rowcount==0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}	
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询报警结果信息出错！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}


bool SqlServerDBConnection::QueryLastRecordDateTime( std::string strFilePath,std::string &strDateTime )
{
	std::string sql="select top 1 * from record where filename like '%";
	sql += strFilePath;
	sql += "%' order by starttime";
	bool ret=false;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询报警电平门限信息
			{
				recordset.GetFieldValue("StartTime",strDateTime);
				ret=true;
			}

		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,"查询最晚录像文件开始时间出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}



bool SqlServerDBConnection::TruncateTable(std::string strTableName)
{
	bool ret=false;
	
	std::string strSql = "truncate table ";
	strSql += strTableName;

	try
	{
		ret=DBCon->Execute( strSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中TruncateTable出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		std::string strError = strSql + "失败！！";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strError,LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteAlarmfromAlarmid(std::string strAlarmid)
{
	bool ret=false;

	std::string strSql = "delete from alarminfo where alarmid = '";
	strSql += strAlarmid + "'";

	try
	{
		ret=DBCon->Execute( strSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中DeleteAlarmfromAlarmid出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		std::string strError = strSql + "失败！！";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strError,LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

bool SqlServerDBConnection::DeleteAlarmInfo(std::string strAlarmTime)
{
	bool ret=false;

	std::string strSql = "delete from alarminfo where starttime<'";
	strSql += strAlarmTime + "'";

	try
	{
		ret=DBCon->Execute( strSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中DeleteAlarmInfo出错",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		std::string strError = strSql + "失败！！";
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_ERROR,strError,LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}

#ifdef ZONG_JU_ZHAO_BIAO

bool SqlServerDBConnection::SetGeneralDstCode( eDVBType eType, std::string strDstCode )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strDstCode;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eType );
	strUpdateSql += "' and configname='generaldstcode'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改前端dstcode失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改前端dstcode失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}


bool SqlServerDBConnection::SetGeneralSrcCode( eDVBType eType, std::string strSrcCode )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strSrcCode;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eType );
	strUpdateSql += "' and configname='generalsrccode'";
	
	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改前端srccode失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改前端srccode失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetGeneralUpUr( eDVBType eType, std::string strUpUr )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strUpUr;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eType );
	strUpdateSql += "' and configname='generalUpUr'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改前端UpUr失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改前端UpUr失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetAlarmUrl( eDVBType eDvbtype, std::string strAlarmUrl )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strAlarmUrl;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' and configname='alarmserver'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改报警url失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改报警url失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strStreamRoundUrl)
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strStreamRoundUrl;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' and configname='streamroundurl'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改马赛克轮播上报url失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改马赛克轮播上报url失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetRecordSavePath(std::string strRecordPath)  //update Config set ConfigValue ='d:/tsrecord/' where ConfigName = 'Record_FileLocation'
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strRecordPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='Record_FileLocation'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改录像保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改录像保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}
bool SqlServerDBConnection::SetLogSavePath(std::string strLogPath)  
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strLogPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='Log_Path'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改服务器日志保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改服务器日志保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetVIDEOLogSavePath(std::string strLogPath)
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strLogPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='LogPath_VIDEO'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改日志(VIDEO)保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改日志(VIDEO)保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;

}

bool SqlServerDBConnection::SetSILogSavePath(std::string strLogPath)
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strLogPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='LogPath_SI'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改日志(SI)保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改日志(SI)保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;

}

bool SqlServerDBConnection::SetSMSLogSavePath(std::string strLogPath)
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strLogPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='LogPath_SMS'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改日志(SMS)保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改日志(SMS)保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;

}

bool SqlServerDBConnection::SetCASLogSavePath(std::string strLogPath)
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strLogPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='LogPath_CAS'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改日志(CAS)保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改日志(CAS)保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;

}



bool SqlServerDBConnection::SetSmsFilePath(std::string strSmsPath)  
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strSmsPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='SmsDir'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改SMS文件保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改SMS文件保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}

	return ret;

}


//void SqlServerDBConnection::CreatFile(std::string path)
//{
//	std::vector<std::string> pathvelc;
//	
//	PROPMANAGER::instance()->GetPath(path,pathvelc);//处理多级目录
//	
//	for (int dirIndex=0; dirIndex<pathvelc.size(); dirIndex++)
//	{
//		if ( !CreateDirectory(pathvelc[dirIndex].c_str(), NULL)) 	
//		{
//			//std::cout<<"文件夹 ["<<vecVscttbDir[dirIndex]<<"] 已经存在,不用创建"<<std::endl;
//		}
//		else
//			std::cout<<"文件夹 ["<<path<<"] 创建成功"<<std::endl;
//	}
//	return;
//}

bool SqlServerDBConnection::SetCasFilePath(std::string strCasPath)  
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strCasPath;
	//	strUpdateSql += "' where dvbtype='";
	//	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' where configname='CasDir'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改CAS文件保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改CAS文件保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}

	return ret;
}
bool SqlServerDBConnection::SetSIFilePath(std::string strSIPath)  
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strSIPath;
	strUpdateSql += "' where configname='TablePath' and DVBType = '3'";

	string strUpdateSqlsi="update config set configvalue='";
	strUpdateSqlsi += strSIPath;
	strUpdateSqlsi += "' where configname='SIDir' ";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改SI文件保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改SI文件保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}


	try
	{
		ret=DBCon->Execute( strUpdateSqlsi.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSqlsi.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改SI文件保存路径失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改SI文件保存路径失败！",LOG_OUTPUT_FILE);
		return true;
	}

	return ret;
}

bool SqlServerDBConnection::SetHDRecordBps( eDVBType eDvbtype, std::string strBps )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strBps;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' and configname='HDRecordBps'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改高清录像bps失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改高清录像bps失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}


bool SqlServerDBConnection::SetSDRecordBps( eDVBType eDvbtype, std::string strBps )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strBps;
	strUpdateSql += "' where dvbtype='";
	strUpdateSql += StrUtil::Int2Str( eDvbtype );
	strUpdateSql += "' and configname='SDRecordBps'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改标清录像bps失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改标清录像bps失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::QueryRecVideoTime( eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID )
{
	bool ret=true;

	std::string strStartTimeSql = "select top 1 * from record where channelId='";
	strStartTimeSql += strChanId;
	strStartTimeSql += string("'");
	if(DeviceID != "")
	{
		strStartTimeSql += string(" and DeviceID ='") + DeviceID + string("'");
	}
	strStartTimeSql += " and starttime > '";
	strStartTimeSql += strExpireDay;
	strStartTimeSql += "' order by starttime";
	std::string strEndTimeSql = strStartTimeSql + " desc";

	try
	{
		CADORecordset startSet;
		if (startSet.Open(DBCon->GetActiveConnection(),strStartTimeSql.c_str()))//执行录像文件查询sql
		{
			while (!startSet.IsEof())
			{
				startSet.GetFieldValue("starttime", strStartTime);//录像文件开始时间
				
				startSet.MoveNext();//移动到下一条记录
			}
			ret=true;
		}

		CADORecordset endSet;
		if (endSet.Open(DBCon->GetActiveConnection(),strEndTimeSql.c_str()))//执行录像文件查询sql
		{
			while (!endSet.IsEof())
			{
				endSet.GetFieldValue("endtime", strEndTime);//录像文件结束时间
				endSet.MoveNext();//移动到下一条记录
			}
			ret=true;
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询录像文件开始和结束时间失败!!",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}
bool SqlServerDBConnection::SetAutoAnalysisTime(std::string strTime )
{
	bool ret=true;

	string strUpdateSql="update config set configvalue='";
	strUpdateSql += strTime;
	strUpdateSql += "' where configname='PsisiSchedulerTask_Time'";

	try
	{
		ret=DBCon->Execute( strUpdateSql.c_str() );
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改高清分辨率失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::GetAutoAnalysisTime(std::string& strTime )
{
	bool ret=false;
	string sql="select configvalue from config where configname='PsisiSchedulerTask_Time'";
	
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("configvalue",strTime);//获得msgid
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询AnalysisTime出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;

}

bool SqlServerDBConnection::SetHDRecordHW( eDVBType eDvbtype, std::string strHeight, std::string strWidth )
{
	bool ret=true;

	string strUpdateHSql="update config set configvalue='";
	strUpdateHSql += strHeight;
	strUpdateHSql += "' where dvbtype='";
	strUpdateHSql += StrUtil::Int2Str( eDvbtype );
	strUpdateHSql += "' and configname='HDRecordHeight'";

	string strUpdateWSql="update config set configvalue='";
	strUpdateWSql += strWidth;
	strUpdateWSql += "' where dvbtype='";
	strUpdateWSql += StrUtil::Int2Str( eDvbtype );
	strUpdateWSql += "' and configname='HDRecordWidth'";

	try
	{
		ret=DBCon->Execute( strUpdateHSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateHSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改高清分辨率失败",LOG_OUTPUT_FILE);
			}
		}
		//
		ret=DBCon->Execute( strUpdateWSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateWSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"strUpdateWSql向备机数据库中修改高清分辨率失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改高清分辨率失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::SetSDRecordHW( eDVBType eDvbtype, std::string strHeight, std::string strWidth )
{
	bool ret=true;

	string strUpdateHSql="update config set configvalue='";
	strUpdateHSql += strHeight;
	strUpdateHSql += "' where dvbtype='";
	strUpdateHSql += StrUtil::Int2Str( eDvbtype );
	strUpdateHSql += "' and configname='SDRecordHeight'";

	string strUpdateWSql="update config set configvalue='";
	strUpdateWSql += strWidth;
	strUpdateWSql += "' where dvbtype='";
	strUpdateWSql += StrUtil::Int2Str( eDvbtype );
	strUpdateWSql += "' and configname='SDRecordWidth'";

	try
	{
		ret=DBCon->Execute( strUpdateHSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateHSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"向备机数据库中修改标清分辨率失败",LOG_OUTPUT_FILE);
			}
		}
		//
		ret=DBCon->Execute( strUpdateWSql.c_str() );
		//
		if(BandDBOK)
		{
			try
			{
				BandDBCon->Execute(strUpdateWSql.c_str());
			}
			catch (...)
			{
				APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"strUpdateWSql向备机数据库中修改标清分辨率失败",LOG_OUTPUT_FILE);
			}
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改标清分辨率失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}

bool SqlServerDBConnection::AgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo)
{
	bool ret=true;


	if(VecAgentSetInfo.size()>0)
	{
		try
		{
			string delAgentInfosql  ="delete from AgentInfoSet "; 
			ret=DBCon->Execute( delAgentInfosql.c_str() );
			//
			for (int index=0; index<VecAgentSetInfo.size(); index++)
			{
				std::string addsql="insert into AgentInfoSet(Type,AgentDesc,AgentValue) values('"+VecAgentSetInfo[index].s_strType+"','"+VecAgentSetInfo[index].s_strDesc+"','"+VecAgentSetInfo[index].s_strValue+"')";
				ret=DBCon->Execute( addsql.c_str() );
			}
			//
		}
		catch (...)
		{
			APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"AgentSetInfo失败！",LOG_OUTPUT_FILE);
			return true;
		}
	}
	return ret;
}

bool SqlServerDBConnection::GetAgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo)
{
	bool ret=false;
	string sql="select * from AgentInfoSet";
	sAgentSetInfo agentSetInfo;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("Type",agentSetInfo.s_strType);
					recordset.GetFieldValue("AgentDesc",agentSetInfo.s_strDesc);
					recordset.GetFieldValue("AgentValue",agentSetInfo.s_strValue);
					VecAgentSetInfo.push_back(agentSetInfo);
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询AnalysisTime出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)
{
	bool ret=true;
	try
	{
		for(int i=0;i<VecOSDFormat.size();i++)
		{
			string delOSDFormatInfosql  ="delete from OSDFormatSet where Freq='"; 
			delOSDFormatInfosql += VecOSDFormat[i].Freq + string("'");
			delOSDFormatInfosql += string(" and ServiceID ='") + VecOSDFormat[i].ServiceID + string("'");
			ret=DBCon->Execute( delOSDFormatInfosql.c_str() );
			//
			std::string addsql="insert into OSDFormatSet(Freq,ServiceID,FontSize,Position,AntiColor,OSDFormatType,Align) values('"+VecOSDFormat[i].Freq+"','"+VecOSDFormat[i].ServiceID+"','"+VecOSDFormat[i].FontSize+"','"+VecOSDFormat[i].Position+"','"+VecOSDFormat[i].AntiColor+"','"+VecOSDFormat[i].Type+"','"+VecOSDFormat[i].Align+"')";
			ret=DBCon->Execute( addsql.c_str() );
		}
		//
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"AutoAnalysisSetInfo失败！",LOG_OUTPUT_FILE);
		return true;
	}
	return ret;
}
bool SqlServerDBConnection::GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)
{
	bool ret=false;
	string sql="select * from OSDFormatSet";
	OSDFormat OSDFormatInfo;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("Freq",OSDFormatInfo.Freq);
					recordset.GetFieldValue("ServiceID",OSDFormatInfo.ServiceID);
					recordset.GetFieldValue("FontSize",OSDFormatInfo.FontSize);
					recordset.GetFieldValue("Position",OSDFormatInfo.Position);
					recordset.GetFieldValue("AntiColor",OSDFormatInfo.AntiColor);
					recordset.GetFieldValue("OSDFormatType",OSDFormatInfo.Type);
					recordset.GetFieldValue("Align",OSDFormatInfo.Align);
					VecOSDFormat.push_back(OSDFormatInfo);
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询OSDFormatSet出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID)
{
	bool ret=true;


	if(strStartTime.size()>0&&strDevID.size()>0)
	{
		try
		{
			string delPSISIAnalysisInfosql  ="delete from PSISIAnalysisTime "; 
			ret=DBCon->Execute( delPSISIAnalysisInfosql.c_str() );
			//
			std::string addsql="insert into PSISIAnalysisTime(StartTime,PSISIAnalysisType,ScanType,DeviceID) values('"+strStartTime+"','"+strPSIType+"','"+strScanType+"','"+strDevID+"')";
			ret=DBCon->Execute( addsql.c_str() );
			//
		}
		catch (...)
		{
			APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"AutoAnalysisSetInfo失败！",LOG_OUTPUT_FILE);
			return true;
		}
	}
	return ret;
}
bool SqlServerDBConnection::GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID)
{
	bool ret=false;
	string sql="select * from PSISIAnalysisTime";
	
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("StartTime",strStartTime);
					recordset.GetFieldValue("PSISIAnalysisType",strPSIType);
					recordset.GetFieldValue("ScanType",strScanType);
					recordset.GetFieldValue("DeviceID",strDevID);
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询AnalysisTime出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::AddUploadTask( sRecordUploadTaskInfo uploadtask )
{
	bool ret=true;
	string sql="insert into RecordUploadTask(dvbtype,taskid,channelid,devcodehead,uploadsucenddtime,uploadsucfilename,taskstatus,taskinfo) values('";
	sql+="3','";
	sql+=uploadtask.s_taskid+"','";
	sql+=uploadtask.s_channelid+"','";
	sql+=uploadtask.s_devcodehead+"','";
	sql+=uploadtask.s_uploadsucenddtime+"','";
	sql+=uploadtask.s_uploadsucfilename+"','";
	sql+="0','";
	sql+=uploadtask.s_taskinfo+"')";
	try
	{
		ret=DBCon->Execute(sql.c_str());//执行添加上传任务信息xml
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"添加上传任务信息出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus)
{
	bool ret=true;
	string updatesql="update RecordUploadTask set UploadSucEndDTime='";
	updatesql += curendtime;
	updatesql += "' ,uploadsucfilename = '";
	updatesql += newfilepathname;
	updatesql += "' ,taskstatus = '";
	updatesql += taskstatus;
	updatesql += "' where taskid='";
	updatesql += taskid;
	updatesql += "'";

	try
	{
		ret=DBCon->Execute(updatesql.c_str());//执行添加上传任务信息xml
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"修改上传任务状态出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
bool SqlServerDBConnection::QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask )
{
	bool ret=false;
	string sql="select * from RecordUploadTask where TaskStatus = '0'";
	sRecordUploadTaskInfo sRUploadTaskInfo;
	try
	{
		CADORecordset recordset;
		{
			if (recordset.Open(DBCon->GetActiveConnection(),sql.c_str()))//查询指标信息
			{
				while(!recordset.IsEof())
				{
					recordset.GetFieldValue("TaskID",sRUploadTaskInfo.s_taskid);
					recordset.GetFieldValue("ChannelID",sRUploadTaskInfo.s_channelid);
					recordset.GetFieldValue("DevCodeHead",sRUploadTaskInfo.s_devcodehead);
					recordset.GetFieldValue("UploadSucEndDTime",sRUploadTaskInfo.s_uploadsucenddtime);
					recordset.GetFieldValue("UploadSucFileName",sRUploadTaskInfo.s_uploadsucfilename);
					recordset.GetFieldValue("TaskInfo",sRUploadTaskInfo.s_taskinfo);
					sRUploadTaskInfo.s_active = "0";
					VecUploadtask.push_back(sRUploadTaskInfo);
					recordset.MoveNext();
				}
				ret=true;
			}
		}
	}
	catch (...)
	{
		APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,"查询RecordUploadTask出错！",LOG_OUTPUT_FILE);
		return false;
	}
	return ret;
}
#endif

