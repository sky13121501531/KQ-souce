#pragma warning(disable:4996)

#include <math.h>
#include "StrUtil.h"
#if defined(WIN32) || defined(__WIN32__)
#include <Windows.h>
#pragma comment(lib,"kernel32.lib")
#endif

string StrUtil::RoundFloat(double fOriginal,int nSaveBits)
{
    static const int DECIMAL_BASE = 10;    
    unsigned int n,scale;
    double fEnd;
    char sEnd[100] = {0};
    string retValue;

    n = 0;
    scale = 1;
    double fOriginal_f = fabs(fOriginal);
    for (int i=0;i<nSaveBits;i++)  
        scale *= DECIMAL_BASE;
    scale *= 2;
    n = (int)(fOriginal_f * scale); 

    n += (n % 2);
    fEnd = ((double)n /scale);
    if(fOriginal < 0)
        sprintf(sEnd,"-%0.2f",fEnd);
    else
        sprintf(sEnd,"%0.2f",fEnd);
    retValue = string(sEnd);
    return retValue;
}

int StrUtil::Str2Int(string Str)
{
    if (!Str.empty())
        return atoi(Str.c_str());
    else return 0;
}

string StrUtil::Int2Str(int integer)
{
    char ret[64] = {0};
    ::sprintf(ret,"%d",integer);
    return ret;
}

long StrUtil::Str2Long(string Str)
{
    if (!Str.empty())
        return atol(Str.c_str());
    else return 0;
}
string StrUtil::Long2Str(long value)
{
    char ret[64] = {0};
    	::sprintf(ret,"%d",value);
    return ret;
}

long StrUtil::CiFang(long baseInt,long numCF)
{  
    if (0 ==numCF) return 1;
    long result = baseInt;
    for ( int i = 2 ; i <= numCF; i ++)
        result = result * baseInt;
    return result;
}

float StrUtil::Str2Float(string str)
{
    if (str.empty() || str.size() == 0) 
		str="0";
    return (float)atof(str.c_str());
}
float StrUtil::Str2FloatForFMFreq(string str)
{
	if (str.empty() || str.size() == 0) 
		str="0";
	return (float)atof(str.c_str()) + 0.0001F;
}

string StrUtil::Float2Str(float floater)
{
    string ret = RoundFloat((double)floater,2);

    return ret;
}
std::string StrUtil::Float2Str1( float floater )
{
	char result[100];
	double dOrginaData=(double)floater*10;
	int iOrginalData=(int)dOrginaData;
	double subRes=dOrginaData-(double)iOrginalData;
	int iLeft=(int)(subRes*2.0);

	int iEnd=dOrginaData;
	iEnd+=iLeft;
	double dResult=(double)iEnd/10;

	sprintf(result,"%0.1f",dResult);

	string strResult=string(result);
	return strResult;
}

int StrUtil::Str2IntForQam(string str)
{
	if(!strcmp(str.c_str(),"QAM16"))
	{
		return 0;
	}
	else if(!strcmp(str.c_str(),"QAM32"))
	{
		return 1;
	}
	else if(!strcmp(str.c_str(),"QAM64"))
	{
		return 2;
	}
	else if(!strcmp(str.c_str(),"QAM128"))
	{
		return 3;
	}
	else if(!strcmp(str.c_str(),"QAM256"))
	{
		return 4;
	}
	else if(!strcmp(str.c_str(),"QAM4"))
	{
		return 5;
	}
	else
	{
		return Str2Int(str);
	}
}

void StrUtil::ConvertUtf8ToGBK( const char* strUtf8,std::string& strGBK )
{
#if defined(WIN32) || defined(__WIN32__)
	WCHAR *strSrc;
	TCHAR *szRes;
	//UTF8转化为Unicode
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
	strSrc = new WCHAR[len+1];
	MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, strSrc, len);

	//Unicode转化为GBK
	len = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[len+1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, len, NULL, NULL);
	strGBK = szRes;
	//改变xml的encoding属性为GB2312
	size_t pos=strGBK.find("encoding=");
	size_t next_pos1=strGBK.find("\"",pos)+1;
	size_t next_pos2=strGBK.find("\"",next_pos1);
	strGBK.erase(next_pos1,next_pos2-next_pos1);
	strGBK.insert(next_pos1,"GB2312");

	delete []strSrc;
	delete []szRes;
#endif
}

void StrUtil::ConvertGBKToUtf8( const char* strGBK,std::string& strUtf8 )
{
#if defined(WIN32) || defined(__WIN32__)
	WCHAR *strSrc;
	TCHAR *szRes;
	//GBK转化为Unicode
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	strSrc = new WCHAR[len+1];
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, strSrc, len);

	//Unicode转化为Utf8
	len = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[len+1];
	WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, len, NULL, NULL);

	strUtf8 = szRes;
	//改变xml的encoding属性为UTF-8
	size_t pos=strUtf8.find("encoding=");
	size_t next_pos1=strUtf8.find("\"",pos)+1;
	size_t next_pos2=strUtf8.find("\"",next_pos1);
	strUtf8.erase(next_pos1,next_pos2-next_pos1);
	strUtf8.insert(next_pos1,"UTF-8");
	
	delete []strSrc;
	delete []szRes;
#endif
}

void StrUtil::ConvertETR290TypeIDToAlarmID(sCheckParam& parm)
{
	if (parm.DVBType == DVBC)
	{
		if (parm.TypeID=="1.1")
		{
			parm.TypeID="2";
		}
		else if (parm.TypeID=="1.2")
		{
			parm.TypeID="3";
		}
		else if (parm.TypeID=="1.3")
		{
			parm.TypeID="4";
		} 
		else if (parm.TypeID=="1.4")
		{
			parm.TypeID="5";
		} 
		else if (parm.TypeID=="1.5")
		{
			parm.TypeID="6";
		} 
		else if (parm.TypeID=="1.6")
		{
			parm.TypeID="7";
		} 
		else if (parm.TypeID=="2.1")
		{
			parm.TypeID="8";
		} 
		else if (parm.TypeID=="2.2")
		{
			parm.TypeID="9";
		} 
		else if (parm.TypeID=="2.3a")
		{
			parm.TypeID="10";
		} 
		else if (parm.TypeID=="2.4")
		{
			parm.TypeID="11";
		} 
		else if (parm.TypeID=="2.5")
		{
			parm.TypeID="12";
		} 
		else if (parm.TypeID=="2.6")
		{
			parm.TypeID="13";
		} 
		else if (parm.TypeID=="2" || parm.TypeID=="3" || parm.TypeID=="4" ||parm.TypeID=="5" 
			|| parm.TypeID=="6" || parm.TypeID=="7" || parm.TypeID=="8" ||parm.TypeID=="9" 
			|| parm.TypeID=="10" || parm.TypeID=="11" || parm.TypeID=="12" ||parm.TypeID=="13" )
		{
			if (parm.TypeID=="2")	
			{
				parm.TypeID="21";
			}
			else if (parm.TypeID=="3")	
			{
				parm.TypeID="22";
			}
			else if (parm.TypeID=="4")	
			{
				parm.TypeID="23";
			}
			else if (parm.TypeID=="5")	
			{
				parm.TypeID="24";
			}
			else if (parm.TypeID=="6")	
			{
				parm.TypeID="25";
			}
			else if (parm.TypeID=="7")	
			{
				parm.TypeID="26";
			}
			else
				parm.TypeID="";
		} 
		
	}
	else if (parm.DVBType == CTTB)
	{
		if (parm.TypeID=="1.1")
		{
			parm.TypeID="1";
		}
		else if (parm.TypeID=="1.2")
		{
			parm.TypeID="2";
		}
		else if (parm.TypeID=="1.3")
		{
			parm.TypeID="3";
		} 
		else if (parm.TypeID=="1.4")
		{
			parm.TypeID="4";
		} 
		else if (parm.TypeID=="1.5")
		{
			parm.TypeID="5";
		} 
		else if (parm.TypeID=="1.6")
		{
			parm.TypeID="6";
		} 
		else if (parm.TypeID=="2.1")
		{
			parm.TypeID="7";
		} 
		else if (parm.TypeID=="2.2")
		{
			parm.TypeID="8";
		} 
		else if (parm.TypeID=="2.3a")
		{
			parm.TypeID="9";
		} 
		else if (parm.TypeID=="2.4")
		{
			parm.TypeID="10";
		} 
		else if (parm.TypeID=="2.5")
		{
			parm.TypeID="11";
		} 
		else if (parm.TypeID=="2.6")
		{
			parm.TypeID="12";
		} 
		else if (parm.TypeID=="2" || parm.TypeID=="3" || parm.TypeID=="4" ||parm.TypeID=="5" 
			|| parm.TypeID=="6" || parm.TypeID=="7")
		{
			if (parm.TypeID=="2")	
			{
				parm.TypeID="21";
			}
			else if (parm.TypeID=="3")	
			{
				parm.TypeID="22";
			}
			else if (parm.TypeID=="4")	
			{
				parm.TypeID="23";
			}
			else if (parm.TypeID=="5")	
			{
				parm.TypeID="24";
			}
			else if (parm.TypeID=="6")	
			{
				parm.TypeID="25";
			}
			else if (parm.TypeID=="7")	
			{
				parm.TypeID="26";
			}
		} 
	}
	else if(parm.DVBType == DVBS)
	{
		if (parm.TypeID=="1.1")
		{
			parm.TypeID="2";
		}
		else if (parm.TypeID=="1.2")
		{
			parm.TypeID="3";
		}
		else if (parm.TypeID=="1.3")
		{
			parm.TypeID="4";
		} 
		else if (parm.TypeID=="1.4")
		{
			parm.TypeID="5";
		} 
		else if (parm.TypeID=="1.5")
		{
			parm.TypeID="6";
		} 
		else if (parm.TypeID=="1.6")
		{
			parm.TypeID="7";
		} 
		else if (parm.TypeID=="2.1")
		{
			parm.TypeID="8";
		} 
		else if (parm.TypeID=="2.2")
		{
			parm.TypeID="9";
		} 
		else if (parm.TypeID=="2.3a")
		{
			parm.TypeID="10";
		} 
		else if (parm.TypeID=="2.4")
		{
			parm.TypeID="11";
		} 
		else if (parm.TypeID=="2.5")
		{
			parm.TypeID="12";
		} 
		else if (parm.TypeID=="2.6")
		{
			parm.TypeID="13";
		} 
		else if (parm.TypeID=="2" || parm.TypeID=="3" || parm.TypeID=="4" ||parm.TypeID=="5" 
			|| parm.TypeID=="6" || parm.TypeID=="7")
		{
			if (parm.TypeID=="2")	
			{
				parm.TypeID="21";
			}
			else if (parm.TypeID=="3")	
			{
				parm.TypeID="22";
			}
			else if (parm.TypeID=="4")	
			{
				parm.TypeID="23";
			}
			else if (parm.TypeID=="5")	
			{
				parm.TypeID="24";
			}
			else if (parm.TypeID=="6")	
			{
				parm.TypeID="25";
			}
			else if (parm.TypeID=="7")	
			{
				parm.TypeID="26";
			}
		} 
	}
	return;
}

void StrUtil::ConvertProgramTypeIDToAlarmID(sCheckParam& parm)
{
	if(parm.DVBType == DVBC)
	{
		if (parm.TypeID=="0x1")			//黑场
		{
			parm.TypeID="51";
		}
		else if (parm.TypeID=="0x2")	//静帧
		{
			parm.TypeID="52";
		}
		else if (parm.TypeID=="0x3")	//视频丢失
		{
			parm.TypeID="53";
		}
		else if (parm.TypeID=="0x4")	//解码异常
		{
			parm.TypeID="54";
		}
		else if (parm.TypeID=="0x5")	//彩场
		{
			parm.TypeID="55";
		}
		else if (parm.TypeID=="0x6")	//马赛克
		{
			parm.TypeID="56";
		}
		else if (parm.TypeID=="0x7")	//彩条
		{
			parm.TypeID="57";
		}
		else if (parm.TypeID=="0x8")	//音量过低
		{
			parm.TypeID="58";
		}
		else if (parm.TypeID=="0x9")	//音量过高
		{
			parm.TypeID="59";
		}
		else if (parm.TypeID=="0x10")	//音频丢失
		{
			parm.TypeID="60";
		}
	}
	if (parm.DVBType==ATV || parm.DVBType==CTV||parm.DVBType == CTTB||parm.DVBType == DVBS/*||parm.DVBType == DVBC*/)
	{
		if (parm.TypeID=="0x1")			//视频黑场
		{
			parm.TypeID="53";
		}
		else if (parm.TypeID=="0x2")	//彩条
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0x4")	//画面静止
		{
			parm.TypeID="54";
		}
		else if (parm.TypeID=="0x8")	//无视频信号
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0xE")	//无载波
		{
			parm.TypeID="51";
		}
		else if (parm.TypeID=="0x10")	//音频静音
		{
			parm.TypeID="52";
		}
		else if (parm.TypeID=="0x5")	//视频黑场+画面静止 -- 黑场
		{
			parm.TypeID="32";
		}
		else if (parm.TypeID=="0x6")	//彩条+画面静止
		{
			parm.TypeID="";
		}
		else
		{
			parm.TypeID="";
		}
	}
	else if(parm.DVBType==RADIO || parm.DVBType==AM)
	{
		if (parm.TypeID=="0x1")			//视频黑场
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0x2")	//彩条
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0x4")	//画面静止
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0x8")	//无视频信号
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0xE")	//无载波
		{
			parm.TypeID="51";
		}
		else if (parm.TypeID=="0x10")	//音频静音
		{
			parm.TypeID="52";
		}
		else if (parm.TypeID=="0x5")	//视频黑场+画面静止 -- 黑场
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="0x6")	//彩条+画面静止
		{
			parm.TypeID="";
		}
		else
		{
			parm.TypeID="";
		}
	}
	return;
}

void StrUtil::ConvertQualityTypeIDToAlarmID(sCheckParam& parm)
{
	if (parm.DVBType == DVBC)
	{
		if (parm.TypeID=="1")	//Level
		{
			parm.TypeID="41";
		}
		else if (parm.TypeID=="2")	//BER
		{
			parm.TypeID="42";
		}
		else if (parm.TypeID=="3")	//C/N
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="4")	//MER
		{
			parm.TypeID="43";
		}
		else if (parm.TypeID=="5")	//EVM
		{
			parm.TypeID="44";
		}
		else if (parm.TypeID=="6")	//Freq Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="7")	//Symbol Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="8")	//EB/N0
		{
			parm.TypeID="";
		}
		else
		{
			parm.TypeID="";
		}
	}
	else if (parm.DVBType == CTTB)
	{
		if (parm.TypeID=="1")	//Level
		{
			parm.TypeID="40";
		}
		else if (parm.TypeID=="2")	//BER
		{
			parm.TypeID="41";
		}
		else if (parm.TypeID=="3")	//CN
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="4")	//MER
		{
			parm.TypeID="42";
		}
		else if (parm.TypeID=="5")	//EVM
		{
			parm.TypeID="43";
		}
		else if (parm.TypeID=="6")	//Freq Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="7")	//Symbol Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="8")	//EB/N0
		{
			parm.TypeID="";
		}
		else
		{
			parm.TypeID="";
		}
	}
	else if (parm.DVBType == DVBS)
	{
		if (parm.TypeID=="1")	//Level
		{
			parm.TypeID="41";
		}
		else if (parm.TypeID=="2")	//BER
		{
			parm.TypeID="42";
		}
		else if (parm.TypeID=="3")	//C/N
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="4")	//MER
		{
			parm.TypeID="43";
		}
		else if (parm.TypeID=="5")	//EVM
		{
			parm.TypeID="44";
		}
		else if (parm.TypeID=="6")	//Freq Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="7")	//Symbol Offset
		{
			parm.TypeID="";
		}
		else if (parm.TypeID=="8")	//EB/N0
		{
			parm.TypeID="";
		}
		else
		{
			parm.TypeID="";
		}
	}
	return;
}

void StrUtil::ConvertEnvTypeIDToAlarmID(sCheckParam& parm)
{
	return;
}

void StrUtil::ConvertEquipTypeIn(string& type)
{
	if(type=="1")
	{
		type="31";
	}
	else if(type=="2")
	{
		type="32";
	}
	else if(type=="3")
	{
		type="33";
	}
	else if(type=="4")
	{
		type="34";
	}
	else if(type=="5")
	{
		type="35";
	}
	else if(type=="6")
	{
		type="36";
	}
	return;
}