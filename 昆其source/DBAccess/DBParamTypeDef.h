#ifndef _DBPARAMTYPEDEF_H_
#define _DBPARAMTYPEDEF_H_

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DBParamTypeDef.h
// 创建者：gaoxd
// 创建时间：2009-05-27
// 内容描述：数据库参数自定义类型定义，用类的方式实现
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

class DBParamChannel
{
public:
	DBParamChannel(){};
	~DBParamChannel(){};
public:
};

typedef std::vector<DBParamChannel*> DBParamChannelVec;

class CTTB_DBParamChannel : public DBParamChannel
{
public:
	CTTB_DBParamChannel(){};
	~CTTB_DBParamChannel(){};
public:
	std::string Freq;
	std::string AudioID;
	std::string VideoID;
};

class CMMB_DBParamChannel : public DBParamChannel
{
public:
	CMMB_DBParamChannel(){};
	~CMMB_DBParamChannel(){};
public:
	std::string Freq;
	std::string AudioID;
	std::string VideoID;
};


#endif