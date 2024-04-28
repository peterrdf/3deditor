#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// ************************************************************************************************
#define NAMEOFVAR(variable) ((decltype(&variable))nullptr, #variable)

// ************************************************************************************************
class _settings_storage
{

private: // Members

	wstring m_strSettingsFile;
	map<string, string> m_mapSettings;

public: // Methods

	_settings_storage()
		: m_strSettingsFile(L"")
		, m_mapSettings()
	{}

	virtual ~_settings_storage()
	{}

	void loadSettings(const wstring& strSettingsFile)
	{
		m_strSettingsFile = strSettingsFile;

		ifstream streamSettings(m_strSettingsFile.c_str());
		if (!streamSettings)
		{
			ASSERT(FALSE);

			return;
		}

		string strSetting;
		while (getline(streamSettings, strSetting))
		{
			stringstream ssLine(strSetting);

			string strName;
			ssLine >> strName;

			string strValue;
			ssLine >> strValue;

			if (m_mapSettings.find(strName) != m_mapSettings.end())
			{
				ASSERT(FALSE);

				continue;
			}

			m_mapSettings[strName] = strValue;
		}
	}

	void saveSettings()
	{
		ofstream streamSettings(m_strSettingsFile.c_str());
		if (!streamSettings)
		{
			ASSERT(FALSE);

			return;
		}

		for (auto itSetting : m_mapSettings)
		{
			streamSettings << itSetting.first.c_str() << "\t" << itSetting.second.c_str() << "\n";
		}

		streamSettings.close();
	}

	string getSetting(const string& strName) const
	{
		auto itSetting = m_mapSettings.find(strName);
		if (itSetting == m_mapSettings.end())
		{
			return "";
		}

		return m_mapSettings.at(strName);
	}

	void setSetting(const string& strName, const string& strValue)
	{
		auto itSetting = m_mapSettings.find(strName);
		if (itSetting == m_mapSettings.end())
		{
			m_mapSettings[strName] = strValue;
		}
		else
		{
			m_mapSettings.at(strName) = strValue;
		}

		saveSettings();
	}
};

