#include "encode.h"
#include <codecvt>
#include <Windows.h>

std::string Encode::w2utf8(const std::wstring& str)
{
	if (str.empty())
		return std::string("");

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(str);
}

std::wstring Encode::utf82w(const std::string& str)
{
	if (str.empty())
		return std::wstring(L"");

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(str);
}

std::string Encode::w2a(const std::wstring& str)
{
	int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, 0, 0, 0, 0);
	char *ch = new char[size];
	if (!WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, ch, size, 0, 0)) {
		delete[] ch;
		return "";
	}
	std::string temp = std::string(ch);
	delete[] ch;
	return temp;
}

std::wstring Encode::a2w(const std::string& str)
{
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, 0, 0);
	wchar_t *ch = new wchar_t[size];
	if (!MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, ch, size)) {
		delete[] ch;
		return L"";
	}
	std::wstring buf(ch, size);
	delete[]ch;
	return buf;
}

std::string Encode::utf82a(const std::string& str)
{
	std::wstring s = Encode::utf82w(str);
	return Encode::w2a(s);
}

std::string Encode::a2utf8(const std::string& str)
{
	std::wstring s = Encode::a2w(str);
	return Encode::w2utf8(s);
}

std::string Encode::b64_encode(const char* src, int src_len)
{
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i<(int)(src_len / 3); i++)
	{
		Tmp[1] = *src++;
		Tmp[2] = *src++;
		Tmp[3] = *src++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		// 
		//if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}

	int Mod = src_len % 3;
	if (Mod == 1)
	{
		Tmp[1] = *src++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *src++;
		Tmp[2] = *src++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

std::string Encode::b64_decode(const char* src, int src_len)
{
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	std::string strDecode;
	int nValue;
	int i = 0;
	while (i < src_len)
	{
		if (*src != '\r' && *src != '\n')
		{
			nValue = DecodeTable[*src++] << 18;
			nValue += DecodeTable[*src++] << 12;
			strDecode += (nValue & 0x00FF0000) >> 16;
			//dst_len++;
			if (*src != '=')
			{
				nValue += DecodeTable[*src++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;
				//dst_len++;
				if (*src != '=')
				{
					nValue += DecodeTable[*src++];
					strDecode += nValue & 0x000000FF;
					//dst_len++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			src++;
			i++;
		}
	}
	return strDecode;
}
