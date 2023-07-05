#pragma once
#include "common.h"
#include <string>

namespace TExtender {
	class Utils {
	public:
		static char* WideCharToConstChar(wchar* inText);
		static wchar* ConstCharToWideChar(char* in);
		static wchar* ConstCharToWideChar(const char* in) { return ConstCharToWideChar((char*)in); }

		// https://github.com/DK22Pac/plugin-sdk/blob/master/shared/Other.h
		static std::wstring AtoW(std::string const& str);
		static std::string WtoA(std::wstring const& str);

		static std::string ToLowerCase(const char* in);
		static std::string ToUpperCase(const char* in);

		// https://github.com/DK22Pac/effects-loader/blob/master/EffectsLoader/Search.h
		static void ForAllFiles(char* folderpath, char* extension, void(*callback)(char*, void*), void* data);
	};
}