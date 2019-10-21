#pragma warning(disable:4996)
#include "WPLFile.h"
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TypeDef.h"
WPLFile::WPLFile(void)
{
}
WPLFile::~WPLFile(void)
{
}
bool WPLFile::CreateWPLFile(std::string fileName, std::vector<std::string> vecURL )
{
	try
	{
		std::string temp;
		temp = "<?wpl version=\"1.0\"?>\n<smil>\n<body>\n<seq>\n";//wpl文件头

		for(size_t i = 0 ; i < vecURL.size(); i++)//文件播放列表地址
		{
			temp += "<media src= \""+vecURL.at(i)+"\"/>\n";

		}
		temp += "</seq>\n</body>\n</smil>\n";//wpl文件尾
		//写入文件
		FILE* pFile=fopen(fileName.c_str(),"a+b");//创建文件
		if (pFile)
		{
			size_t count=0;
			size_t strlength=0;
			//循环写入，防止数据量大一次写入不能完成
			while (strlength<temp.length())
			{
				count=fwrite(temp.c_str(),sizeof(char),temp.length(),pFile);
				strlength+=count;
			}
			fclose(pFile);
			return true;
		}
		else
		{
			string msg = string("WPL文件创建失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return false;
		}
		
	}
	catch (...) {
		return false;
	}
	return true;
}

bool WPLFile::CreateVLCFile( std::string fileName,std::vector<std::string> vecURL )
{
	try
	{
		std::string temp;
		temp = "#EXTM3U\r\n";//a3u文件头

		for(size_t i = 0 ; i < vecURL.size(); i++)//文件播放列表地址
		{
			temp +=vecURL.at(i)+"\r\n";
		}
		//写入文件
		FILE* pFile=fopen(fileName.c_str(),"a+b");//创建文件
		if (pFile)
		{
			size_t count=0;
			size_t strlength=0;
			//循环写入，防止数据量大一次写入不能完成
			while (strlength<temp.length())
			{
				count=fwrite(temp.c_str(),sizeof(char),temp.length(),pFile);
				strlength+=count;
			}
			fclose(pFile);
			return true;
		}
		else
		{
			string msg = string("a3u文件创建失败");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return false;
		}

	}
	catch (...) {
		return false;
	}
	return true;
}

std::string WPLFile::FileSplit( string filename,float precent,bool ishead,bool isAudio )
{
	FILE *file = NULL;
	FILE* retfile =NULL;
	long fileAllSize = 0;
	long fileSize = 0;
	
	static int FimeIndex = 0;

	FimeIndex == 999 ? FimeIndex = 0 : ++FimeIndex;

	string RetFileName = PROPMANAGER::instance()->GetTempFileLoac() + TimeUtil::DateTimeToString(time(0)) + StrUtil::Int2Str(FimeIndex);

	RetFileName=RetFileName + ".ts";

	
	if(precent>1||precent<0)
	{
		precent=1;
		return filename;
	}
	file = fopen(filename.c_str(),"rb");
	if(file  != NULL)
	{
		fseek(file,0,SEEK_END);
		fileAllSize = ftell(file);   //取得文件的大小
		unsigned char ch[1024];
		memset(ch,NULL,1024);

		retfile = fopen(RetFileName.c_str(),"wb");

		long fileSize1 = fileAllSize;//截取位置
		fileSize1 =(long) fileAllSize*precent;//计算截取位置
		if(retfile!=NULL)
		{
			bool tshead=false;
			if(ishead)//截头留尾
			{
				fseek(file,fileSize1,SEEK_SET);//指针移动到要拆分的位置
				long len = fread(ch,1,1024,file);
				while(len>0)
				{
					if (!isAudio&&!tshead)
					{
						int num=0;
						while (num<=len&&num+188<len)
						{
							if (ch[num]==0x47&&ch[num+188]==0x47)
							{
								tshead=true;
								break;
							}
							num++;
						}
						fwrite(&ch[num],1,len-num,retfile);
					}
					else
					{
						fwrite(ch,1,len,retfile);
					}
					len = fread(ch,1,1024,file);
				}
			}
			else//截尾留头
			{
				long tempsize = 0; 
				fseek(file,0,SEEK_SET);//指针移动到文件开始的位置
				long len = fread(ch,1,1024,file);
				while((len>0)/*&&(tempsize<fileSize1)*/)
				{
					if (!isAudio&&!tshead)
					{
						int num=0;
						while (num<=len&&num+188<len)
						{
							if (ch[num]==0x47&&ch[num+188]==0x47)
							{
								tshead=true;
								break;
							}
							num++;
						}
						fwrite(&ch[num],1,len-num,retfile);
					}
					else
					{
						fwrite(ch,1,len,retfile);
					}
					int ilastlen = fileSize1 - tempsize;
					if (ilastlen <= 0)
					{
						len = 0;
						break;
					}
					else if (ilastlen > 1024)
						len = fread(ch,1,1024,file);
					else 
						len = fread(ch, 1, ilastlen, file);
					tempsize+=len;
				}
			}
			fclose(retfile);    //释放资源*/
		}
		fclose(file);    //释放资源*/
	}

	//DBMANAGER::instance()->AddTempFile(RetFileName,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
	
	return RetFileName;
}

std::string WPLFile::FileSplit( string filename, float StartPrecent, float EndPrecent, bool isAudio)
{
	FILE *file = NULL;
	FILE* retfile =NULL;
	long fileAllSize = 0;
	
	static int FimeIndex = 0;
	FimeIndex == 999 ? FimeIndex = 0 : ++FimeIndex;
	string RetFileName = PROPMANAGER::instance()->GetTempFileLoac() + TimeUtil::DateTimeToString(time(0)) + StrUtil::Int2Str(FimeIndex);
	RetFileName=RetFileName + ".ts";

	if(StartPrecent>1||StartPrecent<0||EndPrecent>1||EndPrecent<0)
	{
		return filename;
	}

	file = fopen(filename.c_str(),"rb");
	if(file != NULL)
	{
		fseek(file,0,SEEK_END);
		fileAllSize = ftell(file);   //取得文件的大小
		unsigned char ch[1024];
		memset(ch,NULL,1024);

		retfile = fopen(RetFileName.c_str(),"wb");
		long lStartPos = fileAllSize;//截取文件开始位置
		long lEndPos = fileAllSize;//截取文件结束位置
		lStartPos =(long) fileAllSize*StartPrecent;//计算截取位置
		lEndPos =(long) fileAllSize*EndPrecent;//计算截取位置
		if (retfile!=NULL)
		{
			bool tshead=false;
			long tempsize = lStartPos; //记录文件指针的位置
			fseek(file,lStartPos,SEEK_SET);//指针移动到要拆分的开始位置
			long len = fread(ch,1,1024,file);
			while(len>0)
			{
				if (!isAudio&&!tshead)
				{
					int num=0;
					while (num<=len&&num+188<len)
					{
						if (ch[num]==0x47&&ch[num+188]==0x47)
						{
							tshead=true;
							break;
						}
						num++;
					}
					fwrite(&ch[num],1,len-num,retfile);
				}
				else
				{
					fwrite(ch,1,len,retfile);
				}
				int ilastlen = lEndPos - tempsize;
				if (ilastlen <= 0)
				{
					len = 0;
					break;
				}
				else if (ilastlen > 1024)
					len = fread(ch, 1, 1024, file);
				else 
					len = fread(ch, 1, ilastlen, file);
				tempsize+=len;
			}
			fclose(retfile);    //释放资源*/
		}
		fclose(file);    //释放资源*/
	}
	return RetFileName;
}

std::string WPLFile::TimeFileSplit( string filename,time_t starttime,time_t endtime,int mode )
{
	FILE *file = NULL;
	FILE* retfile =NULL;
	const int BufLen=2048*188;
	long fileAllSize = 0;
	long fileSize = 0;
	unsigned long startlen=0,endlen=0,startlen1=0,endlen1=0;
	time_t curtime=0,cardtime=0,servertime=0;

	bool devicetimeFlag=true; //如果没有板块时间戳 则标识为true,有板块时间戳为false
	bool  startflag=true,startflag1=true;
	bool  endflag=true,endflag1=true;
	static int FimeIndex = 0;
	unsigned long filelen=0;
	FimeIndex > 999 ? FimeIndex = 0 : ++FimeIndex;

	string RetFileName = PROPMANAGER::instance()->GetTempFileLoac() + TimeUtil::DateTimeToString(starttime) + StrUtil::Int2Str(FimeIndex);
	RetFileName=RetFileName + ".ts";

	file = fopen(filename.c_str(),"rb");
	if(file  != NULL)
	{
		unsigned long lensize=0;
		fseek(file,0,SEEK_END);
		fileAllSize=ftell(file);
		
		endlen=fileAllSize;
		while(lensize+188<fileAllSize)
		{		
			unsigned char ch[BufLen];
			memset(ch,0,sizeof(ch));
			fseek(file,lensize,SEEK_SET);
			unsigned long len = fread(ch,1,BufLen,file);
			unsigned int num=0;
			while (num<=len&&num+188<=len)
			{
				if (ch[num]==0x47&&ch[num+188]==0x47)
				{
					break;
				}
				num++;
			}
			lensize+=num;
			while(num+188<len)
			{
				if(devicetimeFlag&&ch[num+2]==0x1A)
				{
					servertime=GetTimeFromStream(&ch[num]);
				}
				else if(((ch[num+1]&0x1f)+ch[num+2])==0x0 && ch[num+4]==0xf)
				{
					char stime[15]={0};
					memcpy(stime,&ch[num+6],14);
					string strtime = stime;
					cardtime = TimeUtil::StrTimeToDateTime(strtime);
					devicetimeFlag=false;
				}   			
				if(endflag&&servertime>endtime)
				{
					endlen  = lensize;
					endflag = false;
				}
				else if (startflag&&servertime>=starttime)
				{
					startlen  = lensize;
					startflag = false;
				}

				if(endflag1&&cardtime>endtime)
				{
					endlen1  = lensize;
					endflag1 = false;
				}
				else if (startflag1&&cardtime>=starttime)
				{
					startlen1  = lensize;
					startflag1 = false;
				}
				num+=188;
				lensize+=188;				
			}
		}

		if(endflag)
			endlen  = fileAllSize;
		if(endflag1)
			endlen1 = fileAllSize;
		if(!devicetimeFlag)
		{
			if(!startflag1)
				startlen  = startlen1;	
			if(!endflag1)
				endlen    = endlen1;		
			startflag = startflag1;
			endflag   = endflag1;
		}
		if(mode==0)
			startlen=0;
		else if(mode==1)
			endlen=fileAllSize;
		
		retfile = fopen(RetFileName.c_str(),"wb");
		if(retfile!=NULL)
		{
			if(startlen>=0)
			{
				unsigned long sizelen=startlen;
				fseek(file,startlen,SEEK_SET);
				while(sizelen+188<=endlen)
				{
					unsigned char ch[BufLen];
					memset(ch,0,sizeof(ch));	
					unsigned long len = fread(ch,1,BufLen,file);
					if(sizelen+len>endlen)
					{
						fwrite(ch,1,endlen-sizelen,retfile);
						filelen+=endlen-sizelen;
						break;
					}
					fwrite(ch,1,len,retfile);
					sizelen+=len;
					filelen+=len;
				}
			}
			fclose(retfile);
		}
		fclose(file);
	}
		
	if((endflag&&startflag)||filelen==0)
		return "";
	return RetFileName;

}
time_t WPLFile::GetTimeFromStream(unsigned char *timebuf)
{
	time_t curtime=(timebuf[4]<<56)+(timebuf[5]<<48)+(timebuf[6]<<40)+(timebuf[7]<<32)+(timebuf[8]<<24)+(timebuf[9]<<16)+(timebuf[10]<<8)+timebuf[11];
	return curtime;
}

bool WPLFile::CreateVLCFileM3U8(std::string fileName,std::vector<std::string> vecURL,std::string strTimestr)
{
	try
	{
#if 1
		std::string temp,strHead,strHeadRecordMaxTime,strRecordTime;
		//string RecordTime = StrUtil::Float2Str(StrUtil::Str2Float(PROPMANAGER::instance()->GetRecordPeriod())*61);
		GetStrValue(strTimestr,strHead,strHeadRecordMaxTime);
		temp = "#EXTM3U\r\n";//m3u8文件头
		temp += "#EXT-X-VERSION:3\r\n";
		temp += "#EXT-X-PLAYLIST-TYPE:VOD\r\n";
		temp += "#EXT-X-MEDIA-SEQUENCE:0\r\n";
		//temp += "#EXT-X-ALLOW-CACHE:YES\r\n";
		temp += "#EXT-X-TARGETDURATION:";
		temp += strHeadRecordMaxTime;
		temp += "\r\n";
		string strRecordstr = "";
		for(size_t i = 0 ; i < vecURL.size();)//文件播放列表地址
		{
			GetStrValue(strTimestr,strRecordstr,strRecordTime);
			if(strRecordstr=="&" && strRecordTime=="0")
			{
				if(i>0)
				{
					temp += "#EXT-X-DISCONTINUITY\r\n";
				}
				continue;
			}
			if(vecURL[i].find(strRecordstr)!=-1)
			{
				temp += "#EXTINF:";
				temp += strRecordTime;
				temp += ",\r\n";
				temp +=vecURL.at(i)+"\r\n";
			}
			i++;
		}
		temp += "#EXT-X-ENDLIST\r\n";
		//写入文件
		FILE* pFile=fopen(fileName.c_str(),"a+b");//创建文件
		if (pFile)
		{
			size_t count=0;
			size_t strlength=0;
			//循环写入，防止数据量大一次写入不能完成
			while (strlength<temp.length())
			{
				count=fwrite(temp.c_str(),sizeof(char),temp.length(),pFile);
				strlength+=count;
			}
			fclose(pFile);
			return true;
		}
		else
		{
			string msg = string("m3u8文件创建失败");
			return false;
		}
#else
		FILE* pFile=fopen(fileName.c_str(),"r+b");//创建文件
		if(!pFile)
		{
			return false;
		}
		else
		{
			std::string tempAddr,strHead,strHeadRecordMaxTime,strRecordTime;
			GetStrValue(strTimestr,strHead,strHeadRecordMaxTime);

			int urlpos = vecURL[0].rfind("/");
			tempAddr = vecURL[0].substr(0,++urlpos);

			char pStrFile[1024] = {0};
			int pos = 0;
			string StrFile = "";
			while(1)
			{
				fgets(pStrFile,1024,pFile);
				string str = pStrFile;
				if(str.find("#EXT-X-TARGETDURATION")!=-1)
				{
					str = string("#EXT-X-TARGETDURATION:") + strHeadRecordMaxTime + string("\r\n");
				}
				pos = str.find("#EXT");
				if(pos==-1)
				{
					str = tempAddr + str;
				}
				if(str.find("#EXT-X-ENDLIST")!=-1)
				{
					break;
				}
				StrFile  += str;
				//StrFile +="\r\n";
			}
			if(StrFile!="")
			{
				string strRecordstr = "";
				for(size_t i = 0 ; i < vecURL.size(); i++)//文件播放列表地址
				{
					GetStrValue(strTimestr,strRecordstr,strRecordTime);
					if(i==0)
					{
						continue;
					}
					if(vecURL[i].find(strRecordstr)!=-1)
					{
						StrFile += "#EXTINF:";
						StrFile += strRecordTime;
						StrFile += ",\r\n";
						StrFile +=vecURL.at(i)+"\r\n";
					}
				}
				StrFile += "#EXT-X-ENDLIST";
				fclose(pFile);
			}
			//写入文件
			FILE* File=fopen(fileName.c_str(),"w+b");//创建文件
			if(File)
			{
				size_t count=0;
				size_t strlength=0;
				//循环写入，防止数据量大一次写入不能完成
				while (strlength<StrFile.length())
				{
					count=fwrite(StrFile.c_str(),sizeof(char),StrFile.length(),File);
					strlength+=count;
				}
				fclose(File);
				return true;
			}
		}
#endif
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void WPLFile::GetStrValue(std::string &str,std::string &strRecord,std::string &strRecordTime )
{
    int pos = str.find(",");
    strRecord = str.substr(0,pos);
    str = str.substr(++pos,str.length());
    int mpos = str.find(";");
    strRecordTime = str.substr(0,mpos);
    str = str.substr(++mpos,str.length());
}
bool WPLFile::CreatVLCTsFile(std::string fileName,std::string sFiletime,std::string HeadTS)
{
	try
	{
		std::string path = PROPMANAGER::instance()->GetTempFileLoac();
		std::string command=std::string("c:\\vscttb\\ffmpeg.exe -i ")+HeadTS+std::string(" -c copy -map 0 -f segment -segment_list ")+fileName+std::string(" -segment_time ") + sFiletime+ std::string(" ")+ path + string("m3u8%3d.ts");
		int status = system(command.c_str());
		if (0 == WEXITSTATUS(status))
		{
			return true;
		}
	}
	catch (...) {
		return false;
	}
	return true;
}
