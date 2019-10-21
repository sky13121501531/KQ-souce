#ifndef _PROPCONFIG_H_
#define _PROPCONFIG_H_


#include <map>
#include <string>
#include <list>
#include "XmlParser.h"
#include "../Foundation/Config.h"


//配置文件操作类
class PropConfig
{
public:
	PropConfig(void);
	virtual~PropConfig(void);
	PropConfig(const char * PropFileName);
	bool LoadPropFile(const char* PropFileName );//加载配置文件
	string GetNodeText(const char * parNode,const char * chilNode);//获得配置文件中某节点的文本描述值
	string GetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute);//获得配置文件中某节点的属性值
	bool SetNodeText(const char * parNode,const char * chilNode,const char * valu);//设置配置文件中某节点的文本描述值
	
	bool isInitiated(){return mInitiated;};//加载配置文件是否成功

	pXMLNODE CreateNode(char* parNode,char* childNode);
	pXMLNODE CreateNode( char* parNode,char* childNode,string dvbtype,string freq,string alarmthreshold );
	bool DelNode(char* parNode,char* childNode);

	bool SetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute,const char * valu);//设置配置文件中某节点的属性值
	bool SetNodeAttribute(pXMLNODE node,string attribute,string valu);
	bool SetNodeAttribute(string dvbtype,const char * parNode,const char * attribute,const char * valu);
	bool SetNodeAttribute(string dvbtype,string freq,const char * parNode,const char * attribute,const char * valu);

#ifdef ZONG_JU_ZHAO_BIAO
	bool SetNodeAttribute(string parNode, string strAttrName, string strAttrValue, string strSetAttrName, string strSetValue/*, string strNoUse*/);

#endif
	
private:
	XmlParser mXmlProp;
	bool mInitiated;
	
};
#endif