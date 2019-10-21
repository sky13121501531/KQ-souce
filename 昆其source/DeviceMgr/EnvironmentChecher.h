///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：EnvironmentChecher.h
// 创建者：gaoxd
// 创建时间：2009-09-15
// 内容描述：环境监测类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Singleton.h"

class EnvironmentChecher : public ACE_Task<ACE_MT_SYNCH>
{
public:
	EnvironmentChecher();
	~EnvironmentChecher();
public:
	int Start();

	int Stop();

	int open(void*);

	virtual int svc();
	
private:
	bool bFlag;
};

typedef  ACE_Singleton<EnvironmentChecher,ACE_Mutex>  ENVIRONMENTCHECHER;