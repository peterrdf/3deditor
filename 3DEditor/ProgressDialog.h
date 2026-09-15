#pragma once

#include "_progress_hub.h"

#include <string>
using namespace std;

// ************************************************************************************************
class CTask
{
public: // Methods

	CTask()
	{}

	virtual ~CTask()
	{}

	virtual void Run() PURE;
};

// ************************************************************************************************
class CProgress
{

public: // Methods

	CProgress()
	{}

	virtual ~CProgress()
	{}

	virtual void Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent) PURE;

protected: // Methods

	string CreateLogEntry(int/*enumLogEvent*/ enLogEvent, const char* szEvent)
	{
		string strEntry =
			enLogEvent == 0/*info*/ ? "Information: " :
			enLogEvent == 1/*warning*/ ? "Warning: " :
			enLogEvent == 2/*error*/ ? "Error: " : "Unknown: ";
		strEntry += szEvent;

		return strEntry;
	}
};

// ************************************************************************************************
#ifdef _PROGRESS_UI_SUPPORT
class CProgressDialog
	: public CDialogEx
	, public CProgress
	, public _progress_view
{
	DECLARE_DYNAMIC(CProgressDialog)

private: // Fields

	CWinThread* m_pThread;
	CTask* m_pTask;

public: // Methods

	// CProgress
	virtual void Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent) override;

	// _progress_view
	virtual void onProgressInit(int iTotal, const std::string& strStage) override;
	virtual void onReport(int iCurrent, int iTotal, const char* szStage) override;	
	virtual void onProgressEnd() override;

private: // Methods

	static UINT ThreadProc(LPVOID pParam);

public:
	CProgressDialog(CWnd* pParent, CTask* pTask);   // standard constructor
	virtual ~CProgressDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_edtProgress;
public:
	virtual BOOL OnInitDialog();
	CProgressCtrl m_progressCtrl;
};
#endif //  _PROGRESS_UI_SUPPORT
