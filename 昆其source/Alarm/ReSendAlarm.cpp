
#include "ReSendAlarm.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Communications/XMLSend.h"
#include "../Foundation/AppLog.h"
#include "ace/OS.h"
#include "fstream"

ReSendAlarm::ReSendAlarm()
{

}

ReSendAlarm::~ReSendAlarm()
{

}

int ReSendAlarm::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 报警检查/发送任务线程开始执行 !\n"));
	this->activate();
	return 0;
}

//通过从xml中解析出来的不同类型发送到不同的监测类型报警服务器
int ReSendAlarm::svc()
{
	bFlag = true;
	while (bFlag)
	{
		try 
		{
			
			ProcessMessage();
			OSFunction::Sleep(0,200);
		}
		catch(...)
		{
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 报警检查/发送任务线程停止执行 !\n"));

	return 0;
}

int ReSendAlarm::ProcessMessage()
{
	string dirs[]={"C:/AlarmUp/DVBS/","C:\\AlarmUp\\DVBC\\","C:\\AlarmUp\\CTTB\\","C:\\AlarmUp\\ATV\\","C:\\AlarmUp\\RADIO\\",\
		"C:\\AlarmUp\\AM\\"};

	//扫描目录
	string Path="C:/AlarmUp/";
	string TempPath = Path + string("*.*");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//先提取NIT信息
	hFind = FindFirstFile(TempPath.c_str(), &FindFileData);

	while (bFlag&&(hFind != INVALID_HANDLE_VALUE))
	{
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				string dvbtype= string(FindFileData.cFileName);  //文件夹名为频点

				string FindName=Path+dvbtype+string("/")+string("*.xml");
				string DirName = Path+dvbtype+string("/");
				HANDLE hFile;
				WIN32_FIND_DATA  wf;
				hFile = FindFirstFile(FindName.c_str(), &wf);//查找文件
				if (hFile == INVALID_HANDLE_VALUE)
				{
					if (!FindNextFile(hFind, &FindFileData))
					{
						break;
					}
					continue;
				}
				do
				{
					string strXML = "";
					char xml_filename[300];
					strcpy(xml_filename,DirName.c_str()) ;
					strcat(xml_filename,wf.cFileName);
					DWORD dwAttrs = GetFileAttributes(wf.cFileName); 

					if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) //如果文件属性只读，改为正常
					{ 
						SetFileAttributes(xml_filename, FILE_ATTRIBUTE_NORMAL); 
					} 

					//读取xml文件中的内容
					ifstream xmlFile(xml_filename);
					string strTemp = "";
					while (getline(xmlFile,strTemp))
					{
						strXML += strTemp;
					}
					xmlFile.close();
					UpAlarmInfo alarminfo;
					PROPMANAGER::instance()->GetUpAlarmInfo(OSFunction::GetEnumDVBType(dvbtype),alarminfo);
					if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
					{
						if (strXML.find("BreakReportQuery")!= -1)
						{
							alarminfo.alarmurl = PROPMANAGER::instance()->GetAgentInfoSet("13");
						}
						else
						{
							alarminfo.alarmurl = PROPMANAGER::instance()->GetAgentInfoSet("12");
						}
					}
					if ((strXML.empty()) || (alarminfo.alarmurl.empty()))
					{
					}
					else
					{
						if(false == XMLSend::SendXML(alarminfo.alarmurl,strXML))
						{
							if(!FindNextFile(hFile, &wf))
							{
								break;
							}
							Sleep(10000);
							continue;
						}
					}
					DeleteFile(xml_filename);//删除文件 
				}while(bFlag&&(FindNextFile(hFile, &wf)));
				FindClose(hFile);	
			}

		}
		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);
	return 0;
}

int ReSendAlarm::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
