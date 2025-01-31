#include "StringFunction.hpp"

void WCharToChar(char* str1, const wchar_t* wstr2, std::size_t max) {
	wcstombs(str1, wstr2, max);
}

void CharToWChar(wchar_t* wstr1, const char* str2, std::size_t max) {
	mbstowcs(wstr1, str2, max);
}
