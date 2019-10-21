
#pragma warning(disable:4996)

#if defined(WIN32) || defined(__WIN32__)
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/stat.h>

#include "FileOperater.h"

unsigned int FileOperater::mFilesCount = 0;

FileOperater::FileOperater(void)
{
	mFile = 0;
}
FileOperater::~FileOperater(void)
{
	if (IfValid()) Close();
}
bool FileOperater::CreateNewFile(const char* name)
{
	if (IfValid()) Close();
	mFile = fopen(name, "w+b");
	if (IfValid()) mFilesCount++;
	return IfValid();
}
bool FileOperater::OpenOrCreate(const char* name)
{
	if (IfValid()) Close();
	mFile = fopen(name, "a+b");
	if (!IfValid()) mFile = ::fopen(name, "w+b");
	if (IfValid()) mFilesCount++;
	return IfValid();
}
bool FileOperater::OpenExists(const char* name)
{
	if (IfValid()) Close();
	mFile = fopen(name, "a+b");
	if (IfValid()) mFilesCount++;
	return IfValid();
}
void FileOperater::Close()
{
	if (!IfValid()) return;
	mFilesCount--;
	fclose(mFile);
	mFile = 0;
}

int FileOperater::ReadBuf(void* buf, int len)
{
	if (!IfValid()) return -1;
	size_t ReadBufed_len = 0;
	ReadBufed_len = fread(buf, sizeof(char), len, mFile);
	return (ReadBufed_len > 0 ? (int)ReadBufed_len : 0);
}
int FileOperater::WriteBuf(void* buf, int len, bool flush)
{
	if (!IfValid()) return 0;
	size_t WriteBufd_len = 0;
	WriteBufd_len = fwrite(buf, sizeof(char), len, mFile);
	return (WriteBufd_len > 0 ? (int)WriteBufd_len : 0);
}
bool FileOperater::SeekFile(int pos, FilePOSType from)
{
	if (!IfValid()) return 0;
	int seek_from = SEEK_SET;
	if (from == end) seek_from = SEEK_END;
	else if (from == current) seek_from = SEEK_CUR;
	else seek_from = SEEK_SET;

	return (::fseek(mFile, pos, seek_from) == 0);
}

int FileOperater::GetFirstLine(void* line_buf, int max_len)
{
	SeekFile(0);
	return GetNextLine(line_buf, max_len);
}
int FileOperater::GetNextLine(void* line_buf, int max_len)
{
	if (!IfValid()) return -1;
	int len, ReadBuf_len, c;
	len = ReadBuf_len = c = 0;
	while (c = fgetc(mFile))
	{
		if (c == EOF) return -1;

		++ReadBuf_len;
		if (c != '\r' && c != '\n')
		{
			if (len <= max_len)
				((char*)line_buf)[len] = c;

			++len;
		}
		if (c == '\n') break;
	}

	if (ReadBuf_len > 0)
	{
		int ret_len = len < max_len ? len : max_len;

		((char*)line_buf)[ret_len] = '\0';

		return ret_len;
	} else
	{
		return -1;
	}
}

int  FileOperater::GetSize()
{
	if (!IfValid()) return 0;
	struct stat st;
	fstat(fileno(mFile), &st);
	return st.st_size;
}
bool FileOperater::ReSize(int new_size)
{
	if (!IfValid()) return false;
	bool ret = true;
#if defined(WIN32) || defined(__WIN32__)
	ret = (chsize(fileno(mFile), new_size) == 0 ? true : false);
#else
	ret = (ftruncate(fileno(mFile), new_size) == 0 ? true : false);
#endif
	return ret;
}

bool FileOperater::IfValid()
{
	return mFile > 0;
}

bool FileOperater::RenameFile(const char* old_name, const char* new_name)
{
	bool ret = false;
	rename(old_name, new_name);
	return ret;
}
bool FileOperater::CreateDir(const char* name)
{
	try 
	{
#if defined(WIN32) || defined(__WIN32__)
		if (mkdir(name) == 0) return true;
#else
		if (mkdir(name, 0) == 0) return true;
#endif
	}
	catch (...) 
	{
	}
	return false;
}
bool FileOperater::DeleteFileByName(const char* name)
{
	try 
	{
		if (unlink(name) == 0) return true;
	} 
	catch (...) 
	{
		return false;
	}

	return false;
}
bool FileOperater::IfExists(const char* name)
{
	FileOperater f;
	return f.OpenExists(name);
}