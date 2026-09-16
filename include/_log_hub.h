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
	int m_iWarningCount;
	int m_iErrorCount;

	std::vector< _log_view*> m_vecLogViews;

public: // Methods

	_log_hub()
		: _log()
		, _log_client()
		, m_vecMessages()
		, m_iWarningCount(0)
		, m_iErrorCount(0)
		, m_vecLogViews()
	{
		setLog(this);
	}

	virtual ~_log_hub()
	{}

	virtual void logWrite(enumLogEvent enLogEvent, const std::string& strEvent) override
	{
		m_vecMessages.push_back(make_pair(enLogEvent, _time::addDateTimeStamp(strEvent)));
		if (m_vecMessages.size() > 200) {
			m_vecMessages.erase(m_vecMessages.begin());
		}

		if (enLogEvent == enumLogEvent::warning) {
			m_iWarningCount++;
		}
		else if (enLogEvent == enumLogEvent::error) {
			m_iErrorCount++;
		}

		for (auto pLogView : m_vecLogViews) {
			if (pLogView != nullptr) {
				auto& lastMessage = m_vecMessages.back();
				pLogView->onLogWrite(lastMessage.first, lastMessage.second);
			}
		}
	}

	void addLogView(_log_view* pLogView)
	{
		if (pLogView != nullptr) {
			m_vecLogViews.push_back(pLogView);
		}
	}

	void removeLogView(_log_view* pLogView)
	{
		if (pLogView != nullptr) {
			auto it = std::find(m_vecLogViews.begin(), m_vecLogViews.end(), pLogView);
			if (it != m_vecLogViews.end()) {
				m_vecLogViews.erase(it);
			}
		}
	}

	const vector<pair<enumLogEvent, string>>& getMessages() const { return m_vecMessages; }
	int getWarningsCount() const { return m_iWarningCount; }
	int getErrorsCount() const { return m_iErrorCount; }

	void clearMessages() { m_vecMessages.clear(); m_iWarningCount = 0; m_iErrorCount = 0; }
};
