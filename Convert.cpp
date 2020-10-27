#include "Convert.h"
#include <string>

int Convert::stoi(const char* text, int defaultValue)
{
	if (text == nullptr) {
		return defaultValue;
	}

	int n = defaultValue;
	try {
		n = std::stoi(text);
	}
	catch (std::invalid_argument e) {

	}
	return n;
}

float Convert::stof(const char* text, float defaultValue)
{
	if (text == nullptr) {
		return defaultValue;
	}

	float n = defaultValue;
	try {
		n = std::stof(text);
	}
	catch (std::invalid_argument e) {

	}
	return n;
}

unsigned long Convert::stoul(const char* text, unsigned long defaultValue)
{
	if (text == nullptr) {
		return defaultValue;
	}

	unsigned long n = defaultValue;
	try {
		n = std::stoul(text);
	}
	catch (std::invalid_argument e) {

	}
	return n;
}

