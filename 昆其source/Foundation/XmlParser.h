#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <string>
#include <list>

#include "../../thirdlib/xml/tinyxml.h"

using namespace std;

#define pXMLNODE TiXmlNode* 
#define pXMLELEMENT TiXmlElement* 
#define pXMLDOC TiXmlDocument* 

//节点列表类，通过此类可以获得节点包含所有的子节点，并放到list中
class XmlNodeList
{
public:
	XmlNodeList(void)
	{
		mNodeListPosition = 0;
		mNodeListCount = 0;
	};
	virtual ~XmlNodeList(void)
	{
		if(0 < mXMLNodeList.size())
		{
			mXMLNodeList.clear();
		}

		mNodeListPosition = 0;
		mNodeListCount = 0;
	};
public:
	pXMLNODE GetFirstNode(void)
	{
		pXMLNODE retNode = NULL;

		if(mXMLNodeList.size() > 0)
		{
			retNode = mXMLNodeList.front();
		}

		mNodeListPosition = 1;

		return retNode;
	};
	pXMLNODE GetNextNode(void)
	{
		pXMLNODE retNode = NULL;

		if(mNodeListPosition < (long)mXMLNodeList.size())
		{
			list<pXMLNODE>::iterator p = mXMLNodeList.begin();
			for(int i = 0; i < mNodeListPosition; i++)
			{
				p ++;
			}
			retNode = (*p);

			mNodeListPosition ++;
		}

		return retNode;
	};

	void PushNode(pXMLNODE& node)
	{
		if(node)
		{
			mXMLNodeList.push_back(node);
			mNodeListCount ++;
		}
		else
		{
		}
	};

	long Size(void)
	{
		return mNodeListCount;
	};

	void operator = (XmlNodeList const& OtherXmlNodeList )
	{
		mXMLNodeList = OtherXmlNodeList.mXMLNodeList;
		mNodeListPosition = OtherXmlNodeList.mNodeListPosition;
		mNodeListCount = OtherXmlNodeList.mNodeListCount;
	};
private:
	list<pXMLNODE> mXMLNodeList;//存放子节点的容器
	long mNodeListPosition;//指向容器的位置
	long mNodeListCount;//子节点数目
};

#define pXMLNODELIST XmlNodeList*
class XmlParser
{
public:
	pXMLNODE findNode(pXMLNODE parNode,char* nodename);
	// char* type must be all path exp: /Msg/Alarm/Music
	pXMLNODE nodeNameToPtr(char* nodename);   
public:

	XmlParser();
	XmlParser(const char* xml);
	virtual ~XmlParser();

	
	bool LoadFromFile(const char *FileName);//加载xml文件
	bool SaveAsFile(const char *FileName);//xml保存到文件
	bool SaveAll();//保存对当前xml文件的修改
	void SaveToString(string& mResult);//xml保存到字符串
	bool Get_xml(string &xmlstr);//读取xml字符串
	bool Set_xml(string xmlstr);//设置xml字符串

	string GetNodeText(char *NodeName);//获得节点的文本描述值
	string GetNodeText( pXMLNODE   CurNode);//获得节点的文本描述值
	bool SetNodeText(char *NodeName,char *NewText);//设置节点的文本描述值
	bool SetNodeText(pXMLNODE CurNode, char * NewText);//设置节点的文本描述值
	bool CreateNode(char *ParNodeName,char *NodeName);//创建新节点
	bool CreateNode(pXMLNODE parNode,char * NodeName);//创建新节点

	pXMLNODE CreateNodePtr(char *ParNodeName,char *NodeName);//创建新节点
	pXMLNODE CreateNodePtr(pXMLNODE parNode,char * NodeName);//创建新节点
	bool DeleteNode(char *ParNodeName,char *DelNodeName);//删除节点
	bool DeleteNode(pXMLNODE parNode, pXMLNODE childNode);//删除节点
	pXMLNODE GetNodeFromPath(char* NodePath);//根据路径得到节点

	bool IsChildofNode(pXMLNODE parNode, pXMLNODE childNode);//判断节点是否是父子关系
	bool IsChildofNode(char* parNode,char* childNode);//判断节点是否是父子关系
	string GetRootNodeName();	//获得根节点的名字
	string GetNodeName(pXMLNODE CurNode);//获得指定节点的名字

	pXMLNODE GetRootNode();//获得根节点
	pXMLNODE GetNodeFirstChild(pXMLNODE CurNode);//获得节点的第一个子节点
	pXMLNODELIST GetNodeList(pXMLNODE CurNode);//获得节点的子节点列表
	long GetChildCount(pXMLNODE CurNode);//获得节点的子节点数
	string GetNodePath(pXMLNODE CurNode);//获得节点在xml文件中的路径
	pXMLNODE GetParentNode(pXMLNODE CurNode);//获得节点的父节点

	
	pXMLNODE GetFirstNode(pXMLNODELIST CurList);//从子节点列表中获得第一个子节点
	pXMLNODE GetNextNode(pXMLNODELIST CurList);//从子节点列表中获得下一个子节点

	bool IsAttributeOfNode(pXMLNODE node,string attrname);//判断属性是否属于节点

	bool GetAttrNode(pXMLNODE pNode,string nodeName,string& nodevalue);//获得指定节点的某个属性值(依据属性值类型重载)
	bool GetAttrNode(pXMLNODE pNode,string nodeName,int& nodevalue);//获得指定节点的某个属性值
	bool GetAttrNode(pXMLNODE pNode,string nodeName,float& nodevalue);//获得指定节点的某个属性值
	bool GetAttrNode(pXMLNODE pNode,string nodeName,double& nodevalue);//获得指定节点的某个属性值
	bool GetAttrNode(pXMLNODE pNode,string nodeName,bool& nodevalue);//获得指定节点的某个属性值
	bool GetAttrNode(pXMLNODE pNode,string nodeName,long& nodevalue);//获得指定节点的某个属性值

	bool SetAttrNode(string nodeName,string nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值(依据属性值类型重载)
	bool SetAttrNode(string nodeName,int nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值
	bool SetAttrNode(string nodeName,double nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值
	bool SetAttrNode(string nodeName,float nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值
	bool SetAttrNode(string nodeName,bool nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值
	bool SetAttrNode(string nodeName,long nodeValue,pXMLNODE pNode);//设置指定节点的某个属性值
private:
	pXMLDOC mXmlDocPtr;
	char* mCurFileName;
	long listIndex;
};


#endif