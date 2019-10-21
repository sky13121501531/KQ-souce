///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskTaskXMLSend.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：任务执行结果，上行发送类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class TaskXMLSend : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskXMLSend();
	~TaskXMLSend();
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();
private:
	int ProcessMessage(ACE_Message_Block* mb);
	std::string GetURL(std::string& upXML);
private:	
	bool bFlag;
};