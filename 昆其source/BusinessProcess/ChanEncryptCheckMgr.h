#pragma once


#include <map>
#include <vector>
#include <string>

#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "../Foundation/TypeDef.h"






class ChanEncryptCheckMgr 
{
public:
	ChanEncryptCheckMgr(void);
	virtual ~ChanEncryptCheckMgr(void);


public:
	bool SetEncryptInfo(std::string strCardNo, std::vector<sChannelInfo>& vecChanInfo);
	bool GetEncryptInfo(std::string strCardNo, std::vector<sChannelInfo>& vecChanInfo);
	bool GetEncryptInfo(STRCHANINFOMAP& mapEncryptInfo);
	bool ClearEncryptInfo();

	int GetMapSize()	{ return m_cardEncryptInfoMap.size() ; }

private:
	STRCHANINFOMAP m_cardEncryptInfoMap;

	ACE_Thread_Mutex m_mutex;
};



typedef ACE_Singleton<ChanEncryptCheckMgr,ACE_Mutex>  CHANENCRYPTCHECKMGR; 