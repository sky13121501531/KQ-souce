///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DownXMLProcess.h
// 创建者：jiangcheng
// 创建时间：2009-05-21
// 内容描述：获取通信层的任务指令（XML格式），解析指令并生成相应的任务实例，交给任务调度模块处理
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class DownXMLProcess : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DownXMLProcess();
	~DownXMLProcess();
public:
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

	int ProcessMessage(ACE_Message_Block* mb);
private:
	bool bFlag;

};