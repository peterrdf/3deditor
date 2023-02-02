
#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
	: m_pItemStateProvider(nullptr)
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CViewTree::OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

// ------------------------------------------------------------------------------------------------
void CViewTree::SetItemStateProvider(CItemStateProvider* pItemStateProvider)
{
    m_pItemStateProvider = pItemStateProvider;
}

// ------------------------------------------------------------------------------------------------
void CViewTree::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMTVCUSTOMDRAW* pNMCD = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);

    switch (pNMCD->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
        {
            *pResult = CDRF_NOTIFYITEMDRAW;            
        }
        break;            

        case CDDS_ITEMPREPAINT:
        {
            if (m_pItemStateProvider != nullptr)
            {
                HTREEITEM hItem = (HTREEITEM)pNMCD->nmcd.dwItemSpec;
                if (m_pItemStateProvider->IsSelected(hItem))
                {
                    pNMCD->clrTextBk = RGB(255, 0, 0);
                }
            }

            *pResult = CDRF_DODEFAULT;
        }
        break;

        default:
            {
                *pResult = 0;
            }            
            break;
    } // switch (pNMCD->nmcd.dwDrawStage)
}