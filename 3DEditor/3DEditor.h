
// 3DEditor.h : main header file for the 3DEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMy3DEditorApp:
// See 3DEditor.cpp for the implementation of this class
//

class CMy3DEditorApp : public CWinAppEx
{
public:
	CMy3DEditorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
};

extern CMy3DEditorApp theApp;
