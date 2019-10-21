




#include "./XmlValidator.h"
#include "../foundation/xmlparser.h"




XmlValidator::XmlValidator()
{

}



XmlValidator::~XmlValidator()
{

}



std::string XmlValidator::ValidChanScanXml( const std::string& strRetXml )
{
	std::string strValidXml = "";

	XmlParser retParser( strRetXml.c_str() );
	
	pXMLNODE csQueryNode = retParser.GetNodeFromPath("Msg/ReturnInfo/ChannelScanQuery");
	
	int nChanScanNum = retParser.GetChildCount( csQueryNode );
	pXMLNODELIST chanScanList = retParser.GetNodeList( csQueryNode );

	int nUnknownNum = 1;
	for (int nCsIndex=0; nCsIndex<nChanScanNum; nCsIndex++)
	{
		pXMLNODE chanScanNode = retParser.GetNextNode( chanScanList );
		std::string strFreq, strOrgNetId, strTsId,strQam;
		retParser.GetAttrNode(chanScanNode, "Freq", strFreq);
		retParser.GetAttrNode(chanScanNode, "OrgNetID", strOrgNetId);
		retParser.GetAttrNode(chanScanNode, "TsID", strTsId);
		retParser.GetAttrNode(chanScanNode, "QAM", strQam);
		strQam = "64";
		retParser.SetAttrNode("QAM", strQam, chanScanNode);
		if(strOrgNetId.length()<1)
		{
			strOrgNetId = "0";
		}
		if(strTsId.length()<1)
		{
			strTsId = "0";
		}
		//freq, orgnedid, tsid不能为空
		if (strFreq.length()<1 || strOrgNetId.length()<1 || strTsId.length()<1)
		{
#ifdef _DEBUG
			std::cout<<"delete node <ChannelScan> : "
				<<"[ freq|orgnetid|tsid is empty ] !"<<std::endl;
#endif
			retParser.DeleteNode(csQueryNode, chanScanNode);
			continue;
		}

		//===================== channel 节点检测 ======================

		int nChanCount = retParser.GetChildCount( chanScanNode );
		if (nChanCount == 0)	//如果没有该频点的节目信息，就删除该节点
		{
#ifdef _DEBUG
			std::cout<<"delete node <ChannelScan> : "
				<<"[ no channel ] !"<<std::endl;
#endif
			retParser.DeleteNode(csQueryNode, chanScanNode);
			continue;
		}
		pXMLNODELIST channelList = retParser.GetNodeList( chanScanNode );

		//bool bDelCSNode = false;	//是否删除chanscan节点
		for (int nChanIndex=0; nChanIndex<nChanCount; nChanIndex++)
		{
			pXMLNODE channelNode = retParser.GetNextNode( channelList );

			//programid 验证
			std::string strProgramId;
			retParser.GetAttrNode(channelNode, "ProgramID", strProgramId);
			if (strProgramId.length()<1)
			{
				strProgramId = "0";
			}
			if (strProgramId.length()<1)		//programId 不能为空
			{
#ifdef _DEBUG
				std::cout<<"delete node <Channel> : "
					<<"[ ProgramId is empty ] !"<<std::endl;
#endif
				retParser.DeleteNode(chanScanNode, channelNode);

				//if (retParser.GetChildCount(chanScanNode) == 0)		//如果ChanScan节点已经没有子节点了，就把它也删除
				//{
				//	bDelCSNode = true;
				//	break;
				//}
				continue;
			}

			std::string strServiceId, strVideoPid, strAudioPid;
			retParser.GetAttrNode(channelNode, "ServiceID", strServiceId);
			retParser.GetAttrNode(channelNode, "VideoPID", strVideoPid);
			retParser.GetAttrNode(channelNode, "AudioPID", strAudioPid);
			if (strServiceId.length()<1)
			{
				strServiceId = "0";
			}
			if (strVideoPid.length()<1)
			{
				strVideoPid = "0";
			}
			if (strAudioPid.length()<1)
			{
				strAudioPid = "0";
			}
			if (strServiceId.length()<1 || /*strVideoPid.length()<1 || */strAudioPid.length()<1)		//3个属性不能为空
			{
#ifdef _DEBUG
				std::cout<<"delete node <Channel> : "
					<<"[ servId|audiopid|videopid is empty ] !"<<std::endl;
#endif
				retParser.DeleteNode(chanScanNode, channelNode);
				continue;
			}
			//if (strProgramId != strServiceId)	//招标时，programId用的是serviceId//FRee_fanrong_20131218  录象查询返回ProgramID与频道扫描结果不一致，改为一致
			//{
			//	strProgramId = strServiceId;
			//	retParser.SetAttrNode("ProgramID", strProgramId, channelNode);
			//}

			std::string strProgramName, strEncrypt;
			retParser.GetAttrNode(channelNode, "Program", strProgramName);
			retParser.GetAttrNode(channelNode, "Encrypt", strEncrypt);
			if (strProgramName.length()<1)
			{
				strProgramName = "0";
			}
			if (strProgramName.length()<1)	//频道名为空，就赋值为：未知频道+数字
			{
				//strProgramName = "未知频道";
				//char tempNum[10] = {0};
				//itoa(nUnknownNum, tempNum,10);
				//strProgramName += tempNum;

				//nUnknownNum ++;
				//retParser.SetAttrNode("Program", strProgramName, channelNode);
#ifdef _DEBUG
				std::cout<<"delete node <Channel> : "
					<<"[ program name is empty ] !"<<std::endl;
#endif
				retParser.DeleteNode(chanScanNode, channelNode);
				continue;
			}
			//if (strEncrypt.length()<1)	//如果encrypt为空，就赋默认值//3.0版为空
			//{
			//	strEncrypt = "1";
			//	retParser.SetAttrNode("Encrypt", strEncrypt, channelNode);
			//}

			//=============== stream 信息检测 ================
			int nStreamCount = retParser.GetChildCount( channelNode );
//			if (nStreamCount < 2)	//如果stream信息不全，就删除节点
//			{
//#ifdef _DEBUG
//				std::cout<<"delete node <Channel> : "
//					<<"[ stream info miss ] !"<<std::endl;
//#endif // _DEBUG
//				retParser.DeleteNode(chanScanNode, channelNode);
//				continue;
//			}

			bool bBadStream = false;	//是否有不完整的stream信息(streamtype或者streampid为空)
			pXMLNODELIST streamList = retParser.GetNodeList( channelNode );
			for (int nStreamIndex=0; nStreamIndex<nStreamCount; nStreamIndex++)
			{
				pXMLNODE streamNode = retParser.GetNextNode( streamList );
				std::string strStreamType, strStreamPid;
				retParser.GetAttrNode(streamNode, "StreamType", strStreamType);
				retParser.GetAttrNode(streamNode, "PID", strStreamPid);
				if (strStreamType.length()<1)
				{
					strStreamType = "0";
				}
				if (strStreamPid.length()<1)
				{
					strStreamPid = "0";
				}
				if (strStreamType.length()<1 || strStreamPid.length()<1)	//stream信息不全，就删除channel节点
				{
#ifdef _DEBUG
					std::cout<<"delete node <Channel> : "
						<<"[ stream type|pid is empty ] !"<<std::endl;
#endif // _DEBUG
					retParser.DeleteNode(chanScanNode, channelNode);
					bBadStream = true;
					break;
				}
			}
			if (bBadStream)		//stream信息不完整，跳过该channel节点
			{
				continue;
			}

		}

		nChanCount = retParser.GetChildCount( chanScanNode );
		if (nChanCount == 0)	//如果没有该频点的节目信息，就删除该节点
		{
#ifdef _DEBUG
			std::cout<<"delete node <ChannelScan> : "
				<<"[ no channel ] !"<<std::endl;
#endif // _DEBUG
			retParser.DeleteNode(csQueryNode, chanScanNode);
			continue;
		}
	}

	retParser.SaveToString( strValidXml );

	return strValidXml;
}

