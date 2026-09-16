#pragma once

#include "_progress.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class _progress_view {

public: // Methods

	_progress_view()
	{
	}

	virtual ~_progress_view()
	{
	}

	virtual void onProgressInit(int /*iTotal*/, const std::string& /*strStage*/)
	{
	}

	virtual void onReport(int /*iCurrent*/, int /*iTotal*/, const char* /*szStage*/) = 0;

	virtual void onProgressEnd()
	{
	}
};

// ************************************************************************************************
class _progress_hub
	: public _progress
	, public _progress_client {

private: // Members

	std::vector< _progress_view*> m_vecProgressViews;

public: // Methods

	_progress_hub()
		: _progress()
		, _progress_client()
		, m_vecProgressViews()
	{
		setProgress(this);
	}

	virtual ~_progress_hub()
	{
	}

	virtual void onProgressInit(int iTotal, const std::string& strStage) override
	{
		for (auto pProgressView : m_vecProgressViews) {
			if (pProgressView != nullptr) {
				pProgressView->onProgressInit(iTotal, strStage);
			}
		}
	}

	virtual void report(int iCurrent, int iTotal, const char* szStage) override
	{
		for (auto pProgressView : m_vecProgressViews) {
			if (pProgressView != nullptr) {
				pProgressView->onReport(iCurrent, iTotal, szStage);
			}
		}
	}

	virtual void onProgressEnd() override
	{
		for (auto pProgressView : m_vecProgressViews) {
			if (pProgressView != nullptr) {
				pProgressView->onProgressEnd();
			}
		}
	}

	void addProgressView(_progress_view* pProgressView)
	{
		if (pProgressView != nullptr) {
			m_vecProgressViews.push_back(pProgressView);
		}
	}

	void removeProgressView(_progress_view* pProgressView)
	{
		if (pProgressView != nullptr) {
			auto it = std::find(m_vecProgressViews.begin(), m_vecProgressViews.end(), pProgressView);
			if (it != m_vecProgressViews.end()) {
				m_vecProgressViews.erase(it);
			}
		}
	}
};
