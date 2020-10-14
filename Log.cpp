#include "log.h"
#include <stdarg.h>
#include <strstream>
#include <windows.h>
#include "encode.h"
#include "Path.h"
#include "defer.h"

Log::Log()
{
	m_stop = false;
	m_type = log2_output;
}

Log::~Log()
{
	if (m_file)
		m_file.close();

	if (m_type == log2_console)
		FreeConsole();
}

void Log::initLog(log_type type, const std::wstring& logName, const std::wstring& logpath)
{
	m_type = type;
	if (type == log2_file)
	{
		std::vector<std::wstring> old_logs;

		m_logName = logName;
		m_logPath = logpath;

		m_logFullPath = makeFileName();
		m_file.open(m_logFullPath, std::ios::app);
	}
	else if (type == log2_output)
	{
	}
	else if (type == log2_console)
	{
		AllocConsole();
	}

	m_thread = std::thread(std::bind(&Log::writeProc, shared_from_this()));
}

void Log::writeLog(const wchar_t* format, ...)
{
	if (m_stop) {
		return;
	}

	va_list args;
	int len;
	wchar_t * buffer;
	va_start(args, format);
	len = _vscwprintf(format, args) + 1; 
	buffer = new wchar_t[len];
	defer([&] {
		delete[] buffer;
	});
	vswprintf_s(buffer, len, format, args);
	std::string temp = formatLogW(buffer);
	m_que.put(temp);
	va_end(args);
}

void Log::writeLog(const char* format, ...)
{
	if (m_stop) {
		return;
	}

	va_list args;
	int len;
	char * buffer;
	va_start(args, format);
	len = _vscprintf(format, args) + 1;
	buffer = new char[len];
	defer([&] {
		delete[] buffer;
	});
	vsprintf_s(buffer, len, format, args);
	std::string temp = formatLogA(buffer);
	m_que.put(temp);
	va_end(args);
}

void Log::stop()
{
	m_stop = true;
	m_que.put(std::string(""));
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

LogPtr& Log::instance()
{
	static LogPtr self = LogPtr(new Log);
	return self;
}

void Log::writeProc(std::weak_ptr<Log> self)
{
	std::shared_ptr<Log> shared = self.lock();
	while (shared && !shared->m_stop)
	{
		std::string str = shared->m_que.take();
		if (str.empty() && shared->m_stop) {
			break;
		}

		switch (shared->m_type)
		{
		case Log::log2_console:
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), NULL, NULL);
			break;
		case Log::log2_output:
			OutputDebugStringA(str.c_str());
			break;
		case Log::log2_file:
			if (shared->m_file.is_open())
			{
				shared->m_file.write(str.c_str(), str.length());
				shared->m_file.flush();

				std::string path = shared->makeFileName();
				if (path != shared->m_logFullPath) {
					shared->m_file.close();
					shared->m_logFullPath = path;
					shared->m_file.open(shared->m_logFullPath, std::ios::app);
				}
			}
			break;
		default:
			break;
		}
	}
}

std::string Log::formatLogW(const std::wstring& str)
{
	std::ostrstream out;
	std::string tmp = Encode::w2a(str);
	char buf[100] = { 0 };
	struct tm now;
	time_t t = time(0);
	localtime_s(&now, &t);
	sprintf_s(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	out << "[" << buf << "--" << std::this_thread::get_id() << "]-->" << tmp << "\r\n" << std::ends;
	tmp = out.str();
	delete out.str();
	return tmp;
}

std::string Log::formatLogA(const std::string& str)
{
	std::ostrstream out;
	std::string tmp = str;
	char buf[100] = { 0 };
	struct tm now;
	time_t t = time(0);
	localtime_s(&now, &t);
	sprintf_s(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	out << "[" << buf << "--" << std::this_thread::get_id() << "]-->" << tmp << "\r\n" << std::ends;
	tmp = out.str();
	delete out.str();
	return tmp;
}

std::string Log::makeFileName()
{
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time, &time_seconds);
	wchar_t tmp[100] = { 0 };
	wsprintf(tmp, L"%s-%04d-%02d-%02d.log", m_logName.c_str(), now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday);

	std::string path = Encode::w2a(Path::join(m_logPath.c_str(), tmp));
	return path;
}
