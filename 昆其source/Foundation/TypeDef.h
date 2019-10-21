#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_
#pragma once
///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TypeDef.h
// 创建者：gaoxd
// 创建时间：2009-05-20
// 内容描述：自定义类型定义，建议多处使用的自定义类型统一在此处定义
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <vector>
#include <map>
#include "ace/SOCK_Acceptor.h"
#include "TimeUtil.h"
#include "Config.h"
class XMLTask;

const std::string strHDMIFreq = "Set-Top-Box";	//机顶盒相关任务的Freq = "Set-Top-Box"
const int ChassisMaxNum = 8;
const int SlotMaxNum = 64;
const int ModulMaxNum = 6;

//定时任务时间段分类
typedef enum eDVBType 
{
	UNKNOWN	= 0,
	CTTB	= 1,
	CMMB	= 2,
	DVBC	= 3,
	RADIO	= 4,
	ATV		= 5,//安播开路电视
	AM		= 6,
	DVBS    = 7,
	CTV     = 8,//有线电视
	THREED  = 9,
	HDMI	= 10

} enumDVBType;
/*
//定时任务时间段分类
typedef enum eFrequence 
{
	ONCE = 0,
	DAILY,
	WEEKLY,
	MONTHLY
} enumFrequence;
*/
//任务状态类型定义
typedef enum eTaskStatus 
{
	WAITING,			//等待（生成任务时状态）
	READY,				//就绪，等待被调度
	RUNNING,			//运行中
	FINISHED,			//单次运行结束
	EXPIRED				//任务过期（单次任务结束后为过期；定时任务最后一次执行完毕后为过期）
} enumTaskStatus;

//任务设置/运行结果状态值
typedef enum eTaskRetStatus 
{
	RUN_SUCCESS				= 0,			//运行成功
	RUN_FAILED				= 1,			//运行失败，无描述
	NOFILE_EXIST		    = 2,            //文件不存在
	CHANNELID_UNAVAIABLE	= 3,			//频道非法
	DEVICEID_UNAVAIABLE		= 4,			//通道非法
	NODEVICEIDMATCH			= 5,			//没有通道匹配该任务
	PREFERENTIALTASK_USE	= 6,			//存在高优先级任务占用
	DATABASEERROR			= 7,			//操作数据库错误
	DEVICEERROR				= 8,			//操作硬件错误
	WAITFORSET				= 100,			//等待设置
	SET_SUCCESS				= 101,			//设置成功
	RECORD_DOWN_OUTOFRAND	=102,			//录像时间超界限(下载)
	ADRESS_ERROR			=103,           //网络存储路径不正确
	USERNAME_ERROR			=104,			//用户名或密码不正确
	CHANNELID_DEVICEID_NO	=105,			//节目频道和通道不匹配
	NO_RECORD				=106,			//时间段无录像文件
	CHANLID_CHANGE			=107,			//请上传当前频道录制文件
	TIMEFORMAT_ERROR		=108,			//时间格式有误(上传开始任务时间默认数据库最早时间,任务结束时间不可为空)
	RECODESOUCE_OUTOFRAND	=109,			//转码卡资源不足
} enumTaskRetStatus ;	


//日志类型定义
typedef enum eLogType              
{
	LOG_EVENT_DEBUG = 0,		//调试信息
	LOG_EVENT_WARNNING = 1,		//警告信息
	LOG_EVENT_ERROR = 2			//错误信息
}enumLogType;

//日志输出类型
typedef enum eLogOutPut
{
	LOG_OUTPUT_UNDEFINE = 0,	//未定义输出类型
	LOG_OUTPUT_SCREEN	= 1,	//输出到屏幕
	LOG_OUTPUT_FILE		= 2,	//输出到文件
	LOG_OUTPUT_BOTH		= 3		//输出到屏幕和文件
}enumLogOutPut;

//模块类型定义
typedef enum eModuleType              
{
	NONE			= 0,		//无类型，属于错误
	RECORD			= 1,		//录像
	VIDEO			= 2,		//视频，包含实时视频，视频轮播，录像视频查看
	DATAANALYSER	= 3,		//指标测量，PSI/SI分析以及其他码流分析
	ALARM			= 4,		//报警		
	OTHER			= 5,		//设置型任务
	DEVICE			= 6,		//硬件层日志
	TASK			= 7,		//任务日志
	DATABASE		= 8			//数据库操作记录日志

}enumModuleType;

//日志保存文件夹属性设置   //FRee_fanrong_20140109
typedef struct  
{
	eModuleType Module;
	string LogPath;
}sLogPath;
typedef std::list<sLogPath> sLogPathModuleInfo;  


//报警类型
typedef enum eAlarmType
{
	ALARM_NONE			      = 0,	//未定义报警类型
	ALARM_ENVIRONMENT	      = 1,	//环境报警类型
	ALARM_FREQ			      = 2,	//射频报警类型
	ALARM_PROGRAM		      = 3,	//节目报警类型
	ALARM_TR101_290		      = 4,	//TR101_290报警类型
	ALARM_EQUIPMENT		      = 5,	//设备故障报警类型
	ALARM_RECORDSCHEDULER     = 6   //录像冲突报警
}enumAlarmType;

//系统消息类型定义
typedef enum eSysMsgType              
{
	VS_MSG_SYSTINFO = 0,		//系统普通信息
	VS_MSG_PROALARM = 1,		//节目异态/报警信息
	VS_MSG_SYSALARM = 2			//系统报警信息
}enumSysMsgType;

//节目类型
typedef enum ProgramType
{
	H264_SD  = 0,              //H264编码标清节目
	H264_HD	 = 1,			   //H264编码高清节目
	MPEG2_SD = 2,			  //MPEG2编码标清节目
	MPEG2_HD = 3			 //	MPEG2编码标清节目
};
//***********************************************
//
struct AlarmParamConfig
{
	eDVBType dvbType;
	std::string s_mutehold;
	std::string s_bfhold;
	std::string s_sthold;
	AlarmParamConfig()
	{
		dvbType = UNKNOWN;
		s_mutehold = "";
		s_bfhold = "";
		s_sthold = "";
	}
};
struct VirtualKeyInfo
{
	std::string s_Index;
	std::string s_Name;
	std::string s_Key;
	VirtualKeyInfo()
	{
		s_Index = "";
		s_Name = "";
		s_Key = "";
	}
};
//高清节目信息配置文件
struct HDInfo
{
	std::string sFreq;
	std::string sServiceID;
	std::string sHDType;
};
//高清节目信息分辨率获取
struct HDInfoForScan
{
	std::string sFreq;
	std::string sServiceID;
	std::string sHD;
};

struct sEPGInfo
{
	std::string sFreq;
	std::string sServiceID;
	std::string sProgram;
	std::string sProgramType;
	std::string sStartTime;
	std::string sProgramLen;
	std::string sState;
};

//HDMI的配置信息
struct HDMIInfo
{
	std::string s_DevIndex;//设备索引
	std::string s_HDMIChannelNum;//HDMI 通道号
	std::string s_COMPort;
	unsigned char c_IDCode;		//红外ID号
	int i_IntervalTime;		//红外命令间隔时间
	HDMIInfo()
	{
		s_DevIndex = "";
		s_HDMIChannelNum = "";
		s_COMPort = "";
		c_IDCode = 0;
		i_IntervalTime = 0;
	}
};

//有线数字 获取场强指标 设备信息
struct Quality_DevInfo
{
	int i_index;
	int i_port;
	std::string s_qualityIP;
	std::string s_cmdProtocol;
	std::string s_devType;
	std::string s_usedType;//指标测量1是必须使用配置通道,0表示存在录像可获取录像通道电平
	map<string, string> mapDvbTypeChls;
	Quality_DevInfo()
	{
		i_index = 0;
		i_port = 0;
		s_qualityIP = "";
		s_cmdProtocol = "";
		s_devType = "";
		s_usedType = "1";
	}
};

typedef struct   
{
	std::string s_scantype;
	std::string s_freq;
	std::string s_weeklyswitch;
	std::string s_epg;
	std::string s_scanTime;//定时扫描的时间
	std::string s_checkHD;//0:通过节目名字区分高清、1:通过分辨率检测高清
	std::string s_scanversion;//扫描支持版本
}Scaninfo;
//设备信息定义
typedef struct
{
	std::string s_deviceindex; // 设备编号 1U设备从配置文件获取 标准化设备 机箱号 真实
	std::string s_deviceip; //设备ip地址
	std::string s_deviceport; //设备ip端口
	std::string s_devicecampronum; //设备解扰节目个数,cam卡有2路的,8路的等,解调解扰设备最大8路
	std::string s_unuseditem; //不用项
	std::string s_manufacturer;
	int i_HDnum;
	int i_SDnum;
	int i_TcoderNum;
	//
	std::string s_cardindex;//卡槽号
	std::string s_modindex;//模块号 
	unsigned char ucharChanNum;
	unsigned char ucharBoardType;//0-解调卡，1-转码卡	
	unsigned char ucharMpeg2Needs; 
	unsigned char ucharH264Needs; 
	unsigned char ucharAVSNeeds;
	unsigned char ucharH265Needs;	
}sDeviceInfo;//1U设备为1个机箱 标准化设备为1个通讯单元
typedef struct  
{
	eDVBType e_tunerdvbtype;
	std::string s_tunerdeviceid;//全局逻辑通道号
	std::string s_monitorindex;//机箱号
	std::string s_cardindex;//卡槽号
	std::string s_modindex;//模块号
	std::string s_chanindex;//通道号
	std::string s_freq; //频率(K)
	std::string s_sym; //符号率
	std::string s_qam; //调制方式
	std::string s_sip; //解调输出TS流IP 转码输入
	std::string s_stsport; //解调输出TS流端口  转码输入
	std::string s_trport; //保留
	std::string s_desc; 
	std::string s_290ip; //290报警ip
	std::string s_290port; //290报警端口
	std::string s_servidList; //配置的解扰节目列表
	std::string s_devicecampronum; //设备解扰节目个数,cam卡有2路的,8路的等,解调解扰设备最大8路
	std::string s_manufacturer;
	std::string s_serialnum;
	std::string s_ICcardNO;
	std::string s_ExistCmd;//已经配置的命令
	bool b_user;
	std::vector<string> vecAllServid;
}TunerDev;
typedef struct  
{
	std::string s_coderdeviceid;//全局逻辑通道号
	std::string s_monitorindex;//机箱号 真实
	std::string s_cardindex;//卡槽号
	std::string s_modindex;//模块号
	std::string s_modcoderindex;//模块通道号 配合平台多模块 标准化设备模块通道号和通道号相同
	std::string s_coderindex;//通道号 真实 标准化设备模块通道号和通道号相同
	std::string s_ena; //1:使能使能黑场/静视帧/静音,  0:停止 
	std::string s_spro; //转码源节目id
	std::string s_code; //mpeg2\h264\h265
	std::string s_fmt; //0:自动,1:D1,2:CIF
	std::string s_fre; //0:自动,其它:帧率
	std::string s_rate; //码率
	std::string s_sip; //解调输出TS流IP 转码输入
	std::string s_stsport; //解调输出TS流端口  转码输入
	std::string s_dip; //转码输出IP
	std::string s_dtsport; //转码输出端口
	std::string s_alarmip; //画面报警ip
	std::string s_alarmport; //画面报警端口
	std::string s_freq; //频点
	std::string s_manufacturer;
	std::string s_serialnum;
	std::string s_desc; 
	std::string s_maincoderdeviceid;//用于高清使用多通道时,记录它的主通道
	bool b_user;
	bool b_ishd;
	eDVBType e_tunerdvbtype;//多信号用于任务的清楚
}CoderDev;

typedef struct
{
	std::string s_coderdeviceid; 
	std::string s_tunerdeviceid; 
	eDVBType e_tunerdvbtype;
	std::string Priority;	//优先级
	std::string Taskid;	
}sDevLinkInfo;

typedef struct
{
	std::string s_taskxml;
	std::string s_taskid; 
	std::string s_active; 
	std::string s_channelid; 
	std::string s_devcodehead; 
	std::string s_uploadsucenddtime; 
	std::string s_uploadsucfilename;
	std::string s_taskinfo;
}sRecordUploadTaskInfo;
//
typedef struct
{
	unsigned char BoardInfo[8];
	bool ExistCard; 
}standardBoardCheckInfo;
typedef struct
{
	unsigned char ucharChassis; 
	unsigned char ucharSlot; 
	unsigned char ucharChanNum;
	unsigned char ucharBoardType;	
	unsigned char ucharIPAddr[4];
	unsigned short ushortPort;
	unsigned short ushortReserved;
	unsigned char ucharMpeg2Needs; 
	unsigned char ucharH264Needs; 
	unsigned char ucharAVSNeeds;
	unsigned char ucharH265Needs;
	time_t startTime;
}standardChannelResource;
typedef struct
{
	unsigned char ucharLevel; 
	unsigned char ucharSubType; 
	unsigned char ucharReason;
	unsigned char ucharReserved;	
	unsigned int  uintPid;
	unsigned int  uintAlarmTime;
}standardTR290Result;
typedef struct
{
	unsigned char ucharLevel; 
	unsigned char ucharlockStatus; 
	unsigned short ushortMer;
	unsigned short ushortEVM;	
	unsigned int  uintBer;
	unsigned short  ushortCNR;
}standardRFResult;
typedef struct
{
	unsigned short ushortVStatus; 
	unsigned short ushortAStatus; 
	unsigned int  uintAlarmTime;
}standardAVErrorStatus;

//**********************************************
typedef struct
{
	int CardIndex; //板卡编号 //FRee_fanrong_20131212
	std::string devicetype; // 设别类型
	std::string TSIP; //tsip地址
	std::string CMDIP; //cmdip地址
	std::string cmdport;//命令端口号
	std::string tsport;//数据端口号
	std::string cmdprotocol;//命令协议
	std::string tsprotocol;//数据协议
	std::string tunerid;//tuner通道号
	std::string logindex;//逻辑通道
	std::string recordlist;//录像通道
}sChanelInfo;

typedef std::map<std::string,std::list<int> > TaskInfoMap;
typedef std::vector<sRecordUploadTaskInfo> vTaskUp;
struct sdvbsfreqinfo
{
	std::string slocfreq;
	std::string sdownfreq;
	std::string shv;
	std::string ssymbol;
	std::string sfec;
	std::string s22k;
};

typedef struct
{
	int DevicID;	//设备ID
	int Statuc; //设备状态     可取值：0 -正常状态  1 -无板卡  2 - 有任务运行
	std::string Desc;	//设备描述   
}sDeviceStatus;

//视频用户信息
typedef struct VEDIOUSERINFO
{
	ACE_SOCK_Stream client;	//用户Socket
	eDVBType DvbType;
	int fail_num;			//发送错误次数
	std::string DeviceID;	//用户使用的通道号
	std::string Name;		//名称
	std::string URL;		//音视频地址
	std::string IP;			//地址
	std::string Port;		//端口
	std::string Priority;	//优先级
	std::string CenterCode;		//中心号
	std::string MsgID;
	std::string Freq;  //频点信息

	VEDIOUSERINFO()
	{
		DvbType = UNKNOWN;
		fail_num = 0;
		DeviceID = "";
		Name = "";
		IP = "";
		Port = "";
		URL = "";
		Priority = "";
		CenterCode = "";
		MsgID = "";
		Freq="";
	};
	VEDIOUSERINFO&	operator =(const VEDIOUSERINFO& User)
	{
		client= User.client;
		DvbType = User.DvbType;
		fail_num = User.fail_num;
		DeviceID = User.DeviceID;
		Name = User.Name;
		IP = User.IP;
		Port = User.Port;
		URL = User.URL;
		Priority = User.Priority;
		CenterCode=User.CenterCode;
		MsgID = User.MsgID;
		Freq = User.Freq;
		return *this;
	};
	bool	operator ==(const VEDIOUSERINFO& User)
	{
		return client.get_handle()==User.client.get_handle()&&DeviceID==User.DeviceID&&Name==User.Name&& \
				IP==User.IP&&Port==User.Port&&Priority==User.Priority&& CenterCode==User.CenterCode && MsgID==User.MsgID && Freq==User.Freq;
	};

}sVedioUserInfo;

//频道扫描信息定义
typedef struct CHANNELINFO{
	//关键频道信息
	std::string ChannelID;
	std::string STD;
	std::string Freq; 
	std::string OrgNetID; 
	std::string TsID;
	std::string PmtPID;  //wz_101123
	//频道信息
	std::string SymbolRate; 
	std::string QAM;
	std::string HeaderMode;
	std::string CarrierNumber;
	std::string EncodeEfficiency;
	std::string InterwovenMode;
	std::string DoublePilot; 
	std::string PnPhase;
	std::string ChannelCode;
	std::string ProgramName;
	std::string ServiceID;
	std::string VideoPID;
	std::string AudioPID;
	std::string HDTV;
	std::string Encryption; //1表示扰流 0表示清流
	std::string FreeCA; //1表示授权或清流 0表示未授权扰流 与存储文件ICInfoChannelEncryptQuery_LastReport和ChannelScanQuery.xml的项Encrypt项对应
	std::string PcrPID;  //wz_101123
	std::string VideoStreamType;
	std::string AudioStreamType;
	std::string ServiceType;   //FRee_fanrong_20140110
public:
	CHANNELINFO(const CHANNELINFO& other) //copy construction
	{
		ChannelID = other.ChannelID;
		STD=other.STD;
		Freq=other.Freq;
		OrgNetID=other.OrgNetID;
		TsID=other.TsID;
		PmtPID = other.PmtPID;  //wz_101123

		SymbolRate=other.SymbolRate;
		QAM=other.QAM;
		HeaderMode=other.HeaderMode;
		CarrierNumber=other.CarrierNumber;
		EncodeEfficiency=other.EncodeEfficiency;
		InterwovenMode=other.InterwovenMode;
		DoublePilot=other.DoublePilot;
		PnPhase=other.PnPhase;
		ChannelCode=other.ChannelCode;
		ProgramName = other.ProgramName;
		ServiceID = other.ServiceID;
		VideoPID = other.VideoPID;
		AudioPID = other.AudioPID;
		HDTV=other.HDTV;
		Encryption = other.Encryption;
		FreeCA = other.FreeCA;
		PcrPID = other.PcrPID;  //wz_101123
		VideoStreamType = other.VideoStreamType;
		AudioStreamType = other.AudioStreamType;
		ServiceType = other.ServiceType; //FRee_fanrong_20140110
	};
   CHANNELINFO()
   {

   };
   bool	operator ==(const CHANNELINFO& other)
   {
	   return STD==other.STD&&Freq==other.Freq&&OrgNetID==other.OrgNetID&&TsID==other.TsID
		   &&ServiceID==other.ServiceID&&VideoPID==other.VideoPID&&AudioPID==other.AudioPID//;
		   &&PmtPID==other.PmtPID&&PcrPID==other.PcrPID;  //wz_101123
   };
   bool	operator >(const CHANNELINFO& other)
   {
	   return true;
   };
   friend  bool	 operator <(const CHANNELINFO& other1,const CHANNELINFO& other2)
   {
	   return other1.Freq<other2.Freq;
   };
   bool	operator <(const CHANNELINFO& other)
   {
	   return Freq<other.Freq;
   };
   bool	operator ()(const CHANNELINFO& other)
   {
	   return true;
   };
   CHANNELINFO&	operator =(const CHANNELINFO& other)
   {
	   ChannelID = other.ChannelID;
	   STD=other.STD;
	   Freq=other.Freq;
	   OrgNetID=other.OrgNetID;
	   TsID=other.TsID;
	   PmtPID = other.PmtPID;  //wz_101123

	   SymbolRate=other.SymbolRate;
	   QAM=other.QAM;
	   HeaderMode=other.HeaderMode;
	   CarrierNumber=other.CarrierNumber;
	   EncodeEfficiency=other.EncodeEfficiency;
	   InterwovenMode=other.InterwovenMode;
	   DoublePilot=other.DoublePilot;
	   PnPhase=other.PnPhase;
#ifndef ZONG_JU_ZHAO_BIAO
	   if (other.ChannelCode!="")
	   {
#endif
		   ChannelCode=other.ChannelCode;
#ifndef ZONG_JU_ZHAO_BIAO
	   }
#endif
	 
#ifndef ZONG_JU_ZHAO_BIAO
	   if (other.ProgramName!="")
	   {
#endif
		   ProgramName = other.ProgramName;
#ifndef ZONG_JU_ZHAO_BIAO
	   }
#endif
	   
	   ServiceID = other.ServiceID;
	   VideoPID = other.VideoPID;
	   AudioPID = other.AudioPID;
	   HDTV=other.HDTV;
	   Encryption = other.Encryption;
	   FreeCA = other.FreeCA;
	   PcrPID = other.PcrPID;  //wz_101123
	   VideoStreamType = other.VideoStreamType;
	   AudioStreamType = other.AudioStreamType;
	   ServiceType = other.ServiceType; //FRee_fanrong_20140110
	   return *this;
   };

}sChannelInfo;

typedef std::map<enumDVBType,std::vector<sChannelInfo> > ChannelInfoMap;
typedef std::map<std::string,std::vector<sChannelInfo>>	STRCHANINFOMAP;

//录像信息
typedef struct{
	enumDVBType dvbtype;
	std::string channelID;
	std::string DeviceID;
	std::string taskid;
	std::string filename;
	std::string starttime;
	std::string endtime;
	std::string expiredays;
}sRecordInfo;
//自动录像文件信息
typedef struct
{
	std::string url;
	std::string starttime;
	std::string endtime;

}sRecordFileInfo;
//中心信息
typedef struct  
{
	std::string centerType;
	std::string centerUrl;
}sCenterInfo;

//报警参数
typedef struct AlarmParam{
	enumDVBType DVBType;			//监测类型
	enumAlarmType AlarmType;		//报警类型
	std::string TypeID;				//报警ID
	std::string TypeDesc;			//报警描述
	std::string STD;				//发射标准
	std::string Freq;				//频率
	std::string SymbolRate;			//符号率
	std::string ChannelID;			//频道ID
	std::string Duration;			//持续时间
	std::string Num;				//发生次数
	std::string TimeInterval;		//时间间隔
	std::string DownThreshold;		//下限
	std::string UpThreshold;		//上限
	std::string Switch;				//开关  1：开 0 ： 关
	std::string DeviceID;           //设备通道号
	
	AlarmParam& operator = (const AlarmParam param)
	{
		DVBType=param.DVBType;
		AlarmType=param.AlarmType;
		TypeID=param.TypeID;
		if (param.TypeDesc!="")
		{
			TypeDesc=param.TypeDesc;
		}
		if(param.STD!="")
		{
			STD=param.STD;
		}
		if (param.TypeDesc!="")
		{
			TypeDesc=param.TypeDesc;
		}
		if(param.Freq!="")
		{
			Freq=param.Freq;
		}
		if (param.SymbolRate!="")
		{
			SymbolRate=param.SymbolRate;
		}
		if(param.ChannelID!="")
		{
			ChannelID=param.ChannelID;
		}
		if (param.Duration!="")
		{
			Duration=param.Duration;
		}
		if(param.Num!="")
		{
			Num=param.Num;
		}
		if (param.TimeInterval!="")
		{
			TimeInterval=param.TimeInterval;
		}
		if(param.DownThreshold!="")
		{
			DownThreshold=param.DownThreshold;
		}
		if (param.UpThreshold!="")
		{
			UpThreshold=param.UpThreshold;
		}
		if(param.Switch!="")
		{
			Switch=param.Switch;
		}
		if(param.DeviceID!="")
		{
			DeviceID=param.DeviceID;
		}

	return *this;
	}
}sAlarmParam;

typedef std::vector<sAlarmParam> AlarmParamVec;
typedef std::map<std::string,std::vector<sAlarmParam>> CodeAlarmParamMap;

//检查报警需要的参数
typedef struct CheckParam{
	enumDVBType DVBType;			//监测类型
	enumAlarmType AlarmType;		//报警类型
	std::string TypeID;				//报警ID
	std::string TypeDesc;			//报警描述
	std::string STD;				//发射标准
	std::string Freq;				//频率
	std::string SymbolRate;			//符号率
	std::string ChannelID;			//频道ID
	std::string TypedValue;			//测量值
	time_t CheckTime;
	std::string AlarmID;
	std::string DeviceID;           //设备通道号
	std::string Cardindex;			//卡槽号
	std::string Modindex;			//模块号
	std::string mode;
	std::string ServiceID;
	std::string VideoID;
	std::string AudioID;
	std::string StrCheckTime;
	std::string StartTime;
	std::string EndTime;
}sCheckParam;
//报警上报方式
struct  AlarmInfo
{
	enumDVBType DVBType;
	enumAlarmType AlarmType;
	std::string STD;				//发射标准
	std::string Freq;				//频率
	std::string SymbolRate;			//符号率
	std::string ChannelID;			//频道ID
	std::string TypeID;				//报警ID
	std::string TypeDesc;			//报警描述
	std::string AlarmValue;			//报警值
	std::string CheckTime;
};
//报警结果信息
struct AlarmResult
{
	enumDVBType DVBType;			//监测类型
	enumAlarmType AlarmType;		//报警类型
	std::string TypeID;				//报警ID
	std::string TypeDesc;			//报警描述
	std::string STD;				//发射标准
	std::string Freq;				//频率
	std::string SymbolRate;			//符号率
	std::string ChannelID;			//频道ID
	std::string TypedValue;			//报警值
	std::string AlarmTime;			//发生报警时间
};
typedef struct  RunPlanParam
{
	enumDVBType dvbtype;			//监测类型
	std::string ChannelID;			//频道ID
	int Type;						//时间段类型 0：单次；1：多次
	std::string Month;				//月份
	std::string DayOfMonth;			//月份中的天数
	std::string DayOfWeek;			//星期
	std::string StartTime;			//开始时间
	std::string EndTime;			//结束时间
	std::string ValidStartDateTime;	//有效起始时间
	std::string ValidEndDateTime;	//有效结束时间
	std::string StartDateTime;		//开始时间
	std::string EndDateTime;		//结束时间
}sRunPlanParam;

typedef std::vector<sRunPlanParam> RunPlanParamVec;
typedef std::map< std::string, RunPlanParamVec > RunPlanParamMap;

typedef struct QualityInfo{
	std::string type;
	std::string valu;
	std::string desc;
	std::string STD;
	std::string checktime;
	std::string freq;
}eQualityInfo;

typedef struct QualityDBInfo{
	enumDVBType s_dvbtype;
	std::string s_strTaskId;
	eQualityInfo s_quaInfo;
}sQualityDBInfo, *pQualityDBInfo;

typedef struct SpecInfo
{
	std::string type;
	std::string valu;
	std::string desc;
	std::string STD;
	std::string checktime;
	std::string freq;
	std::string status;
}eSpecInfo;

//CPU信息
typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;
typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;
typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef struct task
{
//	XMLTask* pTask;

	eDVBType dvbtype;
	eTaskStatus status;

	std::string msgid;
	std::string setdatetime;	//任务下发日期
	int deviceid;
	time_t startdatetime;		//开始时间日期
	time_t enddatetime;			//开始时间日期
	long cycleinterval;			//周期间隔

	std::string taskid;
	std::string taskname;
	std::string freq;
	std::string channelid;
	std::string priority;
	std::string runpriority;
	std::string ServiceID;
	std::string VideoPID;
	std::string AudioPID;
} sTaskInfo;

typedef struct qualityCompenstaion
{
	eDVBType dvbtype;
	std::string deviceid;
	std::string type;
	std::string valu;
	std::string freq;
	std::string level;
} sQualityCompensation;
//环境信息
struct EnvInfo
{
	std::string Type;
	std::string Desc;
	EnvInfo()
	{

	};
	bool operator ==(const EnvInfo& other) const
	{
		return Type==other.Type&&Desc==other.Desc;
	};
	bool operator >(const EnvInfo& other) const
	{
		return Type > other.Type;
	};
	friend  bool operator > (const EnvInfo& other1,const EnvInfo& other2)
	{
		return other1.Type>other2.Type;
	};
	friend  bool operator < (const EnvInfo& other1,const EnvInfo& other2)
	{ 
		return other1.Type<other2.Type;
	};
	bool operator ()(const EnvInfo& other) const
	{
		return true;
	};
};
typedef std::map<EnvInfo,std::string>  EnvMapInfo;


struct sFreqScanInfo{
	std::string CenterFreq;
	std::string SymbolRate;
	std::string QAM;
};

struct OSDInfo{
	std::string FontSize;
	std::string Info;
	std::string InfoX;
	std::string InfoY;
	std::string TimeType;
	std::string TimeX;
	std::string TimeY;
	std::string ProgramX;
	std::string ProgramY;
};

struct SpectrumInfo
{
	float freq;
	float level;
};
struct QualityCard
{
	std::string IP;
	std::string Port;
};

//wz_110309
//配置文件videoprotocol节点信息结构
typedef struct _sVideoProtocol
{
	std::string streamprotocol;
	std::string fileprotocol;
	std::string fileurltype;
	std::string	fileoffset;
} sVideoProcotol, *psVideoProtocol;

typedef struct _sEncryptInfo
{
	std::string freq;
	std::string serviceid;
	std::string programid;
	std::string	Encrypt;
} sEncryptInfo, *psEncryptInfo;

//wz_110309
//recorddown节点信息结构
typedef struct _sRecordDownInfo
{
	std::string protocol;
	std::string urltype;
	std::string offset;
} sRecordDownInfo, *pRecordDownInfo;

//报警上报结构
struct UpAlarmInfo
{ 
	std::string type;		//0：查询；1：主动上报
	std::string interval;	//上报时间间隔 单位：秒
	std::string onceday;    //1:隔天报警;0 不做隔天报警
	std::string alarmurl;	//报警上报地址
	std::string runplanrecord; // 运行图期间是否保存录像 0：保存 1 不保存
};
//录像文件参数结构
typedef struct _sRecordParamInfo
{ 
	std::string recordexpire;		//录像过期天数
	std::string alarmrecordexpire;	//异态录像过期天数
	std::string alarmheadoffset;	//异态录像头偏移长度
	std::string alarmtailoffset;	//异态录像尾偏移长度
	std::string alarmrecordmode;	//异态录像存储模式
} sRecordParamInfo, psRecordParamInfo;

//定时检查类型定义  week,day,single;
typedef enum eCheckType              
{
	PERWEEK				= 0,		//每星期
	PERDAY				= 1,		//每天
	PERSINGLE			= 2,		//单次
	NOTHING             = 3         //不做处理
}enumCheckType;

typedef struct SignalCheck
{
	enumDVBType  dvbtype;
	std::string DeviceID;
	std::string Freq;
	std::string ChannelID;
	int  level;
	SignalCheck ()
	{
		level=100;
	}
	SignalCheck& operator = (const SignalCheck other)
	{
		dvbtype=other.dvbtype;
		DeviceID=other.DeviceID;
		Freq=other.Freq;
		ChannelID=other.ChannelID;
		level = other.level;
		return *this;
	}
	bool operator ==(const SignalCheck& other)
	{
		return dvbtype==other.dvbtype&&DeviceID==other.DeviceID&&Freq==other.Freq&&ChannelID==other.ChannelID;
	}

} sSignalCheck;

//jsp2017.7.25
struct OSDFormat{
	std::string Freq;
	std::string ServiceID;
	std::string FontSize;
	std::string Position;
	std::string AntiColor;
	std::string Type;
	std::string Align;
};

typedef struct SystemConfigureParameter
{
	string SystemIp;
	string VideoHttpServerIp;
	string VideoHttpPort;
	string VideoHttpMaxnum;
	string VideoRtspServerIp;
	string VideoRtspPort;
	string VideoRtspMaxnum;
	string QualityServerIp;
	string QualityPort;
	string QualityMaxnum;
	string DeviceServerIp;
	string DevicePort;
	string DeviceMaxnum;
	string LogServerIp;
	string LogPort;
	string LogMaxnum;
	string XmlServerIp;
	string XmlPort;
	string HttpServerIp;
	string HttpPort;
	string FtpServerIp;
	string FtpPort;
	string RecordQualityIp;
	string RecordQualityPort;
	string RecordQualityMaxnum;
	string Record_FileLocation;
	string Record_FileSharePath;
	string Record_DeviceId;
	string Record_Period;
	string Record_MaxAvailableSize;
	string Record_DBCleanInterval;
	string Record_SystemCleanTime;
	string Record_StoreType;
	string Log_Path;
	string Log_Expire;
	string Log_Type;
	string Log_outputToFile;
	string LogPathType_Analyser;
	string LogPathType_Record;
	string LogPathType_Video;
	string LogPathType_Device;
	string LogPathType_Other;
	string LogPathType_Default;
	string XmlSendTime;
	string XmlOverTime;
	string TempFileLocation;
	string TempSharePath;
	string TempFileExpireTime;
	string DeviceSchedulerTask_Type;
	string DeviceSchedulerTask_WeekDay;
	string DeviceSchedulerTask_Date;
	string DeviceSchedulerTask_Time;
	string PsisiSchedulerTask_Type;
	string PsisiSchedulerTask_WeekDay;
	string PsisiSchedulerTask_Date;
	string PsisiSchedulerTask_Time;
}sSysConfigParam;

typedef struct DvbConfigureParameter
{
	string type;
	string CenterFreq;
	string Symbolrate;
	string Qam;
	string ChannelScanType;
	string Osd_fontsize;
	string Osd_infoosd;
	string Osd_infoosdx;
	string Osd_infoosdy;
	string Osd_timeosdtype;
	string Osd_timeosdx;
	string Osd_timeosdy;
	string Osd_programx;
	string Osd_programy;
	string TablePath;
	string SharePath;
	string AlarmType;
	string AlarmInterval;
	string OnceDay;
	string RunplanRecord;
	string AlarmServer;
	string StreamRoundUrl; //FRee_fanrong_20140103
	string LogPath_VIDEO;	//FRee_fanrong_20140106
	string LogPath_SI;		//FRee_fanrong_20140106
	string LogPath_SMS;		//FRee_fanrong_20140106
	string LogPath_CAS;		//FRee_fanrong_20140106

	string SmsDir;		//FRee_fanrong_20140108
	string CasDir;		//FRee_fanrong_20140108
	string SIDir;		//FRee_fanrong_20140108

	string VideoStreamProtocol;
	string VideoFileProtocol;
	string VideoFileUrlType;
	string VideoFileOffSet;
	string RecordParam_recordexpire;
	string RecordParam_alarmrecordexpire;
	string RecordParam_alarmheadoffset;
	string RecordParam_alarmtailoffset;
	string RecordParam_alarmrecordmode;
	string RecordDown_protocol;
	string RecordDown_urltype;
	string RecordDown_offset;
	string GeneralDestCode;
	string GeneralSrcCode;
	string RealTimeFromRecord;

	string HDRealStreamWidth;
	string HDRealStreamHeight;
	string HDRealStreamBps;
	string SDRealStreamWidth;
	string SDRealStreamHeight;
	string SDRealStreamBps;

	string HDRecordWidth;
	string HDRecordHeight;
	string HDRecordBps;
	string SDRecordWidth;
	string SDRecordHeight;
	string SDRecordBps;
	string AudioBps;

	string AlarmStorage_switch;		//异态录像存储开关	wz
	string AlarmStorage_filenum;	//异态录像存储文件个数	wz
}sDvbConfigParam;
//模拟无载波报警电平门限
typedef struct
{
	eDVBType dvbtype;
	string freq;
	string downthreshod;
}ThresHold;

//分别率，码率
typedef struct
{
	string width;
	string height;
	string bps;
}VideoParam;

typedef struct  
{
	std::string _switch;
	std::string _filenum;
} sAlarmRecordStorageCfg;
typedef struct AlarmPriority
{
	eDVBType  dvbtype;
	string    type;
	string    priority;
}sAlarmPriority;
struct SeperateTypeID
{
	string   head;
	string   type;
};
typedef struct AlarmTypePriority
{
	string UnLock;
	string Static;
	string Black;
	string NoAudio;
	string NoVideo;
	string Slice;
}sAlarmTypePriority;


typedef struct AlarmSpecInfo
{
	bool bIsSpecing;				//是否在做频谱扫描
	std::string strLastSpecTime;	//上次频谱扫描的结束时间
} sAlarmSpecInfo, *pAlarmSpecInfo;


//马赛克轮播上报结构    //FRee_fanrong_20140103
struct UpStreamRoundInfo
{
	std::string type;		//0：查询；1：主动上报
	std::string streamroundurl;
};

//入网测试新增：录像路数查询时使用
typedef struct _RecChanInfo
{
	std::string freq;
	std::string serviceId;
	std::string programId;
	bool canRecord;
}RecChanInfo, *pRecChanInfo;

typedef struct _RateParam
{
	std::string width;
	std::string height;
	std::string fps;
	std::string bps;

}RateParam,*pRateParam;

typedef struct _UdpSendTsAddr
{
	std::string destip;
	u_short port;
}UdpSendTsAddr;


typedef struct _AudioParam
{
	std::string videopid;
	std::string audiopid;
	std::string audioencode;
	std::string left;
	std::string center;
	std::string _right; 
	std::string leftsurround;
	std::string rightsurround;
	std::string subwoofer;
}AudioParam,*pAudioParam;

typedef struct _CACardInfo
{
	std::string cardno;
	std::string position;
	std::string desc;
	std::string ip;
}CACardInfo,*pCACardInfo;

typedef struct _MHPInfo
{
	std::string ftpurl;
	std::string username;
	std::string pass;
}MHPInfo,*pMHPInfo;

typedef struct _EquInfo
{
	std::string strSrcCode;
	std::string strDesCode;
	std::string strUrl;
}sEquInfo, *pEquInfo;


typedef struct _RecordSetInfo
{
	std::string strTaskXml;
	int nDeviceId;
	bool bSetAction;
} sRecordSetInfo, *pRecordSetInfo;

typedef struct _RecVideoTimeInfo
{
	std::string strProgramId;
	std::string strFreq;
	std::string strServiceId;
	std::string strStartTime;
	std::string strEndTime;
} sRecVideoTimeInfo, *pRecVideoTimeInfo;
typedef std::vector<sRecVideoTimeInfo> VIDEOTIMEVEC;

typedef struct _AgentSetInfo
{
	std::string s_strType;
	std::string s_strDesc;
	std::string s_strValue;
} sAgentSetInfo, *pAgentSetInfo;

typedef struct _RecordUpLoadInfo
{
	std::string s_FileName;//上传文件名
	std::string s_FileSize;//上传文件大小
	std::string s_SubStartTime;//开始时间
	std::string s_SubEndTime;//结束时间
	std::string s_LoadTime;//上传时长

}sRecordUpLoadInfo;

typedef struct _RecordUpload
{
	std::string s_taskid;  
	std::string s_uploadsucenddtime; 
	std::string s_uploadsucfilename;
	std::string s_Freq;
	std::string s_ServiceID;
	std::string s_Active;
	std::string s_UserName; 
	std::string s_PassWord; 
	std::string s_Address;
	std::string s_Length;
	std::string s_StartDateTime;
	std::string s_EndDateTime;
	std::string s_strChassisID; 
	std::string s_strPosition; 
	std::string s_strModuleID; 
	std::string s_strTranscode;
	std::string s_strChanId; 
	std::string s_TaskInfo; 
	std::string s_devcodehead;
	std::string s_Folder;
	std::string s_dstURL;
	std::string s_type;
}RecordUpload;
typedef struct _ChannelSetSouceInfo
{
	std::string c_Freq;
	std::string c_coderIndex;
	std::string c_CoderUnit;
	std::string c_UnitCodeNum;
	std::string c_Width;
	std::string c_Height;
	std::string c_DeviceID;
	std::string c_ServiceID;
	std::string c_Bps;
}sChannelSetSouceInfo;
#endif