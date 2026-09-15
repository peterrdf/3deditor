#pragma once

#include <string>

// ************************************************************************************************
#ifdef _WINDOWS
typedef void(__stdcall* _progress_callback)(int iCurrent, int iTotal, const char* szStage);
#else
typedef void(*_progress_callback)(int iCurrent, int iTotal, const char* szStage);
#endif

// ************************************************************************************************
class _progress {

public: // Methods

	_progress()
	{
	}

	virtual ~_progress()
	{
	}

public: // Methods

	virtual void onProgressInit(int /*iTotal*/, const std::string& /*strStage*/)
	{
	}

	virtual void report(int iCurrent, int iTotal, const char* szStage) = 0;

	virtual void onProgressEnd()
	{
	}
};

// ************************************************************************************************
class _c_progress : public _progress {

private: // Members

	_progress_callback m_pProgressCallback;

public: // Methods

	_c_progress(_progress_callback pProgressCallback)
		: _progress()
		, m_pProgressCallback(pProgressCallback)
	{
	}

	virtual ~_c_progress()
	{
	}

public: // Methods

	virtual void report(int iCurrent, int iTotal, const char* szStage)
	{
		if (m_pProgressCallback != nullptr) {
			(*m_pProgressCallback)(iCurrent, iTotal, szStage);
		}
	}
};

// ************************************************************************************************
class _progress_client {

private: // Members

	_progress* m_pProgress;
	int m_iCurrent;
	int m_iTotal;
	std::string m_strStage;

public: // Methods

	_progress_client()
		: m_pProgress(nullptr)
		, m_iCurrent(0)
		, m_iTotal(0)
		, m_strStage("")
	{
	}

	virtual ~_progress_client()
	{
	}

	void setProgress(_progress* pProgress) { m_pProgress = pProgress; }
	_progress* getProgress() { return m_pProgress; }

	void progressInit(int iTotal, const std::string& strStage)
	{
		m_iCurrent = 0;
		m_iTotal = iTotal;
		m_strStage = strStage;

		if (m_pProgress != nullptr) {
			m_pProgress->onProgressInit(iTotal, strStage);
		}
	}

	void progressStep()
	{
		m_iCurrent++;
		report(m_iCurrent, m_iTotal, m_strStage.c_str());
	}

	void report(int iCurrent, int iTotal, const char* szStage)
	{
		if (m_pProgress != nullptr) {
			m_pProgress->report(iCurrent, iTotal, szStage);
		}
	}

	void progressEnd()
	{
		if (m_pProgress != nullptr) {
			m_pProgress->onProgressEnd();
		}
	}
};