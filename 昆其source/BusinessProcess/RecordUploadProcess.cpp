
#include "RecordUploadProcess.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../DBAccess/DBManager.h"
#include "../FileSysAccess/WPLFile.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Communications/XMLSend.h"
#include "../Task/AutoRecordUploadTask.h"
extern sRecordUploadTaskInfo m_utaskinfo;
RecordUploadProcess::RecordUploadProcess()
{
	bFlag = false;
	Finished = false;
	bEnd = false;
	bPause = false;
	m_utaskinfo.s_taskid = "-1";
}

RecordUploadProcess::~RecordUploadProcess()
{

}

int RecordUploadProcess::open(void*)
{
	activate();
	return 0;
}

int RecordUploadProcess::svc()
{
	bFlag = true;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) [%s]录像上传线程开始执行 !\n",m_utaskinfo.s_taskid.c_str()));

	string strCurStarttime = m_utaskinfo.s_uploadsucenddtime;
	if(strCurStarttime.size() <= 0)
	{
		strCurStarttime = pUpload.s_StartDateTime;
	}
	string strCurEndtime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(strCurStarttime)+(StrUtil::Str2Int(pUpload.s_Length)*60));
	//任务状态 0 未完成 1已完成
	while (bFlag)
	{
		if((bEnd)||(TimeUtil::StrToDateTime(strCurStarttime) >= TimeUtil::StrToDateTime(strCurEndtime)))
		{
			//string sucTime = TimeUtil::GetCurDateTime();
			DBMANAGER::instance()->UpdateUploadTask(m_utaskinfo.s_taskid,m_utaskinfo.s_uploadsucenddtime,m_utaskinfo.s_uploadsucfilename,"1");
			break;
		}
		if((m_utaskinfo.s_taskid != "-1")&&(!bPause)&&(time(0) > TimeUtil::StrToDateTime(strCurEndtime)+600))
		{
			std::vector<sRecordInfo> vecRecordInfo;
			//string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(pUpload.s_strChassisID),StrUtil::Str2Int(pUpload.s_strPosition),StrUtil::Str2Int(pUpload.s_strModuleID),StrUtil::Str2Int(pUpload.s_strTranscode)));
			string CoderDeviceID = pUpload.s_strChassisID + "_" + pUpload.s_strPosition + "_" + pUpload.s_strModuleID + "_" + pUpload.s_strTranscode;
			string strchannelId;
			CHANNELINFOMGR::instance()->GetChanIdByFreqServId(sqltype, pUpload.s_Freq, pUpload.s_ServiceID, strchannelId);
			//if(DBMANAGER::instance()->QueryRecordByDeviceID(sqltype,"0",CoderDeviceID,vecRecordInfo,strCurStarttime,strCurEndtime))
			if(DBMANAGER::instance()->QueryRecordByChannelID(sqltype,"0",strchannelId,vecRecordInfo,strCurStarttime,strCurEndtime,CoderDeviceID))
			{
				if(vecRecordInfo.size() > 0)
				{
					SpliterRecordFile(vecRecordInfo,strCurStarttime,strCurEndtime);
					//?????
					if(!OpenShareServer())
					{
						string strmsg = "DstServer open fail";
						APPLOG::instance()->WriteLog(TASK,LOG_EVENT_WARNNING,strmsg,LOG_OUTPUT_BOTH);
					}
					//
					string newfilepathname = pUpload.s_Address;
					string nlastchar = newfilepathname.substr(newfilepathname.size()-1,1);
					if((nlastchar.find("\\")==-1)&&(nlastchar.find("/")==-1))
					{
						newfilepathname = newfilepathname+"\\";
					}
					newfilepathname+=pUpload.s_Folder;
					newfilepathname+="\\";
					//??????4
					PROPMANAGER::instance()->Creatfolder(newfilepathname);
					//
					//newfilepathname+="\\";
					newfilepathname+=m_utaskinfo.s_taskid;
					newfilepathname+="_";
					newfilepathname+=TimeUtil::DateTimeToString(TimeUtil::StrToDateTime(strCurStarttime));
					newfilepathname+="_";
					newfilepathname+=TimeUtil::DateTimeToString(TimeUtil::StrToDateTime(strCurEndtime));
					newfilepathname+=".ts";
					//
					string tempfilename = PROPMANAGER::instance()->GetTempFileLoac();
					string tlastchar = tempfilename.substr(tempfilename.size()-1,1);
					if((tlastchar.find("\\")==-1)&&(tlastchar.find("/")==-1))
					{
						tempfilename = tempfilename+"\\";
					}
					tempfilename+=m_utaskinfo.s_taskid;
					tempfilename+="_";
					tempfilename+=TimeUtil::DateTimeToString(TimeUtil::StrToDateTime(strCurStarttime));
					tempfilename+="_";
					tempfilename+=TimeUtil::DateTimeToString(TimeUtil::StrToDateTime(strCurEndtime));
					tempfilename+=".ts";
					//
					size_t pos;
					while((pos=tempfilename.find("/"))!=string::npos)
					{
						tempfilename.replace(pos,1,"\\");
					}
					time_t CurTime = time(0);
					CreateUploadFile(tempfilename,vecRecordInfo);
					DWORD files = 0;
					bool senderfirst = true;
					while(true)
					{
						if(CopyUploadFile(newfilepathname,tempfilename,files))
						{
							senderfirst = true;
							//*********************
							m_utaskinfo.s_uploadsucenddtime = strCurEndtime;
							time_t endTime = time(0);
							m_utaskinfo.s_uploadsucfilename = newfilepathname;
							//
							DBMANAGER::instance()->UpdateUploadTask(m_utaskinfo.s_taskid,m_utaskinfo.s_uploadsucenddtime,m_utaskinfo.s_uploadsucfilename,"0");
							string retxml = CreateUploadResultXMLFile(strCurStarttime,strCurEndtime,files,endTime - CurTime);
							//m_utaskinfo.s_taskxml = retxml;
							std::string recuploadUrl;
							//recuploadUrl = PROPMANAGER::instance()->GetAgentInfoSet("16");
							recuploadUrl = pUpload.s_dstURL;
							if(recuploadUrl.size() > 0)
							{
								XMLSend::SendXML(recuploadUrl,retxml);
								APPLOG::instance()->WriteLog(TASK,LOG_EVENT_WARNNING,retxml,LOG_OUTPUT_BOTH);					
							}
							else
							{
								ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传线程上传结果地址为空 !\n"));
							}
							//
							strCurStarttime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(strCurEndtime)+1);
							strCurEndtime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(strCurStarttime)+(StrUtil::Str2Int(pUpload.s_Length)*60));
							if(TimeUtil::StrToDateTime(strCurEndtime) >= TimeUtil::StrToDateTime(pUpload.s_EndDateTime))
							{
								strCurEndtime = pUpload.s_EndDateTime;
							}
							break;
						}
						else
						{
							if(senderfirst)
							{
								ACE_DEBUG ((LM_DEBUG,"(%T| %t) [%s]录像上传线程上传文件失败 !\n",tempfilename.c_str()));
							}
							senderfirst = false;
							OSFunction::Sleep(60);
						}
						if(bEnd)
						{
							break;
						}
					}
				}
				else
				{
					strCurStarttime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(strCurEndtime));
					strCurEndtime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(strCurStarttime)+(StrUtil::Str2Int(pUpload.s_Length)*60));
					if(TimeUtil::StrToDateTime(strCurEndtime) >= TimeUtil::StrToDateTime(pUpload.s_EndDateTime))
					{
						strCurEndtime = pUpload.s_EndDateTime;
					}
					OSFunction::Sleep(0,100);	
				}
			}
			else
			{
				std::string msg = "读取数据库上传视频信息失败";
				APPLOG::instance()->WriteLog(TASK,LOG_EVENT_WARNNING,msg,LOG_OUTPUT_BOTH);
			}
		}
		else
		{
			OSFunction::Sleep(0,100);
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) [%s]录像上传线程停止执行 !\n",m_utaskinfo.s_taskid.c_str()));
	Finished = true;
	return 0;
}

int RecordUploadProcess::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
void RecordUploadProcess::PaserTaskXml(string txml)
{
	XmlParser parser(txml.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"SrcURL",pUpload.s_dstURL);  
	parser.GetAttrNode(rootNode,"Type",pUpload.s_type);  
	if(pUpload.s_type == "DTMBDown")
	{
		sqltype = CTTB;
		UpType = "DTMBUp";
	}
	else if(pUpload.s_type == "DVBCDown")
	{
		sqltype = DVBC;
		UpType = "DVBCUp";
	}
	else if(pUpload.s_type == "RADIODown")
	{
		sqltype = RADIO;
		UpType = "RADIOUp";
	}
	else if( pUpload.s_type == "AMDown")
	{
		sqltype = AM;
		UpType = "AMUp";
	}
	else if( pUpload.s_type == "DVBSDown")
	{
		sqltype = DVBS;
		UpType = "DVBSUp";
	}
	string atrAddressl;
	pXMLNODE ServerInfoNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ServerInfo");
	parser.GetAttrNode(ServerInfoNode,"Address",atrAddressl); 
	
	parser.GetAttrNode(ServerInfoNode,"UserName",pUpload.s_UserName); 
	parser.GetAttrNode(ServerInfoNode,"Password",pUpload.s_PassWord); 
	//
	int Enfpos = atrAddressl.rfind("$");
	pUpload.s_Address = atrAddressl.substr(0,++Enfpos);//映射盘符
	if(!strcmp(pUpload.s_Address.c_str(),atrAddressl.c_str()))
	{
		pUpload.s_Folder = "";
	}
	else
	{
		pUpload.s_Folder = atrAddressl.substr(++Enfpos,atrAddressl.length());//共享文件夹
	}
	int pos = -1;
	while((pos=pUpload.s_Address.find("\\\\"))!=string::npos)
	{
		pUpload.s_Address.replace(pos,2,"\\");
	}
	pUpload.s_Address = "\\" + pUpload.s_Address;
	//
	pXMLNODE ChannelInfoNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ChannelInfo");
	parser.GetAttrNode(ChannelInfoNode,"Freq",pUpload.s_Freq);  
	parser.GetAttrNode(ChannelInfoNode,"ServiceID",pUpload.s_ServiceID); 
	parser.GetAttrNode(ChannelInfoNode,"StartDateTime",pUpload.s_StartDateTime);
	parser.GetAttrNode(ChannelInfoNode,"EndDateTime",pUpload.s_EndDateTime);  
	parser.GetAttrNode(ChannelInfoNode,"Length",pUpload.s_Length);  
	pXMLNODE TranscodeNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ChannelInfo/Transcode");
	pXMLNODELIST pInfoList = parser.GetNodeList(TranscodeNode);
	string STRTRANSCODE;//高请频道0;1;2;3
	string strTranscode;
	for(int i =0;i<pInfoList->Size();++i)
	{
		pXMLNODE pInfoNode = parser.GetNextNode(pInfoList);
		parser.GetAttrNode(pInfoNode,"ChassisID",pUpload.s_strChassisID);
		parser.GetAttrNode(pInfoNode,"Position",pUpload.s_strPosition);
		parser.GetAttrNode(pInfoNode,"ModuleID",pUpload.s_strModuleID);
		parser.GetAttrNode(pInfoNode,"Transcode",strTranscode);
		STRTRANSCODE += strTranscode + ";";
	}
	pUpload.s_strTranscode = STRTRANSCODE.substr(0,STRTRANSCODE.length()-1);
	string CoderDeviceID = pUpload.s_strChassisID + "_" + pUpload.s_strPosition + "_" + pUpload.s_strModuleID + "_" + pUpload.s_strTranscode;
	if(pUpload.s_StartDateTime=="")//如果任务开始时间为空,按数据库该频道最早时间给定
	{
		CHANNELINFOMGR::instance()->GetChanIdByFreqServId(sqltype, pUpload.s_Freq, pUpload.s_ServiceID, pUpload.s_strChanId);
		DBMANAGER::instance()->GetEarliestStartTimeRecordByChannelIDandDeviceID(sqltype,CoderDeviceID,pUpload.s_strChanId,pUpload.s_StartDateTime);
	}
}
bool RecordUploadProcess::SpliterRecordFile(vector<sRecordInfo>& fileVector,string StartDateTime,string EndDateTime)
{
	if(StartDateTime.length()<1&&EndDateTime.length()<1)
	{
		return true;
	}
	if (fileVector.size()==1)
	{
		string newfilename;
		time_t taskstarttime=TimeUtil::StrToDateTime(StartDateTime);
		time_t taskendtime= TimeUtil::StrToDateTime(EndDateTime);
		time_t filestarttime=TimeUtil::StrToDateTime(fileVector[0].starttime);
		time_t fileendtime=TimeUtil::StrToDateTime(fileVector[0].endtime);
		if (taskstarttime>filestarttime&&taskstarttime<fileendtime&&taskendtime>filestarttime&&taskendtime<fileendtime)//??????????????
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),
				float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false);
		}
		else if(taskstarttime>filestarttime&&taskstarttime<fileendtime&&taskendtime>fileendtime)//????????
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),true,false);
		}
		else if(taskendtime>filestarttime&&taskendtime<fileendtime&&taskstarttime<filestarttime)//????????
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false,false);
		}
		if(newfilename.size()>0)
		{
			fileVector[0].filename=newfilename;
		}
		fileVector[0].starttime=StartDateTime;
		fileVector[0].endtime=EndDateTime;
		
		if(newfilename.size()>0)
		{
			DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}
	}
	else
	{
		string newfilename;
		time_t taskstarttime=TimeUtil::StrToDateTime(StartDateTime);
		time_t taskendtime= TimeUtil::StrToDateTime(EndDateTime);
		time_t filestarttime=TimeUtil::StrToDateTime(fileVector[0].starttime);
		time_t fileendtime=TimeUtil::StrToDateTime(fileVector[0].endtime);
		if(taskstarttime>filestarttime&&taskstarttime<fileendtime)//????????
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),true,false);
		}

		if(newfilename.size()>0)
		{
			fileVector[0].filename=newfilename;
			fileVector[0].starttime=StartDateTime;
		}
		if(newfilename!="")
		{
			DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}
		//
		filestarttime=TimeUtil::StrToDateTime(fileVector[fileVector.size()-1].starttime);
		fileendtime=TimeUtil::StrToDateTime(fileVector[fileVector.size()-1].endtime);
		if(taskendtime>filestarttime&&taskendtime<fileendtime)//????????
		{
			newfilename=WPLFile::FileSplit(fileVector[fileVector.size()-1].filename,float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false,false);
		}
		if(newfilename.size()>0)
		{
			fileVector[fileVector.size()-1].filename=newfilename;
			fileVector[fileVector.size()-1].endtime=EndDateTime;
		}
		if(newfilename!="")
		{
			DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}
	}
	return true;
}
bool RecordUploadProcess::CreateUploadFile(string tempfilename,vector <sRecordInfo>& vecFile)
{
	if (tempfilename.empty()||vecFile.empty())
	{
		string msg = "查询任务文件不存在";
		APPLOG::instance()->WriteLog(TASK, LOG_EVENT_ERROR, msg,LOG_OUTPUT_BOTH);
		return false;
	}
	size_t pos;
	string cmdstr="copy /B ";
	//±é?ú?ù??????
	int index = vecFile.size();
	for (std::vector<sRecordInfo>::iterator ptr = vecFile.begin();ptr!= vecFile.end();++ptr)
	{
		if(index<=0)
		{
			break;
		}
		string tempfile=(*ptr).filename;
		if(tempfile.length()<1)
			continue;
		while((pos=tempfile.find("/"))!=string::npos)
		{
			tempfile.replace(pos,1,"\\");
		}
		cmdstr+=tempfile+"+";
		index--;
	}
	pos = cmdstr.find_last_of('+');
	if (pos != std::string::npos)
	{
		cmdstr.replace(pos,1," ");
		cmdstr+=tempfilename;
		system(cmdstr.c_str());
	}
	//???±????????
	DBMANAGER::instance()->AddTempFile(tempfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)查询任务创建文件[%s]结束 !\n",tempfilename.c_str()));
	return true;
}
bool RecordUploadProcess::CopyUploadFile(string newpathfilename,string respathfilename,DWORD& Filesize)
{
	bool ret = true;
	try
    {
		if(bEnd || bPause)
		{
			return true;
		}
		HANDLE hNewFile = INVALID_HANDLE_VALUE;
		HANDLE hresFile = INVALID_HANDLE_VALUE;
		DWORD nSize = 2048;		//Per 2k data;
		PBYTE pBuf  = NULL;
		DWORD nRet;
		DWORD nPlayFileLen = 0;
		DWORD nNewFileLen = 0;
		DWORD nBlock  = 0;
		DWORD nRemain = 0;
		DWORD nCount;

		try
		{
			hresFile = CreateFile(
							respathfilename.c_str(),
							 GENERIC_READ,
							 FILE_SHARE_READ|FILE_SHARE_WRITE,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
								 NULL);
			if(hresFile == INVALID_HANDLE_VALUE)
			{
				string strin = "Open file faild:";
				return false;
			}
			//
			if(GetFileAttributes(newpathfilename.c_str())!=-1)
			{
				APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "The file already exists");
				hNewFile = CreateFile(
									 newpathfilename.c_str(),
									 GENERIC_WRITE,
									 FILE_SHARE_READ | FILE_SHARE_WRITE,
									 NULL,
									 OPEN_EXISTING,
									 FILE_ATTRIBUTE_NORMAL,
									 NULL );
				if(hNewFile == INVALID_HANDLE_VALUE)
				{
					hNewFile = CreateFile(
										 newpathfilename.c_str(),
										 GENERIC_WRITE,
										 FILE_SHARE_READ | FILE_SHARE_WRITE,
										 NULL,
										 CREATE_ALWAYS,
										 FILE_ATTRIBUTE_NORMAL,
										 NULL );
					if(hNewFile == INVALID_HANDLE_VALUE)
					{
						APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Create saving file faild");
						return false;
					}
				}
				else
				{
					nNewFileLen = GetFileSize(hNewFile, NULL);
					SetFilePointer(hNewFile, 0, 0, FILE_END);
				}
			}
			else
			{

				hNewFile = CreateFile(
									 newpathfilename.c_str(),
									 GENERIC_WRITE,
									 FILE_SHARE_READ | FILE_SHARE_WRITE,
									 NULL,
									 CREATE_ALWAYS,
									 FILE_ATTRIBUTE_NORMAL,
									 NULL );
				if(hNewFile == INVALID_HANDLE_VALUE)
				{
					string msg = "Create saving file faild:"+newpathfilename;
					APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, msg);
					return false;
				}
			}
			

			pBuf = new BYTE [nSize];
			if(NULL == pBuf)
			{
				APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Alloc memory failed");
				return false;
			}

			nPlayFileLen = GetFileSize(hresFile, NULL);
			nPlayFileLen = nPlayFileLen - nNewFileLen;
			Filesize = nPlayFileLen;
			nBlock  = (nPlayFileLen) / nSize;
			nRemain = (nPlayFileLen) % nSize;

			SetFilePointer(hresFile, 0, 0, FILE_BEGIN+nNewFileLen);
			for(nCount = 0; nCount < nBlock; nCount ++)
			{
				if(bEnd || bPause)
				{
					break;
				}
				if(!ReadFile(hresFile, pBuf, nSize, &nRet, NULL))
				{
					APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Read file faild!");
					ret = false;
					break;
				}
				if(!WriteFile(hNewFile, pBuf, nSize, &nRet, NULL))
				{
					APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Write file faild!");
					ret = false;
					break;
				}
			}
			if(bEnd || bPause)
			{
				if(hresFile != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hresFile);
					hresFile = INVALID_HANDLE_VALUE;
				}
				return true;
			}
			if(!ReadFile(hresFile, pBuf, nRemain, &nRet, NULL))
			{
				APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Read file faild!");
				ret = false;
			}
			else
			{
				if(!WriteFile(hNewFile, pBuf, nRemain, &nRet, NULL))
				{
					APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, "Write file faild!");
					ret = false;
				}
			}
			if(hresFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hresFile);
				hresFile = INVALID_HANDLE_VALUE;
			}
		}
		catch(...)
		{
		}
		if(pBuf != NULL)
		{
			delete []pBuf;
			pBuf = NULL;
		}

		if(hNewFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hNewFile);
			hNewFile = INVALID_HANDLE_VALUE;
		}
		//
	}
	catch(...)
    {
		string smg = "文件拷贝程序异常........";
		APPLOG::instance()->WriteLog(RECORD, LOG_EVENT_ERROR, smg.c_str());
		ret = false;
	}
	return ret;
}
string RecordUploadProcess::CreateUploadResultXMLFile(string StartDateTime,string EndDateTime,DWORD Filesize,DWORD LoadTime)
{
	string strVersion,strMsgID,strSrcCode,strDstCode;
	XmlParser sparser;
	sparser.Set_xml(m_utaskinfo.s_taskinfo);
	pXMLNODE sroot=sparser.GetRootNode();
	sparser.GetAttrNode(sroot,"Version",strVersion);
	sparser.GetAttrNode(sroot,"MsgID",strMsgID);
	sparser.GetAttrNode(sroot,"SrcCode",strSrcCode);
	sparser.GetAttrNode(sroot,"DstCode",strDstCode);
	//
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//????xml??
	xml+="<Msg><AutoRecordUploadTaskResults><AutoRecordIntegraity></AutoRecordIntegraity></AutoRecordUploadTaskResults></Msg>";
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",strVersion,root );
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );
	parser.SetAttrNode( "DateTime",strDateTime,root );
	parser.SetAttrNode( "Type",UpType,root );
	parser.SetAttrNode( "SrcCode",strDstCode,root );
	parser.SetAttrNode( "DstCode",strSrcCode,root );
	parser.SetAttrNode( "ReplyID",string("-1"),root );
	//
	string ChannelID;
	if(CHANNELINFOMGR::instance()->GetChanIdByFreqServId(sqltype, pUpload.s_Freq, pUpload.s_ServiceID, ChannelID))
	{
		std::vector<sRecordInfo> vecRecordInfo;
		//string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(pUpload.s_strChassisID),StrUtil::Str2Int(pUpload.s_strPosition),StrUtil::Str2Int(pUpload.s_strModuleID),StrUtil::Str2Int(pUpload.s_strTranscode)));
		string CoderDeviceID = pUpload.s_strChassisID + "_" + pUpload.s_strPosition + "_" + pUpload.s_strModuleID + "_" + pUpload.s_strTranscode;

		if(DBMANAGER::instance()->QueryLostRecordByChannelID(sqltype,"0",ChannelID,vecRecordInfo,StartDateTime,EndDateTime,CoderDeviceID))
		{
			pXMLNODE TaskResultsNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTaskResults");
			parser.SetAttrNode( "TaskID",m_utaskinfo.s_taskid,TaskResultsNode );
			parser.SetAttrNode( "Freq",pUpload.s_Freq,TaskResultsNode );
			parser.SetAttrNode( "ServiceID",pUpload.s_ServiceID,TaskResultsNode );
			parser.SetAttrNode( "SubTaskStartTime",StartDateTime,TaskResultsNode );
			parser.SetAttrNode( "SubTaskEndTime",EndDateTime,TaskResultsNode );
			parser.SetAttrNode( "Status",string("0"),TaskResultsNode );
			parser.SetAttrNode( "Desc",string("成功"),TaskResultsNode );
			parser.SetAttrNode( "FileSize",StrUtil::Long2Str(Filesize),TaskResultsNode );
			parser.SetAttrNode( "LoadTime",StrUtil::Long2Str(LoadTime),TaskResultsNode );
			/*\\10.1.105.125\c$\tttt\11-30\\__20181130183000_20181130185000.ts*/
			int npos = m_utaskinfo.s_uploadsucfilename.rfind("\\");
			string fileName = m_utaskinfo.s_uploadsucfilename.substr(++npos,m_utaskinfo.s_uploadsucfilename.length()-npos);
			
			parser.SetAttrNode( "FileName",fileName,TaskResultsNode );
			//************************************[s_channelid]
			pUpload.s_strChanId = ChannelID;
			pXMLNODE IntegraityNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTaskResults/AutoRecordIntegraity");
			if(vecRecordInfo.size() <= 0)
			{
				parser.SetAttrNode( "Integraity",string("0"),IntegraityNode );
				parser.SetAttrNode( "LostNum",string("0"),IntegraityNode );
				parser.SetAttrNode( "LostLength",string("0"),IntegraityNode );
			}
			else
			{
				parser.SetAttrNode( "Integraity",string("1"),IntegraityNode );
				int i=0;//***
				std::vector<sRecordInfo>::const_iterator iter = vecRecordInfo.begin();
				long totallosttime = 0;
				while (iter != vecRecordInfo.end())
				{
					int losttime = TimeUtil::StrToDateTime((*iter).endtime)-TimeUtil::StrToDateTime((*iter).starttime);
					if(losttime > 30)//***
					{//***
						pXMLNODE infoNode = parser.CreateNodePtr(IntegraityNode, "LostVideoInfo");
						parser.SetAttrNode("LostStartTime", (*iter).starttime, infoNode);
						//2018/11/30 18:23:07
						std::string strTime = (*iter).endtime;
						replace(strTime.begin(),strTime.end(),'/','-');
						parser.SetAttrNode("LostEndTime", strTime, infoNode);
						i++;//***
						totallosttime+=losttime;
					}//***
					iter ++;
				}
				parser.SetAttrNode("LostNum", StrUtil::Int2Str(i), IntegraityNode);//***
				parser.SetAttrNode("LostLength", StrUtil::Long2Str(totallosttime), IntegraityNode);
			}
		}
	}
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}

bool RecordUploadProcess::OpenShareServer()
{
	bool ret = false;
	string strOrder;
	strOrder = "net use " + pUpload.s_Address + " " + string("\"") +  pUpload.s_PassWord +  string("\"") + " "+ "/user:" + string("\"") + pUpload.s_UserName + string("\"");

	int status = system(strOrder.c_str());
	if (0 == WEXITSTATUS(status))
	{
		ret = true;
	}
	return ret;
}