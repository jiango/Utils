#pragma once

#include <string>
#include <vector>

class Os {
public:
	Os() = delete;
	~Os() = delete;

	static bool copy(const std::wstring& src, const std::wstring& dst);
	static bool del(const std::wstring& path);
	static bool del(const std::string& path);
	static bool mkdir(const wchar_t* dirname);
	static std::vector<std::wstring> selectfiles(void* parent, const wchar_t *filter);
	static bool isProcessExist(const wchar_t* processName);
	static bool killProcess(const wchar_t *processName);
	static std::string system(const char* cmd);
};