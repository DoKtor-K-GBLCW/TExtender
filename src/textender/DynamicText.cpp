#include "DynamicText.h"
#include <string>

namespace TExtender {
	std::unordered_map<std::string, std::wstring> CDynamicText::dynamicStringMap;
	std::unordered_map<std::string, CDynamicText::WCharReter> CDynamicText::dynamicFuncMap;

	void CDynamicText::Set(std::string key, std::wstring value)
	{
		if (FuncExists(key))
			RemoveFunc(key);

		dynamicStringMap[key] = value;
	}

	void CDynamicText::Set(std::string key, wchar* value)
	{
		Set(key, (wchar_t*)value);
	}

	void CDynamicText::Set(std::string key, WCharReter value)
	{
		if (StrExists(key))
			RemoveStr(key);

		dynamicFuncMap[key] = value;
	}

	wchar* CDynamicText::Get(std::string key)
	{
		if (FuncExists(key))
			return dynamicFuncMap.find(key)->second();

		return StrExists(key) ? ((wchar*)(dynamicStringMap.find(key)->second.c_str())) : NULL;
	}

	void CDynamicText::Add(std::string key, std::wstring value)
	{
		Set(key, value);
	}

	void CDynamicText::Add(std::string key, wchar* value)
	{
		Set(key, value);
	}

	void CDynamicText::Add(std::string key, WCharReter value)
	{
		Set(key, value);
	}

	void CDynamicText::Clear()
	{
		dynamicFuncMap.clear();
		dynamicStringMap.clear();
	}

	bool CDynamicText::FuncExists(std::string key)
	{
		return (dynamicFuncMap.find(key) != dynamicFuncMap.end());
	}

	bool CDynamicText::StrExists(std::string key)
	{
		return (dynamicStringMap.find(key) != dynamicStringMap.end());
	}

	bool CDynamicText::Exists(std::string key)
	{
		return FuncExists(key) || StrExists(key);
	}

	void CDynamicText::RemoveFunc(std::string key)
	{
		auto it = dynamicFuncMap.find(key);
		if (it != dynamicFuncMap.end())
			dynamicFuncMap.erase(it);
	}

	void CDynamicText::RemoveStr(std::string key)
	{
		auto it2 = dynamicStringMap.find(key);
		if (it2 != dynamicStringMap.end())
			dynamicStringMap.erase(it2);
	}

	void CDynamicText::Remove(std::string key)
	{
		RemoveFunc(key);
		RemoveStr(key);
	}

	void CDynamicText::Set(std::string section, std::string key, std::wstring value)
	{
		auto realKey = section + "::";
		realKey.append(key);
		Set(realKey, value);
	}

	void CDynamicText::Set(std::string section, std::string key, wchar* value)
	{
		auto realKey = section + "::";
		realKey.append(key);
		Set(realKey, (wchar_t*)value);
	}

	void CDynamicText::Set(std::string section, std::string key, WCharReter value)
	{
		auto realKey = section + "::";
		realKey.append(key);
		Set(realKey, value);
	}

	wchar* CDynamicText::Get(std::string section, std::string key)
	{
		auto realKey = section + "::";
		realKey.append(key);
		return Get(realKey);
	}
	
	void CDynamicText::Add(std::string section, std::string key, std::wstring value)
	{
		Set(section, key, value);
	}

	void CDynamicText::Add(std::string section, std::string key, wchar* value)
	{
		Set(section, key, (wchar_t*)value);
	}

	void CDynamicText::Add(std::string section, std::string key, WCharReter value)
	{
		Set(section, key, value);
	}

	bool CDynamicText::Exists(std::string section, std::string key)
	{
		auto realKey = section + "::";
		realKey.append(key);
		return Exists(realKey);
	}

	void CDynamicText::Remove(std::string section, std::string key)
	{
		auto realKey = section + "::";
		realKey.append(key);
		Remove(realKey);
	}

	void CDynamicText::RemoveSection(std::string section)
	{
		for (auto& entry : dynamicStringMap) {
			auto &key = entry.first;
			if (key.length() < (section.length() + 2))
				continue;
			if (key.substr(0, section.size()) == section && key.substr(section.size(), 2) == "::")
				Remove(key);
				//dynamicStringMap.erase(dynamicStringMap.find(key));
		}
		for (auto& entry : dynamicFuncMap) { // dynamicStringMap
			auto& key = entry.first;
			if (key.length() < (section.length() + 2))
				continue;
			if (key.substr(0, section.size()) == section && key.substr(section.size(), 2) == "::")
				Remove(key);
				//dynamicFuncMap.erase(dynamicFuncMap.find(key));
		}
	}

}