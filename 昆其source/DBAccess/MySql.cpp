 
#include "MySql.h"

MySql::MySql()
{	
	m_bInit = false;
	BInit();
}

MySql::~MySql()
{
	BUnInit();
}

bool MySql::BInit()
{
	if (!m_bInit)
	{
		m_pMySqlRes = 0;
		m_pMySqlField = 0;
		m_nCurRowIndex = 0;
		m_pMySqlConn = mysql_init(NULL);
		m_bConnected = false;
		if (!m_pMySqlConn) return false;
		m_bInit = true;
		return m_bInit;
	}
	return true;
};
bool MySql::BUnInit()
{
	if (m_bInit)
	{
		BConnDisConnect();
		m_bInit = false;
	}
	return true;
};
//connectoper
bool MySql::BConnDisConnect()
{	
	bool ret = false;
	try{
		if (!BConnIsConnected()) 
			return true;
		VCloseSql();
		mysql_close(m_pMySqlConn);
		m_pMySqlConn = 0;
		m_pMySqlRes = 0;
		m_pMySqlField = 0;
		m_bConnected = false;
		ret = true;
	} catch(...)
	{
	};
	return ret;
};
bool MySql::BConnRetryConnect()
{
	bool ret = false;
	if (!BUnInit()) return ret;
	if (!BInit()) return ret;
	return BConnByStr();
};

bool MySql::BConnIsConnected()
{
	return m_bConnected;
};

bool MySql::BConnKeepAlive()
{	
	return BDoSql("SELECT NOW() AS cur_time limit 1 ");	
};

bool MySql::BConnByStr()
{
	if (!m_bInit) { if (!BInit()) return false; }
	if (m_strDatabase.empty()) return false;
	if (BConnIsConnected()) BConnDisConnect();
	if (!mysql_real_connect(m_pMySqlConn, m_strServer.c_str(), m_strUsername.c_str(), m_strPassword.c_str(),
		m_strDatabase.c_str(), atoi(m_strPort.c_str()), 0, 0))
	{
		//mysql_close(m_pMySqlConn);
		fprintf(stderr, "______Failed to connect to database: Error: %s\n",
			mysql_error(m_pMySqlConn));
		return false;
	}
	m_bConnected = true;
	BDoSql("SET NAMES 'gb2312';");
	return BConnIsConnected();
};
//sqloper
bool MySql::BOpenFmtSql(const char* format, ...)
{
	if (!BConnIsConnected()) return false;
	char* sql = new char[strlen(format) + 2048];
	va_list args;
	va_start(args, format);
	vsnprintf(sql, 2048, format, args);
	va_end(args);
	bool ret = BOpenSql(sql);
	delete[] sql;
	return ret;
};
bool MySql::BOpenSql(const char* sql)
{
	if (!BConnIsConnected()) 
	{
		if (!BConnRetryConnect()) return false;
	}
	VCloseSql();
	bool ret = false;	
	if (mysql_query(m_pMySqlConn, sql) == 0)
	{
		m_pMySqlRes = mysql_store_result(m_pMySqlConn);
		if (m_pMySqlRes)
		{
			m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
			m_pMySqlField = mysql_fetch_fields(m_pMySqlRes);
			
		}
		ret=GetFileds();
	}
	else
	{
		fprintf(stderr, "\"%s\" execution error %d: %s\n", sql, mysql_errno(m_pMySqlConn), mysql_error(m_pMySqlConn));
	}
	return ret;
};	
bool MySql::BDoFmtSql(const char* format, ...)
{
	if (!BConnIsConnected()) return false;
	size_t len = strlen(format) + 2048;
	char* sql = new char[len];
	va_list args;
	va_start(args, format);
	vsnprintf(sql, len, format, args);
	va_end(args);
	bool ret = BDoSql(sql);
	delete[] sql;
	return ret;
};
bool MySql::BDoSql(const char* sql)
{	
	if (!BConnIsConnected()) 
	{
		if (!BConnRetryConnect()) return false;
	}
	bool ret = false;
	if (mysql_query(m_pMySqlConn, sql) == 0)
	{
		//mysql_errno(m_pMySqlConn);
		ret = true;
	}
	else
	{
		fprintf(stderr, "\"%s\" execution error %d: %s\n", sql, mysql_errno(m_pMySqlConn), mysql_error(m_pMySqlConn));
	}
	return ret;
};
void MySql::VCloseSql()
{
	if (!BConnIsConnected()) return;
	if (m_pMySqlRes)
	{
		mysql_free_result(m_pMySqlRes);
		m_pMySqlRes = 0;
		m_pMySqlField = 0;
		m_nCurRowIndex = 0;
		m_fields.clear();
	}
};
//recordsetoper
bool MySql::BRecIsEof()
{
	return (m_MySqlRows ? false : true);
};
bool MySql::BRecMoveToFirst()
{
	if (m_pMySqlRes)
	{
		m_nCurRowIndex = 0;
		mysql_data_seek(m_pMySqlRes, m_nCurRowIndex);
		m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
		if (m_MySqlRows) return true;
	}
	return false;
};
bool MySql::BRecMoveToLast()
{
	int record_count = IRecGetRecordCount();
	if (record_count > 0)
	{
		m_nCurRowIndex = record_count - 1;
		mysql_data_seek(m_pMySqlRes, m_nCurRowIndex);
	}
	m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
	return (m_MySqlRows != NULL);
	return false;
};
bool MySql::BRecMoveToPrier()
{
	if (m_pMySqlRes && m_nCurRowIndex > 0)
	{
		mysql_data_seek(m_pMySqlRes, --m_nCurRowIndex);
		m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
		if (m_MySqlRows) return true;
	}
	return false;
};
bool MySql::BRecMoveToNext()
{
	if (m_pMySqlRes)
	{
		m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
		if (m_MySqlRows)
		{
			++m_nCurRowIndex;
			return true;
		}
	}
	return false;
};

bool MySql::BRecMoveTo(int index)
{
	if (m_pMySqlRes)
	{
		if (index > IRecGetRecordCount() - 1)
			index = IRecGetRecordCount() - 1;
		if (index < 0)
		{
			index = 0;
		}
		m_nCurRowIndex = index;
		//BRecMoveToFirst();
		mysql_data_seek(m_pMySqlRes, index);
		m_MySqlRows = mysql_fetch_row(m_pMySqlRes);
		if (m_MySqlRows)
		{
			//m_nCurRowIndex += index;
			return true;
		}
	}
	return false;
};

int  MySql::IRecGetRecordCount()
{
	if (m_pMySqlRes) 
		return (int)mysql_num_rows(m_pMySqlRes);
	return 0;
};

int  MySql::IRecGetFieldCount()
{
	if (m_pMySqlRes) 
		return (int) mysql_num_fields(m_pMySqlRes);
	return 0;
};

//fieldoper	
bool  MySql::BFldIsValid(const char* field_name)
{
	if (m_pMySqlField && m_MySqlRows&&m_fields.size()>0)
	{
		unsigned long* lengths = mysql_fetch_lengths(m_pMySqlRes);
		for (size_t i = 0; i < m_fields.size(); ++i)
		{
#if defined(WIN32) || defined(__WIN32__)
			if (stricmp(m_fields[i].c_str(), field_name) == 0)
#else
			if (strcasecmp(m_fields[i].c_str(), field_name) == 0)
#endif
			{
				return true;
			}
		}
	}
	return false;
};
bool  MySql::BFldIsValid(int field_index)
{
	return field_index < IRecGetFieldCount();
	return false;
};

string MySql::SFldGetBuffer(const char* field_name)
{
	return SFldItemToString(field_name);
};
string MySql::SFldGetBuffer(int field_index)
{
	return SFldItemToString(field_index);
};

int MySql::IFldGetLength(int field_index)
{
	if (field_index < IRecGetFieldCount())
	{
		unsigned long* lengths = mysql_fetch_lengths(m_pMySqlRes);
		return lengths[field_index];
	}
	return 0;
};

int MySql::IFldGetLength(const char* field_name)
{
	if (m_pMySqlField && m_MySqlRows&&m_fields.size()>0)
	{
		unsigned long* lengths = mysql_fetch_lengths(m_pMySqlRes);
		for (size_t i = 0; i < m_fields.size(); ++i)
		{
#if defined(WIN32) || defined(__WIN32__)
			if (stricmp(m_fields[i].c_str(), field_name) == 0)
#else
			if (strcasecmp(m_fields[i].c_str(), field_name) == 0)
#endif
			{
				return lengths[i];
			}
		}
	}
	return 0;
};

string MySql::SFldItemToString(const char* field_name)
{
	if (m_pMySqlField && m_MySqlRows&&m_fields.size()>0)
	{
		for (size_t i = 0; i < m_fields.size(); ++i)
		{
#if defined(WIN32) || defined(__WIN32__)
			if (stricmp(m_fields[i].c_str(), field_name) == 0)
#else
			if (strcasecmp(m_fields[i].c_str(), field_name) == 0)
#endif
			{
				return m_MySqlRows[i] ? m_MySqlRows[i] : "";
			}
		}
	}
	return "";
};

string MySql::SFldItemToString(int field_index)
{
	if (field_index >= 0 && m_MySqlRows && field_index < IRecGetFieldCount())
		return m_MySqlRows[field_index] ? m_MySqlRows[field_index] : "";
	return "";
};

long   MySql::LFldItemtoInteger(const char* field_name)
{
	string s = SFldItemToString(field_name);
	if (!s.empty()) return atoi(s.c_str());
	return 0;
};
long   MySql::LFldItemtoInteger(int field_index)
{
	string s = SFldItemToString(field_index);
	if (!s.empty()) return atoi(s.c_str());
	return 0;
};
short  MySql::SFldItemtoSmallInteger(const char* field_name)
{
	return LFldItemtoInteger(field_name);
	return 0;
};
short  MySql::SFldItemtoSmallInteger(int field_index)
{
	return LFldItemtoInteger(field_index);
	return 0;
};

float MySql::LFldItemtoFloat(const char* field_name)
{
	string s = SFldItemToString(field_name);
	if (!s.empty()) return atof(s.c_str());
	return 0.0;	
}

float MySql::LFldItemtoFloat(int field_index)
{
	string s = SFldItemToString(field_index);
	if (!s.empty()) return atof(s.c_str());
	return 0.0;	
}

unsigned char MySql::CFldItemtoTinyInteger(const char* field_name)
{
	return LFldItemtoInteger(field_name);
	return 0;
};
unsigned char MySql::CFldItemtoTinyInteger(int field_index)
{
	return LFldItemtoInteger(field_index);
	return 0;
};

int    MySql::IFldItemToBinary(const char* field_name, void* out_buf, int max_len)
{
	if (m_pMySqlField && m_MySqlRows&&m_fields.size()>0)
	{
		for (size_t i = 0; i < m_fields.size(); ++i)
		{
#if defined(WIN32) || defined(__WIN32__)
			if (stricmp(m_fields[i].c_str(), field_name) == 0)
#else
			if (strcasecmp(m_fields[i].c_str(), field_name) == 0)
#endif
			{
				int rec_len = IFldGetLength((int)i);
				int cpy_len = (rec_len < max_len ? rec_len : max_len);
				memcpy(out_buf, m_MySqlRows[i], cpy_len);
				return cpy_len;
			}
		}
	}
	return 0;
};
int    MySql::IFldItemToBinary(int field_index, void* out_buf, int max_len)
{
	if (m_pMySqlField && m_MySqlRows && field_index < IRecGetFieldCount())
	{
		int rec_len = IFldGetLength(field_index);
		int cpy_len = (rec_len < max_len ? rec_len : max_len);
		memcpy(out_buf, m_MySqlRows[field_index], cpy_len);
		return cpy_len;
	}
	return 0;
}

bool   MySql::BFldItemToBool(const char* field_name)
{
	return LFldItemtoInteger(field_name) != 0;
	return 0;
};

bool   MySql::BFldItemToBool(int field_index)
{
	return LFldItemtoInteger(field_index) != 0 ;
	return 0;
};

bool MySql::GetFileds( void )
{
	if (m_pMySqlField && m_MySqlRows)
	{
		for (int i = 0; i < IRecGetFieldCount(); ++i)
		{
			m_pMySqlField=mysql_fetch_field(m_pMySqlRes);
			m_fields.push_back(m_pMySqlField->name);
		}
	
	}
	return true;
}
