#include "Path.h"
#include <Windows.h>
#include <Shlobj.h>

std::wstring Path::getcwd()
{
#ifdef WIN32
	wchar_t szAppPath[MAX_PATH];
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	(wcsrchr(szAppPath, L'\\'))[1] = 0;
	return szAppPath;
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
	std::wstring name;
	size_t n = path.rfind(L"\\");
	if (n != std::string::npos) {
		name = path.substr(n + 1, path.length() - n);
	}
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
