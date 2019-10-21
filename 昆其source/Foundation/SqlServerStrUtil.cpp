#pragma warning(disable:4996)

#include <math.h>
#include "SqlServerStrUtil.h"

#if defined(WIN32) || defined(__WIN32__)
__int64 SqlStrUtil::Str2Int64(string Str)
{
	if (!Str.empty())
		return _atoi64(Str.c_str());
	else return -1;
}

string SqlStrUtil::Bstr2Str(BSTR bstr)
{   
	string Ret("");
	_bstr_t Temp = bstr;
	LPCSTR TempLp = Temp;
	Ret = TempLp;
	return Ret;
}

string SqlStrUtil::Variant2Str(_variant_t variant)
{
	string Ret("");
	if (variant.vt == VT_NULL || variant.vt == VT_EMPTY) return Ret;
	SYSTEMTIME stime;
	_bstr_t TempBstr = variant;
	char rst[64];
	if (variant.vt == VT_DATE)
	{
		VariantTimeToSystemTime(variant,&stime);
		::sprintf(rst, "%04d-%02d-%02d %02d:%02d:%02d", stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute,stime.wSecond);
		TempBstr = rst;
	}    
	Ret = Bstr2Str(TempBstr);
	return Ret;
}

string SqlStrUtil::StlStr2Str(basic_string<char> stlstr)
{
	string Ret("");
	Ret = stlstr.c_str();
	return Ret;
}

string SqlStrUtil::CComBstr2Str(CComBSTR combstr)
{
	string Ret("");
	BSTR TempBstr = combstr;
	Ret = Bstr2Str(TempBstr);
	return Ret;
}

string SqlStrUtil::_bstr_t2Str(_bstr_t Temp)
{
	string Ret("");
	LPCSTR TempLp = Temp;
	Ret = TempLp;
	return Ret;
}
#endif

