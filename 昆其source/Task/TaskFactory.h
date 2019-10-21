///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskFactory.h
// 创建者：jiangcheng
// 创建时间：2009-05-25
// 内容描述：任务工厂基类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

class XMLTask;
class TaskFactory
{
public:
	TaskFactory();
	virtual ~TaskFactory();
public:
	virtual XMLTask* CreateTask(std::string strStandardXML);

private:
	bool ValidateXML(std::string& xml);
};