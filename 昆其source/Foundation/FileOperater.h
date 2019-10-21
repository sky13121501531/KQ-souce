#ifndef _FILEOPERATER_H_
#define _FILEOPERATER_H_

#include <stdio.h>

typedef enum {begin = 0, current, end} FilePOSType;

class FileOperater
{
public:
	FileOperater(void);
	~FileOperater(void);
public:
	bool CreateNewFile(const char* name);
	bool OpenOrCreate(const char* name);
	bool OpenExists(const char* name);
	void Close();

	int ReadBuf(void* buf, int len);
	int WriteBuf(void* buf, int len, bool flush = false);
	bool SeekFile(int pos, FilePOSType from = begin);
	void Flush() { if (IfValid()) ::fflush(mFile); }

	int GetFirstLine(void* line_buf, int max_len);
	int GetNextLine(void* line_buf, int max_len);

	int  GetSize();
	bool ReSize(int new_size);

	bool IfValid();
	bool operator!() { return IfValid();}
public:
	static bool RenameFile(const char* old_name, const char* new_name);
	static bool CreateDir(const char* name);
	static bool DeleteFileByName(const char* name);
	static bool IfExists(const char* name);
private:
	FILE* mFile;
	static unsigned int mFilesCount;
};
#endif