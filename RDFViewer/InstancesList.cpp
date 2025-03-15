
#include "stdafx.h"
#include "InstancesList.h"
#include "RDFInstance.h"

#include "_rdf_mvc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------------------------------------------------------------------------
CInstancesList::CInstancesList()
	: m_pController(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CInstancesList::~CInstancesList()
{
}

// ------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CInstancesList, CListCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CInstancesList::OnNMCustomdraw)
END_MESSAGE_MAP()

// ------------------------------------------------------------------------------------------------
BOOL CInstancesList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CListCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

// ------------------------------------------------------------------------------------------------
void CInstancesList::SetController(_rdf_controller* pController)
{
    m_pController = pController;
}

// ------------------------------------------------------------------------------------------------
void CInstancesList::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
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
            if ((m_pController != nullptr) && (pNMCD->nmcd.lItemlParam != 0))
            {
                auto pInstance = (_rdf_instance*)pNMCD->nmcd.lItemlParam;
                if (m_pController->getSelectedInstance() == pInstance)
                {
                    if (CDIS_SELECTED == (pNMCD->nmcd.uItemState & CDIS_SELECTED))
                    {
                        pNMCD->nmcd.uItemState = CDIS_DEFAULT;
                    }

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