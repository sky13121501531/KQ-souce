#ifndef _H_LSQL_H_
#define _H_LSQL_H_

/************************************************************************/
/* 
    FileName:   LSql.h
    Description:简易封装SQLITE3的类
    Author:     lynn    2008.10.31
    Relative:   LSql.lib,LSql.dll,Sqlite3.dll,SQlite3.lib,Iconv.dll
                                                                        */
/************************************************************************/
#ifndef L_DLL
#define L_DLL _declspec(dllimport)
#endif

#include <string>
#include <iostream>

# define SQLITE_THREADSAFE 1

//函数返回值
#define LSQL_OK      0   /*正确返回*/
#define LSQL_ERROR   1   /*一般错误*/
#define LSQL_BUSY    5   /*数据库繁忙，正在写入数据*/

/*
 * 1. 函数SetTimeOut(int ms) 可以设置在发生暂时锁定的情况下程序等待时间。此函数在open()函数正确返回后调用有效.
 * 2. 所有函数如果返回LSQL_BUSY,表示正在写数据库，数据库暂时被锁定，可以稍等一会再次执行。
 * 3. 默认情况下，在发生数据库被暂时锁定时，执行数据库操作时，程序等待500ms，如果500ms内数据库未被解除锁定，返回LSQL_BUSY
 * 4. 默认情况下，不会发生数据库暂时锁定的情况。如果一次性提交大量'数据增加'或者'数据修改'操作可能会发生。例如一条包含100条INSERT语句SQL操作
 * 5. 程序对SQL语句，表名，列名 不区分大小写。函数IsTableExist,GetCollect(fieldname...),Blob 字段操作函数除外。应尽量保证大小写一致
 */
class L_DLL LSql
{
public:
    LSql();
    ~LSql();
    
    /*如果文件不存在，自动创建此文件。如果文件所属目录不存在返回,filename文件创建失败并且返回0xe */
    int Open(const char *filename);
    int Close();

    /*
     * 设置超时时间,单位：毫秒,返回设置以前的超时值
     * 应该在数据库成功打开后调用
     *
     * 如果存取数据时数据库被LOCK，那么等待ms毫秒时间，
     * 经过TIMEOUT时间后仍然处于LOCK状态，调用函数返回LSQL_BUSY
     */
    int SetTimeOut(int ms=500);
    /*获取超时时间,单位：毫秒*/
    int GetTimeOut();
    
    /*执行SQL语句，但不返回记录集，一般用于Insert,Delete,Update,Create,Drop */
    int ExecNonQuery(const char *szSql);
    /*执行SQL语句,返回记录集,一般用于Select */
    int ExecSql(const char *szSql);
    
    /*返回错误信息指针,如果调用，应及时保存指针指向的数据，以免被其他操作产生的错误信息覆盖，而导致内存错误 */
    const char *ErrorMessage();
    /*返回上次执行时产生的错误编码 */
    int ErrorCode();

    /*记录集游标操作 */
    int MoveFirst();
    int MoveLast();
    int MoveNext();
    int MovePrevious();

    /*这两个函数，'是'返回LSQL_OK，'否'返回LSQL_ERROR，数据库被锁定返回LSQL_BUSY*/
    int IsBOF();
    int IsEOF();
    /*注意：表名在此时，区分大小写；'存在'返回LSQL_OK，'不存在'返回LSQL_ERROR,库被锁定返回LSQL_BUSY*/
    int IsTableExist(const char *szTbl);
    /*返回最后插入的整数值，用在获取刚插入的记录的自增字段值*/
    int LastInsertRowId();

    /*取得记录条数*/
    int GetRowCount();
    /*取得列数*/
    int GetColumnCount();

    /*根据序号，取得列名；nBufSize:缓冲区长度，如果为0不考虑缓冲区长度*/
    int GetColumnName(int nIndex,char *pBuf,unsigned int nBufSize=0);
    /*
     * 在ExecSql进行了有效查询之后，根据序号读取某字段的值,
     * 如果nBufSize不为0，则向fieldvalue中写入nBufSize长度值，否则写入全部  
     */
    int GetCollect(const int nIndex,char *pBuf,unsigned int nBufSize=0);
    /* 
     * fieldname 区分大小写
     * 根据列名取得字段值
     */
    int GetCollect(const char *fieldname,char *pBuf,unsigned int nBufSize=0);


/*Blob 字段操作 */
    /*
    * 如果nBufSize不为0，则向pBuf中写入nBufSize长度值，否则写入全部 
    * 调用次函数成功后，需要调用Free释放(void *)(*pBuf)
    * 如果根据where条件查询出多个记录，只返回第一条记录
    * fieldname 区分大小写
    */
    int GetBlob(const char *tblname,const char *fieldname,const char *szWhere,void **pBuf,unsigned int *pBufSize);
    /*
    * 从数据库中读取blob字段，写入文件。
    * 如果建立文件失败或者读取数据库失败，均返回LSQLITE_ERROR
    * 如果根据where条件查询出多个记录，只返回第一条记录
    */
    int GetBlobToFile(const char *tblname,const char *fieldname,const char *szWhere,const char *filename);
    /*向数据库中写入blob字段*/
    int SetBlob(const char *tblname,const char *fieldname,const char *szWhere,const void *pBuf,const unsigned int nBufSize);
    /*把一个文件写入blob字段*/
    int SetBlobFromFile(const char *tblname,const char *fieldname,const char *szWhere,const char *filename);

    /*释放由Blob操作产生的内存*/
    void Free(void *pbuf);
    
protected:
    void *m_db; 
    char *m_errmsg;
    char **m_result;
    int m_nErrorCode;
    int m_nRow;
    int m_nCol;
    int m_nCursor;
    int m_nTimeOut;
};




class DeriveLsql : public LSql
{
public:
	DeriveLsql() : LSql()
	{
		m_pFieldValue = new char[1000];
	}

	~DeriveLsql()
	{
		if (m_pFieldValue != NULL)
		{
			delete []m_pFieldValue;
		}
	}

	int GetCollect(std::string fieldname, std::string& value)
	{
		try
		{
			memset(m_pFieldValue, 0, 1000);
			LSql::GetCollect(fieldname.c_str(), m_pFieldValue);
			value = m_pFieldValue;
		}
		catch ( ... )
		{
			std::cout<<"DeriveLsql::GetCollect() 获取字段异常!!"<<std::endl;
			return LSQL_ERROR;
		}
		
		return LSQL_OK;
	};

private:
	char* m_pFieldValue;	//用于获取字段的值
};



#endif/*__ #ifndef _H_LSQL_H_ */