#include "Utils.h"

#include <Windows.h>
#include <stdio.h>

#include <string>

#include <algorithm>

//#include <cctype>
//#include <codecvt>

#ifndef MAX_TEXT_SIZE
#define MAX_TEXT_SIZE 2048
#endif

namespace TExtender {

	wchar*
	Utils::ConstCharToWideChar(char* text)
	{
		static wchar_t message_buf[MAX_TEXT_SIZE];
		swprintf(message_buf, MAX_TEXT_SIZE, L"%hs", text);
		return (wchar*)message_buf;

		//wchar_t* vIn = 0;
		//wchar *vOut = (wchar*)vIn;
		// as easy as this :D
	}

	char*
	Utils::WideCharToConstChar(wchar* inText)
	{
		//std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		//return (char*)converter.to_bytes((wchar_t*)inText).c_str();
		static char outText[MAX_TEXT_SIZE];
		snprintf(outText, MAX_TEXT_SIZE, "%ws", inText);
		return outText;
	}

	std::wstring
	Utils::AtoW(std::string const& str)
	{
		std::wstring result;
		result.resize(str.size());
		for (unsigned int i = 0; i < str.size(); i++)
			result[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
		return result;
	}

	std::string
	Utils::WtoA(std::wstring const& str)
	{
		std::string result;
		result.resize(str.size());
		for (unsigned int i = 0; i < str.size(); i++)
			result[i] = static_cast<char>(static_cast<unsigned char>(str[i]));
		return result;
	}

	std::string
	Utils::ToLowerCase(const char* in)
	{
		static std::string tmpData;
		tmpData.clear();
		tmpData.append(in);
		std::transform(tmpData.begin(), tmpData.end(), tmpData.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return tmpData;
	}

	std::string
	Utils::ToUpperCase(const char* in)
	{
		static std::string tmpData;
		tmpData.clear();
		tmpData.append(in);
		std::transform(tmpData.begin(), tmpData.end(), tmpData.begin(),
			[](unsigned char c) { return std::toupper(c); });
		return tmpData;
	}

	// https://github.com/DK22Pac/effects-loader/blob/master/EffectsLoader/Search.cpp
	void Utils::ForAllFiles(char* folderpath, char* extension, void(*callback)(char*, void*), void* data) {
		char search_path[MAX_PATH]; // increase if you need &want
		sprintf(search_path, "%s\\*.%s", folderpath, extension);
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path, &fd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && fd.cFileName[0] != '.') {
					char path[MAX_PATH];
					sprintf(path, "%s\\%s", folderpath, fd.cFileName);
					callback(path, data);
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
}