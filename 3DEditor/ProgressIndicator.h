#pragma once

// ------------------------------------------------------------------------------------------------
// Progress indicator
struct ProgressStatus
{
	ProgressStatus(LPCTSTR phase);
	~ProgressStatus();

	void Start(int64_t range);
	void Step();
	void Finish();

private:
	void SetStatusText(LPCTSTR text = NULL);

private:
	LPCTSTR m_phase;
	int64_t m_range;
	int64_t m_step;
	int64_t m_10procents;
};

