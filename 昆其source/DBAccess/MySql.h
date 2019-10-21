#ifndef __MYSQL__
#define __MYSQL__

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <vector>

#if  defined(WIN32) || defined(__WIN32__)
	#include "../../thirdlib/mysql/win32/mysql.h"
    #pragma comment(lib, "../../thirdlib/mysql/win32/libmysql.lib")
    #pragma warning(disable : 4244 4996)
#else
    #include "unistd.h"
    #include "../../thirdlib/mysql/linux/mysql.h"
#endif

using namespace std;

class MySql
{
public :
	MySql();
	~MySql();

	//attribute
	std::string Server() const { return m_strServer; }
	void Server(std::string val) { m_strServer = val; }

	std::string Username() const { return m_strUsername; }
	void Username(std::string val) { m_strUsername = val; }

	std::string Password() const { return m_strPassword; }
	void Password(std::string val) { m_strPassword = val; }

	std::string Port() const { return m_strPort; }
	void Port(std::string val) { m_strPort = val; }

	std::string Connectstring() const { return m_strConnectstring; }
	void Connectstring(std::string val) { m_strConnectstring = val; }

	std::string Database() const { return m_strDatabase; }
	void Database(std::string val) { m_strDatabase = val; }

	// init
	bool BInit();
	bool BUnInit();

	// connect operation
	bool BConnDisConnect();
	bool BConnRetryConnect();
	bool BConnIsConnected();
	bool BConnKeepAlive();
	bool BConnByStr();

	// sql operation
	bool BOpenFmtSql(const char* format, ...);
	bool BOpenSql(const char* sql);	
	bool BDoFmtSql(const char* format, ...);
	bool BDoSql(const char* sql);
	void VCloseSql();

	// record set operation
	bool BRecIsEof();
	bool BRecMoveToFirst();
	bool BRecMoveToLast();
	bool BRecMoveToPrier();
	bool BRecMoveToNext();
	bool BRecMoveTo(int index);
	int  IRecGetRecordCount();
	int  IRecGetFieldCount();

	// field operation
	bool  BFldIsValid(const char* field_name);
	bool  BFldIsValid(int field_index);

	string SFldGetBuffer(const char* field_name);
	string SFldGetBuffer(int field_index);
	int    IFldGetLength(int field_index);
	int    IFldGetLength(const char* field_name);

	//	type conversion	
	string SFldItemToString(const char* field_name);
	string SFldItemToString(int field_index);

	long   LFldItemtoInteger(const char* field_name);
	long   LFldItemtoInteger(int field_index);
	short  SFldItemtoSmallInteger(const char* field_name);
	short  SFldItemtoSmallInteger(int field_index);
	float   LFldItemtoFloat(const char* field_name);
	float   LFldItemtoFloat(int field_index);

	unsigned char CFldItemtoTinyInteger(const char* field_name);
	unsigned char CFldItemtoTinyInteger(int field_index);
	int    IFldItemToBinary(const char* field_name, void* out_buf, int max_len);
	int    IFldItemToBinary(int field_index, void* out_buf, int max_len);

	bool   BFldItemToBool(const char* field_name);
	bool   BFldItemToBool(int field_index);	
	
private:
	MYSQL*			m_pMySqlConn;
	MYSQL_RES*		m_pMySqlRes;
	MYSQL_ROW		m_MySqlRows;
	MYSQL_FIELD*	m_pMySqlField;
	int m_nCurRowIndex;
	bool   GetFileds(void);
	bool m_bInit;
	bool m_bConnected;
	int  m_nTimeout;

	string m_strServer;
	string m_strDatabase;
	string m_strUsername;
	string m_strPassword;
	string m_strPort;
	string m_strConnectstring;
	vector<string> m_fields;

#if defined(WIN32) || defined(__WIN32__) 
	#define vsnprintf _vsnprintf
	#define vsprintf _vsprintf
#endif
};
#endif