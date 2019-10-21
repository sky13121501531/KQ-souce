
#include "DeviceManager.h"
#include "OS_Environment.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "ace/OS.h"
string m_time,Action,strURL;
DeviceManager::DeviceManager()
{
	bFlag = false;
	meCheckType = NOTHING;
	ManageWeekday = "2";				//Ĭ��Ϊÿ�����ڶ�
	ManageSingledatetime = "2010-05-01 00:00:00";		//Ĭ��...
	ManageTime = "00:00:00";
}
DeviceManager::~DeviceManager()
{

}
int DeviceManager::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%D | %t) �豸����״̬�����߳̿�ʼִ�� !\n"));
	this->activate();
	return 0;
}

int DeviceManager::svc()
{
	bFlag = true;	
	SetNextRunTime();	//�����´�����ʱ��

	while (bFlag)
	{
		time_t currentTime = time(0);//��ǰʱ��
		std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);

		if (abs(TimeUtil::DiffSecond(strCurTime,m_time)) < 3)//������ʱ�����²�����3s
		{	
			//���浽�ط�xmlĿ¼
			string strxml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?>\
																				  <Msg Version=\"3.0\"\
																				  MsgID=\"2\"\
																				  Type=\"MonUp\"\
																				  DateTime=\"2017-05-17 15:30:00\"\
																				  SrcCode=\"110000D01\"\
																				  DstCode=\"110000G01\"\
																				  ReplyID=\"1000_ID\">\
																				  <Return Type=\"RebootSet\" Value=\"2\" Desc=\"�ɹ�\"/>\
																				  <Reboot ReStartTime=\"2017-05-17 15:30:00\"/>\
																				  </Msg>";
			string strAction,strRTime;
			XmlParser parser(strxml.c_str());
			pXMLNODE TypeNode=parser.GetNodeFromPath("Msg/Return");
			parser.GetAttrNode(TypeNode,"Action",strAction);
			strAction = Action;
			pXMLNODE RTimeNode=parser.GetNodeFromPath("Msg/Return/Reboot");
			parser.GetAttrNode(RTimeNode,"Action",strRTime);
			strRTime = TimeUtil::GetCurDateTime();
			parser.SetAttrNode("Type",strAction,TypeNode);
			parser.SetAttrNode("ReStartTime",strRTime,RTimeNode);
			string ParentDir="C:/AlarmUp/";

			if(ACE_OS::opendir(ParentDir.c_str())==NULL)   
			{
				mkdir(ParentDir.c_str());
			} 
			string dir=ParentDir+string("DVBC/"); 
			if(ACE_OS::opendir(dir.c_str())==NULL)   
			{
				mkdir(dir.c_str());
			} 
			string FileName=dir+strAction+string(".xml");
			parser.SaveAsFile(FileName.c_str());

			if(Action=="Restart")
			{
				RebootDevice();
			}
			else if(Action=="Reset")
			{
				string killcmd="taskkill -im vscttb.exe ";
				system(killcmd.c_str());
			}
			else if(Action=="Upgrade")
			{
				if(UpVSCTTB(strURL))
				{
					DWORD Monitorpid =GetProcessIdByName(MONITOR);
					if(Monitorpid != 0)
					{
						DWORD Vscttbpid =GetProcessIdByName(VSCTTB);
						HANDLE token1 = OpenProcess(PROCESS_ALL_ACCESS,FALSE,Vscttbpid);
						if(token1)
						{
							//�رս���  �ɹ���������
							TerminateProcess(token1, 0);
						}
					}
					else
					{
						DWORD Vscttbpid =GetProcessIdByName(VSCTTB);
						if(Vscttbpid!=0)
						{
							HANDLE token1 = OpenProcess(PROCESS_ALL_ACCESS,FALSE,Vscttbpid);
							if(token1)
							{
								//�رս���
								if(TerminateProcess(token1, 0))
								{
									Sleep(1000);
									WinExec(VSCTTBPATH,SW_SHOW);
								}
							}
						}
						else
						{
							WinExec(VSCTTBPATH,SW_SHOW);
						}
					}
				}
			}
			SetNextRunTime();				//�����´�����ʱ��
		}
		Sleep(500);				// ÿ���Ӽ��һ��
	}

	ACE_DEBUG ((LM_DEBUG,"(%D | %t) �豸����״̬�����߳�ִֹͣ�� !\n"));

	return 0;
}

int DeviceManager::Start()
{
	open(0);
	return 0;
};
int DeviceManager::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
bool DeviceManager::RebootDevice()
{
	std::string cmd="shutdown -r -t 0";
	system(cmd.c_str());
	return true;
}
void DeviceManager::SetStatus(int Type,std::string action,std::string strTime,std::string strurl)
{
	Action = action;
	strURL = strurl;
	if(Type==-1)
	{
		meCheckType = PERSINGLE;
		this->ManageSingledatetime=strTime;
	}
	else if(Type>=0)
	{
		meCheckType = PERWEEK;
		ManageWeekday = StrUtil::Int2Str(Type);
		ManageTime    = strTime;
	}
	SetNextRunTime();
}

bool DeviceManager::SetNextRunTime()
{
	if (meCheckType == PERSINGLE)		//����
	{
		mNextRunTime = ManageSingledatetime;
		m_time = ManageSingledatetime;
	}
	else if (meCheckType == PERWEEK)	//ÿ����
	{
		time_t currentTime = time(0);		//��ǰʱ��
		std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
		long CurWeekday = TimeUtil::DateIsWeekDay(strCurTime);						//��ȡ��ǰ����
		long diffday = StrUtil::Str2Long(ManageWeekday) - CurWeekday;				//���ڵĲ�ֵ

		std::string strCurDate = TimeUtil::CalDay(TimeUtil::GetCurDate(),diffday);
		mNextRunTime = TimeUtil::GetDateFromDatetime(strCurDate) + std::string(" ") + ManageTime;					//������Ӧ�ü���ʱ��
		if (TimeUtil::DiffSecond(mNextRunTime,strCurTime) < 0)						//�����ڼ��ʱ����ڵ�ǰʱ��
		{
			mNextRunTime = TimeUtil::CalDay(mNextRunTime,7);										//�´μ��ʱ��˳��Ϊ������
		}
	}
	return true;
}

bool DeviceManager::UpVSCTTB(string strurl)
{
	string netusestr = "net use * /del /y";
	system(netusestr.c_str());
	string usename="",passwd="";
	usename = PROPMANAGER::instance()->GetuserName();
	passwd = PROPMANAGER::instance()->Getpassed();
	bool ret = true;
	string oldvscttb = "c:\\vscttb\\GD-C1000.exe";//�ļ�·��
	string newvscttb= "c:\\vscttb\\VSCTTBold.exe";
	if(!_access(newvscttb.c_str(),0))
	{
		DeleteFile(newvscttb.c_str());
	}
	//����ӳ�乲����
	int pos = strurl.rfind("\\");
	string cmdshare = strurl.substr(0,pos);
	string EXEname = strurl.substr(++pos,strurl.length());
	int sharepos = cmdshare.rfind("\\");
	string shareDisk = cmdshare.substr(++sharepos,cmdshare.length());
	int ipPos = cmdshare.rfind("\\");
	string cmdIP = cmdshare.substr(0,ipPos);
	string ip = cmdIP.substr(2,cmdIP.length());
	string strName = strurl.substr(pos,strurl.length()-pos);
	if(!_access(oldvscttb.c_str(),0))//����ļ�����:
	{
		if(!rename(oldvscttb.c_str(),newvscttb.c_str()))
		{
			string CreateShareDiskOrder = "net use z: " + cmdshare + " \"" +  passwd +  string("\"") + " "+ "/user:" + string("\"") + usename + string("\"");
			int status = system(CreateShareDiskOrder.c_str());
			if (0 == WEXITSTATUS(status))
			{
				string strCopy = "copy ";
				strCopy += "z:";
				strCopy += "\\";
				strCopy += EXEname;
				strCopy += " ";
				strCopy += "c:\\vscttb";
				int mstatus = system(strCopy.c_str());
				cout<<"�����ɹ�"<<endl;
				string netusestr = "net use * /del /y";
				system(netusestr.c_str());
				return true;
				/*if(0 == WEXITSTATUS(mstatus))
				{
					
				}
				else
				{
					string netusestr = "net use * /del /y";
					system(netusestr.c_str());
					return false;
				}*/
			}
			else
			{
				string netusestr = "net use * /del /y";
				system(netusestr.c_str());
				return false;
			}
		}
	}
	else
		return false;
}
int DeviceManager::GetProcessIdByName(string szProcessName)
{
	// ��������ʼ��
	DWORD lpPID;
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	ZeroMemory(&st, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	st.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ps,sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);
	// ��������
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if(!Process32First(hSnapshot,&ps))
	{
		return FALSE;
	}
	do
	{
		// �ȽϽ�����
		if(lstrcmpi(ps.szExeFile,szProcessName.c_str())==0)
		{
			// �ҵ���
			lpPID = ps.th32ProcessID;
			CloseHandle(hSnapshot);
			return lpPID;
		}
	}
	while(Process32Next(hSnapshot,&ps));
	//û���ҵ�
	CloseHandle(hSnapshot);
	return 0;
}