#ifndef HSTRING_FUNCTION
#define HSTRING_FUNCTION

#include "base.hpp"



#define TSTR   template <typename TypeVarStr>
#define TDSTR  template <typename TStr1, typename TStr2>

typedef char    achar_t;
typedef wchar_t uchar_t;

#define STRING_CHAR_CODE "char *"
#define STRING_CHAR_CODE_x64 "char * __ptr64"
#define STRING_WCHAR_CODE "wchar_t *"
#define STRING_WCHAR_CODE_x64 "wchar_t * __ptr64"
#define STRING_CONST_CHAR_CODE "const char *"
#define STRING_CONST_CHAR_CODE_x64 "const char * __ptr64"
#define STRING_CONST_WCHAR_CODE "const wchar_t *"
#define STRING_CONST_WCHAR_CODE_x64 "const wchar_t * __ptr64"

void WCharToChar(char* str1, const wchar_t* wstr2, std::size_t max);
void CharToWChar(wchar_t* wstr1, const char* str2, std::size_t max);

TSTR int32_t Cmp(const TypeVarStr *str1, const TypeVarStr *str2) {

	while ((*str1 != '\0') && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return (TypeVarStr)*str1 - (TypeVarStr)*str2;
}

TSTR int32_t CmpCopy(const TypeVarStr *str1, const TypeVarStr *str2) {

	while ((*str1 != '\0') && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return (TypeVarStr)*str1 - (TypeVarStr)*str2;
}

enum var_t
{
	unknown = 0,  // null
	achar = 1,
	uchar = 2
};

TSTR var_t GetType(TypeVarStr *PointBuffer)
{
	if (PointBuffer == NULL)
		return var_t::unknown;

	if ((Cmp(typeid(PointBuffer).name(), STRING_CHAR_CODE) == 0))            return var_t::achar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_WCHAR_CODE) == 0))           return var_t::uchar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_CHAR_CODE_x64) == 0))        return var_t::achar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_WCHAR_CODE_x64) == 0))       return var_t::uchar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_CONST_CHAR_CODE) == 0))      return var_t::achar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_CONST_WCHAR_CODE) == 0))     return var_t::uchar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_CONST_CHAR_CODE_x64) == 0))  return var_t::achar;
	if ((Cmp(typeid(PointBuffer).name(), STRING_CONST_WCHAR_CODE_x64) == 0)) return var_t::uchar;

	return var_t::unknown;
}


TSTR uint32_t Len(TypeVarStr *str1) {
	uint32_t len = 0;
	while (*str1 != 0x00) {
		str1++;
		len++;
	}
	return len;
}

TSTR void Cpy(TypeVarStr *str1, const TypeVarStr *str2) {
	TypeVarStr *TmpStr = str1;
	while ((*TmpStr++ = *str2++) != 0x00);
}

TSTR void Cat(TypeVarStr *str1, const TypeVarStr *str2)
{
	Cpy(&str1[Len(str1)], str2);
}

TSTR void CatInEnd(TypeVarStr *str1, const TypeVarStr *str2, uint32_t PositionEnd)
{
	Cpy(&str1[PositionEnd], str2);
}


TSTR void Clr(TypeVarStr *Str1, uint32_t Size) {
	for (uint32_t i = 0; i < Size; i++)
		Str1[i] = 0x00;
}

TDSTR void ConvertStr(TStr1* str1, const TStr2* str2) {

	if (GetType(str1) == GetType(str2)) return;
	if (GetType(str1) == var_t::achar) WCharToChar((char *)str1, (wchar_t*)str2, Len(str2));
	if (GetType(str1) == var_t::uchar) CharToWChar((wchar_t*)str1, (char *)str2, Len(str2));

}

TDSTR void Cpy(TStr1 *str1, const TStr2 *str2) {

	uint32_t len = Len(str2);
	if (len == 0) return;
	TStr1 *tmp = new TStr1[len + 1];
	ConvertStr(tmp, str2);
	tmp[len] = NULL;
	Cpy<TStr1>(str1, tmp);
	str1[len] = NULL;
	delete[] tmp;
}

TDSTR void Cat(TStr1 *str1, const TStr2 *str2) {
	uint32_t len = Len(str2);
	if (len == 0) return;
	TStr1 *tmp = new TStr1[len + 1];
	ConvertStr(tmp, str2);
	tmp[len] = NULL;
	Cat<TStr1>(str1, tmp);
	delete[] tmp;
}

TDSTR int32_t Cmp(TStr1 *str1, const TStr2 *str2) {
	uint32_t len = Len(str2);
	if (len == 0) return -1;
	TStr1 *tmp = new TStr1[len + 1];
	tmp[len] = NULL;
	Cpy(tmp, str2);
	int32_t Result = CmpCopy<TStr1>(str1, tmp);
	delete[] tmp;
	return Result;
}

#endif
