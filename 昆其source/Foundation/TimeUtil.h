#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_

#include <string>
using namespace std;

const int ONEMINUTELONG = 60;
const int ONEHOURLONG = ONEMINUTELONG * 60;
const int ONEDAYLONG = ONEHOURLONG * 24;

class TimeUtil
{
public:
	//当前时间
	static string GetCurDateTime();
	static string GetCurTime();
	static string GetCurDate();
	//时间转换
	static string  DateTimeToStr(time_t datetime);
	static time_t StrToDateTime(string datetime); 
	static string TimeToStr(time_t time);
	static time_t StrToTime(string time);
	static long StrToSecondTime(string time);
	//具体时间
	static string GetCurDay();
	static string GetCurMonth();
	static string GetCuryear();
	static string GetYearFromDatetime(string datetime);
	static string GetYearFromDatetime(time_t datetime);
	static string GetMonthFromDatetime(string datetime);
	static string GetMonthFromDatetime(time_t datetime);
	static string GetDayFromDatetime(string datetime);
	static string GetDayFromDatetime(time_t datetime);
	static time_t StrTimeToDateTime(string datetime);
	static string GetDateFromDatetime(string datetime);
	static string GetDateFromDatetime(time_t datetime);
	static string GetTimeFromDatetime(string datetime);
	static string GetTimeFromDatetime(time_t datetime);
	//时间差异
	static long DiffDay(string SrcDateTime,string DesDateTime);
	static long DiffDay(time_t SrcDateTime,time_t DesDateTime);
	static long DiffHour(string SrcDateTime,string DesDateTime);
	static long DiffHour(time_t SrcDateTime,time_t DesDateTime);
	static long DiffMinute(string SrcDateTime,string DesDateTime);
	static long DiffMinute(time_t SrcDateTime,time_t DesDateTime);
	static long DiffSecond(string SrcDateTime,string DesDateTime);
	static long DiffSecond(time_t SrcDateTime,time_t DesDateTime);

	static long DiffDate(string SrcDate,string DesDate);		//比较日期(不带具体时间，只有日期)		add by jidushan 110704
	//星期计算 
	static long  DateIsWeekDay(string date);
	static long  DateIsWeekDay(time_t date);
	//时间加减
	static string CalDay(string datetime,int Days);
	static string CalDay(time_t datetime, int Days);
	static string CalHour(string datetime, int Hours);
	static string CalHour(time_t datetime, int Hours);
	static string CalMinutes(string datetime, int Minutes);
	static string CalMinutes(time_t datetime, int Minutes);
	//保存录像文件使用
	static string DateTimeToString(time_t time);
	//获得无穷大时间
	static string GetEndLessTime();
};
#endif