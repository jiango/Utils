#pragma once

#include <string>

class Encode
{
public:
	Encode() = delete;
	~Encode() = delete;

	static std::string w2utf8(const std::wstring& str);
	static std::wstring utf82w(const std::string& str);
	static std::string w2a(const std::wstring& str);
	static std::wstring a2w(const std::string& str);
	static std::string utf82a(const std::string& str);
	static std::string a2utf8(const std::string& str);

	static std::string b64_encode(const char* src, int src_len);
	static std::string b64_decode(const char* src, int src_len);
};

