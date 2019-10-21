#pragma once

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：AlarmSender.h
// 创建者：gaoxd
// 创建时间：2009-09-08
// 内容描述：报警发送任务
// 备注：使用线程对报警数据进行发送，保证不阻塞其他任务执行
///////////////////////////////////////////////////////////////////////////////////////////

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <string>

class AlarmSender : public ACE_Task<ACE_MT_SYNCH>
{
public:
	AlarmSender();
	virtual ~AlarmSender();
public:
	int open(void*);

	virtual int svc();

	int Stop();
protected:
	int ProcessMessage(ACE_Message_Block* mb);//报警xml处理
private:
	bool bFlag;

};