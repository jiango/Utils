#pragma once

class Convert
{
public:
	Convert() = delete;
	~Convert() = delete;

	static int stoi(const char* text, int defaultValue = 0);
	static float stof(const char* text, float defaultValue = 0.0f);
	static unsigned long stoul(const char* text, unsigned long defaultValue = 0);
};

