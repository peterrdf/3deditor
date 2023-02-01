
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "3DEditor.h"
#include "RDFModel.h"
#include "Generic.h"
#include "NewInstanceDialog.h"
#include "FileViewConsts.h"
#include "ProgressIndicator.h"

#include <algorithm>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CFileViewMenuButton : public CMFCToolBarMenuButton
{

	DECLARE_SERIAL(CFileViewMenuButton)

public:
	CFileViewMenuButton(HMENU hMenu = nullptr) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		// Patch: wrong background when the app starts
		pImages->SetTransparentColor(::GetSysColor(COLOR_BTNFACE));

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CFileViewMenuButton, CMFCToolBarMenuButton, 1)

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnModelChanged()
{
	if (GetController()->IsTestMode())
	{
		return;
	}

	m_hSelectedItem = nullptr;

	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnInstanceSelected(CRDFView * pSender)
{
	if (pSender == this)
	{
		return;
	}

	if (m_hSelectedItem != nullptr)
	{
		m_wndFileView.SetItemState(m_hSelectedItem, 0, TVIS_BOLD);
		m_hSelectedItem = nullptr;
	}

	ASSERT(GetController() != nullptr);

	CRDFInstance * pSelectedInstance = GetController()->GetSelectedInstance();
	if (pSelectedInstance == nullptr)
	{
		/*
		* Select the Model by default
		*/
		HTREEITEM hModel = m_wndFileView.GetChildItem(nullptr);
		ASSERT(hModel != nullptr);

		m_wndFileView.SelectItem(hModel);

		return;
	}

	/*
	* Disable the drawing
	*/
	m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.find(pSelectedInstance->getInstance());
	if (itInstance2Item == m_mapInstance2Item.end())
	{
		/*
		* Load all ancestors
		*/
		vector<int64_t> vecAncestors;

		int64_t iInstance = GetInstanceInverseReferencesByIterator(pSelectedInstance->getInstance(), 0);
		while (iInstance != 0)
		{
			vecAncestors.push_back(iInstance);

			iInstance = GetInstanceInverseReferencesByIterator(iInstance, 0);			
		}		

		/*
		* Load the ancestors
		*/
		for (int64_t iAncestor = vecAncestors.size() - 1; iAncestor >= 0; iAncestor--)
		{
			itInstance2Item = m_mapInstance2Item.find(vecAncestors[iAncestor]);
			if (itInstance2Item != m_mapInstance2Item.end())
			{
				// The item is visible	
				ASSERT(!itInstance2Item->second->items().empty());
				m_wndFileView.Expand(itInstance2Item->second->items()[0], TVE_EXPAND);
			} 
			else
			{
				// The item is not visible - it is in "..." group
				break;
			}
		} // for (size_t iAncestor = ...

		itInstance2Item = m_mapInstance2Item.find(pSelectedInstance->getInstance());
	} // if (itInstance2Item == m_mapInstance2Item.end())

	if (itInstance2Item != m_mapInstance2Item.end())
	{
		// The item is visible
		ASSERT(!itInstance2Item->second->items().empty());

		m_hSelectedItem = itInstance2Item->second->items()[0];

		m_wndFileView.SetItemState(m_hSelectedItem, TVIS_BOLD, TVIS_BOLD);
		m_wndFileView.EnsureVisible(m_hSelectedItem);
		m_wndFileView.SelectItem(m_hSelectedItem);
	}
	else
	{
		// The item is not visible - it is in "..." group
		if (m_hSelectedItem != nullptr)
		{
			m_wndFileView.SetItemState(m_hSelectedItem, 0, TVIS_BOLD);
			m_hSelectedItem = nullptr;
		}

		MessageBox(L"The selected item is not visible in Instance View.\nPlease, increase 'Visible values count limit' property.", L"Information", MB_ICONINFORMATION | MB_OK);
	}

	/*
	* Enable the drawing
	*/
	m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty)
{
	ASSERT(pInstance != nullptr);
	ASSERT(pProperty != nullptr);

	ASSERT(GetController() != nullptr);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	/*
	* Update non-referenced item
	*/
	if ((m_nCurrSort == ID_SORTING_INSTANCES_NOT_REFERENCED) && (pProperty->getType() == TYPE_OBJECTTYPE))
	{
		HTREEITEM hModel = m_wndFileView.GetChildItem(nullptr);
		ASSERT(hModel != nullptr);

		UpdateRootItemsUnreferencedItemsView(pModel->GetModel(), hModel);
	} // if ((m_nCurrSort == ID_SORTING_INSTANCES_NOT_REFERENCED) && ...

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.find(pInstance->getInstance());
	if (itInstance2Properties == m_mapInstance2Properties.end())
	{
		// The properties are not loaded
		return;
	}

	map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.find(pProperty->getInstance());
	ASSERT(itPropertyItem != itInstance2Properties->second.end());

	CRDFPropertyItem * pPropertyItem = itPropertyItem->second;
	ASSERT(pPropertyItem != nullptr);

	ASSERT(pPropertyItem->items().size() > 0);	

	wchar_t szBuffer[100];

	switch (pPropertyItem->getProperty()->getType())
	{
	case TYPE_OBJECTTYPE:
	{
		int64_t * piInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(pInstance->getInstance(), pProperty->getInstance(), &piInstances, &iCard);

		ASSERT(iCard >= 0);

		for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
		{
			vector<HTREEITEM> vecValues;
			HTREEITEM hCardinality = nullptr;

			HTREEITEM hChild = m_wndFileView.GetChildItem(pPropertyItem->items()[iItem]);
			while (hChild != nullptr)
			{
				CString strText = m_wndFileView.GetItemText(hChild);
				if (strText.Find(_T("owl:cardinality : ")) == 0)
				{
					hCardinality = hChild;
				}
				else
				{
					if (strText.Find(_T("rdfs:range")) == -1)
					{
						vecValues.push_back(hChild);
					}
				}

				hChild = m_wndFileView.GetNextSiblingItem(hChild);
			}

			/*
			* Disable the drawing
			*/
			m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

			/*
			* Update the cardinality
			*/
			ASSERT(hCardinality != nullptr);

			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.SetItemText(hCardinality, strCardinality.c_str());

			/*
			* Delete all values
			*/
			for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
			{
				RemoveItemData(vecValues[iValue]);

				vector<HTREEITEM> vecDescendants;
				GetDescendants(vecValues[iValue], vecDescendants);

				for (size_t iDescendant = 0; iDescendant < vecDescendants.size(); iDescendant++)
				{
					RemoveItemData(vecDescendants[iDescendant]);
				}

				m_wndFileView.DeleteItem(vecValues[iValue]);
			} // for (int64_t iValue = ...

			/*
			* Add the values
			*/
			HTREEITEM hProperty = pPropertyItem->items()[iItem];

			int64_t iInstancesCount = iCard;
			for (int64_t iInstance = 0; iInstance < iInstancesCount; iInstance++)
			{
				if (piInstances[iInstance] != 0)
				{
					map<int64_t, CRDFInstance *>::const_iterator itInstance = mapInstances.find(piInstances[iInstance]);
					ASSERT(itInstance != mapInstances.end());

					AddInstance(hProperty, itInstance->second);					
				} // if (piInstances[iInstance] != 0)
				else
				{
					m_wndFileView.InsertItem(EMPTY_INSTANCE, IMAGE_INSTANCE, IMAGE_INSTANCE, hProperty);
				}

				if ((iInstance + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iInstance = ...

			if (iInstancesCount > GetController()->GetVisibleValuesCountLimit())
			{
				m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
			}
		} // for (size_t iItem = ...

		/*
		* Enable the drawing
		*/
		m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
	} // case TYPE_OBJECTTYPE:
	break;

	case TYPE_BOOL_DATATYPE:
	{
		int64_t iCard = 0;
		bool* pbValue = nullptr;
		GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void**)&pbValue, &iCard);

		ASSERT(iCard >= 0);

		for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
		{
			vector<HTREEITEM> vecValues;
			HTREEITEM hCardinality = nullptr;

			HTREEITEM hChild = m_wndFileView.GetChildItem(pPropertyItem->items()[iItem]);
			while (hChild != nullptr)
			{
				CString strText = m_wndFileView.GetItemText(hChild);
				if ((strText.Find(_T("value = ")) == 0) || strText.Find(_T("...")) == 0)
				{
					vecValues.push_back(hChild);
				}
				else
				{
					if (strText.Find(_T("owl:cardinality : ")) == 0)
					{
						hCardinality = hChild;
					}
				}

				hChild = m_wndFileView.GetNextSiblingItem(hChild);
			}

			/*
			* Disable the drawing
			*/
			m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

			/*
			* Update the cardinality
			*/
			ASSERT(hCardinality != nullptr);

			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.SetItemText(hCardinality, strCardinality.c_str());

			/*
			* Delete all values
			*/
			for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
			{
				m_wndFileView.DeleteItem(vecValues[iValue]);
			}

			/*
			* Add the values
			*/

			HTREEITEM hProperty = pPropertyItem->items()[iItem];

			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				swprintf(szBuffer, 100, L"value = %s", pbValue[iValue] ? L"True" : L"False");

				m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
			}
		} // for (size_t iItem = ...

		/*
		* Enable the drawing
		*/
		m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
	} // case TYPE_BOOL_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		int64_t iCard = 0;
		char ** szValue = nullptr;
		GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&szValue, &iCard);

		ASSERT(iCard >= 0);

		for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
		{
			vector<HTREEITEM> vecValues;
			HTREEITEM hCardinality = nullptr;

			HTREEITEM hChild = m_wndFileView.GetChildItem(pPropertyItem->items()[iItem]);
			while (hChild != nullptr)
			{
				CString strText = m_wndFileView.GetItemText(hChild);
				if ((strText.Find(_T("value = ")) == 0) || strText.Find(_T("...")) == 0)
				{
					vecValues.push_back(hChild);
				}
				else
				{
					if (strText.Find(_T("owl:cardinality : ")) == 0)
					{
						hCardinality = hChild;
					}
				}

				hChild = m_wndFileView.GetNextSiblingItem(hChild);
			}

			/*
			* Disable the drawing
			*/
			m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

			/*
			* Update the cardinality
			*/
			ASSERT(hCardinality != nullptr);

			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.SetItemText(hCardinality, strCardinality.c_str());

			/*
			* Delete all values
			*/
			for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
			{
				m_wndFileView.DeleteItem(vecValues[iValue]);
			}

			/*
			* Add the values
			*/

			HTREEITEM hProperty = pPropertyItem->items()[iItem];

			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				wstring strValue = CA2W(szValue[iValue]);
				swprintf(szBuffer, 100, L"value = '%s'", strValue.c_str());

				m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
			}
		} // for (size_t iItem = ...

		/*
		* Enable the drawing
		*/
		m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
	}
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&pdValue, &iCard);

		ASSERT(iCard >= 0);

		for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
		{
			vector<HTREEITEM> vecValues;
			HTREEITEM hCardinality = nullptr;

			HTREEITEM hChild = m_wndFileView.GetChildItem(pPropertyItem->items()[iItem]);
			while (hChild != nullptr)
			{
				CString strText = m_wndFileView.GetItemText(hChild);
				if ((strText.Find(_T("value = ")) == 0) || strText.Find(_T("...")) == 0)
				{
					vecValues.push_back(hChild);
				}
				else
				{
					if (strText.Find(_T("owl:cardinality : ")) == 0)
					{
						hCardinality = hChild;
					}
				}

				hChild = m_wndFileView.GetNextSiblingItem(hChild);
			}

			/*
			* Disable the drawing
			*/
			m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);			

			/*
			* Update the cardinality
			*/
			ASSERT(hCardinality != nullptr);

			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.SetItemText(hCardinality, strCardinality.c_str());

			/*
			* Delete all values
			*/
			for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
			{
				m_wndFileView.DeleteItem(vecValues[iValue]);
			}

			/*
			* Add the values
			*/

			HTREEITEM hProperty = pPropertyItem->items()[iItem];

			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				swprintf(szBuffer, 100, L"value = %.6f", pdValue[iValue]);

				m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
			}			
		} // for (size_t iItem = ...

		/*
		* Enable the drawing
		*/
		m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pInstance->getInstance(), pPropertyItem->getProperty()->getInstance(), (void **)&piValue, &iCard);

		ASSERT(iCard >= 0);

		for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
		{
			vector<HTREEITEM> vecValues;
			HTREEITEM hCardinality = nullptr;

			HTREEITEM hChild = m_wndFileView.GetChildItem(pPropertyItem->items()[iItem]);
			while (hChild != nullptr)
			{
				CString strText = m_wndFileView.GetItemText(hChild);
				if ((strText.Find(_T("value = ")) == 0) || strText.Find(_T("...")) == 0)
				{
					vecValues.push_back(hChild);
				}
				else
				{
					if (strText.Find(_T("owl:cardinality : ")) == 0)
					{
						hCardinality = hChild;
					}
				}

				hChild = m_wndFileView.GetNextSiblingItem(hChild);
			}

			/*
			* Disable the drawing
			*/
			m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

			/*
			* Update the cardinality
			*/
			ASSERT(hCardinality != nullptr);

			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.SetItemText(hCardinality, strCardinality.c_str());

			/*
			* Delete all values
			*/
			for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
			{
				m_wndFileView.DeleteItem(vecValues[iValue]);
			}

			/*
			* Add the values
			*/

			HTREEITEM hProperty = pPropertyItem->items()[iItem];

			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				swprintf(szBuffer, 100, L"value = %lld", piValue[iValue]);

				m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
			}
		} // for (size_t iItem = ...

		/*
		* Enable the drawing
		*/
		m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unknown property
	}
	break;
	} // switch (pPropertyItem->getProperty()->getType())
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnNewInstanceCreated(CRDFView * /*pSender*/, CRDFInstance * /*pInstance*/)
{
	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnInstanceDeleted(CRDFView * /*pSender*/, int64_t /*iInstance*/)
{
	m_hSelectedItem = nullptr;

	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnMeasurementsAdded(CRDFView * /*pSender*/, CRDFInstance * /*pInstance*/)
{
	m_hSelectedItem = nullptr;

	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CFileView::OnVisibleValuesCountLimitChanged()
{
	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ bool CFileView::IsSelected(HTREEITEM hItem)
{
	CRDFItem* pItem = (CRDFItem*)m_wndFileView.GetItemData(hItem);
	if ((pItem != nullptr) && (pItem->getType() == enumItemType::Instance) &&
		(GetController()->GetSelectedInstance() == pItem->getInstance()))
	{
		return pItem->getInstance()->getEnable();
	}

	return false;
}

// ------------------------------------------------------------------------------------------------
void CFileView::GetItemPath(HTREEITEM hItem, vector<pair<CRDFInstance *, CRDFProperty *> > & vecPath)
{
	if (hItem == nullptr)
	{
		return;
	}

	auto pItem = (CRDFItem*)m_wndFileView.GetItemData(hItem);
	if (pItem != nullptr)
	{
		switch (pItem->getType())
		{
			case enumItemType::Instance:
			{
				CRDFInstanceItem * pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);
				ASSERT(pInstanceItem != nullptr);

				vecPath.insert(vecPath.begin(), pair<CRDFInstance *, CRDFProperty *>(pInstanceItem->getInstance(), nullptr));
			}
			break;

			case enumItemType::Property:
			{
				CRDFPropertyItem * pPropertyItem = dynamic_cast<CRDFPropertyItem *>(pItem);
				ASSERT(pPropertyItem != nullptr);

				vecPath.insert(vecPath.begin(), pair<CRDFInstance *, CRDFProperty *>(pPropertyItem->getInstance(), pPropertyItem->getProperty()));
			}
			break;

			default:
			{
				ASSERT(false); // unexpected
			}
			break;
		}
	} // if (pItem != nullptr)

	GetItemPath(m_wndFileView.GetParentItem(hItem), vecPath);
}

// ------------------------------------------------------------------------------------------------
void CFileView::GetDescendants(HTREEITEM hItem, vector<HTREEITEM> & vecDescendants)
{
	ASSERT(hItem != nullptr);

	HTREEITEM hChild = m_wndFileView.GetChildItem(hItem);
	while (hChild != nullptr)
	{
		if (m_wndFileView.GetItemData(hChild) != NULL)
		{
			vecDescendants.push_back(hChild);

			GetDescendants(hChild, vecDescendants);
		}		

		hChild = m_wndFileView.GetNextSiblingItem(hChild);
	}
}

// ------------------------------------------------------------------------------------------------
void CFileView::RemoveInstanceItemData(CRDFInstance * pInstance, HTREEITEM hInstance)
{
	ASSERT(pInstance != nullptr);
	ASSERT(hInstance != nullptr);

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.find(pInstance->getInstance());
	ASSERT(itInstance2Item != m_mapInstance2Item.end());

	vector<HTREEITEM>::const_iterator itInstance = find(itInstance2Item->second->items().begin(), itInstance2Item->second->items().end(), hInstance);
	ASSERT(itInstance != itInstance2Item->second->items().end());

	itInstance2Item->second->items().erase(itInstance);
}

// ------------------------------------------------------------------------------------------------
void CFileView::RemovePropertyItemData(CRDFInstance * pInstance, CRDFProperty * pProperty, HTREEITEM hProperty)
{
	ASSERT(pInstance != nullptr);
	ASSERT(pProperty != nullptr);
	ASSERT(hProperty != nullptr);

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.find(pInstance->getInstance());
	ASSERT(itInstance2Properties != m_mapInstance2Properties.end());

	map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.find(pProperty->getInstance());
	ASSERT(itPropertyItem != itInstance2Properties->second.end());

	vector<HTREEITEM>::const_iterator itInstance = find(itPropertyItem->second->items().begin(), itPropertyItem->second->items().end(), hProperty);
	ASSERT(itInstance != itPropertyItem->second->items().end());

	itPropertyItem->second->items().erase(itInstance);
}

// ------------------------------------------------------------------------------------------------
void CFileView::RemoveItemData(HTREEITEM hItem)
{
	ASSERT(hItem != nullptr);

	CRDFItem * pItem = (CRDFItem *)m_wndFileView.GetItemData(hItem);
	if (pItem != nullptr)
	{
		switch (pItem->getType())
		{
		case enumItemType::Instance:
		{
			CRDFInstanceItem * pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);
			ASSERT(pInstanceItem != nullptr);

			RemoveInstanceItemData(pInstanceItem->getInstance(), hItem);
		}
		break;

		case enumItemType::Property:
		{
			CRDFPropertyItem * pPropertyItem = dynamic_cast<CRDFPropertyItem *>(pItem);
			ASSERT(pPropertyItem != nullptr);

			RemovePropertyItemData(pPropertyItem->getInstance(), pPropertyItem->getProperty(), hItem);
		}
		break;

		default:
		{
			ASSERT(false); // unexpected
		}
		break;
		} // switch (pItem->getType())
	} // if (pItem != nullptr)
}

// ------------------------------------------------------------------------------------------------
void CFileView::UpdateView()
{
//	LOG_DEBUG("CFileView::UpdateView() BEGIN");

	m_pSearchDialog->Reset();

	/*
	* Disable the drawing
	*/
	m_bInitInProgress = true;
	m_wndFileView.SendMessage(WM_SETREDRAW, 0, 0);

	switch (m_nCurrSort)
	{
	case ID_SORTING_INSTANCES_SORTALPHABETIC:
	{
		InstancesAlphabeticalView();
	}
	break;

	case ID_SORTING_INSTANCES_SORTBYTYPE:
	{
		InstancesGroupByClassView();
	}
	break;

	case ID_SORTING_INSTANCES_NOT_REFERENCED:
	{
		InstancesUnreferencedItemsView();
	}
	break;

	default:
	{
		ASSERT(false); // unknown view
	}
	break;
	} // switch (m_nCurrSort)

	/*
	* Restore the selected instance
	*/
	ASSERT(GetController() != nullptr);
	
	if (GetController()->GetSelectedInstance() != nullptr)
	{
		OnInstanceSelected(nullptr);
	}

	/*
	* Restore the selected property
	*/
	pair<CRDFInstance *, CRDFProperty *> prSelectedInstanceProperty = GetController()->GetSelectedInstanceProperty();
	if ((prSelectedInstanceProperty.first != nullptr) && (prSelectedInstanceProperty.second != nullptr))
	{
		ASSERT(m_hSelectedItem != nullptr);
		m_wndFileView.SetItemState(m_hSelectedItem, 0, TVIS_BOLD);
		m_wndFileView.Expand(m_hSelectedItem, TVE_EXPAND);

		map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.find(prSelectedInstanceProperty.first->getInstance());
		ASSERT(itInstance2Properties != m_mapInstance2Properties.end());

		map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.find(prSelectedInstanceProperty.second->getInstance());
		ASSERT(itPropertyItem != itInstance2Properties->second.end());
		ASSERT(!itPropertyItem->second->items().empty());

		m_hSelectedItem = itPropertyItem->second->items()[0];

		m_wndFileView.SetItemState(m_hSelectedItem, TVIS_BOLD, TVIS_BOLD);
		m_wndFileView.EnsureVisible(m_hSelectedItem);
		m_wndFileView.SelectItem(m_hSelectedItem);
	} // if ((prSelectedInstanceProperty.first != nullptr) && ...

	/*
	* Enable the drawing
	*/
	m_bInitInProgress = false;
	m_wndFileView.SendMessage(WM_SETREDRAW, 1, 0);

//	LOG_DEBUG("CFileView::UpdateView() END");
}

// ------------------------------------------------------------------------------------------------
void CFileView::InstancesAlphabeticalView()
{
	m_wndFileView.DeleteAllItems();

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.begin();
	for (; itInstance2Item != m_mapInstance2Item.end(); itInstance2Item++)
	{
		delete itInstance2Item->second;
	}

	m_mapInstance2Item.clear();

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.begin();
	for (; itInstance2Properties != m_mapInstance2Properties.end(); itInstance2Properties++)
	{
		map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.begin();
		for (; itPropertyItem != itInstance2Properties->second.end(); itPropertyItem++)
		{
			delete itPropertyItem->second;
		}
	}

	m_mapInstance2Properties.clear();

	ASSERT(GetController() != nullptr);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	vector<CRDFInstance *> vecModel;

	auto itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pInstance = itRFDInstances->second;

		if (pInstance->GetModel() == pModel->GetModel())
		{
			vecModel.push_back(pInstance);
		}
		else
		{
			ASSERT(FALSE);
		}
	} // for (; itRFDInstances != ...

	/*
	* Model
	*/
	sort(vecModel.begin(), vecModel.end(), SORT_RDFINSTANCES());

	HTREEITEM hModel = m_wndFileView.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndFileView.SetItemState(hModel, TVIS_BOLD, TVIS_BOLD);

	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		AddInstance(hModel, vecModel[iInstance]);
	}

	m_wndFileView.Expand(hModel, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
void CFileView::InstancesGroupByClassView()
{
	m_wndFileView.DeleteAllItems();

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.begin();
	for (; itInstance2Item != m_mapInstance2Item.end(); itInstance2Item++)
	{
		delete itInstance2Item->second;
	}

	m_mapInstance2Item.clear();

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.begin();
	for (; itInstance2Properties != m_mapInstance2Properties.end(); itInstance2Properties++)
	{
		map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.begin();
		for (; itPropertyItem != itInstance2Properties->second.end(); itPropertyItem++)
		{
			delete itPropertyItem->second;
		}
	}

	m_mapInstance2Properties.clear();

	ASSERT(GetController() != nullptr);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	map<wstring, vector<CRDFInstance *> > mapModel;

	auto itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pInstance = itRFDInstances->second;

		char* szName = nullptr;
		GetNameOfClass(pInstance->getClassInstance(), &szName);

		wstring strName = CA2W(szName);

		if (pInstance->GetModel() == pModel->GetModel())
		{
			auto itModel = mapModel.find(strName);
			if (itModel == mapModel.end())
			{
				vector<CRDFInstance*> vecInstances;
				vecInstances.push_back(pInstance);

				mapModel[strName] = vecInstances;
			}
			else
			{
				itModel->second.push_back(pInstance);
			}
		}
		else
		{
			ASSERT(false);
		}
	} // for (; itRFDInstances != ...

	/*
	* Model
	*/
	HTREEITEM hModel = m_wndFileView.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndFileView.SetItemState(hModel, TVIS_BOLD, TVIS_BOLD);

	map<wstring, vector<CRDFInstance *> >::iterator itModel = mapModel.begin();
	for (; itModel != mapModel.end(); itModel++)
	{
		HTREEITEM hClass = m_wndFileView.InsertItem(itModel->first.c_str(), IMAGE_INSTANCE, IMAGE_INSTANCE, hModel);

		vector<CRDFInstance *> vecInstances = itModel->second;
		sort(vecInstances.begin(), vecInstances.end(), SORT_RDFINSTANCES());

		for (size_t iInstance = 0; iInstance < vecInstances.size(); iInstance++)
		{
			AddInstance(hClass, vecInstances[iInstance]);
		}
	} // for (; itModel != ...

	m_wndFileView.Expand(hModel, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
void CFileView::InstancesUnreferencedItemsView()
{
	ProgressStatus prgs(L"Build project tree");

	m_wndFileView.DeleteAllItems();

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.begin();
	for (; itInstance2Item != m_mapInstance2Item.end(); itInstance2Item++)
	{
		delete itInstance2Item->second;
	}

	m_mapInstance2Item.clear();

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.begin();
	for (; itInstance2Properties != m_mapInstance2Properties.end(); itInstance2Properties++)
	{
		map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.begin();
		for (; itPropertyItem != itInstance2Properties->second.end(); itPropertyItem++)
		{
			delete itPropertyItem->second;
		}
	}

	m_mapInstance2Properties.clear();

	ASSERT(GetController() != nullptr);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	vector<CRDFInstance *> vecModel;

	prgs.Start(mapInstances.size());
	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		prgs.Step();

		CRDFInstance * pInstance = itRFDInstances->second;

		if (pInstance->isReferenced())
		{
			continue;
		}

		if (pInstance->GetModel() == pModel->GetModel())
		{
			vecModel.push_back(pInstance);
		}
		else
		{
			ASSERT(FALSE);
		}
	} // for (; itRFDInstances != ...
	prgs.Finish();

	/*
	* Model
	*/
	sort(vecModel.begin(), vecModel.end(), SORT_RDFINSTANCES());

	HTREEITEM hModel = m_wndFileView.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndFileView.SetItemState(hModel, TVIS_BOLD, TVIS_BOLD);

	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		AddInstance(hModel, vecModel[iInstance]);
	}

	m_wndFileView.Expand(hModel, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
void CFileView::AddInstance(HTREEITEM hParent, CRDFInstance * pInstance)
{
	/*
	* The instances will be loaded on demand
	*/
	TV_INSERTSTRUCT tvInsertStruct;
	tvInsertStruct.hParent = hParent;
	tvInsertStruct.hInsertAfter = TVI_LAST;
	tvInsertStruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvInsertStruct.item.pszText = (LPWSTR)pInstance->getUniqueName();
	tvInsertStruct.item.iImage = tvInsertStruct.item.iSelectedImage = IMAGE_INSTANCE;
	tvInsertStruct.item.lParam = NULL;
	tvInsertStruct.item.cChildren = IMAGE_INSTANCE;

	HTREEITEM hInstance = m_wndFileView.InsertItem(&tvInsertStruct);

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.find(pInstance->getInstance());
	if (itInstance2Item == m_mapInstance2Item.end())
	{
		CRDFInstanceItem * pInstanceItem = new CRDFInstanceItem(pInstance);
		pInstanceItem->items().push_back(hInstance);

		m_mapInstance2Item[pInstance->getInstance()] = pInstanceItem;

		m_wndFileView.SetItemData(hInstance, (DWORD_PTR)pInstanceItem);
	}
	else
	{
		itInstance2Item->second->items().push_back(hInstance);

		m_wndFileView.SetItemData(hInstance, (DWORD_PTR)itInstance2Item->second);
	}
}

// ------------------------------------------------------------------------------------------------
void CFileView::AddProperties(HTREEITEM hParent, CRDFInstance * pInstance)
{
	wchar_t szBuffer[100];

	if (pInstance == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	if (GetController() == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = GetController()->GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapProperties = pModel->GetProperties();
	auto& mapInstances = pModel->GetInstances();

	int64_t iPropertyInstance = GetInstancePropertyByIterator(pInstance->getInstance(), 0);
	while (iPropertyInstance != 0)
	{
		map<int64_t, CRDFProperty *>::const_iterator itProperty = mapProperties.find(iPropertyInstance);
		ASSERT(itProperty != mapProperties.end());

		auto pProperty = itProperty->second;

		wstring strNameAndType = pProperty->getName();
		strNameAndType += L" : ";
		strNameAndType += pProperty->getTypeAsString();		

		HTREEITEM hProperty = m_wndFileView.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hParent);

		map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.find(pInstance->getInstance());
		if (itInstance2Properties == m_mapInstance2Properties.end())
		{
			auto pPropertyItem = new CRDFPropertyItem(pInstance, pProperty);
			pPropertyItem->items().push_back(hProperty);

			map<int64_t, CRDFPropertyItem *> mapItemProperties;
			mapItemProperties[pProperty->getInstance()] = pPropertyItem;

			m_mapInstance2Properties[pInstance->getInstance()] = mapItemProperties;

			m_wndFileView.SetItemData(hProperty, (DWORD_PTR)pPropertyItem);
		}
		else
		{
			map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.find(pProperty->getInstance());
			if (itPropertyItem == itInstance2Properties->second.end())
			{
				CRDFPropertyItem * pPropertyItem = new CRDFPropertyItem(pInstance, pProperty);
				pPropertyItem->items().push_back(hProperty);

				itInstance2Properties->second[pProperty->getInstance()] = pPropertyItem;

				m_wndFileView.SetItemData(hProperty, (DWORD_PTR)pPropertyItem);
			}
			else
			{
				itPropertyItem->second->items().push_back(hProperty);

				m_wndFileView.SetItemData(hProperty, (DWORD_PTR)itPropertyItem->second);
			}
		}

		/*
		* rdfs:range
		*/
		if (pProperty->getType() == TYPE_OBJECTTYPE)
		{
			HTREEITEM hRange = m_wndFileView.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

			CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
			ASSERT(pObjectRDFProperty != nullptr);

			auto& vecRestrictions = pObjectRDFProperty->getRestrictions();
			for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
			{
				char * szClassName = nullptr;
				GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

				m_wndFileView.InsertItem(CA2W(szClassName), IMAGE_VALUE, IMAGE_VALUE, hRange);
			}
		} // if (pProperty->getType() == TYPE_OBJECTTYPE)
		else
		{
			wstring strRange = L"rdfs:range : ";
			strRange += pProperty->getRange();

			m_wndFileView.InsertItem(strRange.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}

		/*
		* owl:cardinality and value
		*/
		switch (pProperty->getType())
		{
		case TYPE_OBJECTTYPE:
		{
			int64_t * piInstances = nullptr;
			int64_t iCard = 0;
			GetObjectProperty(pInstance->getInstance(), pProperty->getInstance(), &piInstances, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iInstancesCount = iCard;
				for (int64_t iInstance = 0; iInstance < iInstancesCount; iInstance++)
				{
					if (piInstances[iInstance] != 0)
					{
						map<int64_t, CRDFInstance *>::const_iterator itInstance = mapInstances.find(piInstances[iInstance]);
						ASSERT(itInstance != mapInstances.end());

						AddInstance(hProperty, itInstance->second);
					} // if (piInstances[iInstance] != 0)
					else
					{
						m_wndFileView.InsertItem(EMPTY_INSTANCE, IMAGE_INSTANCE, IMAGE_INSTANCE, hProperty);
					}					

					if ((iInstance + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iInstance = ...

				if (iInstancesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			int64_t iCard = 0;
			bool * pbValue = nullptr;
			GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&pbValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %s", pbValue[iValue] ? L"TRUE" : L"FALSE");

					m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)			
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			int64_t iCard = 0;
			char ** szValue = nullptr;
			GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&szValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					wstring strValue = CA2W(szValue[iValue]);
					swprintf(szBuffer, 100, L"value = '%s'", strValue.c_str());

					m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)			
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			int64_t iCard = 0;
			double * pdValue = nullptr;
			GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&pdValue, &iCard);


			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %.6f", pdValue[iValue]);

					m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			int64_t iCard = 0;
			int64_t * piValue = nullptr;
			GetDatatypeProperty(pInstance->getInstance(), pProperty->getInstance(), (void **)&piValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->getInstance());

			m_wndFileView.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %lld", piValue[iValue]);

					m_wndFileView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_wndFileView.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)			
		}
		break;

		default:
		{
			ASSERT(false); // unknown property
		}
		break;
		} // switch (pProperty->getType())

		iPropertyInstance = GetInstancePropertyByIterator(pInstance->getInstance(), iPropertyInstance);
	} // while (iPropertyInstance != 0)
}

// ------------------------------------------------------------------------------------------------
void CFileView::UpdateRootItemsUnreferencedItemsView(int64_t iModel, HTREEITEM hModel)
{
	ASSERT(iModel != 0);
	ASSERT(hModel != nullptr);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	vector<CRDFInstance *> vecInstances;
	vector<HTREEITEM> vecObsoleteItems;

	HTREEITEM hItem = m_wndFileView.GetChildItem(hModel);
	while (hItem != nullptr)
	{
		CRDFItem * pItem = (CRDFItem *)m_wndFileView.GetItemData(hItem);
		ASSERT(pItem != nullptr);
		ASSERT(pItem->getType() == enumItemType::Instance);

		CRDFInstanceItem * pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);
		ASSERT(pInstanceItem != nullptr);

		if (pInstanceItem->getInstance()->isReferenced())
		{
			vecObsoleteItems.push_back(hItem);
		}
		else
		{
			vecInstances.push_back(pInstanceItem->getInstance());
		}

		hItem = m_wndFileView.GetNextSiblingItem(hItem);
	} // while (hChild != nullptr)

	/*
	* Delete the items with references
	*/
	for (int64_t iItem = 0; iItem < (int64_t)vecObsoleteItems.size(); iItem++)
	{
		RemoveItemData(vecObsoleteItems[iItem]);

		vector<HTREEITEM> vecDescendants;
		GetDescendants(vecObsoleteItems[iItem], vecDescendants);

		for (size_t iDescendant = 0; iDescendant < vecDescendants.size(); iDescendant++)
		{
			RemoveItemData(vecDescendants[iDescendant]);
		}

		m_wndFileView.DeleteItem(vecObsoleteItems[iItem]);
	} // for (int64_t iItem = ...

	/*
	* Add the missing items without references
	*/
	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pInstance = itRFDInstances->second;

		if (pInstance->isReferenced())
		{
			continue;
		}

		if (pInstance->GetModel() != iModel)
		{
			continue;
		}

		if (find(vecInstances.begin(), vecInstances.end(), pInstance) != vecInstances.end())
		{
			continue;
		}

		AddInstance(hModel, pInstance);
	} // for (; itRFDInstances != ...
}

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
	: m_mapInstance2Item()
	, m_mapInstance2Properties()
	, m_hSelectedItem(nullptr)
	, m_bInitInProgress(false)
	, m_pSearchDialog(nullptr)
{
	m_nCurrSort = ID_SORTING_INSTANCES_NOT_REFERENCED;
}

CFileView::~CFileView()
{	
	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.begin();
	for (; itInstance2Item != m_mapInstance2Item.end(); itInstance2Item++)
	{
		delete itInstance2Item->second;
	}

	m_mapInstance2Item.clear();

	map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.begin();
	for (; itInstance2Properties != m_mapInstance2Properties.end(); itInstance2Properties++)
	{
		map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.begin();
		for (; itPropertyItem != itInstance2Properties->second.end(); itPropertyItem++)
		{
			delete itPropertyItem->second;
		}
	}

	m_mapInstance2Properties.clear();	
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND_RANGE(ID_SORTING_INSTANCES_SORTALPHABETIC, ID_SORTING_INSTANCES_NOT_REFERENCED, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_INSTANCES_SORTALPHABETIC, ID_SORTING_INSTANCES_NOT_REFERENCED, OnUpdateSort)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INSTANCE_VIEW, OnSelectedItemChanged)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_INSTANCE_VIEW, OnItemExpanding)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_COMMAND(ID_NEW_INSTANCE, OnNewInstance)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(GetController() != nullptr);
	GetController()->RegisterView(this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, IDC_TREE_INSTANCE_VIEW))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// State provider
	m_wndFileView.SetItemStateProvider(this);

	// Load view images:
	m_FileViewImages.Create(IDB_CLASS_VIEW, 16, 0, RGB(255, 0, 0));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT_INSTANCES);
	m_wndToolBar.LoadToolBar(IDR_SORT_INSTANCES, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT_INSTANCES);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CFileViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CFileViewMenuButton* pButton = DYNAMIC_DOWNCAST(CFileViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	AdjustLayout();

	//  Search
	m_pSearchDialog = new CSearchInstancesDialog(&m_wndFileView);
	m_pSearchDialog->Create(IDD_DIALOG_SEARCH_INSTANCES, this);

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point == CPoint(-1, -1))
	{
		return;
	}

	// Select clicked item:
	CPoint ptTree = point;
	pWndTree->ScreenToClient(&ptTree);

	UINT flags = 0;
	HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
	if (hTreeItem == nullptr)
	{
		return;
	}

	pWndTree->SelectItem(hTreeItem);

	pWndTree->SetFocus();

	auto pItem = (CRDFItem *)m_wndFileView.GetItemData(hTreeItem);
	if (pItem == nullptr)
	{
		return;
	}

	if (pItem->getType() != enumItemType::Instance)
	{
		return;
	}

	if (GetController() == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = GetController()->GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	auto pInstanceItem = dynamic_cast<CRDFInstanceItem*>(pItem);
	auto pInstance = pInstanceItem->getInstance();

	/*
	* Instances with a geometry
	*/
	if (pInstance->hasGeometry())
	{
		CMenu menu;
		VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES));

		auto pPopup = menu.GetSubMenu(0);

		// Enable
		if (pInstance->getEnable())
		{
			pPopup->CheckMenuItem(ID_INSTANCES_ENABLE, MF_BYCOMMAND | MF_CHECKED);
		}

		// Zoom to
		if (!pInstance->getEnable())
		{
			pPopup->EnableMenuItem(ID_INSTANCES_ZOOM_TO, MF_BYCOMMAND | MF_DISABLED);
		}

		UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
		if (uiCommand == 0)
		{
			return;
		}		

		switch (uiCommand)
		{
			case ID_INSTANCES_ZOOM_TO:
			{
				GetController()->ZoomToInstance(pInstance->getInstance());
			}
			break;

			case ID_VIEW_ZOOM_OUT:
			{
				GetController()->ZoomOut();
			}
			break;

			case ID_INSTANCES_BASE_INFORMATION:
			{
				GetController()->ShowBaseInformation(pInstance);
			}
			break;

			case ID_INSTANCES_META_INFORMATION:
			{
				GetController()->ShowMetaInformation(pInstance);
			}
			break;

			case ID_INSTANCES_SAVE:
			{
				GetController()->Save(pInstance);
			}
			break;

			case ID_INSTANCES_DISABLE_ALL_BUT_THIS:
			{
				auto itRFDInstances = mapInstances.begin();
				for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
				{
					if (pInstance->GetModel() != itRFDInstances->second->GetModel())
					{
						continue;
					}

					if (itRFDInstances->second == pInstance)
					{
						itRFDInstances->second->setEnable(true);

						continue;
					}

					itRFDInstances->second->setEnable(false);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE_ALL:
			{
				auto itRFDInstances = mapInstances.begin();
				for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
				{
					itRFDInstances->second->setEnable(true);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;
			 
			case ID_INSTANCES_ENABLE_ALL_UNREFERENCED:
			{
				auto itRFDInstances = mapInstances.begin();
				for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
				{
					itRFDInstances->second->setEnable(!pInstance->isReferenced());
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE_ALL_REFERENCED:
			{
				auto itRFDInstances = mapInstances.begin();
				for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
				{
					itRFDInstances->second->setEnable(pInstance->isReferenced());
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE:
			{
				pInstance->setEnable(!pInstance->getEnable());

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_REMOVE:
			{
				if (pInstance->isReferenced())
				{
					MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

					return;
				}

				GetController()->DeleteInstance(this, pInstance);
			}
			break;

			case ID_INSTANCES_REMOVE_TREE:
			{
				if (pInstance->isReferenced())
				{
					MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

					return;
				}

				GetController()->DeleteInstanceTree(this, pInstance);
			}
			break;

			case ID_INSTANCES_ADD_MEASUREMENTS:
			{
				GetController()->AddMeasurements(this, pInstance);
			}
			break;

			case ID_INSTANCES_SEARCH:
			{
				if (!m_pSearchDialog->IsWindowVisible())
				{
					m_pSearchDialog->ShowWindow(SW_SHOW);
				}
				else
				{
					m_pSearchDialog->ShowWindow(SW_HIDE);
				}
			}
			break;

			default:
			{
				ASSERT(false);
			}
			break;
		} // switch (uiCommand)	

		return;
	} // if (pInstance->hasGeometry())	

	/*
	* Instances without a geometry
	*/
	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_NO_GEOMETRY));

	auto pPopup = menu.GetSubMenu(0);

	UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
	if (uiCommand == 0)
	{
		return;
	}

	switch (uiCommand)
	{
		case ID_INSTANCES_ENABLE_ALL:
		{
			auto itRFDInstances = mapInstances.begin();
			for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
			{
				itRFDInstances->second->setEnable(true);
			}

			GetController()->OnInstancesEnabledStateChanged();
		}
		break;

		case ID_INSTANCES_ENABLE_ALL_UNREFERENCED:
		{
			auto itRFDInstances = mapInstances.begin();
			for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
			{
				itRFDInstances->second->setEnable(!pInstance->isReferenced());
			}

			GetController()->OnInstancesEnabledStateChanged();
		}
		break;

		case ID_INSTANCES_ENABLE_ALL_REFERENCED:
		{
			auto itRFDInstances = mapInstances.begin();
			for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
			{
				itRFDInstances->second->setEnable(pInstance->isReferenced());
			}

			GetController()->OnInstancesEnabledStateChanged();
		}
		break;

		case ID_INSTANCES_REMOVE:
		{
			if (pInstance->isReferenced())
			{
				MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

				return;
			}

			GetController()->DeleteInstance(this, pInstance);
		}
		break;

		case ID_INSTANCES_REMOVE_TREE:
		{
			if (pInstance->isReferenced())
			{
				MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

				return;
			}

			MessageBox(L"Not an option yet I.", L"Error", MB_ICONERROR | MB_OK);
			GetController()->DeleteInstance(this, pInstance);
		}
		break;

		case ID_INSTANCES_SEARCH:
		{
			if (!m_pSearchDialog->IsWindowVisible())
			{
				m_pSearchDialog->ShowWindow(SW_SHOW);
			}
			else
			{
				m_pSearchDialog->ShowWindow(SW_HIDE);
			}
		}
		break;

		default:
		{
			ASSERT(false);
		}
		break;
	} // switch (uiCommand)	

	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);	
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("Properties...."));

}

void CFileView::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CFileView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CFileView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CFileView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CFileViewMenuButton* pButton = DYNAMIC_DOWNCAST(CFileViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}

	UpdateView();
}

void CFileView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_INSTANCES_24 : IDR_SORT_INSTANCES, 0, 0, TRUE /* Locked */);
}

void CFileView::OnDestroy()
{
	ASSERT(GetController() != nullptr);
	GetController()->UnRegisterView(this);

	__super::OnDestroy();

	delete m_pSearchDialog;
}

void CFileView::OnSelectedItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	if (m_bInitInProgress)
	{
		return;
	}

	NM_TREEVIEW * pNMTreeView = (NM_TREEVIEW *)pNMHDR;

	if (m_wndFileView.GetParentItem(m_hSelectedItem) != nullptr)
	{
		// keep the roots always bold
		m_wndFileView.SetItemState(m_hSelectedItem, 0, TVIS_BOLD);
	}

	m_wndFileView.SetItemState(pNMTreeView->itemNew.hItem, TVIS_BOLD, TVIS_BOLD);
	m_hSelectedItem = pNMTreeView->itemNew.hItem;

	ASSERT(GetController() != nullptr);

	CRDFItem * pItem = (CRDFItem *)m_wndFileView.GetItemData(pNMTreeView->itemNew.hItem);
	if (pItem == nullptr)
	{
		GetController()->SelectInstance(this, nullptr);

		return;
	}	

	if (pItem->getType() == enumItemType::Instance)
	{
		auto pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);

		GetController()->SelectInstance(this, pInstanceItem->getInstance());		

		return;
	} // if (pItem->getType() == enumItemType::Instance)	

	if (pItem->getType() == enumItemType::Property)
	{
		CRDFPropertyItem * pPropertyItem = dynamic_cast<CRDFPropertyItem *>(pItem);
		
		GetController()->SelectInstanceProperty(pPropertyItem->getInstance(), pPropertyItem->getProperty());		

		return;
	} // if (pItem->getType() == enumItemType::Property)

	GetController()->SelectInstance(this, nullptr);
}

void CFileView::OnItemExpanding(NMHDR * pNMHDR, LRESULT * pResult)
{
	*pResult = 0;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (m_wndFileView.GetChildItem(pNMTreeView->itemNew.hItem) != nullptr)
	{
		// it is loaded already
		return;
	}

	CRDFItem * pItem = (CRDFItem *)m_wndFileView.GetItemData(pNMTreeView->itemNew.hItem);
	ASSERT(pItem != nullptr);
	ASSERT(pItem->getType() == enumItemType::Instance);

	CRDFInstanceItem * pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);

	AddProperties(pNMTreeView->itemNew.hItem, pInstanceItem->getInstance());
}

void CFileView::OnNewInstance()
{
	CNewInstanceDialog dlgNewInstance(GetController(), ::AfxGetMainWnd());
	if (dlgNewInstance.DoModal() != IDOK)
	{
		return;
	}

	ASSERT(dlgNewInstance.m_pNewInstanceRDFClass != nullptr);

	GetController()->CreateNewInstance(this, dlgNewInstance.m_pNewInstanceRDFClass->getInstance());
}

void CFileView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (!bShow)
	{
		m_pSearchDialog->ShowWindow(SW_HIDE);
	}
}
