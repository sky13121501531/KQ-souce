
#include "DeviceAccess.h"

DeviceAccess::DeviceAccess(int deviceid, std::string strIP,int nPort)
{
	DeviceId = deviceid;
	strIPAddress=strIP;
	port=nPort;
}
DeviceAccess::DeviceAccess(void)
{

}
DeviceAccess:: ~DeviceAccess()
{

}