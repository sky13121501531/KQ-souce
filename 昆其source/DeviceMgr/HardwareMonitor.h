///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：HardwareMonitor.h
// 创建者：wuyy	
// 创建时间：2014-6-25
// 内容描述：硬件监测模块
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
class HardwareMonitor
{
public:
	HardwareMonitor(void);
	~HardwareMonitor(void);
public:
	void CasePower();   //判断机箱电源故障
	void HarddiskMonitor(); //判断硬盘故障	
	void FanMonitor();  //判断风扇故障
	void BoardMonitor(); //判断板卡故障

	bool harddisk;
	bool casepower;
	bool fan    ;
	bool board;
	std::vector<std::string> BrokeHarddiskVec;
	std::vector<std::string> O_brokeBoardVec,T_brokeBoardVec; //分别存放第一个机箱上的所有坏板卡和第二个机箱上的所有坏板
	std::vector<std::string> BrokeCasePower;
};
