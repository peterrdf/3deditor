#pragma once
#include "afxdialogex.h"
#include "../DragFace/DragFace.h"


// CSelectDragPropsDialog dialog

class CSelectDragPropsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectDragPropsDialog)

public:
	CSelectDragPropsDialog(DragFace& dragFace, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSelectDragPropsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECT_DRAG_PROPS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_wndPropsList;
	DragFace& m_dragFace;
};
