#pragma once

#include "_log.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class _log_view
{

public: // Methods

	_log_view()
	{}

	virtual ~_log_view()
	{}

	virtual void onLogWrite(enumLogEvent /*enLogEvent*/, const std::string& /*strEvent*/) = 0;
};

// ************************************************************************************************
class _log_hub 
	: public _log
	, public _log_client
{

private: // Members

	vector<pair<enumLogEvent, string>> m_vecMessages;
	_log_view* m_pLogView;

public: // Methods

	_log_hub()
		: _log()
		, _log_client()
		, m_vecMessages()
		, m_pLogView(nullptr)
	{
		setLog(this);
	}

	virtual ~_log_hub()
	{}

	virtual void logWrite(enumLogEvent enLogEvent, const std::string& strEvent) override
	{
		m_vecMessages.push_back(make_pair(enLogEvent, _time::addDateTimeStamp(strEvent)));

		if (m_pLogView != nullptr) {
			auto& lastMessage = m_vecMessages.back();
			m_pLogView->onLogWrite(lastMessage.first, lastMessage.second);
		}
	}

	void setLogView(_log_view* pLogView)
	{
		m_pLogView = pLogView;
	}
};
