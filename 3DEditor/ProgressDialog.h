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
// CProgressDialog dialog
class CProgressDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDialog)

private: // Members

	CWinThread* m_pThread;
	CTask* m_pTask;

private: // Methods

	static UINT ThreadProc(LPVOID pParam);	

public: // Methods

	void Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent);

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
