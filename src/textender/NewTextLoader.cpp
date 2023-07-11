/*
    Plugin-SDK (Grand Theft Auto) SHARED header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "NewTextLoader.h"
#include "DynamicText.h"

#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>

#include "Search.h"
#include "Utils.h"

#include <string>

namespace TExtender {

	std::unordered_map<std::string, std::wstring> CNewTextLoader::stringMap;
	std::map<wchar_t, wchar_t> gxt_table = {
		// https://github.com/Sergeanur/GXT-compiler/blob/main/tables/vc_sa_table.txt
			{ 0x3C, 0x5B },
			{ 0x3E, 0x5D },
			{ 0xA1, 0x5E },
			{ 0xB4, 0xB1 },
			{ 0xBF, 0xAF },
			{ 0xC0, 0x80 },
			{ 0xC1, 0x81 },
			{ 0xC2, 0x82 },
			{ 0xC4, 0x83 },
			{ 0xC6, 0x84 },
			{ 0xC7, 0x85 },
			{ 0xC8, 0x86 },
			{ 0xC9, 0x87 },
			{ 0xCA, 0x88 },
			{ 0xCB, 0x89 },
			{ 0xCC, 0x8A },
			{ 0xCD, 0x8B },
			{ 0xCE, 0x8C },
			{ 0xCF, 0x8D },
			{ 0xD1, 0xAD },
			{ 0xD2, 0x8E },
			{ 0xD3, 0x8F },
			{ 0xD4, 0x90 },
			{ 0xD6, 0x91 },
			{ 0xD9, 0x92 },
			{ 0xDA, 0x93 },
			{ 0xDB, 0x94 },
			{ 0xDC, 0x95 },
			{ 0xDF, 0x96 },
			{ 0xE0, 0x97 },
			{ 0xE1, 0x98 },
			{ 0xE2, 0x99 },
			{ 0xE4, 0x9A },
			{ 0xE6, 0x9B },
			{ 0xE7, 0x9C },
			{ 0xE8, 0x9D },
			{ 0xE9, 0x9E },
			{ 0xEA, 0x9F },
			{ 0xEB, 0xA0 },
			{ 0xEC, 0xA1 },
			{ 0xED, 0xA2 },
			{ 0xEE, 0xA3 },
			{ 0xEF, 0xA4 },
			{ 0xF1, 0xAE },
			{ 0xF2, 0xA5 },
			{ 0xF3, 0xA6 },
			{ 0xF4, 0xA7 },
			{ 0xF6, 0xA8 },
			{ 0xF9, 0xA9 },
			{ 0xFA, 0xAA },
			{ 0xFB, 0xAB },
			{ 0xFC, 0xAC },
	};

	void CNewTextLoader::Init()
	{
		//XLOGL("CNewTextLoader::Started()");
		Utils::ForAllFiles("TEXT", "xfxt", [](char* path, void* dictionary) {
			//XLOGL("CNewTextLoader::Load() %s", path);
			/*bool res =*/ CNewTextLoader::Load(path);
			//if(res)
				//XLOGL("CNewTextLoader::Loaded() %s", path);
		}, nil);

	}

	void ApplyTable(std::wstring& str) {
		bool tag = false;
		for (auto& c : str) {
			if (c == '~')
				tag ^= true;

			if (tag)
				continue;

			auto it = gxt_table.find(c);
			if (it != gxt_table.end()) {
				c = it->second;
			}
		}
	}

	bool CNewTextLoader::Load(const std::string& fileName) {
		std::wifstream file(fileName);
		file.imbue(std::locale(file.getloc(), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>));
		//file.exceptions(std::ios::badbit);

		if (!file.is_open()) {
			return false;
		}

		//XLOGL("CNewTextLoader::Loaded()");

		std::wstring line;
		std::string currentKey;
		std::wstring currentValue;

		uint16 numEntries = 0;
		while (std::getline(file, line)) {

			if (line.size() < 2)
				continue;
			if (line.at(0) == '#' || line.at(0) == ';' || (line.at(0) == '/' && line.at(1) == '/'))
				continue;

			//XLOGL("CNewTextLoader::__LINE__: %ws", line.c_str());
			if (line.size() > 2 && line.front() == '[' && line.back() == ']') {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				currentKey = converter.to_bytes(line.substr(1, line.size() - 2));
				currentValue = {};
			}
			else {
				currentValue = line;
			}

			if (!currentKey.empty() && !currentValue.empty()) {
				ApplyTable(currentValue);
				stringMap[currentKey] = currentValue;
				numEntries++;
				//XLOGL("CNewTextLoader::Register_Key() %s", currentKey.c_str());
			}
		}

		file.close();

		//XLOGL("CNewTextLoader::Closed()");

		return !!numEntries;
	}

	wchar* CNewTextLoader::Get(const char* key) {

		auto iter = stringMap.find(key);

		static wchar_t value[MAX_TEXT_SIZE];
		if ((iter != stringMap.end()) || CDynamicText::Exists(key)) {

			wmemset(value, 0, MAX_TEXT_SIZE);

			if(CDynamicText::Exists(key)) { // First Search in Dynamic Entries
				auto dyvalue_w = std::wstring((wchar_t*)CDynamicText::Get(key));
				std::wmemcpy(value, dyvalue_w.c_str(), min(dyvalue_w.length(), MAX_TEXT_SIZE));
			}
			else
				std::wmemcpy(value, iter->second.c_str(), min(iter->second.length(), MAX_TEXT_SIZE));
			
			// Unlimited Forward-Linking
			while (value[0] == L'@') {
				auto value_w = std::wstring(value);
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				auto newKey = converter.to_bytes(value_w.substr(1, value_w.size() - 1));

				// Search In Dynamic Entries
				if (CDynamicText::Exists(newKey)) {
					wmemset(value, 0, MAX_TEXT_SIZE);
					auto dyvalue_w = std::wstring((wchar_t*)CDynamicText::Get(newKey));
					std::wmemcpy(value, dyvalue_w.c_str(), min(dyvalue_w.length(), MAX_TEXT_SIZE));
					continue;
				}

				auto iter2 = stringMap.find(newKey);
				if (iter2 != stringMap.end()) { // search in xfxt storage
					wmemset(value, 0, MAX_TEXT_SIZE);
					std::wmemcpy(value, iter2->second.c_str(), min(iter2->second.length(), MAX_TEXT_SIZE));
				}
				else { // search in gxt storage
					auto gxtVal = TheText.Get(newKey.c_str(), true); // return null if you cant find
					if (gxtVal) {
						auto gxtValW = std::wstring((wchar_t*)gxtVal);
						wmemset(value, 0, MAX_TEXT_SIZE);
						std::wmemcpy(value, gxtValW.c_str(), min(gxtValW.length(), MAX_TEXT_SIZE));
					}
					else // not found, return value exactly, eg: "@MAP_YAH"
						break;
				}
			}

			auto s_len = wcslen(value);
			if(!s_len)
				return NULL;
			value[s_len] = '\0';
			return (wchar *)value;
		}

		return NULL;
	}

	void CNewTextLoader::Clear() {
		stringMap.clear();
	}

}