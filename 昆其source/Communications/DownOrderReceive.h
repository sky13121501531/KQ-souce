///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DownOrderReceive.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：接收应用系统下达命令的基类，子类可实现不同接收方式
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"

class DownOrderReceive : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DownOrderReceive();
	virtual ~DownOrderReceive();
public:
	virtual int Init();

	virtual int open(void*);

	virtual int svc();

	virtual int Stop();
protected:
	bool bFlag;
};