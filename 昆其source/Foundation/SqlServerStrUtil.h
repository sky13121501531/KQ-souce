#ifndef SQLSERSTRUTIL
#define SQLSERSTRUTIL
#pragma once
#include <string>
#if defined(WIN32) || defined(__WIN32__)
#include "comutil.h"
#include <atlbase.h>
#pragma comment(lib, "comsupp.lib" )
#endif
using namespace std;

class SqlStrUtil
{
public:
#if defined(WIN32) || defined(__WIN32__)//sqlserver π”√
static __int64 Str2Int64(string Str);
static string Bstr2Str(BSTR bstr);
static string Variant2Str(_variant_t variant);
static string StlStr2Str(basic_string<char> stlstr);
static string CComBstr2Str(CComBSTR combstr);
static string _bstr_t2Str(_bstr_t Temp);
#endif
};
#endif
