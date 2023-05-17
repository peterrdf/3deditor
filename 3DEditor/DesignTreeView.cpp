
#include "stdafx.h"
#include "mainfrm.h"
#include "DesignTreeView.h"
#include "Resource.h"
#include "3DEditor.h"
#include "RDFModel.h"
#include "Generic.h"
#include "NewInstanceDialog.h"
#include "DesignTreeViewConsts.h"
#include "ProgressIndicator.h"

#include <algorithm>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CDesignTreeViewMenuButton : public CMFCToolBarMenuButton
{

	DECLARE_SERIAL(CDesignTreeViewMenuButton)

public:
	CDesignTreeViewMenuButton(HMENU hMenu = nullptr) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
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

IMPLEMENT_SERIAL(CDesignTreeViewMenuButton, CMFCToolBarMenuButton, 1)

/*virtual*/ void CDesignTreeView::OnModelChanged()
{
	if (GetController()->IsTestMode())
	{
		return;
	}

	UpdateView();
}

/*virtual*/ void CDesignTreeView::OnInstanceSelected(CRDFView* pSender)
{
	if (pSender == this)
	{
		return;
	}

	SelectInstance(GetController()->GetSelectedInstance(), TRUE);
}

/*virtual*/ void CDesignTreeView::OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty)
{
	ASSERT(pInstance != nullptr);
	ASSERT(pProperty != nullptr);

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	/*
	* Update non-referenced item
	*/
	if ((m_nCurrSort == ID_SORTING_INSTANCES_NOT_REFERENCED) && (pProperty->getType() == TYPE_OBJECTTYPE))
	{
		HTREEITEM hModel = m_treeCtrl.GetChildItem(nullptr);
		ASSERT(hModel != nullptr);

		UpdateRootItemsUnreferencedItemsView(pModel->GetModel(), hModel);
	} // if ((m_nCurrSort == ID_SORTING_INSTANCES_NOT_REFERENCED) && ...

	auto itInstance2Properties = m_mapInstance2Properties.find(pInstance->GetInstance());
	if (itInstance2Properties == m_mapInstance2Properties.end())
	{
		// The properties are not loaded
		return;
	}

	auto itPropertyItem = itInstance2Properties->second.find(pProperty->GetInstance());
	ASSERT(itPropertyItem != itInstance2Properties->second.end());

	CRDFPropertyItem * pPropertyItem = itPropertyItem->second;
	ASSERT(pPropertyItem != nullptr);

	ASSERT(pPropertyItem->items().size() > 0);	

	wchar_t szBuffer[100];

	switch (pPropertyItem->GetProperty()->getType())
	{
		case TYPE_OBJECTTYPE:
		{
			int64_t * piInstances = nullptr;
			int64_t iCard = 0;
			GetObjectProperty(pInstance->GetInstance(), pProperty->GetInstance(), &piInstances, &iCard);

			ASSERT(iCard >= 0);

			for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
			{
				vector<HTREEITEM> vecValues;
				HTREEITEM hCardinality = nullptr;

				HTREEITEM hChild = m_treeCtrl.GetChildItem(pPropertyItem->items()[iItem]);
				while (hChild != nullptr)
				{
					CString strText = m_treeCtrl.GetItemText(hChild);
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

					hChild = m_treeCtrl.GetNextSiblingItem(hChild);
				}

				/*
				* Disable the drawing
				*/
				m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);

				/*
				* Update the cardinality
				*/
				ASSERT(hCardinality != nullptr);

				wstring strCardinality = L"owl:cardinality : ";
				strCardinality += pProperty->getCardinality(pInstance->GetInstance());

				m_treeCtrl.SetItemText(hCardinality, strCardinality.c_str());

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

					m_treeCtrl.DeleteItem(vecValues[iValue]);
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
						m_treeCtrl.InsertItem(EMPTY_INSTANCE, IMAGE_INSTANCE, IMAGE_INSTANCE, hProperty);
					}

					if ((iInstance + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iInstance = ...

				if (iInstancesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // for (size_t iItem = ...

			/*
			* Enable the drawing
			*/
			m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
		} // case TYPE_OBJECTTYPE:
		break;

		case TYPE_BOOL_DATATYPE:
		{
			int64_t iCard = 0;
			bool* pbValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&pbValue, &iCard);

			ASSERT(iCard >= 0);

			for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
			{
				vector<HTREEITEM> vecValues;
				HTREEITEM hCardinality = nullptr;

				HTREEITEM hChild = m_treeCtrl.GetChildItem(pPropertyItem->items()[iItem]);
				while (hChild != nullptr)
				{
					CString strText = m_treeCtrl.GetItemText(hChild);
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

					hChild = m_treeCtrl.GetNextSiblingItem(hChild);
				}

				/*
				* Disable the drawing
				*/
				m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);

				/*
				* Update the cardinality
				*/
				ASSERT(hCardinality != nullptr);

				wstring strCardinality = L"owl:cardinality : ";
				strCardinality += pProperty->getCardinality(pInstance->GetInstance());

				m_treeCtrl.SetItemText(hCardinality, strCardinality.c_str());

				/*
				* Delete all values
				*/
				for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
				{
					m_treeCtrl.DeleteItem(vecValues[iValue]);
				}

				/*
				* Add the values
				*/

				HTREEITEM hProperty = pPropertyItem->items()[iItem];

				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %s", pbValue[iValue] ? L"True" : L"False");

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // for (size_t iItem = ...

			/*
			* Enable the drawing
			*/
			m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
		} // case TYPE_BOOL_DATATYPE:
		break;

		case TYPE_CHAR_DATATYPE:
		{
			int64_t iCard = 0;
			char ** szValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&szValue, &iCard);

			ASSERT(iCard >= 0);

			for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
			{
				vector<HTREEITEM> vecValues;
				HTREEITEM hCardinality = nullptr;

				HTREEITEM hChild = m_treeCtrl.GetChildItem(pPropertyItem->items()[iItem]);
				while (hChild != nullptr)
				{
					CString strText = m_treeCtrl.GetItemText(hChild);
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

					hChild = m_treeCtrl.GetNextSiblingItem(hChild);
				}

				/*
				* Disable the drawing
				*/
				m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);

				/*
				* Update the cardinality
				*/
				ASSERT(hCardinality != nullptr);

				wstring strCardinality = L"owl:cardinality : ";
				strCardinality += pProperty->getCardinality(pInstance->GetInstance());

				m_treeCtrl.SetItemText(hCardinality, strCardinality.c_str());

				/*
				* Delete all values
				*/
				for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
				{
					m_treeCtrl.DeleteItem(vecValues[iValue]);
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

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // for (size_t iItem = ...

			/*
			* Enable the drawing
			*/
			m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			int64_t iCard = 0;
			double * pdValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&pdValue, &iCard);

			ASSERT(iCard >= 0);

			for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
			{
				vector<HTREEITEM> vecValues;
				HTREEITEM hCardinality = nullptr;

				HTREEITEM hChild = m_treeCtrl.GetChildItem(pPropertyItem->items()[iItem]);
				while (hChild != nullptr)
				{
					CString strText = m_treeCtrl.GetItemText(hChild);
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

					hChild = m_treeCtrl.GetNextSiblingItem(hChild);
				}

				/*
				* Disable the drawing
				*/
				m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);			

				/*
				* Update the cardinality
				*/
				ASSERT(hCardinality != nullptr);

				wstring strCardinality = L"owl:cardinality : ";
				strCardinality += pProperty->getCardinality(pInstance->GetInstance());

				m_treeCtrl.SetItemText(hCardinality, strCardinality.c_str());

				/*
				* Delete all values
				*/
				for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
				{
					m_treeCtrl.DeleteItem(vecValues[iValue]);
				}

				/*
				* Add the values
				*/

				HTREEITEM hProperty = pPropertyItem->items()[iItem];

				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %.6f", pdValue[iValue]);

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}			
			} // for (size_t iItem = ...

			/*
			* Enable the drawing
			*/
			m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
		} // case TYPE_DOUBLE_DATATYPE:
		break;

		case TYPE_INT_DATATYPE:
		{
			int64_t iCard = 0;
			int64_t * piValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pPropertyItem->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

			ASSERT(iCard >= 0);

			for (size_t iItem = 0; iItem < pPropertyItem->items().size(); iItem++)
			{
				vector<HTREEITEM> vecValues;
				HTREEITEM hCardinality = nullptr;

				HTREEITEM hChild = m_treeCtrl.GetChildItem(pPropertyItem->items()[iItem]);
				while (hChild != nullptr)
				{
					CString strText = m_treeCtrl.GetItemText(hChild);
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

					hChild = m_treeCtrl.GetNextSiblingItem(hChild);
				}

				/*
				* Disable the drawing
				*/
				m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);

				/*
				* Update the cardinality
				*/
				ASSERT(hCardinality != nullptr);

				wstring strCardinality = L"owl:cardinality : ";
				strCardinality += pProperty->getCardinality(pInstance->GetInstance());

				m_treeCtrl.SetItemText(hCardinality, strCardinality.c_str());

				/*
				* Delete all values
				*/
				for (int64_t iValue = 0; iValue < (int64_t)vecValues.size(); iValue++)
				{
					m_treeCtrl.DeleteItem(vecValues[iValue]);
				}

				/*
				* Add the values
				*/

				HTREEITEM hProperty = pPropertyItem->items()[iItem];

				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %lld", piValue[iValue]);

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // for (size_t iItem = ...

			/*
			* Enable the drawing
			*/
			m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
		} // case TYPE_INT_DATATYPE:
		break;

		default:
		{
			ASSERT(false); // unknown property
		}
		break;
	} // switch (pPropertyItem->GetProperty()->getType())
}

/*virtual*/ void CDesignTreeView::OnNewInstanceCreated(CRDFView* pSender, CRDFInstance * /*pInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	UpdateView();
}

/*virtual*/ void CDesignTreeView::OnInstanceDeleted(CRDFView* pSender, int64_t /*iInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	UpdateView();
}

/*virtual*/ void CDesignTreeView::OnMeasurementsAdded(CRDFView* pSender, CRDFInstance * /*pInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	UpdateView();
}

/*virtual*/ void CDesignTreeView::OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty)
{
	if (pSender == this)
	{
		return;
	}

	switch (enApplicationProperty)
	{
		case enumApplicationProperty::Projection:
		case enumApplicationProperty::View:
		case enumApplicationProperty::ShowFaces:
		case enumApplicationProperty::CullFaces:
		case enumApplicationProperty::ShowFacesWireframes:
		case enumApplicationProperty::ShowConceptualFacesWireframes:
		case enumApplicationProperty::ShowLines:
		case enumApplicationProperty::ShowPoints:
		case enumApplicationProperty::ShowNormalVectors:
		case enumApplicationProperty::ShowTangenVectors:
		case enumApplicationProperty::ShowBiNormalVectors:
		case enumApplicationProperty::ScaleVectors:
		case enumApplicationProperty::ShowBoundingBoxes:
		case enumApplicationProperty::RotationMode:
		case enumApplicationProperty::PointLightingLocation:
		case enumApplicationProperty::AmbientLightWeighting:
		case enumApplicationProperty::SpecularLightWeighting:
		case enumApplicationProperty::DiffuseLightWeighting:
		case enumApplicationProperty::MaterialShininess:
		case enumApplicationProperty::Contrast:
		case enumApplicationProperty::Brightness:
		case enumApplicationProperty::Gamma:
		{
			// Not supported
		}
		break;

		case enumApplicationProperty::VisibleValuesCountLimit:
		{
			UpdateView();
		}
		break;

		case enumApplicationProperty::ScalelAndCenter:
		{
			// Not supported
		}
		break;

		default:
		{
			ASSERT(FALSE); // Internal error!
		}
		break;
	} // switch (enApplicationProperty)
}

/*virtual*/ bool CDesignTreeView::IsSelected(HTREEITEM hItem, COLORREF& clr) /*override*/
{
	auto pItem = (CRDFItem*)m_treeCtrl.GetItemData(hItem);
	if ((pItem != nullptr) && (pItem->getType() == enumItemType::Instance))
	{
		clr = RGB(255, 0, 0);
		return pItem->GetInstance() == GetController()->GetSelectedInstance();
	}

	return false;
}

CRDFModel* CDesignTreeView::GetModel() const
{
	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return nullptr;
	}

	return pController->GetModel();
}

void CDesignTreeView::SelectInstance(CRDFInstance* pInstance, BOOL bSelectTreeItem)
{
	if (m_pSelectedInstance == pInstance)
	{
		return;
	}

	if (m_hSelectedInstance != NULL)
	{
		m_treeCtrl.SetItemState(m_hSelectedInstance, 0, TVIS_BOLD);
		m_hSelectedInstance = NULL;
		m_pSelectedInstance = nullptr;
	}

	m_pSelectedInstance = pInstance;
	
	if (m_pSelectedInstance == nullptr)
	{
		/** Select the Model by default */
		if (bSelectTreeItem)
		{
			HTREEITEM hModel = m_treeCtrl.GetRootItem();
			ASSERT(hModel != NULL); // Internal error!

			if (hModel != NULL)
			{
				m_treeCtrl.SelectItem(hModel);
			}
		}

		return;
	}

	/** Disable the drawing */
	m_treeCtrl.SendMessage(WM_SETREDRAW, 0, 0);

	/** Load instance on demand */
	auto itInstance2Item = m_mapInstance2Item.find(m_pSelectedInstance->GetInstance());
	if (itInstance2Item == m_mapInstance2Item.end())
	{
		/** Find all ancestors */
		vector<int64_t> vecAncestors;

		int64_t iInstance = GetInstanceInverseReferencesByIterator(m_pSelectedInstance->GetInstance(), 0);
		while (iInstance != 0)
		{
			vecAncestors.push_back(iInstance);

			iInstance = GetInstanceInverseReferencesByIterator(iInstance, 0);
		}

		/** Load the ancestors */
		for (int64_t iAncestor = vecAncestors.size() - 1; iAncestor >= 0; iAncestor--)
		{
			itInstance2Item = m_mapInstance2Item.find(vecAncestors[iAncestor]);
			if (itInstance2Item != m_mapInstance2Item.end())
			{
				// The item is visible	
				ASSERT(!itInstance2Item->second->items().empty());
				m_treeCtrl.Expand(itInstance2Item->second->items()[0], TVE_EXPAND);
			}
			else
			{
				// The item is not visible - it is in "..." group
				break;
			}
		} // for (size_t iAncestor = ...

		itInstance2Item = m_mapInstance2Item.find(m_pSelectedInstance->GetInstance());
	} // if (itInstance2Item == m_mapInstance2Item.end())

	/** Select */
	if (itInstance2Item != m_mapInstance2Item.end())
	{
		// The item is visible
		ASSERT(!itInstance2Item->second->items().empty());

		m_hSelectedInstance = itInstance2Item->second->items()[0];

		m_treeCtrl.SetItemState(m_hSelectedInstance, TVIS_BOLD, TVIS_BOLD);		

		if (bSelectTreeItem)
		{
			m_treeCtrl.EnsureVisible(m_hSelectedInstance);
			m_treeCtrl.SelectItem(m_hSelectedInstance);
		}
	}
	else
	{
		// The item is not visible - it is in "..." group
		MessageBox(L"The selected item is not visible in Instance View.\nPlease, increase 'Visible values count limit' property.",
			L"Information",
			MB_ICONINFORMATION | MB_OK);
	}

	/** Enable the drawing */
	m_treeCtrl.SendMessage(WM_SETREDRAW, 1, 0);
}

void CDesignTreeView::GetItemPath(HTREEITEM hItem, vector<pair<CRDFInstance*, CRDFProperty*>>& vecPath)
{
	if (hItem == nullptr)
	{
		return;
	}

	auto pItem = (CRDFItem*)m_treeCtrl.GetItemData(hItem);
	if (pItem != nullptr)
	{
		switch (pItem->getType())
		{
			case enumItemType::Instance:
			{
				auto pInstanceItem = dynamic_cast<CRDFInstanceItem*>(pItem);
				ASSERT(pInstanceItem != nullptr);

				vecPath.insert(vecPath.begin(), pair<CRDFInstance*, CRDFProperty *>(pInstanceItem->GetInstance(), nullptr));
			}
			break;

			case enumItemType::Property:
			{
				auto pPropertyItem = dynamic_cast<CRDFPropertyItem*>(pItem);
				ASSERT(pPropertyItem != nullptr);

				vecPath.insert(vecPath.begin(), pair<CRDFInstance*, CRDFProperty*>(pPropertyItem->GetInstance(), pPropertyItem->GetProperty()));
			}
			break;

			default:
			{
				ASSERT(false); // Internal error!
			}
			break;
		}
	} // if (pItem != nullptr)

	GetItemPath(m_treeCtrl.GetParentItem(hItem), vecPath);
}

void CDesignTreeView::GetDescendants(HTREEITEM hItem, vector<HTREEITEM> & vecDescendants)
{
	ASSERT(hItem != nullptr);

	HTREEITEM hChild = m_treeCtrl.GetChildItem(hItem);
	while (hChild != nullptr)
	{
		if (m_treeCtrl.GetItemData(hChild) != NULL)
		{
			vecDescendants.push_back(hChild);

			GetDescendants(hChild, vecDescendants);
		}		

		hChild = m_treeCtrl.GetNextSiblingItem(hChild);
	}
}

void CDesignTreeView::RemoveInstanceItemData(CRDFInstance * pInstance, HTREEITEM hInstance)
{
	ASSERT(pInstance != nullptr);
	ASSERT(hInstance != nullptr);

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.find(pInstance->GetInstance());
	ASSERT(itInstance2Item != m_mapInstance2Item.end());

	vector<HTREEITEM>::const_iterator itInstance = find(itInstance2Item->second->items().begin(), itInstance2Item->second->items().end(), hInstance);
	ASSERT(itInstance != itInstance2Item->second->items().end());

	itInstance2Item->second->items().erase(itInstance);
}

void CDesignTreeView::RemovePropertyItemData(CRDFInstance* pInstance, CRDFProperty* pProperty, HTREEITEM hProperty)
{
	ASSERT(pInstance != nullptr);
	ASSERT(pProperty != nullptr);
	ASSERT(hProperty != NULL);

	auto itInstance2Properties = m_mapInstance2Properties.find(pInstance->GetInstance());
	ASSERT(itInstance2Properties != m_mapInstance2Properties.end());

	auto itPropertyItem = itInstance2Properties->second.find(pProperty->GetInstance());
	ASSERT(itPropertyItem != itInstance2Properties->second.end());

	auto itInstance = find(itPropertyItem->second->items().begin(), itPropertyItem->second->items().end(), hProperty);
	ASSERT(itInstance != itPropertyItem->second->items().end());

	itPropertyItem->second->items().erase(itInstance);
}

void CDesignTreeView::RemoveItemData(HTREEITEM hItem)
{
	if (hItem == NULL)
	{
		ASSERT(FALSE);

		return;
	}

	auto pItem = (CRDFItem*)m_treeCtrl.GetItemData(hItem);
	if (pItem != nullptr)
	{
		switch (pItem->getType())
		{
			case enumItemType::Instance:
			{
				auto pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);
				ASSERT(pInstanceItem != nullptr);

				RemoveInstanceItemData(pInstanceItem->GetInstance(), hItem);
			}
			break;

			case enumItemType::Property:
			{
				auto pPropertyItem = dynamic_cast<CRDFPropertyItem *>(pItem);
				ASSERT(pPropertyItem != nullptr);

				RemovePropertyItemData(pPropertyItem->GetInstance(), pPropertyItem->GetProperty(), hItem);
			}
			break;

			default:
			{
				ASSERT(false); // Internal error!
			}
			break;
		} // switch (pItem->getType())
	} // if (pItem != nullptr)
}

void CDesignTreeView::UpdateView()
{
	m_pSearchDialog->Reset();
	
	/** Disable the drawing */	
	m_bUpdateInProgress = true;

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
	
	/** Restore the selected instance */
	if (GetController()->GetSelectedInstance() != nullptr)
	{
		OnInstanceSelected(nullptr);
	}
	
	/** Enable the drawing */	
	m_bUpdateInProgress = false;
}

void CDesignTreeView::InstancesAlphabeticalView()
{
	m_treeCtrl.DeleteAllItems();	
	Clean();

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	vector<CRDFInstance*> vecModel;	
	for (auto itRFDInstances = mapInstances.begin();
		itRFDInstances != mapInstances.end(); 
		itRFDInstances++)
	{
		auto pInstance = itRFDInstances->second;

		if (pInstance->GetModel() == pModel->GetModel())
		{
			vecModel.push_back(pInstance);
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	/*
	* Model
	*/
	sort(vecModel.begin(), vecModel.end(), SORT_RDFINSTANCES());

	HTREEITEM hModel = m_treeCtrl.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);
	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		AddInstance(hModel, vecModel[iInstance]);
	}

	m_treeCtrl.Expand(hModel, TVE_EXPAND);
}

void CDesignTreeView::InstancesGroupByClassView()
{
	m_treeCtrl.DeleteAllItems();
	Clean();

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	map<wstring, vector<CRDFInstance*>> mapModel;	
	for (auto itRFDInstances = mapInstances.begin();
		itRFDInstances != mapInstances.end(); 
		itRFDInstances++)
	{
		auto pInstance = itRFDInstances->second;

		char* szName = nullptr;
		GetNameOfClass(pInstance->GetClassInstance(), &szName);

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
	} // for (auto itRFDInstances = ...

	/*
	* Model
	*/
	HTREEITEM hModel = m_treeCtrl.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);	
	for (auto itModel = mapModel.begin();
		itModel != mapModel.end(); 
		itModel++)
	{
		HTREEITEM hClass = m_treeCtrl.InsertItem(itModel->first.c_str(), IMAGE_INSTANCE, IMAGE_INSTANCE, hModel);

		vector<CRDFInstance *> vecInstances = itModel->second;
		sort(vecInstances.begin(), vecInstances.end(), SORT_RDFINSTANCES());

		for (size_t iInstance = 0; iInstance < vecInstances.size(); iInstance++)
		{
			AddInstance(hClass, vecInstances[iInstance]);
		}
	} // for (; itModel != ...

	m_treeCtrl.Expand(hModel, TVE_EXPAND);
}

void CDesignTreeView::InstancesUnreferencedItemsView()
{
	ProgressStatus prgs(L"Build project tree");

	m_treeCtrl.DeleteAllItems();
	Clean();	

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	prgs.Start(mapInstances.size());

	vector<CRDFInstance *> vecModel;
	for (auto itRFDInstances = mapInstances.begin();
		itRFDInstances != mapInstances.end(); 
		itRFDInstances++)
	{
		prgs.Step();

		CRDFInstance * pInstance = itRFDInstances->second;

		if (pInstance->IsReferenced())
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

	HTREEITEM hModel = m_treeCtrl.InsertItem(_T("Model"), IMAGE_MODEL, IMAGE_MODEL);
	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		AddInstance(hModel, vecModel[iInstance]);
	}

	m_treeCtrl.Expand(hModel, TVE_EXPAND);
}

void CDesignTreeView::AddInstance(HTREEITEM hParent, CRDFInstance * pInstance)
{
	/*
	* The instances will be loaded on demand
	*/
	TV_INSERTSTRUCT tvInsertStruct;
	tvInsertStruct.hParent = hParent;
	tvInsertStruct.hInsertAfter = TVI_LAST;
	tvInsertStruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvInsertStruct.item.pszText = (LPWSTR)pInstance->GetUniqueName();
	tvInsertStruct.item.iImage = tvInsertStruct.item.iSelectedImage = IMAGE_INSTANCE;
	tvInsertStruct.item.lParam = NULL;
	tvInsertStruct.item.cChildren = IMAGE_INSTANCE;

	HTREEITEM hInstance = m_treeCtrl.InsertItem(&tvInsertStruct);

	map<int64_t, CRDFInstanceItem *>::iterator itInstance2Item = m_mapInstance2Item.find(pInstance->GetInstance());
	if (itInstance2Item == m_mapInstance2Item.end())
	{
		CRDFInstanceItem * pInstanceItem = new CRDFInstanceItem(pInstance);
		pInstanceItem->items().push_back(hInstance);

		m_mapInstance2Item[pInstance->GetInstance()] = pInstanceItem;

		m_treeCtrl.SetItemData(hInstance, (DWORD_PTR)pInstanceItem);
	}
	else
	{
		itInstance2Item->second->items().push_back(hInstance);

		m_treeCtrl.SetItemData(hInstance, (DWORD_PTR)itInstance2Item->second);
	}
}

void CDesignTreeView::AddProperties(HTREEITEM hParent, CRDFInstance * pInstance)
{
	wchar_t szBuffer[100];

	if (pInstance == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapProperties = pModel->GetProperties();
	auto& mapInstances = pModel->GetInstances();

	int64_t iPropertyInstance = GetInstancePropertyByIterator(pInstance->GetInstance(), 0);
	while (iPropertyInstance != 0)
	{
		map<int64_t, CRDFProperty *>::const_iterator itProperty = mapProperties.find(iPropertyInstance);
		ASSERT(itProperty != mapProperties.end());

		auto pProperty = itProperty->second;

		wstring strNameAndType = pProperty->GetName();
		strNameAndType += L" : ";
		strNameAndType += pProperty->getTypeAsString();		

		HTREEITEM hProperty = m_treeCtrl.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hParent);

		map<int64_t, map<int64_t, CRDFPropertyItem *> >::iterator itInstance2Properties = m_mapInstance2Properties.find(pInstance->GetInstance());
		if (itInstance2Properties == m_mapInstance2Properties.end())
		{
			auto pPropertyItem = new CRDFPropertyItem(pInstance, pProperty);
			pPropertyItem->items().push_back(hProperty);

			map<int64_t, CRDFPropertyItem *> mapItemProperties;
			mapItemProperties[pProperty->GetInstance()] = pPropertyItem;

			m_mapInstance2Properties[pInstance->GetInstance()] = mapItemProperties;

			m_treeCtrl.SetItemData(hProperty, (DWORD_PTR)pPropertyItem);
		}
		else
		{
			map<int64_t, CRDFPropertyItem *>::iterator itPropertyItem = itInstance2Properties->second.find(pProperty->GetInstance());
			if (itPropertyItem == itInstance2Properties->second.end())
			{
				CRDFPropertyItem * pPropertyItem = new CRDFPropertyItem(pInstance, pProperty);
				pPropertyItem->items().push_back(hProperty);

				itInstance2Properties->second[pProperty->GetInstance()] = pPropertyItem;

				m_treeCtrl.SetItemData(hProperty, (DWORD_PTR)pPropertyItem);
			}
			else
			{
				itPropertyItem->second->items().push_back(hProperty);

				m_treeCtrl.SetItemData(hProperty, (DWORD_PTR)itPropertyItem->second);
			}
		}

		/*
		* rdfs:range
		*/
		if (pProperty->getType() == TYPE_OBJECTTYPE)
		{
			HTREEITEM hRange = m_treeCtrl.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

			CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
			ASSERT(pObjectRDFProperty != nullptr);

			auto& vecRestrictions = pObjectRDFProperty->getRestrictions();
			for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
			{
				char * szClassName = nullptr;
				GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

				m_treeCtrl.InsertItem(CA2W(szClassName), IMAGE_VALUE, IMAGE_VALUE, hRange);
			}
		} // if (pProperty->getType() == TYPE_OBJECTTYPE)
		else
		{
			wstring strRange = L"rdfs:range : ";
			strRange += pProperty->getRange();

			m_treeCtrl.InsertItem(strRange.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
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
			GetObjectProperty(pInstance->GetInstance(), pProperty->GetInstance(), &piInstances, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->GetInstance());

			m_treeCtrl.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

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
						m_treeCtrl.InsertItem(EMPTY_INSTANCE, IMAGE_INSTANCE, IMAGE_INSTANCE, hProperty);
					}					

					if ((iInstance + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iInstance = ...

				if (iInstancesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			int64_t iCard = 0;
			bool * pbValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&pbValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->GetInstance());

			m_treeCtrl.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %s", pbValue[iValue] ? L"TRUE" : L"FALSE");

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)			
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			int64_t iCard = 0;
			char ** szValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&szValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->GetInstance());

			m_treeCtrl.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					wstring strValue = CA2W(szValue[iValue]);
					swprintf(szBuffer, 100, L"value = '%s'", strValue.c_str());

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)			
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			int64_t iCard = 0;
			double * pdValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&pdValue, &iCard);


			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->GetInstance());

			m_treeCtrl.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %.6f", pdValue[iValue]);

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
				}
			} // if (iCard > 0)
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			int64_t iCard = 0;
			int64_t * piValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void **)&piValue, &iCard);

			/*
			* owl:cardinality
			*/
			wstring strCardinality = L"owl:cardinality : ";
			strCardinality += pProperty->getCardinality(pInstance->GetInstance());

			m_treeCtrl.InsertItem(strCardinality.c_str(), IMAGE_VALUE, IMAGE_VALUE, hProperty);

			if (iCard > 0)
			{
				int64_t iValuesCount = iCard;
				for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
				{
					swprintf(szBuffer, 100, L"value = %lld", piValue[iValue]);

					m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);

					if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
					{
						break;
					}
				} // for (int64_t iValue = ...

				if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
				{
					m_treeCtrl.InsertItem(L"...", IMAGE_VALUE, IMAGE_VALUE, hProperty);
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

		iPropertyInstance = GetInstancePropertyByIterator(pInstance->GetInstance(), iPropertyInstance);
	} // while (iPropertyInstance != 0)
}

void CDesignTreeView::UpdateRootItemsUnreferencedItemsView(int64_t iModel, HTREEITEM hModel)
{
	ASSERT(iModel != 0);
	ASSERT(hModel != nullptr);

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	vector<CRDFInstance*> vecInstances;
	vector<HTREEITEM> vecObsoleteItems;

	HTREEITEM hItem = m_treeCtrl.GetChildItem(hModel);
	while (hItem != nullptr)
	{
		CRDFItem * pItem = (CRDFItem *)m_treeCtrl.GetItemData(hItem);
		ASSERT(pItem != nullptr);
		ASSERT(pItem->getType() == enumItemType::Instance);

		CRDFInstanceItem * pInstanceItem = dynamic_cast<CRDFInstanceItem *>(pItem);
		ASSERT(pInstanceItem != nullptr);

		if (pInstanceItem->GetInstance()->IsReferenced())
		{
			vecObsoleteItems.push_back(hItem);
		}
		else
		{
			vecInstances.push_back(pInstanceItem->GetInstance());
		}

		hItem = m_treeCtrl.GetNextSiblingItem(hItem);
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

		m_treeCtrl.DeleteItem(vecObsoleteItems[iItem]);
	} // for (int64_t iItem = ...

	/*
	* Add the missing items without references
	*/
	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pInstance = itRFDInstances->second;

		if (pInstance->IsReferenced())
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

void CDesignTreeView::Clean()
{
	for (auto itInstance2Item = m_mapInstance2Item.begin();
		itInstance2Item != m_mapInstance2Item.end();
		itInstance2Item++)
	{
		delete itInstance2Item->second;
	}
	m_mapInstance2Item.clear();

	for (auto itInstance2Properties = m_mapInstance2Properties.begin();
		itInstance2Properties != m_mapInstance2Properties.end();
		itInstance2Properties++)
	{
		for (auto itPropertyItem = itInstance2Properties->second.begin();
			itPropertyItem != itInstance2Properties->second.end();
			itPropertyItem++)
		{
			delete itPropertyItem->second;
		}
	}
	m_mapInstance2Properties.clear();

	m_hSelectedInstance = NULL;
	m_pSelectedInstance = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// CDesignTreeView

CDesignTreeView::CDesignTreeView()
	: m_mapInstance2Item()
	, m_mapInstance2Properties()
	, m_pSelectedInstance(nullptr)
	, m_hSelectedInstance(NULL)
	, m_bUpdateInProgress(false)
	, m_pSearchDialog(nullptr)
{
	m_nCurrSort = ID_SORTING_INSTANCES_NOT_REFERENCED;
}

CDesignTreeView::~CDesignTreeView()
{	
	Clean();
}

BEGIN_MESSAGE_MAP(CDesignTreeView, CDockablePane)
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

int CDesignTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(GetController() != nullptr);
	GetController()->RegisterView(this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_treeCtrl.Create(dwViewStyle, rectDummy, this, IDC_TREE_INSTANCE_VIEW))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// State provider
	m_treeCtrl.SetItemStateProvider(this);

	// Load view images:
	m_images.Create(IDB_CLASS_VIEW, 16, 0, RGB(255, 0, 0));
	m_treeCtrl.SetImageList(&m_images, TVSIL_NORMAL);

	m_toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT_INSTANCES);
	m_toolBar.LoadToolBar(IDR_SORT_INSTANCES, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_toolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_toolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT_INSTANCES);

	m_toolBar.ReplaceButton(ID_SORT_MENU, CDesignTreeViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CDesignTreeViewMenuButton* pButton = DYNAMIC_DOWNCAST(CDesignTreeViewMenuButton, m_toolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	AdjustLayout();

	//  Search
	m_pSearchDialog = new CSearchInstancesDialog(&m_treeCtrl);
	m_pSearchDialog->Create(IDD_DIALOG_SEARCH_INSTANCES, this);

	return 0;
}

void CDesignTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CDesignTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_treeCtrl;
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

	auto pItem = (CRDFItem *)m_treeCtrl.GetItemData(hTreeItem);
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

	auto pModel = GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto& mapInstances = pModel->GetInstances();

	auto pInstanceItem = dynamic_cast<CRDFInstanceItem*>(pItem);
	auto pInstance = pInstanceItem->GetInstance();

	/*
	* Instances with a geometry
	*/
	if (pInstance->HasGeometry())
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
				GetController()->ZoomToInstance(pInstance->GetInstance());
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
					itRFDInstances->second->setEnable(!pInstance->IsReferenced());
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE_ALL_REFERENCED:
			{
				auto itRFDInstances = mapInstances.begin();
				for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
				{
					itRFDInstances->second->setEnable(pInstance->IsReferenced());
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
				if (pInstance->IsReferenced())
				{
					MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

					return;
				}

				GetController()->DeleteInstance(this, pInstance);
			}
			break;

			case ID_INSTANCES_REMOVE_TREE:
			{
				if (pInstance->IsReferenced())
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
	} // if (pInstance->HasGeometry())	

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
				itRFDInstances->second->setEnable(!pInstance->IsReferenced());
			}

			GetController()->OnInstancesEnabledStateChanged();
		}
		break;

		case ID_INSTANCES_ENABLE_ALL_REFERENCED:
		{
			auto itRFDInstances = mapInstances.begin();
			for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
			{
				itRFDInstances->second->setEnable(pInstance->IsReferenced());
			}

			GetController()->OnInstancesEnabledStateChanged();
		}
		break;

		case ID_INSTANCES_REMOVE:
		{
			if (pInstance->IsReferenced())
			{
				MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

				return;
			}

			GetController()->DeleteInstance(this, pInstance);
		}
		break;

		case ID_INSTANCES_REMOVE_TREE:
		{
			if (pInstance->IsReferenced())
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

void CDesignTreeView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_toolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_toolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_treeCtrl.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDesignTreeView::OnProperties()
{
	::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"TODO.", L"Error", MB_ICONERROR | MB_OK);
}

void CDesignTreeView::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CDesignTreeView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	auto pButton = DYNAMIC_DOWNCAST(CDesignTreeViewMenuButton, m_toolBar.GetButton(0));
	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_toolBar.Invalidate();
		m_toolBar.UpdateWindow();
	}

	UpdateView();
}

void CDesignTreeView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CDesignTreeView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_treeCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CDesignTreeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_treeCtrl.SetFocus();
}

void CDesignTreeView::OnChangeVisualStyle()
{
	m_images.DeleteImageList();

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

	m_images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_images.Add(&bmp, RGB(255, 0, 0));

	m_treeCtrl.SetImageList(&m_images, TVSIL_NORMAL);

	m_toolBar.CleanUpLockedImages();
	m_toolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_INSTANCES_24 : IDR_SORT_INSTANCES, 0, 0, TRUE /* Locked */);
}

void CDesignTreeView::OnDestroy()
{
	ASSERT(GetController() != nullptr);
	GetController()->UnRegisterView(this);

	__super::OnDestroy();

	delete m_pSearchDialog;
}

void CDesignTreeView::OnSelectedItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	if (m_bUpdateInProgress)
	{
		return;
	}

	auto pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	auto pItem = (CRDFItem*)m_treeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
	if (pItem != nullptr)
	{
		if (pItem->getType() == enumItemType::Instance)
		{
			auto pInstanceItem = dynamic_cast<CRDFInstanceItem*>(pItem);

			GetController()->SelectInstance(nullptr/*update this view*/, pInstanceItem->GetInstance());
		}
		else if (pItem->getType() == enumItemType::Property)
		{
			auto pPropertyItem = dynamic_cast<CRDFPropertyItem*>(pItem);
			auto pSelectedInstance = pPropertyItem->GetInstance();
			auto pSelectedProperty = pPropertyItem->GetProperty();			

			GetController()->SelectInstanceProperty(pSelectedInstance, pSelectedProperty);

			SelectInstance(GetController()->GetSelectedInstance(), FALSE);
		} // else if (pItem->getType() == enumItemType::Property)
		else
		{
			ASSERT(FALSE); // Internal error!
		}
	} // if (pItem != nullptr)
	else
	{
		GetController()->SelectInstance(this, nullptr);

		SelectInstance(nullptr, FALSE);
	}
}

void CDesignTreeView::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	auto pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (m_treeCtrl.GetChildItem(pNMTreeView->itemNew.hItem) != nullptr)
	{
		// it is loaded already
		return;
	}

	auto pItem = (CRDFItem*)m_treeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
	ASSERT((pItem != nullptr) && (pItem->getType() == enumItemType::Instance));

	if (pItem != nullptr)
	{
		auto pInstanceItem = dynamic_cast<CRDFInstanceItem*>(pItem);

		AddProperties(pNMTreeView->itemNew.hItem, pInstanceItem->GetInstance());
	}
}

void CDesignTreeView::OnNewInstance()
{
	CNewInstanceDialog dlgNewInstance(GetController(), ::AfxGetMainWnd());
	if (dlgNewInstance.DoModal() != IDOK)
	{
		return;
	}

	if (dlgNewInstance.m_pNewInstanceRDFClass != nullptr)
	{
		GetController()->CreateNewInstance(this, dlgNewInstance.m_pNewInstanceRDFClass->GetInstance());
	}
}

void CDesignTreeView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (!bShow)
	{
		m_pSearchDialog->ShowWindow(SW_HIDE);
	}
}
