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

	_progress_view* m_pProgressView;

public: // Methods

	_progress_hub()
		: _progress()
		, _progress_client()
		, m_pProgressView(nullptr)
	{
		setProgress(this);
	}

	virtual ~_progress_hub()
	{
	}

	virtual void onProgressInit(int iTotal, const std::string& strStage) override
	{
		if (m_pProgressView != nullptr) {
			m_pProgressView->onProgressInit(iTotal, strStage);
		}
	}

	virtual void report(int iCurrent, int iTotal, const char* szStage) override
	{
		if (m_pProgressView != nullptr) {
			m_pProgressView->onReport(iCurrent, iTotal, szStage);
		}
	}

	virtual void onProgressEnd() override
	{
		if (m_pProgressView != nullptr) {
			m_pProgressView->onProgressEnd();
		}
	}

	void setProgressView(_progress_view* pProgressView)
	{
		m_pProgressView = pProgressView;
	}
};
