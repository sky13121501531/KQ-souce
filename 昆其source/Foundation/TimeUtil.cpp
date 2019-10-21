#pragma warning(disable:4996)
#include <time.h>
#include "TimeUtil.h"

/// <summary>获得当前的日期时间值</summary>
/// <param name=""></param>
/// <retvalue>string类型日期时间</retvalue>
string TimeUtil::GetCurDateTime()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0 == cur_t) return ret;
	char now[64];	

	sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};

/// <summary>获得当前的时间值</summary>
/// <param name=""></param>
/// <retvalue>string类型时间</retvalue>
string TimeUtil::GetCurTime()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	

	sprintf(now, "%02d:%02d:%02d ",cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};


/// <summary>获得当前的日期值</summary>
/// <param name=""></param>
/// <retvalue>string类型日期</retvalue>
string TimeUtil::GetCurDate()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	

	sprintf(now, "%04d-%02d-%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday);
	ret = now;
	return ret;
};

//时间转换

/// <summary>time_t日期时间转化为字符串</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型日期时间</retvalue>
string  TimeUtil::DateTimeToStr(time_t datetime)
{
	string ret("");
	struct tm *cur_t = localtime(&datetime);
	if (0 == cur_t) return ret;

	char now[64];	
	sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};

/// <summary>string日期时间转化为time_t</summary>
/// <param name="datetime">格式为"yyyy-mm-dd- hh:mm:ss"</param>
/// <retvalue>string类型日期时间</retvalue>
time_t TimeUtil::StrToDateTime(string datetime)
{
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return 0;

	int count = sscanf(datetime.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	if((datetime.find("/") != -1)&&(count < 3))
	{
		count = sscanf(datetime.c_str(), "%u/%u/%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	}
	if (count < 3) return 0;
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>string日期时间转化为time_t</summary>
/// <param name="datetime">格式为"yyyymmddhhmmss"</param>
/// <retvalue>string类型日期时间</retvalue>
time_t TimeUtil::StrTimeToDateTime(string datetime)
{
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return 0;

	int count = sscanf(datetime.c_str(), "%4u%2u%2u%2u%2u%2u", &year, &mon, &day, &hour, &min, &sec);
	if (count < 3) return 0;
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>time_t时间转化为string</summary>
/// <param name="time"></param>
/// <retvalue>string类型时间</retvalue>
string TimeUtil::TimeToStr(time_t time)
{
	string ret("");
	char now[64];
	memset(now,NULL,64*sizeof(char));
	sprintf(now, "%02d:%02d:%02d",  int((time/3600)%24), int((time%3600)/60), int((time%3600)%60));
	ret = now;
	return ret;
};

/// <summary>时间字符串转换为时间类型</summary>
/// <param name="time">格式为"hh:mm:ss"，或"hh:mm"，或"hh"</param>
/// <retvalue>time_t类型时间</retvalue>
time_t TimeUtil::StrToTime(string time)
{
	int year, mon, day, hour, min, sec;
	year = 1970;
	mon = 1;
	day  = 1;
	hour = min = sec =0;
	if (0== time.c_str()) return 0;

	int count = sscanf(time.c_str(), "%u:%u:%u", &hour, &min, &sec);
	if (count == 0) { hour = 0; min = 0; sec = 0; }
	if (count == 1) { min = 0; sec = 0; }
	if (count == 2) { sec = 0; }
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>时间字符串转换为秒</summary>
/// <param name="time">格式为"hh:mm:ss"，或"hh:mm"，或"hh"</param>
/// <retvalue>秒</retvalue>
long TimeUtil::StrToSecondTime(string time)
{
	int year, mon, day, hour, min, sec;
	year = 1970;
	mon = 1;
	day  = 1;
	hour = min = sec =0;
	if (0== time.c_str()) return 0;

	int count = sscanf(time.c_str(), "%u:%u:%u", &hour, &min, &sec);
	if (count == 0) { hour = 0; min = 0; sec = 0; }
	if (count == 1) { min = 0; sec = 0; }
	if (count == 2) { sec = 0; }

	return (long)(((hour*60)+min)*60+sec);
};

//取日期和时间
/// <summary>获得当前日值</summary>
/// <param name=""></param>
/// <retvalue>string类型的日值</retvalue>
string TimeUtil::GetCurDay()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%02d",  cur_t->tm_mday);
	ret = now;
	return ret;
};

/// <summary>获得当前月份值</summary>
/// <param name=""></param>
/// <retvalue>string类型的月份值</retvalue>
string TimeUtil::GetCurMonth()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%02d",  cur_t->tm_mon+1);
	ret = now;
	return ret;
};

/// <summary>获得当前年份值</summary>
/// <param name=""></param>
/// <retvalue>string类型的年份值</retvalue>
string TimeUtil::GetCuryear()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%04d",  cur_t->tm_year+1900);
	ret = now;
	return ret;
};

/// <summary>依据string参数获得当前年份值</summary>
/// <param name="datetime">格式为yyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string类型的年份值</retvalue>
string  TimeUtil::GetYearFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string syear = "0000";
	sprintf((char *)syear.c_str(),"%04d",year);
	return syear;
};

/// <summary>依据time_t参数获得当前年份值</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型的年份值</retvalue>
string  TimeUtil::GetYearFromDatetime(time_t datetime)
{
	string ret("");
	ret = DateTimeToStr(datetime);

	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%04d",year);
	return ret;
};

/// <summary>依据string参数获得当前年份值</summary>
/// <param name="datetime">格式为yyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string类型的月份值</retvalue>
string  TimeUtil::GetMonthFromDatetime(string datetime)
{
	string ret = datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string smon = "00";
	sprintf((char *)smon.c_str(),"%02d",mon);;
	return smon;
};

/// <summary>依据time_t参数获得当前年份值</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型的月份值</retvalue>
string  TimeUtil::GetMonthFromDatetime(time_t datetime)
{
	string ret = DateTimeToStr(datetime);
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d",mon);;
	return ret;
};

/// <summary>依据string参数获得当前日值</summary>
/// <param name="datetime">格式为yyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string类型的日值</retvalue>
string  TimeUtil::GetDayFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string sday = "00";
	sprintf((char *)sday.c_str(),"%02d",day);
	return sday;
};

/// <summary>依据time_t参数获得当前年份值</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型的日值</retvalue>
string  TimeUtil::GetDayFromDatetime(time_t datetime)
{
	string ret("");
	ret = DateTimeToStr(datetime);

	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d",day);
	return ret;
};

/// <summary>依据string参数获得当前日期值</summary>
/// <param name="datetime">格式为yyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string类型的日期值</retvalue>
string TimeUtil::GetDateFromDatetime(string datetime)
{
	string year,month,day;
	char ret[100];
	memset(ret,0,100);
	year = GetYearFromDatetime(datetime);
	month = GetMonthFromDatetime(datetime);
	day = GetDayFromDatetime(datetime);
	sprintf(ret,"%s-%s-%s",year.c_str(),month.c_str(),day.c_str());
	return ret;
};

/// <summary>依据time_t参数获得当前日期值</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型的日期值</retvalue>
string TimeUtil::GetDateFromDatetime(time_t datetime)
{
	string year,month,day;
	char ret[100];
	memset(ret,0,100);
	year = GetYearFromDatetime(datetime);
	month = GetMonthFromDatetime(datetime);
	day = GetDayFromDatetime(datetime);
	sprintf(ret,"%s-%s-%s",year.c_str(),month.c_str(),day.c_str());
	return ret;
};

/// <summary>依据string参数获得当前时间值</summary>
/// <param name="datetime">格式为yyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string类型的时间值</retvalue>
string TimeUtil::GetTimeFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = ::sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d:%02d:%02d",hour,min,sec);
	return ret;        
};

/// <summary>依据time_t参数获得当前时间值</summary>
/// <param name="datetime"></param>
/// <retvalue>string类型的时间值</retvalue>
string TimeUtil::GetTimeFromDatetime(time_t datetime)
{
	string ret = DateTimeToStr(datetime);
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d:%02d:%02d",hour,min,sec);
	return ret;     
};

//时间计算
/// <summary>获取两参数之间相差的日数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long TimeUtil::DiffDay(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEDAYLONG);
};

/// <summary>获取两参数之间相差的日数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DiffDay(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEDAYLONG);
};


long TimeUtil::DiffDate(string SrcDate,string DesDate)		//比较日期(不带具体时间，只有日期)
{
	string time = " 00:00:00";
	string SrcDateTime = SrcDate + time;
	string DesDateTime = DesDate + time;

	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEDAYLONG);
}


/// <summary>获取两参数之间相差的小时数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DiffHour(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEHOURLONG);
};

/// <summary>获取两参数之间相差的小时数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DiffHour(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEHOURLONG);
};

/// <summary>获取两参数之间相差的分钟数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DiffMinute(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEMINUTELONG);
};

/// <summary>获取两参数之间相差的分钟数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DiffMinute(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEMINUTELONG);
};

/// <summary>获取两参数之间相差的秒数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long TimeUtil::DiffSecond(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)(Src - Dst);
};

/// <summary>获取两参数之间相差的秒数</summary>
/// <param name="SrcDateTime">时间参数</param>
/// <param name="DesDateTime">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long TimeUtil::DiffSecond(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)(SrcDateTime - DesDateTime);
};

//星期计算
/// <summary>根据日期参数获得星期值</summary>
/// <param name="date">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DateIsWeekDay(string date)
{
	time_t d = StrToDateTime(date);
	struct tm *cur_t = localtime(&d);
	if (0 == cur_t) return -1;
	return cur_t->tm_wday;
};

/// <summary>根据日期参数获得星期值</summary>
/// <param name="date">时间参数</param>
/// <retvalue>long类型的整数</retvalue>
long  TimeUtil::DateIsWeekDay(time_t date)
{
	struct tm * cur_t = localtime(&date);
	if (0 == cur_t) return -1;
	return cur_t->tm_wday;
};

//时间加减
/// <summary>将时间加上指定的天数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Days">天数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalDay(string datetime,int Days)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Days * ONEDAYLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>将时间加上指定的天数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Days">天数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalDay(time_t datetime, int Days)
{
	time_t curd = datetime;
	curd += (Days * ONEDAYLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>将时间加上指定的小时数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Hours">小时数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalHour(string datetime, int Hours)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Hours * ONEHOURLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>将时间加上指定的小时数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Hours">小时数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalHour(time_t datetime, int Hours)
{
	time_t curd = datetime;
	curd += (Hours * ONEHOURLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>将时间加上指定的分钟数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Minutes">分钟数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalMinutes(string datetime, int Minutes)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Minutes * ONEMINUTELONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>将时间加上指定的分钟数</summary>
/// <param name="datetime">时间参数</param>
/// <param name="Minutes">分钟数</param>
/// <retvalue>string类型的时间</retvalue>
string TimeUtil::CalMinutes(time_t datetime, int Minutes)
{
	time_t curd = datetime;
	curd += (Minutes * ONEMINUTELONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

std::string TimeUtil::DateTimeToString( time_t time )
{
	string ret("");
	struct tm *cur_t = localtime(&time);
	if (0 == cur_t) return ret;

	char now[64];	
	sprintf(now, "%04d%02d%02d%02d%02d%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
}

std::string TimeUtil::GetEndLessTime()
{
	return DateTimeToStr(StrToDateTime(CalDay(TimeUtil::GetCurDateTime(),365*10)));
}