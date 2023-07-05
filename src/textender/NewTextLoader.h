/*
    Plugin-SDK (Grand Theft Auto) SHARED header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include "common.h"

namespace TExtender {
	class CNewTextLoader {
	public:
		static std::unordered_map<std::string, std::wstring> stringMap;

		static void Init();
		static bool Load(const std::string& fileName);
		static void Clear();

		static wchar* Get(const char* key); // Get() must be called in CText::Get()
	};
}