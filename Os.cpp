#include "Os.h"

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <commdlg.h>
#pragma comment(lib, "Psapi.lib")
#endif

bool Os::copy(const std::wstring& src, const std::wstring& dst)
{
#ifdef _WIN32
	return (CopyFile(src.c_str(), dst.c_str(), false) == TRUE);
#else
#endif
}

bool Os::del(const std::wstring& path)
{
#ifdef _WIN32
	return (DeleteFile(path.c_str()) == TRUE);
#else
#endif
}

bool Os::del(const std::string& path)
{
#ifdef _WIN32
	return (DeleteFileA(path.c_str()) == TRUE);
#else
#endif
}

bool Os::mkdir(const wchar_t* dirname)
{
#ifdef _WIN32
	int en = _wmkdir(dirname);
	if (en == 0 || en == EEXIST) {
		return true;
	}
	return false;
#else
#endif
}

std::vector<std::wstring> Os::selectfiles(void* parent, const wchar_t *filter)
{
#ifdef _WIN32
	std::vector<std::wstring> filelist;
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH * 10] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = (HWND)parent;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	if (!GetOpenFileName(&ofn))
	{
		return filelist;
	}

	wchar_t path[MAX_PATH] = { 0 };
	wmemcpy(path, szFile, ofn.nFileOffset);
	if (path[ofn.nFileOffset - 1] != L'\\')
	{
		path[ofn.nFileOffset - 1] = L'\\';
		path[ofn.nFileOffset] = 0;
	}
	wchar_t* str = szFile + ofn.nFileOffset;
	while (*str) {
		TCHAR szFileName[MAX_PATH] = { 0 };
		wsprintf(szFileName, L"%s%s", path, str);
		filelist.push_back(szFileName);
		str += wcslen(str);
		str++;
	}

	return filelist;
#else
	return std::vector<std::wstring>();
#endif
}

bool Os::isProcessExist(const wchar_t* processName)
{
#ifdef WIN32
	//step1: ö�����еĽ��̣���ȡ�����id
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	for (int i = 0; i < 1024; i++) {
		aProcesses[i] = 0;
	}
	unsigned int i = 0;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return false;

	wchar_t szProcessName[MAX_PATH];
	wchar_t szProcessPath[MAX_PATH];

	//step2: �ȽϽ������ƣ������ͬ���򷵻�true
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		memset(szProcessName, 0, MAX_PATH);
		memset(szProcessPath, 0, MAX_PATH);

		HANDLE hProcess = NULL;
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, aProcesses[i]);

		if (hProcess != NULL) {
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
				DWORD  dwRetVal = 0;
				DWORD  dwRetValEx = 0;
				dwRetVal = GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName));
				dwRetValEx = GetModuleFileNameEx(hProcess, hMod, szProcessPath, sizeof(szProcessPath));

				if (!wcscmp(processName, szProcessName)) {
					CloseHandle(hProcess);
					return true;
				}
			}

			CloseHandle(hProcess);
		}
	}

	return false;
#else
	return false;
#endif
}

bool Os::killProcess(const wchar_t *processName)
{
#ifdef WIN32
	if (!processName)
		return false;

	int len = wcslen(processName);
	if (!len)
		return false;

	//�������̿���(TH32CS_SNAPPROCESS��ʾ�������н��̵Ŀ���)
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
		return false;

	//PROCESSENTRY32���̿��յĽṹ��
	PROCESSENTRY32 pe;

	//ʵ������ʹ��Process32First��ȡ��һ�����յĽ���ǰ�����ĳ�ʼ������
	pe.dwSize = sizeof(PROCESSENTRY32);

	//�����IFЧ��ͬ:
	if (!Process32First(hSnapShot, &pe)) {
		CloseHandle(hSnapShot);
		return false;
	}

	bool result = false;
	int compareResult = 0;

	//��������Ч  ��һֱ��ȡ��һ�����ѭ����ȥ
	while (Process32Next(hSnapShot, &pe)) {
#ifdef UNICODE
		compareResult = wcscmp(processName, pe.szExeFile);
#else
		compareResult = wcscmp(processName, pe.szExeFile);
#endif

		//�Ƚϵ�ǰ���̵Ŀ�ִ���ļ����ƺʹ��ݽ������ļ������Ƿ���ͬ
		//��ͬ�Ļ�Compare����0
		if (!compareResult) {
			//�ӿ��ս����л�ȡ�ý��̵�PID(������������е�PID)
			DWORD dwProcessID = pe.th32ProcessID;
			CloseHandle(hSnapShot);
			hSnapShot = INVALID_HANDLE_VALUE;

			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			if (!hProcess) {
				result = false;
				break;
			}
			::TerminateProcess(hProcess, 0);
			WaitForSingleObject(hProcess, INFINITE);

			int count = 100;
			while (isProcessExist(processName) && count) {
				Sleep(100);
				--count;
			}

			CloseHandle(hProcess);

			result = true;
			break;
		}
	}

	if (hSnapShot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapShot);
	return result;

#else
	return false;
#endif // WIN32
}

std::string Os::system(const char* cmd)
{
	std::string ret;

	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		return ret;
	}

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;  
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if (!CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		return ret;
	}
	CloseHandle(hWrite);

	char buffer[4096] = { 0 };
	DWORD bytesRead;
	while (true) {
		memset(buffer, 0, 4096);
		if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
			break;
		ret += buffer;
	}
	CloseHandle(hRead);
	return ret;
}