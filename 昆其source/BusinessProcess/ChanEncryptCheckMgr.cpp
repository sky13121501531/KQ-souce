


#include "ChanEncryptCheckMgr.h"





ChanEncryptCheckMgr::ChanEncryptCheckMgr(void)
{
	
}



ChanEncryptCheckMgr::~ChanEncryptCheckMgr(void)
{
}


bool ChanEncryptCheckMgr::SetEncryptInfo( std::string strCardNo, std::vector<sChannelInfo>& vecChanInfo )
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mutex );

	m_cardEncryptInfoMap[strCardNo] = vecChanInfo;

	return true;
}


bool ChanEncryptCheckMgr::GetEncryptInfo( std::string strCardNo, std::vector<sChannelInfo>& vecChanInfo )
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mutex );

	if (m_cardEncryptInfoMap.find(strCardNo) == m_cardEncryptInfoMap.end())
	{
		return false;
	}
	else
	{
		vecChanInfo = m_cardEncryptInfoMap[strCardNo];
	}
	return true;
}
bool ChanEncryptCheckMgr::ClearEncryptInfo()
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mutex );

	m_cardEncryptInfoMap.clear();
	return true;
}
bool ChanEncryptCheckMgr::GetEncryptInfo( std::map<std::string, std::vector<sChannelInfo>>& mapEncryptInfo )
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mutex );

	mapEncryptInfo = m_cardEncryptInfoMap;
	return true;
}