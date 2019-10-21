#include "QualityCompensationMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/StrUtil.h"

QualityCompensationMgr::QualityCompensationMgr()
{
	init();
}
QualityCompensationMgr::~QualityCompensationMgr()
{

}

bool QualityCompensationMgr::UpdateQualityCompensation(std::vector<sQualityCompensation> qualityCmpVec)
{
	std::vector<sQualityCompensation>::iterator ptr=qualityCmpVec.begin();
	for (;ptr!=qualityCmpVec.end();++ptr)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(QualityComMutex);
		std::vector<sQualityCompensation>::iterator pptr=mQulaityCmpVec.begin();
		for (;pptr!=mQulaityCmpVec.end();++pptr)
		{
			if((*pptr).dvbtype==(*ptr).dvbtype&&(*pptr).deviceid==(*ptr).deviceid&&(*pptr).type==(*ptr).type
				&&((*pptr).freq==(*ptr).freq||(*pptr).freq.size()==0||(*ptr).freq.size()==0)
				&&((*pptr).level==(*ptr).level||(*pptr).level.size()==0||(*ptr).level.size()==0))
			{
				(*pptr).valu=(*ptr).valu;
				break;
			}
		}
		if(pptr==mQulaityCmpVec.end())
		{
			mQulaityCmpVec.push_back(*ptr);
		}
		DBMANAGER::instance()->UpdateCompensationValu(*ptr);
	}
	return true;
}

bool QualityCompensationMgr::GetQualityCompensation( eDVBType dvbtype,std::string deviceid,std::string type,std::string freq,std::string level,std::string& valu )
{
	ACE_Guard<ACE_Thread_Mutex> guard(QualityComMutex);
	std::vector<sQualityCompensation>::iterator ptr=mQulaityCmpVec.begin();
	if((freq.size()==0)&&(level.size()==0))
	{
		for (;ptr!=mQulaityCmpVec.end();++ptr)
		{
			if((*ptr).dvbtype==dvbtype&&(*ptr).deviceid==deviceid&&(*ptr).type==type)
			{
				valu=(*ptr).valu;
				break;
			}
		}
		return true;
	}
	ptr=mQulaityCmpVec.begin();
	for (;ptr!=mQulaityCmpVec.end();++ptr)
	{
		long lsrfreq = 0;
		long lenfreq = 0;
		if((*ptr).freq.size()>0)
		{
			int pos = (*ptr).freq.find("~");
			if(pos!=-1)
			{
				string stfreq = (*ptr).freq.substr(0,pos);
				lsrfreq = StrUtil::Str2Long(stfreq);
				string enfreq = (*ptr).freq.substr(pos+1,(*ptr).freq.size()-pos-1);
				lenfreq = StrUtil::Str2Long(enfreq);
			}
		}
		long lowlevel = 0;
		long hignlevel = 0;
		if((*ptr).level.size()>0)
		{
			int pos = (*ptr).level.find("~");
			if(pos!=-1)
			{
				string strlowlevel = (*ptr).level.substr(0,pos);
				lowlevel = StrUtil::Str2Long(strlowlevel);
				string strhignlevel = (*ptr).level.substr(pos+1,(*ptr).level.size()-pos-1);
				hignlevel = StrUtil::Str2Long(strhignlevel);
			}
		}
		//
		if((*ptr).dvbtype==dvbtype&&(*ptr).deviceid==deviceid&&(*ptr).type==type&&(*ptr).freq.size()>0&&(*ptr).level.size()>0
				&&((freq==(*ptr).freq)||((StrUtil::Str2Long(freq)>=lsrfreq)&&(StrUtil::Str2Long(freq)<=lenfreq)))
				&&((StrUtil::Str2Int(level)<=hignlevel)&&(StrUtil::Str2Int(level)>=lowlevel)))
		{
			valu=(*ptr).valu;
			return true;
		}
	}
	ptr=mQulaityCmpVec.begin();
	for (;ptr!=mQulaityCmpVec.end();++ptr)
	{
		long lsrfreq = 0;
		long lenfreq = 0;
		if((*ptr).freq.size()>0)
		{
			int pos = (*ptr).freq.find("~");
			if(pos!=-1)
			{
				string stfreq = (*ptr).freq.substr(0,pos);
				lsrfreq = StrUtil::Str2Long(stfreq);
				string enfreq = (*ptr).freq.substr(pos+1,(*ptr).freq.size()-pos-1);
				lenfreq = StrUtil::Str2Long(enfreq);
			}
		}
		//
		if((*ptr).dvbtype==dvbtype&&(*ptr).deviceid==deviceid&&(*ptr).type==type&&(*ptr).freq.size()>0&&(*ptr).level.size()<=0
				&&((freq==(*ptr).freq)||((StrUtil::Str2Long(freq)>=lsrfreq)&&(StrUtil::Str2Long(freq)<=lenfreq))))
		{
			valu=(*ptr).valu;
			return true;
		}
	}
	ptr=mQulaityCmpVec.begin();
	for (;ptr!=mQulaityCmpVec.end();++ptr)
	{
		long lowlevel = 0;
		long hignlevel = 0;
		if((*ptr).level.size()>0)
		{
			int pos = (*ptr).level.find("~");
			if(pos!=-1)
			{
				string strlowlevel = (*ptr).level.substr(0,pos);
				lowlevel = StrUtil::Str2Long(strlowlevel);
				string strhignlevel = (*ptr).level.substr(pos+1,(*ptr).level.size()-pos-1);
				hignlevel = StrUtil::Str2Long(strhignlevel);
			}
		}
		//
		if((*ptr).dvbtype==dvbtype&&(*ptr).deviceid==deviceid&&(*ptr).type==type&&(*ptr).freq.size()<=0&&(*ptr).level.size()>0
				&&((StrUtil::Str2Int(level)<=hignlevel)&&(StrUtil::Str2Int(level)>=lowlevel)))
		{
			valu=(*ptr).valu;
			return true;
		}
	}
	ptr=mQulaityCmpVec.begin();
	for (;ptr!=mQulaityCmpVec.end();++ptr)
	{
		if((*ptr).dvbtype==dvbtype&&(*ptr).deviceid==deviceid&&(*ptr).type==type&&(*ptr).freq.size()<=0&&(*ptr).level.size()<=0)
		{
			valu=(*ptr).valu;
			return true;
		}
	}
	return true;
}
void QualityCompensationMgr::init()
{
	DBMANAGER::instance()->QueryCompensationValu(mQulaityCmpVec);
}
