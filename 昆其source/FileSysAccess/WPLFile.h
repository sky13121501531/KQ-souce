#pragma once
#include <string>
#include <vector>
using namespace std;
class WPLFile
{
public:
	WPLFile(void);
	~WPLFile(void);
	static bool CreateWPLFile(std::string fileName,std::vector<std::string> vecURL);
	static bool CreateVLCFile(std::string fileName,std::vector<std::string> vecURL);
	static std::string FileSplit(string filename,float precent,bool ishead,bool isAudio);
	static std::string FileSplit(string filename, float StartPrecent, float EndPrecent, bool isAudio);//截头截尾留中间
	static std::string TimeFileSplit( string filename,time_t starttime,time_t endtime,int mode=2 );//mode=0 录像取头 mode=1录像取尾 mode=2取中间  默认取中间
	static time_t GetTimeFromStream(unsigned char *timebuf);
    static bool CreateVLCFileM3U8(std::string fileName,std::vector<std::string> vecURL,std::string strTimestr);

    static void GetStrValue(std::string &str,std::string &strRecord,std::string &strRecordTime);
	static bool WPLFile::CreatVLCTsFile(std::string fileName,std::string sFiletime,std::string HeadTS);
};
