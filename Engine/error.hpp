#pragma once
#include "base.hpp"

class CError
{
public:
	CError() = default;
	~CError() = default;

	bool        isError = false;
	std::string TextError = "";

	CError(std::string Text_Error) { isError = true;  TextError = Text_Error; }

	inline operator bool() { return isError; }
	inline operator const char*()       const { return TextError.c_str(); }
	inline operator const std::string() const { return TextError; }

	CError& operator = (std::string Text_Error) { isError = true; TextError = Text_Error; printf("%s\n", TextError.c_str()); return *this; }
	CError& operator = (const char *Text_Error) { isError = true; TextError = Text_Error; printf("%s\n", TextError.c_str()); return *this; }
	CError& operator = (const bool  newStatus)  { isError = newStatus;                    return *this; }

	CError format(const char *text, ...)
	{
		char Buffer[512];

		va_list vl;
		va_start(vl, text);
		VSNPRINTF(Buffer, 511, text, vl);
		va_end(vl);

		*this = Buffer;		

		return *this;
	}

};
