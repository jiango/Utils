#include "Path.h"
#include <Windows.h>
#include <Shlobj.h>

HMODULE GetThisModuleHandle()
{
	HMODULE hModule = NULL;
	return GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetThisModuleHandle, &hModule) ? hModule : NULL;
}

std::wstring Path::getcwd()
{
#ifdef WIN32
	wchar_t szAppPath[MAX_PATH];
	GetModuleFileName(GetThisModuleHandle(), szAppPath, MAX_PATH);
	(wcsrchr(szAppPath, L'\\'))[1] = 0;
	return szAppPath;
#else
#endif
}

std::string Path::getcwdA()
{
#ifdef WIN32
	char szAppPath[MAX_PATH];
	GetModuleFileNameA(GetThisModuleHandle(), szAppPath, MAX_PATH);
	(strrchr(szAppPath, L'\\'))[1] = 0;
	return szAppPath;
#else
#endif
}

std::wstring Path::getProgramDir()
{
#ifdef WIN32
	wchar_t szPath[MAX_PATH] = { 0 };
	BOOL bWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(),&bWow64);
	if (bWow64)
		SHGetFolderPath(NULL, CSIDL_PROGRAM_FILESX86, NULL, SHGFP_TYPE_CURRENT, szPath);
	else 
		SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, szPath);
	return szPath;
#else
#endif
}

std::wstring Path::getappdir()
{
#ifdef WIN32
	wchar_t dir[MAX_PATH + 1] = { 0 };
	wchar_t doc[MAX_PATH + 1] = { 0 };
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &pidl);
	if (pidl && SHGetPathFromIDList(pidl, doc)) {
		GetShortPathName(doc, dir, _MAX_PATH);
	}
	return dir;
#else
#endif
}

std::wstring Path::join(const wchar_t* l, const wchar_t* r)
{
	std::wstring str(l);
	str += L"\\";
	str += r;
	return clean(str);
}

std::string Path::join(const char* l, const char* r)
{
	std::string str(l);
	str += "\\";
	str += r;
	return clean(str);
}

std::wstring Path::clean(std::wstring& path)
{
	std::wstring str;
	bool flag = false;

	for (auto iter = path.begin(); iter != path.end(); ++iter) {
		if (*iter == L'\\' || *iter == L'/') {
			if (flag) {
				continue;
			} else {
				flag = true;
				str += (*iter);
			}
		} else {
			flag = false;
			str += (*iter);
		}
	}

	return str;
}

std::string Path::clean(std::string& path)
{
	std::string str;
	bool flag = false;

	for (auto iter = path.begin(); iter != path.end(); ++iter) {
		if (*iter == '\\' || *iter == '/') {
			if (flag) {
				continue;
			} else {
				flag = true;
				str += (*iter);
			}
		} else {
			flag = false;
			str += (*iter);
		}
	}

	return str;
}

std::wstring Path::baseName(const std::wstring& path)
{
	std::wstring name = path;

	if (name.empty())
		return L".";

	if (name[name.length() - 1] == L'/' ||
		name[name.length() - 1] == L'\\') {
		name = name.substr(0, name.length() - 1);
	}

	size_t n = name.rfind(L"\\");
	if (n != std::string::npos) {
		name = name.substr(n + 1, name.length() - n);
	}

	if (name.empty())
		return L"\\";

	return name;
}

std::wstring Path::ext(const std::wstring& path)
{
	std::wstring extName;
	size_t n = path.rfind(L".");
	if (n != std::string::npos) {
		extName = path.substr(n, path.length() - n);
	}
	return extName;
}
