
#pragma warning(disable:4996)

#include <sys/stat.h>
#include <vector>
#include "XmlParser.h"
#include "ace/OS.h"

XmlParser::XmlParser()
{
	mXmlDocPtr  = new TiXmlDocument;
	mCurFileName = NULL;
	listIndex = 0;
	
};
XmlParser::XmlParser(const char* xml)
{
	listIndex = 0;
	mXmlDocPtr  = new TiXmlDocument;
	if (strlen(xml) ==0)
	{
		cout << "buf empty" <<endl;
		return;
	}
	if(!mXmlDocPtr->Parse(xml))
	{
		//cout << mXmlDocPtr->ErrorDesc() << endl;
		return;
	}
}
XmlParser::~XmlParser()
{	
	if (mXmlDocPtr != NULL)
		delete mXmlDocPtr;
};
// <summary>xml对象保存到字符串</summary>
// <param name="mResult">用来保存xml的字符串</param>
// <retvalue></retvalue>
void XmlParser::SaveToString(std::string& mResult)
{
	 Get_xml(mResult);
};
// <summary>依据文件名构造xml对象</summary>
// <param name="FileName">文件名</param>
// <retvalue>成功或失败</retvalue>
bool XmlParser::LoadFromFile(const char *FileName)
{  
	bool ret = false;
	if(FileName != NULL)
	{
		try
		{
			if( mXmlDocPtr->LoadFile(FileName))
			{
				mCurFileName =(char*) FileName;
				ret = true;
			}
		}
		catch(...)
		{
			cout<<"非法的xml文件或者指定的xml文件不存在"<<endl;
			return false;
		}
	}
	return ret;
}
// <summary>依据子节点名称查找子节点</summary>
// <param name="parNode">父节点</param>
// <param name="nodename">子节点名字</param>
// <retvalue>找到的子节点或者空</retvalue>
pXMLNODE XmlParser::findNode(pXMLNODE parNode,char* nodename)
{
	if ((parNode==NULL)||(strlen(nodename)==0))
		return NULL;
	if((parNode->Type() == TiXmlNode::ELEMENT) && (!strcmp(parNode->Value(),nodename)))
		return parNode;

	pXMLNODE child = NULL;
	for(  child= parNode->FirstChild();child;child=child->NextSibling())
	{
		if((child->Type() == TiXmlNode::ELEMENT) && (!strcmp(child->Value(),nodename)))
			break;               
	}
	return child;
}
// <summary>xml对象保存到文件</summary>
// <param name="FileName">文件名</param>
// <retvalue>保存结果成功或者失败</retvalue>
bool XmlParser::SaveAsFile(const char *FileName)
{
	bool ret = false;
	if (!mXmlDocPtr) return ret;
	if (FileName!=NULL)
	{
		ret=mXmlDocPtr ->SaveFile(FileName);
	}
	return ret;
};
// <summary>保存对xml文件的修改</summary>
// <param name=""></param>
// <retvalue>保存结果成功或者失败</retvalue>
bool XmlParser::SaveAll()
{
	bool ret = false;
	if (!mXmlDocPtr) return ret;
	if (mCurFileName!=NULL)
	{
		ret = mXmlDocPtr->SaveFile(mCurFileName);
	}	
	return ret;
};
// <summary>为节点创建子节点</summary>
// <param name="ParNodeName">父节点名字</param>
// <param name="NodeName">子节点名字</param>
// <retvalue>创建成功或者失败</retvalue>
bool XmlParser::CreateNode(char *ParNodeName,char *NodeName)
{
	if(ParNodeName!=NULL)
	{
		pXMLNODE temp =  nodeNameToPtr(ParNodeName);
		if (temp == NULL)
			return false;
		TiXmlElement pNode(NodeName);
		if (temp->InsertEndChild(pNode) != NULL)
			return true;
		else
			return false;
	}
	else
		return false;
}
// <summary>为节点创建子节点</summary>
// <param name="parNode">父节点</param>
// <param name="NodeName">子节点名字</param>
// <retvalue>创建成功或者失败</retvalue>
bool XmlParser::CreateNode(pXMLNODE parNode,char * NodeName)
{
		TiXmlElement pNode(NodeName);
		if (parNode == NULL)
			return false;
		if (parNode->InsertEndChild(pNode) != NULL)
			return true;
		else
			return false;
}
// <summary>为节点创建子节点</summary>
// <param name="ParNodeName">父节点名字</param>
// <param name="NodeName">子节点名字</param>
// <retvalue>新创建子节点的指针</retvalue>
pXMLNODE XmlParser::CreateNodePtr(char *ParNodeName,char *NodeName)
{
	if(ParNodeName!=NULL)
	{
		pXMLNODE temp =  nodeNameToPtr(ParNodeName);
		if (temp == NULL)
			return NULL;
		TiXmlElement pNode(NodeName);
		pXMLNODE temp1 = NULL;
		temp1 = temp->InsertEndChild(pNode);
		if (temp1 != NULL)
			return temp1;
		else
			return NULL;
	}
	return NULL;
}
// <summary>为节点创建子节点</summary>
// <param name="parNode">父节点</param>
// <param name="NodeName">子节点名字</param>
// <retvalue>新创建子节点的指针</retvalue>
pXMLNODE XmlParser::CreateNodePtr(pXMLNODE parNode,char * NodeName)
{
	if(parNode!=NULL)
	{
		TiXmlElement pNode(NodeName);
		pXMLNODE temp1 = NULL;
		temp1 = parNode->InsertEndChild(pNode);
		if (temp1 != NULL)
			return temp1;
		else
			return NULL;
	}
	else
		return NULL;
}
// <summary>删除节点</summary>
// <param name="ParNodeName">父节点名字</param>
// <param name="DelNodeName">要删除的子节点名字</param>
// <retvalue>删除成功或失败</retvalue>
bool XmlParser::DeleteNode(char *ParNodeName,char *DelNodeName)
{
	if ((ParNodeName==NULL)||(DelNodeName==NULL))
		return false;
	pXMLNODE temp =  nodeNameToPtr(ParNodeName);
	pXMLNODE temp1 =  nodeNameToPtr(DelNodeName);
	if ((temp == NULL) || (temp1 == NULL))
		return false;
	if (temp1->Parent() != temp)
		return false;
	return temp->RemoveChild(temp1);
}
// <summary>删除节点</summary>
// <param name="parNode">父节点</param>
// <param name="childNode">要删除的子节点</param>
// <retvalue>删除成功或失败</retvalue>
bool XmlParser::DeleteNode(pXMLNODE parNode, pXMLNODE childNode)
{
	if ((parNode == NULL) || (childNode == NULL))
		return false;
	if (childNode->Parent() != parNode)
		return false;
	return parNode->RemoveChild(childNode);
}
// <summary>根据节点路径得到节点</summary>
// <param name="NodePath">节点路径，形如a/b/c形式</param>
// <retvalue>对应路径的节点</retvalue>
pXMLNODE XmlParser:: GetNodeFromPath(char* NodePath)
{
	return nodeNameToPtr(NodePath);
}

// <summary>获得节点的文本</summary>
// <param name="CurNode">节点的名字</param>
// <retvalue>节点的文本描述</retvalue>
string XmlParser::GetNodeText(char *NodeName)
{
	string ret("");
	if(NodeName==NULL)
		return ret;
	TiXmlElement * temp =  (TiXmlElement *)nodeNameToPtr(NodeName);
	if (temp != NULL)
	{
		if (temp->GetText() != NULL)
			ret += temp->GetText();
	}
	return ret;
}

// <summary>获得节点的文本</summary>
// <param name="CurNode">节点的指针</param>
// <retvalue>节点的文本描述</retvalue>
string XmlParser::GetNodeText(pXMLNODE CurNode)
{
	string ret("");
	if (CurNode != NULL)
		if(CurNode->Type() == TiXmlNode::ELEMENT)
		{
			const pXMLELEMENT box = (const pXMLELEMENT)CurNode;
			if (box->GetText() != NULL)
				ret +=box->GetText();
		}
		return ret;
}

// <summary>设置节点的文本</summary>
// <param name="CurNode">节点的名字</param>
// <param name="NewText">文本描述</param>
// <retvalue>true成功，false失败</retvalue>
bool XmlParser::SetNodeText(char *NodeName,char *NewText)
{
	if ((NodeName==NULL)||(NewText==NULL))
		return false;
	pXMLNODE temp =  nodeNameToPtr(NodeName);
	if (temp == NULL)
		return false;
	TiXmlText  pText((const char*)NewText);        
	if (GetNodeText(NodeName).length() ==0)
	{
		temp->InsertEndChild(pText);
	} else
	{
		temp->Clear();
		temp->InsertEndChild(pText);
	}            
	return true;
}
// <summary>设置节点的文本</summary>
// <param name="CurNode">节点指针</param>
// <param name="NewText">文本描述</param>
// <retvalue>true成功，false失败</retvalue>
bool XmlParser::SetNodeText(pXMLNODE CurNode, char * NewText)
{
	if ((CurNode==NULL)||(NewText==NULL))
		return false;
	TiXmlText  pText((const char*)NewText);    
	if (CurNode == NULL)
		return false;
	if (GetNodeText(CurNode).length() ==0)
	{
		CurNode->InsertEndChild(pText);
	} else
	{
		CurNode->Clear();
		CurNode->InsertEndChild(pText);
	}            
	return true;
}

// <summary>从xml对象中获得xml字符串</summary>
// <param name="xmlstr">保存xml字符串</param>
// <retvalue>true成功，false失败</retvalue>
bool XmlParser::Get_xml(std::string &xmlstr)
{
	bool ret = false;
	char temp[100] = {0};
	unsigned long pid=ACE_OS::thr_self();
	ultoa(pid,temp,10);
	if (mXmlDocPtr == NULL)
		return false;
	if (mXmlDocPtr != NULL)
		mXmlDocPtr->SaveFile(temp);
	FILE* mFile = ::fopen(temp, "rb");
	if (mFile == NULL)
		return false;

	struct stat st;
	fstat(fileno(mFile), &st);

	unsigned char*  p = new unsigned char[st.st_size+1];
	memset(p,0,st.st_size+1);
	::fread(p, sizeof(char), st.st_size, mFile);
	xmlstr.clear();
	xmlstr.append((const char*)p);
	::fclose(mFile);
	unlink(temp);
	delete[] p;
	p=NULL;
	return true;
}

// <summary>加载xml字符串</summary>
// <param name="xmlstr">符合规则的xml字符串</param>
// <retvalue>true成功，false失败</retvalue>
bool XmlParser::Set_xml(std::string xmlstr)
{
	if (!mXmlDocPtr->Parse(xmlstr.c_str()))
	{
		//cout<<mXmlDocPtr->ErrorDesc()<<endl;
		return false;
	}
	return true;
}

// <summary>判断childNode节点是否是parNode节点的子节点</summary>
// <param name="childNode">子节点的指针</param>
// <param name="parNode">父节点的指针</param>
// <retvalue>true是，false不是</retvalue>
bool XmlParser::IsChildofNode(pXMLNODE parNode, pXMLNODE childNode)
{
	if ((parNode==NULL)||(childNode==NULL))
		return false;
	bool ret = false;
	pXMLNODE calParNode= NULL;
	calParNode = childNode ->Parent();
	if(calParNode)
	{
		//if (GetNodePath(calParNode) == GetNodePath(parNode))//都可以
		if(parNode==calParNode)
			ret =true;
	}
	return ret;
};

// <summary>判断childNode节点是否是parNode节点的子节点</summary>
// <param name="childNode">子节点的名字</param>
// <param name="parNode">父节点的名字</param>
// <retvalue>true是，false不是</retvalue>
bool XmlParser::IsChildofNode(char* parNode,char* childNode)
{
	bool ret = false;
	if ((parNode==NULL)||(childNode==NULL))
		return ret;
	pXMLNODE calParNodeA=NULL;
	pXMLNODE calParNode=NULL;
	pXMLNODE calChildNode=NULL;
	try
	{
		calParNode = GetNodeFromPath(parNode);
		calChildNode = GetNodeFromPath(childNode);
		if ((calParNode!=NULL)&&(calChildNode!=NULL)&&(calParNode->Parent() != NULL) && (calChildNode->Parent()!=NULL ))
		{
			calParNodeA = calChildNode ->Parent();
			if (GetNodePath(calParNodeA) == GetNodePath(calParNode))
					ret =true;
		}
	} catch(...)
	{}
	return ret;
};

// <summary>获得根节点的名字</summary>
// <param name=""></param>
// <retvalue>根节点的名字</retvalue>
std::string XmlParser::GetRootNodeName()
{
	return mXmlDocPtr->RootElement()->ValueStr();
};

// <summary>获得节点的名字</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的名字</retvalue>
std::string XmlParser::GetNodeName(pXMLNODE CurNode)
{
	if(CurNode!=NULL)
		return CurNode->ValueStr();
	else
		return string("");
};

// <summary>获得节点的父节点</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的父节点</retvalue>
pXMLNODE  XmlParser::GetParentNode(pXMLNODE CurNode)
{
	pXMLNODE ParNode=NULL;
	if(CurNode!=NULL)
		ParNode= CurNode->Parent();
	if (ParNode!=NULL)
	{
		return ParNode;
	} else
		return NULL;
};

// <summary>获得根节点</summary>
// <param name=""></param>
// <retvalue>根节点</retvalue>
pXMLNODE XmlParser::GetRootNode()
{
	return mXmlDocPtr->RootElement();
};

// <summary>获得节点的第一个子节点</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的第一个子节点</retvalue>
pXMLNODE XmlParser::GetNodeFirstChild(pXMLNODE CurNode)
{
	pXMLNODE ChildNode=NULL;
	if(CurNode!=NULL)
		ChildNode= CurNode->FirstChild();
	if (ChildNode)
		return ChildNode;
	else return NULL;
};

// <summary>获得节点的子节点列表</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的子节点列表</retvalue>
pXMLNODELIST XmlParser::GetNodeList(pXMLNODE CurNode)
{
	pXMLNODELIST ret = new XmlNodeList();
	if (CurNode == NULL)
		return NULL;
	pXMLNODE child = NULL;
	for(child= CurNode->FirstChild();child;child=child->NextSibling())
	{
		if(child->Type() == TiXmlNode::ELEMENT)
			ret->PushNode(child);
	}
	return ret;
};

// <summary>获得节点的子节点数</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的子节点个数</retvalue>
long XmlParser::GetChildCount(pXMLNODE CurNode)
{
	if (CurNode == NULL)
		return 0;
	long ret =0;
	if (CurNode->NoChildren())
		return ret;

	pXMLNODE child = NULL;
	for(  child= CurNode->FirstChild();child;child=child->NextSibling())
		ret ++;
	return ret;
};

// <summary>获得节点的路径信息</summary>
// <param name="CurNode">节点指针</param>
// <retvalue>节点的路径</retvalue>
std::string XmlParser::GetNodePath(pXMLNODE CurNode)
{
	string ret1;
	if (CurNode == NULL)
		return "";
	vector<string> nodeNameList;
	pXMLNODE temp = CurNode;
	if (temp != GetRootNode())
	{
		do{
			nodeNameList.push_back(temp->ValueStr());
			temp = temp->Parent();
		}while((temp != GetRootNode())&&(temp!=NULL));
		if (temp!=NULL)
		{
			nodeNameList.push_back(temp->ValueStr());
		}
		for (size_t i = nodeNameList.size(); i > 0; i --)
		{
			string temp = nodeNameList[i-1];
			ret1.append(temp);
			if (i != 1)
				ret1.append("/");
		}
	} else
		ret1.append(temp->ValueStr());
	return ret1;
}

// <summary>从子节点列表中获得第一个子节点</summary>
// <param name="CurList">子节点列表</param>
// <retvalue>节点列表中的第一个节点</retvalue>
pXMLNODE XmlParser::GetFirstNode(pXMLNODELIST CurList)
{
	listIndex = 0;
	pXMLNODE temp = NULL;
	if (CurList!=NULL && CurList->Size() >0)
		temp = CurList->GetFirstNode();	
	//listIndex ++;
	return temp;
}

// <summary>从子节点列表中获得下一个子节点</summary>
// <param name="CurList">子节点列表</param>
// <retvalue>节点列表中的下一个节点</retvalue>
pXMLNODE XmlParser::GetNextNode(pXMLNODELIST CurList)
{       
	pXMLNODE temp = NULL;
    if(CurList!=NULL)
		temp = CurList->GetNextNode();

	return temp;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,string& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	if (!IsAttributeOfNode(pNode,nodeName))
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	nodevalue = temp->Attribute((const char*)nodeName.c_str());
	return true;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,int& nodevalue)
{   
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->Attribute(nodeName,&nodevalue);
	return true;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,long& nodevalue)
{   
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	string str;
    GetAttrNode(pNode,nodeName,str);
	if (str.size()>0)
	{
		nodevalue=atol(str.c_str());
	}
	return true;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,float& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;    
	temp->QueryFloatAttribute((const char*)nodeName.c_str(),&nodevalue);
	return true;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,double& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->QueryDoubleAttribute((const char*)nodeName.c_str(),&nodevalue);
	return true;
}

// <summary>获得节点属性值</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">保存得到的属性值</param>
// <param name="pNode">指向要获得属性的节点指针</param>
// <retvalue>获得成功或者失败</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,bool& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	string nodevaluea = temp->Attribute((const char*)nodeName.c_str());
	if (nodevaluea.compare("TRUE") ==0)
		nodevalue = true;
	else
		nodevalue = false;
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,string nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetAttribute(nodeName,nodeValue);
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,int nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetAttribute(nodeName,nodeValue);
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,long nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	char buff[64];
	memset(buff,0,sizeof(buff));
	sprintf(buff,"%d",nodeValue);
	temp->SetAttribute(nodeName,string(buff));
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,double nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetDoubleAttribute((const char*)nodeName.c_str(),nodeValue);
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,float nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetDoubleAttribute((const char*)nodeName.c_str(),(double)nodeValue);
	return true;
}

// <summary>设置节点属性</summary>
// <param name="nodename">属性名</param>
// <param name="nodeValue">属性值</param>
// <param name="pNode">要设置属性的节点指针</param>
// <retvalue>设置成功或者失败</retvalue>
bool XmlParser::SetAttrNode(string nodeName,bool nodeValue,pXMLNODE pNode)
{
	if (pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	string Value;
	if (nodeValue)
		Value = "TRUE";
	else
		Value = "FALSE";
	temp->SetAttribute(nodeName,Value);
	return true;
}

// <summary>依据子节点名称获得指向该节点的指针</summary>
// <param name="nodename">节点的名字，形如msg/return形式</param>
// <retvalue>对应名字的节点的指针</retvalue>
pXMLNODE  XmlParser::nodeNameToPtr(char* nodename)
{
	list<string> nodes;       
	string subStr;
	string allPath(nodename);
	long lastFlag = 0;
	if (allPath[0] != '/')
		allPath.insert(0,"/");
	for (size_t i =0 ; i <allPath.length()-1; i++)
	{
		if (allPath[i] == '/')
		{       
			if (i ==0) continue;
			subStr = allPath.substr(lastFlag+1,i - lastFlag -1);
			lastFlag= (long)i;
			nodes.push_back(subStr);
		}
	}
	subStr = allPath.substr(lastFlag+1,allPath.length() -lastFlag -1);
	nodes.push_back(subStr);
	pXMLNODE parent = mXmlDocPtr->RootElement();
	for (list<string>::iterator p = nodes.begin(); p != nodes.end() ; p ++)
	{
		string nodename = *p;            
		pXMLNODE temp =  findNode(parent,(char*)nodename.c_str());
		if (temp!=NULL)
		{
			parent  = temp;
		}
		else
		{
			string xml;
			cout<<"XmlParser::nodeNameToPtr文件中不存在节点"<<nodename.c_str()<<endl;
			return NULL;
		}
	}
	return parent;
}

bool XmlParser::IsAttributeOfNode( pXMLNODE node,string attrname )
{
	bool ret=false;
	if (node==NULL ||node->Type() != TiXmlNode::ELEMENT)
		return ret;
	TiXmlElement* temp=(TiXmlElement*)node;
	TiXmlAttribute* attr=temp->FirstAttribute();
	if (!attr)
	{
		return false;
	}
	while (true)
	{
		if (attr->Name()==attrname)
		{
			ret=true;
			break;
		}
		if (attr==temp->LastAttribute())
		{
			break;
		}
		attr=attr->Next();
	}
	return ret;
}