#pragma once

#include "base.hpp"
#include "error.hpp"

#ifdef PLATFORM_WINDOWS
#define RunArg	wchar_t ResultArg[MAX_PATH];   va_list vl;	va_start(vl, Arg);	VSNWPRINTF(ResultArg, MAX_PATH - 1, Arg, vl);	va_end(vl);
#else
#define RunArg  wchar_t ResultArg[MAX_PATH];    va_list vl;	va_start(vl, Arg);	vswprintf(ResultArg, MAX_PATH - 1, Arg, vl);	va_end(vl);
#endif


class CPath
{

public:

	CPath()
	{
		PathExe[0] = 0x00;
		PathDir[0] = 0x00;
	}

#ifdef PLATFORM_LINUX
	void GetPathExeLinux(wchar_t *Path) {
		char buff[MAX_PATH] = { 0 };
		Path[0] = 0x00;

		ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);

		if (len != -1) {
			buff[len] = 0x00;
			Cpy(Path, buff);
		}
	}
#endif

	wchar_t PathExe[MAX_PATH];
	wchar_t PathDir[MAX_PATH];

	// Получаем путь нашей программы
	wchar_t *GetPathExe() {
#ifdef  PLATFORM_WINDOWS
		GetModuleFileNameW(NULL, PathExe, sizeof(PathExe));
#else
		GetPathExeLinux(PathExe);
#endif
		return PathExe;
	}

	wchar_t *GetPathExe(wchar_t const *Arg, ...) {
		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif

		return PathDir;
	}

	wchar_t *GetPathExe(wchar_t *Str, wchar_t const *Arg, ...) {
		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif


		wcscpy(Str, PathDir);
	}

	void GetPathExe(wchar_t *Str) {
		wcscpy(Str, GetPathExe());
	}

	wchar_t *GetPathDir() {

#ifdef PLATFORM_WINDOWS
		wcscpy(PathDir, GetPathExe());
		PathRemoveFileSpecW(PathDir);
#endif

#ifdef PLATFORM_LINUX

		char   PathBuffer[MAX_PATH] = {};
		getcwd(PathBuffer, MAX_PATH);
		Cpy(PathDir, PathBuffer);

#endif

		return PathDir;
	}

	wchar_t *GetPathDir(wchar_t const *Arg, ...)  {
		RunArg;
#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathDir(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathDir(), ResultArg);
#endif
		return PathDir;
	}

	void GetPathDir(wchar_t *Str) { wcscpy(Str, GetPathDir()); }
	void GetPathDir(wchar_t *Str, wchar_t const *Arg, ...) {

		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathDir(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathDir(), ResultArg);
#endif

		wcscpy(Str, PathDir);
	}

#ifdef PLATFORM_WINDOWS

	wchar_t* GetPathProcess(DWORD processID) {

		HMODULE hMods[1024];
		HANDLE  hProcess;
		DWORD   cbNeeded;

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

		if (NULL == hProcess)
			return L"";

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (uint32_t i = 0; i < 1; i++)
				GetModuleFileNameExW(hProcess, hMods[i], PathExe, sizeof(PathExe));

			return PathExe;
		}

		CloseHandle(hProcess);
		return L"";
	}

#endif

};