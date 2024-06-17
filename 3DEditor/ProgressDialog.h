#pragma once

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
};

// ************************************************************************************************
// CProgressDialog dialog
class CProgressDialog
	: public CDialogEx
	, CProgress
{
	DECLARE_DYNAMIC(CProgressDialog)

private: // Members

	CWinThread* m_pThread;
	CTask* m_pTask;

public: // Methods

	// CProgress
	virtual void Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent) override;

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
};
