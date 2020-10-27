#pragma once
#include <string>

class Path
{
public:
	Path() = delete;
	~Path() = delete;

	static std::wstring getcwd();
	static std::string getcwdA();
	static std::wstring getappdir();
	static std::wstring getProgramDir();
	static std::wstring join(const wchar_t* l, const wchar_t* r);
	static std::string join(const char* l, const char* r);
	static std::wstring clean(std::wstring& path);
	static std::string clean(std::string& path);
	static std::wstring baseName(const std::wstring& path);
	static std::wstring ext(const std::wstring& path);
};

