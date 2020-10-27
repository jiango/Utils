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

enum LogLevel {
	kTrace,
	kDebug,
	kInfo,
	kWarn,
	kError,
	kFatal,
	kLevelNum
};

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
	void stop();
	void writeLog(LogLevel level, const wchar_t* format, ...);
	void writeLog(LogLevel level, const char* format, ...);
	void setLevel(LogLevel l);
	LogLevel level();

private:
	Log();
	static void writeProc(std::weak_ptr<Log> self);
	std::string Log::formatLogW(const std::wstring& str, LogLevel level);
	std::string Log::formatLogA(const std::string& str, LogLevel level);
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
	LogLevel m_logLevel = kInfo;
};

#define LOG_DEBUG(format, ...) if (Log::instance()->level() <= LogLevel::kDebug) \
	Log::instance()->writeLog(LogLevel::kDebug, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) if (Log::instance()->level() <= LogLevel::kInfo) \
	Log::instance()->writeLog(LogLevel::kInfo, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) if (Log::instance()->level() <= LogLevel::kError) \
	Log::instance()->writeLog(LogLevel::kError, format, ##__VA_ARGS__)
