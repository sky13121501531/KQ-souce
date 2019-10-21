


#ifndef STOP_PLAYING_VIDEO_TASK_H
#define STOP_PLAYING_VIDEO_TASK_H


#include "DeviceIndependentTask.h"


#ifdef ZONG_JU_ZHAO_BIAO

//视频播放停止任务
class StopPlayingVideoTask : public DeviceIndependentTask
{
public:
	StopPlayingVideoTask();
	StopPlayingVideoTask(std::string strXML);
	virtual ~StopPlayingVideoTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	string m_CoderIdList;
};

#endif






















#endif