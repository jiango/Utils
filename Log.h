#pragma once
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include "BlockingQueue.h"

#define LOG_FILE_NAME  "xyy.log"

#define LOG Log::instance()

class Log;
typedef std::shared_ptr<Log> LogPtr;


class Log : public std::enable_shared_from_this<Log>
{
public:
	enum log_type
	{
		log2_console = 0,	
		log2_output,
		log2_file,
	};

public:
	~Log();
	static LogPtr& instance();
	void initLog(log_type type, const std::wstring& logName, const std::wstring& logpath);
	void writeLog(const wchar_t* format, ...);
	void writeLog(const char* format, ...);
	void stop();

private:
	Log();
	static void writeProc(std::weak_ptr<Log> self);
	std::string formatLogW(const std::wstring& str);
	std::string formatLogA(const std::string& str);
	std::string makeFileName();

private:
	log_type m_type;
	std::atomic<bool> m_stop;
	std::ofstream m_file;
	BlockingQueue<std::string> m_que;
	std::wstring m_logName;
	std::wstring m_logPath;
	std::string m_logFullPath;
	std::thread m_thread;
};
