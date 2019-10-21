
#include "cmdLine.h"
#include "DBAccess/DBManager.h"
#include "Task/TaskFactory.h"
#include "Foundation/AppLog.h"
#include "Foundation/OSFunction.h"
#include "Foundation/PropManager.h"
#include "DeviceAccess/DeviceAccessMgr.h"
#include "Communications/CommunicationMgr.h"
#include "ace/OS.h"
#include "Foundation/StrUtil.h"
#include <vector>
#include <string>
#include <fstream>
using namespace std;
extern std::vector<sEncryptInfo> g_sEncryptInfo;

void PrintTitle (void)
{
	cout<<"\t*****************************************************************"<<endl;;
	cout<<"\t* GD-C1000   multiRF wacth and measure system]"<<endl;
	cout<<"\t* Version:"<<VSCTTB_VERSION<<endl;
	cout<<"\t*\t"<<VSCTTB_URL<<endl;
	cout<<"\t*\t"<<VSCTTB_COPYRIGHT<<endl;
	cout<<"\t*****************************************************************"<<endl<<endl;
}

void PrintUsage(void)
{
	cout<<"\t|-----------------------------------------|"<<endl;
	cout<<"\t| Usage: GD-C1000 help						"<<endl;
	cout<<"\t|\trun:\tstart work process				"<<endl;
	cout<<"\t|\tstop:\tstop work process				"<<endl;
	cout<<"\t|\t>:\tdisplay the cmd line				"<<endl;
	cout<<"\t|\thelp:\tPrint The Help Info				"<<endl;
	cout<<"\t|\tquit:\tExit The System					"<<endl;
	cout<<"\t|-----------------------------------------|"<<endl;
}

void LoadXMLTask(void)
{
	
	
	std::string strVecInfo;
	int nLastNum=0, nCurNum=0;
	std::vector<std::string>  vctXML;

	bool rnt1 = DBMANAGER::instance()->QueryTask(CTTB,vctXML);
	nLastNum = nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 CTTB 任务 : ";
	strVecInfo += StrUtil::Int2Str( nLastNum );
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt2 = DBMANAGER::instance()->QueryTask(DVBC,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 DVBC 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt3 = DBMANAGER::instance()->QueryTask(RADIO,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 RADIO 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt4 = DBMANAGER::instance()->QueryTask(ATV,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 ATV 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt5 = DBMANAGER::instance()->QueryTask(AM,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 AM 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt6 = DBMANAGER::instance()->QueryTask(DVBS,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 DVBS 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);

	bool rnt7 = DBMANAGER::instance()->QueryTask(CTV,vctXML);
	nCurNum = vctXML.size();
	strVecInfo = "从数据库中获取到 CTV 任务 : ";
	strVecInfo += StrUtil::Int2Str( nCurNum-nLastNum );
	nLastNum = nCurNum;
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strVecInfo, LOG_OUTPUT_FILE);


	if (rnt1==false||rnt2==false||rnt3==false||rnt4==false||rnt5==false||rnt6==false||rnt7==false )
	{
		OSFunction::ExitProcess("系统初始化(数据库任务)失败");
	}
	//
	for (size_t i = 0;i<vctXML.size();i++)
	{
		std::string strXML = vctXML.at(i);
		ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
		memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
		mbXML->wr_ptr(strXML.length());

		if (mbXML != NULL)
		{
			ACE_Time_Value OutTime(ACE_OS::time(0)+1);
			COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mbXML,&OutTime);
			OSFunction::Sleep(0,500);
		}
	}

	//**************************************************
	if(g_sEncryptInfo.size()>0)
	{
		g_sEncryptInfo.clear();
	}
	string strXML = "";
	//读取xml文件中的内容
	ifstream xmlFile("C:/vscttb/ICInfoChannelEncryptQuery_LastReport.xml");
	string strTemp = "";
	while (getline(xmlFile,strTemp))
	{
		strXML += strTemp;
	}
	xmlFile.close();
	//
	if(strXML.size()>0)
	{
		XmlParser parser(strXML.c_str());
		pXMLNODE rootNode=parser.GetRootNode();//根节点msg
		cout<<"初始化ICInfoChannelEncryptQuery_LastReport V26"<<endl;
		pXMLNODE queryTaskNode=parser.GetNodeFromPath("Msg/ReturnInfo/ICInfoChannelEncryptQuery");
		if(queryTaskNode == NULL)
		{
			cout<<"初始化ICInfoChannelEncryptQuery_LastReport V26 失败"<<endl;
			cout<<"初始化ICInfoChannelEncryptQuery_LastReport V30"<<endl;
			queryTaskNode=parser.GetNodeFromPath("Msg/ICCardEncryptQueryResult");
			if(queryTaskNode == NULL)
			{
				cout<<"初始化ICInfoChannelEncryptQuery_LastReport V30 失败"<<endl;
			}
			else
			{
				cout<<"初始化ICInfoChannelEncryptQuery_LastReport V30 成功"<<endl;
			}
		}
		else
		{
				cout<<"初始化ICInfoChannelEncryptQuery_LastReport V26 成功"<<endl;
		}
		int nICInfoCount = parser.GetChildCount(queryTaskNode);
		pXMLNODELIST infoNodeList = parser.GetNodeList(queryTaskNode);
		for (int nIndex=0; nIndex<nICInfoCount; nIndex++)
		{
			pXMLNODE ICInfoNode = parser.GetNextNode(infoNodeList);
			pXMLNODELIST freqList = parser.GetNodeList(ICInfoNode);
			int freqCount = parser.GetChildCount(ICInfoNode);
			for (int i=0; i!=freqCount; i++)
			{
				pXMLNODE freqNode = parser.GetNextNode(freqList);
				sEncryptInfo EncI;
				string Encryption;
				parser.GetAttrNode(freqNode,"Freq",EncI.freq);
				parser.GetAttrNode(freqNode,"ServiceID",EncI.serviceid);
				parser.GetAttrNode(freqNode,"ProgramID",EncI.programid);
				parser.GetAttrNode(freqNode,"Encrypt",Encryption);
				if(Encryption == "1")//小卡查询和频道扫描的相反
				{
					EncI.Encrypt = "0";
				}
				else
				{
					EncI.Encrypt = "1";
				}
				g_sEncryptInfo.push_back(EncI);
			}
		}
	}
	return;
}

void CheckSoleProcess()
{
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, "VSDVBUNIQUENESS");
	if (GetLastError() == ERROR_ALREADY_EXISTS)// 检查错误代码
	{
		CloseHandle(m_hMutex);// 如果已有互斥量存在则释放句柄并复位互斥量
		m_hMutex = NULL;

		OSFunction::ExitProcess("系统已经运行");
	}
	return;
}


void CheckDirectoryExsit()
{
	std::vector<std::string> vecVscttbDir;

	/* xml接收文件夹 */
	vecVscttbDir.push_back( "C:/SMSDown/" );
	vecVscttbDir.push_back( "C:/CASDown/" );
	vecVscttbDir.push_back( "C:/MonDown/" );
	vecVscttbDir.push_back( "C:/DVBTHDown/" );
	vecVscttbDir.push_back( "C:/TVMonDown/" );
	vecVscttbDir.push_back( "C:/RadioDown/" );
	vecVscttbDir.push_back( "C:/AMDown/" );
	vecVscttbDir.push_back( "C:/CommonDown/" );
	vecVscttbDir.push_back( "C:/SatDown/" );
	vecVscttbDir.push_back( "C:/3DDown/" );
	vecVscttbDir.push_back( "C:/SMSUp/" );
	vecVscttbDir.push_back( "C:/SMS/" );
	vecVscttbDir.push_back( "C:/RecordList/" );
	
	/* 临时文件夹 */
	vecVscttbDir.push_back( "C:/vscttb/temp/" );

	/* 日志文件夹 */
	std::vector<std::string> logvelc;
	std::string logPath = PROPMANAGER::instance()->GetLogPath();
	logPath =PROPMANAGER::instance()->GetPath(logPath,logvelc);//处理多级目录
	
	for (int i=0; i<logvelc.size(); i++)
	{
		vecVscttbDir.push_back( logvelc[i] );
	}

	vecVscttbDir.push_back( logPath );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogOther()+"/" );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogRecord()+"/" );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogAnalyser()+"/" );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogDevice()+"/" );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogVideo()+"/" );
	vecVscttbDir.push_back( logPath+PROPMANAGER::instance()->GetLogDefault()+"/" );
	vecVscttbDir.push_back( logPath+"task/");			//临时添加对于task的日志
	vecVscttbDir.push_back( logPath+"database/");		//临时添加对于database的日志

	/* sms cas */
	std::string dir = "";
	PROPMANAGER::instance()->GetShareDir( dir );
	vecVscttbDir.push_back( dir );
	
	std::vector<std::string> smsVector, casVector,siVector;
	std::string smsdir="";
	PROPMANAGER::instance()->GetSmsDir( smsdir );
	
	smsdir =PROPMANAGER::instance()->GetPath(smsdir,smsVector);
	
	for (int i=0; i<smsVector.size(); i++)
	{
		vecVscttbDir.push_back( smsVector[i] );
	}
	vecVscttbDir.push_back( smsdir );
	
	std::string casdir="";
	PROPMANAGER::instance()->GetCasDir( casdir );
	
	casdir =PROPMANAGER::instance()->GetPath(casdir,casVector);
	
	for (int i=0; i<casVector.size(); i++)
	{
		vecVscttbDir.push_back( casVector[i] );
	}
	vecVscttbDir.push_back( casdir );

	std::string sidir="";
	PROPMANAGER::instance()->GetSIDir( sidir );

	sidir =PROPMANAGER::instance()->GetPath(sidir,siVector);

	for (int i=0; i<siVector.size(); i++)
	{
		vecVscttbDir.push_back( siVector[i] );
	}
	vecVscttbDir.push_back( sidir );

	/* record path */
	std::vector<std::string> vecLoc;
	PROPMANAGER::instance()->GetRecFileLocVec( vecLoc );
	for (int i=0; i<vecLoc.size(); i++)
	{
		vecVscttbDir.push_back( vecLoc[i] );
	}


	/* 依据监测类型而有所不同的 */
	vecVscttbDir.push_back( "C:/table/" );
	for (int type=1; type<=9; type++)
	{
		/* table path */
		std::string tablePath = "";
		PROPMANAGER::instance()->GetXmlTablePath((enumDVBType)type, tablePath);
		if (tablePath != "")
		{
			vecVscttbDir.push_back( tablePath );
		}
	}
	//
	vecVscttbDir.push_back( "C:/table/fm" );
	vecVscttbDir.push_back( "C:/table/am" );
	//

	/* 检测这些文件夹是否已经被创建，没有，就创建它 */
	for (int dirIndex=0; dirIndex<vecVscttbDir.size(); dirIndex++)
	{
		if ( !CreateDirectory(vecVscttbDir[dirIndex].c_str(), NULL) )
		{
			//std::cout<<"文件夹 ["<<vecVscttbDir[dirIndex]<<"] 已经存在,不用创建"<<std::endl;
		}
		else
			std::cout<<"文件夹 ["<<vecVscttbDir[dirIndex]<<"] 创建成功"<<std::endl;
	}
	PROPMANAGER::instance()->CreatPathTS();
	return;
}