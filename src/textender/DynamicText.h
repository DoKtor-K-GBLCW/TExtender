#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include "common.h"

namespace TExtender {
	class CDynamicText {
	protected:
		typedef std::function<wchar*()> WCharReter;
		static std::unordered_map<std::string, std::wstring> dynamicStringMap;
		static std::unordered_map<std::string, WCharReter> dynamicFuncMap;

	public:
		static void Clear();

		static void Set(std::string key, std::wstring value);
		static void Set(std::string key, wchar* value);
		static void Set(std::string key, WCharReter value);
		static wchar* Get(std::string key);
		static void Add(std::string key, std::wstring value);
		static void Add(std::string key, wchar* value);
		static void Add(std::string key, WCharReter value);
		static bool FuncExists(std::string key);
		static bool StrExists(std::string key);
		static bool Exists(std::string key);
		static void RemoveFunc(std::string key);
		static void RemoveStr(std::string key);
		static void Remove(std::string key);

		static void Set(std::string section, std::string key, std::wstring value);
		static void Set(std::string section, std::string key, wchar* value);
		static void Set(std::string section, std::string key, WCharReter value);
		static wchar* Get(std::string section, std::string key);
		static void Add(std::string section, std::string key, std::wstring value);
		static void Add(std::string section, std::string key, wchar* value);
		static void Add(std::string section, std::string key, WCharReter value);
		static bool Exists(std::string section, std::string key);
		static void Remove(std::string section, std::string key);
		static void RemoveSection(std::string section);
	};
}