#ifndef XmlValidator_h__
#define XmlValidator_h__



#include <string>


//xml验证类
class XmlValidator
{
public:
	XmlValidator();
	~XmlValidator();

public:
	//验证频道扫描结果xml的正确性
	static std::string ValidChanScanXml(const std::string& strRetXml);

};
























#endif // XmlValidator_h__