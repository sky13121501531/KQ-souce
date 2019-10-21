///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TableQuery
// 创建者：gaoxd
// 创建时间：2009-10-25
// 内容描述：表查询任务类
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DeviceIndependentTask.h"
#include <string>

class TableQueryTask:public DeviceIndependentTask
{
public:
	TableQueryTask(void);
	TableQueryTask(std::string strXML);
	virtual~TableQueryTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	bool TableQuery( const std::string STD,const std::string Freq,const std::string SymbolRate,std::string& strFileName );
	string CreatePreTableQueryRet();
};
