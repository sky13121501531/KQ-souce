
#include "TsFetcher.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/AppLog.h"
TsFetcher::TsFetcher(int deviceid)
{
	DeviceId = deviceid;//Í¨µÀºÅ
	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,TunerID);
}

TsFetcher::~TsFetcher()
{

}

int TsFetcher::FindBegin(unsigned char* tsBuf,unsigned int tsLen)
{
	int nIndex = -1;
	bool bFindGroup = false;
	char tmpBuf[255]="";
	while((nIndex+(188*2)) < tsLen)
	{
		if((tsBuf[nIndex] == 0x47) && (tsBuf[nIndex+188] == 0x47))
		{
			bFindGroup = false;
			if ((0x41==(tsBuf[nIndex+1]&0x41))&&(0x00==tsBuf[nIndex+2]))
			{
				//ACE_OS::snprintf(tmpBuf,sizeof(tmpBuf),"echo ********************************VID11frameout!0X%02X%02X************* >> E:\test.txt",tsBuf[nIndex+1],tsBuf[nIndex+2]);
				//system(tmpBuf);
				//std::string msg = "********************************VID11frameout!";
				//msg += StrUtil::Int2Str(tsBuf[nIndex+1]);
				//msg += StrUtil::Int2Str(tsBuf[nIndex+2]);
				//APPLOG::instance()->WriteLog(VIDEO,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);

			//	printf("********************************VID frame out! 0X%02X%02X*************\n",tsBuf[nIndex+1],tsBuf[nIndex+2]);
				for (int i=0;i<188-3;++i)
				{
					if (tsBuf[nIndex+i] == 0x00 && tsBuf[nIndex+i+1] == 0x00 && tsBuf[nIndex+i+2] == 0x00 && tsBuf[nIndex+i+3] == 0x01/* &&(tsBuf[nIndex+i+4] &31== 5)*/)
					{
						bFindGroup = true;
						//cout<<"********************************I frame out! 0x*************"<<tsBuf[nIndex+1]<<"---"<<tsBuf[nIndex+2]<<endl;
						printf("********************************I frame out! 0X%02X%02X*************\n",tsBuf[nIndex+1],tsBuf[nIndex+2]);
						//ACE_OS::snprintf(tmpBuf,sizeof(tmpBuf),"echo ********************************VID22frameout!0X%02X%02X************* >> E:\test.txt",tsBuf[nIndex+1],tsBuf[nIndex+2]);
						//system(tmpBuf);
						//std::string msg = "********************************VID22frameout!";
						//msg += StrUtil::Int2Str(tsBuf[nIndex+1]);
						//msg += StrUtil::Int2Str(tsBuf[nIndex+2]);
						//APPLOG::instance()->WriteLog(VIDEO,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
						break;
					}
				}
			}
			
			if (bFindGroup == true)
			{
				break;
			}
			nIndex += 188;
			continue;
		}
		else
		{
			++nIndex;
		}
	}
	if(bFindGroup == true)
	{
		return nIndex;
	}
	else
	{
		return -1;
	}
}