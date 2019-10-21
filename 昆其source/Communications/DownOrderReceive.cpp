///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DownOrderReceive.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：接收应用系统下达命令的基类，子类可实现不同接收方式
///////////////////////////////////////////////////////////////////////////////////////////
#include "DownOrderReceive.h"

DownOrderReceive::DownOrderReceive()
{
	bFlag = false;
}

DownOrderReceive::~DownOrderReceive()
{

}

int DownOrderReceive::Init()
{
	bFlag = true;
	return 0;
}

int DownOrderReceive::open(void*)
{
	this->activate();
	return 0;
}

int DownOrderReceive::svc()
{
	return 0;
}

int DownOrderReceive::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}
