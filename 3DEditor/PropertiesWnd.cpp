
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "3DEditor.h"
#include "OpenGLRDFView.h"
#include "RDFClass.h"
#include "RDFModel.h"
#include "EditObjectPropertyDialog.h"
#include "SelectInstanceDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

#define WM_LOAD_INSTANCE_PROPERTY_VALUES WM_USER + 1
#define WM_LOAD_INSTANCE_PROPERTIES WM_USER + 2

#define TRUE_VALUE_PROPERTY L"Yes"
#define FALSE_VALUE_PROPERTY L"No"
#define REMOVE_OBJECT_PROPERTY_COMMAND L"---<REMOVE>---"
#define SELECT_OBJECT_PROPERTY_COMMAND L"---<SELECT>---"

// Moved in CSelectInstanceDialog
//#define USED_SUFFIX L" [used]"

// ------------------------------------------------------------------------------------------------
CApplicationPropertyData::CApplicationPropertyData(enumPropertyType enPropertyType)
{
	m_enPropertyType = enPropertyType;
}

// ------------------------------------------------------------------------------------------------
enumPropertyType CApplicationPropertyData::GetType() const
{
	return m_enPropertyType;
}

// ------------------------------------------------------------------------------------------------
CLightPropertyData::CLightPropertyData(enumPropertyType enPropertyType, int iLightIndex)
	: CApplicationPropertyData(enPropertyType)
	, m_iLightIndex(iLightIndex)
{
}

// ------------------------------------------------------------------------------------------------
int CLightPropertyData::GetLightIndex() const
{
	return m_iLightIndex;
}

// ------------------------------------------------------------------------------------------------
CApplicationProperty::CApplicationProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

// ------------------------------------------------------------------------------------------------
CApplicationProperty::CApplicationProperty(const CString & strGroupName, DWORD_PTR dwData, BOOL bIsValueList)
	: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CApplicationProperty::~CApplicationProperty()
{
	delete (CApplicationPropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
CColorApplicationProperty::CColorApplicationProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridColorProperty(strName, color, pPalette, szDescription, dwData)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CColorApplicationProperty::~CColorApplicationProperty()
{
	delete (CApplicationPropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
CRDFInstanceData::CRDFInstanceData(CRDFController * pController, CRDFInstance * pInstance)
	: m_pController(pController)
	, m_pInstance(pInstance)
{
	ASSERT(m_pController != NULL);
	ASSERT(m_pInstance != NULL);
}

// ------------------------------------------------------------------------------------------------
CRDFController * CRDFInstanceData::GetController() const
{
	return m_pController;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFInstanceData::GetInstance() const
{
	return m_pInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstancePropertyData::CRDFInstancePropertyData(CRDFController * pController, CRDFInstance * pInstance, CRDFProperty * pProperty, int64_t iCard)
	: CRDFInstanceData(pController, pInstance)
	, m_pProperty(pProperty)
	, m_iCard(iCard)
{
	ASSERT(m_pProperty != NULL);
}

// ------------------------------------------------------------------------------------------------
CRDFProperty * CRDFInstancePropertyData::GetProperty() const
{
	return m_pProperty;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstancePropertyData::GetCard() const
{
	return m_iCard;
}

void CRDFInstancePropertyData::SetCard(int64_t iCard)
{
	ASSERT(iCard >= 0);

	m_iCard = iCard;
}

// ------------------------------------------------------------------------------------------------
CRDFInstanceProperty::CRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CRDFInstanceProperty::~CRDFInstanceProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ BOOL CRDFInstanceProperty::HasButton() const
{
	ASSERT(GetData() != NULL);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != NULL);

	BOOL bHasButton = FALSE;

	/*
	* Read the restrictions
	*/
	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getInstance(), iMinCard, iMaxCard);

	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		iMinCard = 0;
	}

	switch (pData->GetProperty()->getType())
	{
	case TYPE_BOOL_DATATYPE:
	{
		bHasButton = FALSE;
	} // case TYPE_BOOL_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&szValue, &iCard);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_CHAR_DATATYPE:
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		/*
		* Read the values
		*/
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&pdValue, &iCard);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		/*
		* Read the values
		*/
		int64_t iCard = 0;
		int64_t * piValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&piValue, &iCard);

		ASSERT(iCard > 0);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())

	return bHasButton;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFInstanceProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT(GetData() != NULL);	

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != NULL);

	/*
	* Read the restrictions
	*/
	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->getType())
	{
	case TYPE_BOOL_DATATYPE:
	{
		// NA
	} // case TYPE_BOOL_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&szValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		char ** szNewValues = (char **)new size_t[iCard - 1];

		int iCurrentValue = 0;
		for (int iValue = 0; iValue < iCard; iValue++)
		{
			if (iValue == pData->GetCard())
			{
				continue;
			}

			szNewValues[iCurrentValue] = new char[strlen(szValue[iCurrentValue]) + 1];
			strcpy(szNewValues[iCurrentValue], szValue[iCurrentValue]);

			iCurrentValue++;
		}

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)szNewValues, iCard - 1);		

		for (int iValue = 0; iValue < iCard - 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete [] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		CMFCPropertyGridProperty * pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		CMFCPropertyGridProperty * pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_CHAR_DATATYPE:
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&pdValue, &iCard);		

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		vector<double> vecValues;
		vecValues.assign(pdValue, pdValue + iCard);

		vecValues.erase(vecValues.begin() + pData->GetCard());

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());		

		/*
		* Update the values
		*/
		CMFCPropertyGridProperty * pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		CMFCPropertyGridProperty * pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		int64_t * piValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&piValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		vector<int64_t> vecValues;
		vecValues.assign(piValue, piValue + iCard);

		vecValues.erase(vecValues.begin() + pData->GetCard());

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());		

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		CMFCPropertyGridProperty * pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		CMFCPropertyGridProperty * pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CString CRDFInstanceProperty::FormatProperty()
{
	if (m_varValue.vt != VT_I8)
	{
		return __super::FormatProperty();
	}

	CString strValue;
	strValue.Format(L"%lld", m_varValue.llVal);

	return strValue;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ BOOL CRDFInstanceProperty::TextToVar(const CString & strText)
{
	/*
	* Support for int64_t
	*/
	if (m_varValue.vt != VT_I8)
	{
		return __super::TextToVar(strText);
	}

	m_varValue.llVal = _ttoi64(strText);

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CWnd * CRDFInstanceProperty::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	/*
	* Support for int64_t
	*/
	bool bInt64 = false;
	if (m_varValue.vt == VT_I8)
	{
		// Cheat the base class
		m_varValue.vt = VT_I4;
		bInt64 = true;
	}

	CWnd * pWnd = __super::CreateInPlaceEdit(rectEdit, bDefaultFormat);

	if (bInt64)
	{
		// Restore type
		m_varValue.vt = VT_I8;
	}

	return pWnd;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFInstanceProperty::EnableSpinControlInt64()
{
	ASSERT(m_varValue.vt == VT_I8);

	// Cheat the base class
	m_varValue.vt = VT_I4;

	EnableSpinControl(TRUE, 0, INT_MAX);

	m_varValue.vt = VT_I8;
}

// ------------------------------------------------------------------------------------------------
CRDFInstanceObjectProperty::CRDFInstanceObjectProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
	, m_mapValues()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CRDFInstanceObjectProperty::~CRDFInstanceObjectProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
void CRDFInstanceObjectProperty::AddValue(const wstring & strValue, int64_t iInstance)
{
	ASSERT(m_mapValues.find(strValue) == m_mapValues.end());

	m_mapValues[strValue] = iInstance;
}

// --------------------------------------------------------------------------------------------
// Gets an instance by value
int64_t CRDFInstanceObjectProperty::GetInstance(const wstring & strValue) const
{
	map<wstring, int64_t>::const_iterator itValues = m_mapValues.find(strValue);
	ASSERT(itValues != m_mapValues.end());

	return itValues->second;
}

// ------------------------------------------------------------------------------------------------
CRDFColorSelectorProperty::CRDFColorSelectorProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridColorProperty(strName, color, pPalette, szDescription, dwData)
{

}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CRDFColorSelectorProperty::~CRDFColorSelectorProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
CAddRDFInstanceProperty::CAddRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CAddRDFInstanceProperty::~CAddRDFInstanceProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

// ------------------------------------------------------------------------------------------------
void CAddRDFInstanceProperty::SetModified(BOOL bModified)
{
	m_bIsModified = bModified;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ BOOL CAddRDFInstanceProperty::HasButton() const
{
	ASSERT(GetData() != NULL);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != NULL);

	BOOL bHasButton = FALSE;

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->getType())
	{
	case TYPE_OBJECTTYPE:
	{
		/*
		* Read the card
		*/
		int64_t * piObjectInstances = NULL;
		int64_t iCard = 0;
		GetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), &piObjectInstances, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case TYPE_OBJECTTYPE:
	break;

	case TYPE_BOOL_DATATYPE:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		bool* pbValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void**)&pbValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case TYPE_CHAR_DATATYPE
	break;

	case TYPE_CHAR_DATATYPE:
	{
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&szValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case TYPE_CHAR_DATATYPE
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&pdValue, &iCard);
		
		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		double * piValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&piValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())

	return bHasButton;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CAddRDFInstanceProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT(GetData() != NULL);

	/*
	* Select the property
	*/
	CMFCPropertyGridProperty * pPropertyGroup = GetParent();
	m_pWndList->SetCurSel(pPropertyGroup);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != NULL);

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->getType())
	{
	case TYPE_OBJECTTYPE:
	{
		CEditObjectPropertyDialog dlgEditObjectProperty(pData->GetController(), pData->GetInstance(), pData->GetProperty(), ::AfxGetMainWnd());
		if (dlgEditObjectProperty.DoModal() != IDOK)
		{
			return;
		}

		/*
		* Read the original values
		*/
		int64_t * piInstances = NULL;
		int64_t iCard = 0;
		GetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), &piInstances, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));

		/*
		* Add a value
		*/
		vector<int64_t> vecValues;
		if (iCard > 0)
		{
			vecValues.assign(piInstances, piInstances + iCard);
		}

		switch (dlgEditObjectProperty.m_iMode)
		{
		case 0: // Existing instance
		{
			ASSERT(dlgEditObjectProperty.m_pExisitngRDFInstance != NULL);

			vecValues.push_back(dlgEditObjectProperty.m_pExisitngRDFInstance->getInstance());
		}
		break;

		case 1: // New instance
		{
			ASSERT(dlgEditObjectProperty.m_iNewInstanceRDFClass != NULL);

			CRDFInstance * pNewRDFInstance = pData->GetController()->CreateNewInstance((CPropertiesWnd *)m_pWndList->GetParent(), dlgEditObjectProperty.m_iNewInstanceRDFClass);
			ASSERT(pNewRDFInstance != NULL);

			vecValues.push_back(pNewRDFInstance->getInstance());
		}
		break;

		default:
		{
			ASSERT(false);
		}
		break;
		} // switch (dlgEditObjectProperty.m_iMode)

		SetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), vecValues.data(), vecValues.size());		

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update UI
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTIES, 0, 0);
	} // case TYPE_OBJECTTYPE:
	break;

	case TYPE_BOOL_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		bool* pbValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void**)&pbValue, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));

		/*
		* Add a value
		*/
		vector<bool> vecValues;
		if (iCard > 0)
		{
			vecValues.assign(pbValue, pbValue + iCard);
		}

		vecValues.push_back(false);

		bool* pbNewValues = new bool[vecValues.size()];
		for (size_t iValue = 0; iValue < vecValues.size(); iValue++)
		{
			pbNewValues[iValue] = vecValues[iValue];
		}

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void**)pbNewValues, vecValues.size());

		delete[] pbNewValues;
		pbNewValues = NULL;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetInstance()->getInstance(), (void **)&szValue, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));
		ASSERT(iMaxCard == 1);

		char ** szNewValues = (char **)new size_t[iCard + 1];
		if (iCard > 0)
		{
			for (int iValue = 0; iValue < iCard; iValue++)
			{
				szNewValues[iValue] = new char[strlen(szValue[iValue]) + 1];
				strcpy(szNewValues[iValue], szValue[iValue]);
			}
		}

		szNewValues[iCard] = new char[1];
		szNewValues[iCard][0] = '\0';

		/*
		* Add a value
		*/
		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)szNewValues, iCard + 1);

		for (int iValue = 0; iValue < iCard + 1; iValue++)
		{
			delete [] szNewValues[iValue];
		}
		delete [] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_CHAR_DATATYPE
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&pdValue, &iCard);		

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));

		/*
		* Add a value
		*/
		vector<double> vecValues;
		if (iCard > 0)
		{
			vecValues.assign(pdValue, pdValue + iCard);
		}

		vecValues.push_back(0.);

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		int64_t * piValue = NULL;
		GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&piValue, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));

		/*
		* Add a value
		*/
		vector<int64_t> vecValues;
		if (iCard > 0)
		{
			vecValues.assign(piValue, piValue + iCard);
		}

		vecValues.push_back(0);

		SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != NULL);
		pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnModelChanged()
{
	if (GetController()->IsTestMode())
	{
		return;
	}

	m_wndObjectCombo.SetCurSel(0 /*Application*/);

	LoadApplicationProperties();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnShowMetaInformation()
{
	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadMetaInformation();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnInstanceSelected(CRDFView * /*pSender*/)
{
	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadInstanceProperties();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnInstancePropertySelected()
{
	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadInstanceProperties();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * /*pInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	if (m_wndObjectCombo.GetCurSel() == 1)
	{
		LoadInstanceProperties();
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CPropertiesWnd::OnInstanceDeleted(CRDFView * pSender, int64_t /*iInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	if (m_wndObjectCombo.GetCurSel() == 1)
	{
		LoadInstanceProperties();
	}
}

// ------------------------------------------------------------------------------------------------
/*afx_msg*/ LRESULT CPropertiesWnd::OnPropertyChanged(__in WPARAM /*wparam*/, __in LPARAM lparam)
{
	ASSERT(GetController() != NULL);

	/*
	* Application
	*/
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		COpenGLRDFView * pOpenGLView = GetController()->GetView<COpenGLRDFView>();
		ASSERT(pOpenGLView != NULL);

		CApplicationProperty * pApplicationProperty = dynamic_cast<CApplicationProperty *>((CMFCPropertyGridProperty *)lparam);
		if (pApplicationProperty != NULL)
		{
			CString strValue = pApplicationProperty->GetValue();

			CApplicationPropertyData * pData = (CApplicationPropertyData *)pApplicationProperty->GetData();
			ASSERT(pData != NULL);

			if ((pData->GetType() == ptLightPosition) || ((pData->GetType() == ptLightPositionItem)))
			{
				CMFCPropertyGridProperty * pPositionProperty = pData->GetType() == ptLightPosition ? pApplicationProperty : pApplicationProperty->GetParent();
				ASSERT(pPositionProperty->GetSubItemsCount() == 4);

				CLightPropertyData * pLightData = (CLightPropertyData *)pApplicationProperty->GetData();
				ASSERT(pLightData != NULL);

				/*
				* X
				*/
				CMFCPropertyGridProperty * pX = pPositionProperty->GetSubItem(0);

				/*
				* Y
				*/
				CMFCPropertyGridProperty * pY = pPositionProperty->GetSubItem(1);

				/*
				* Z
				*/
				CMFCPropertyGridProperty * pZ = pPositionProperty->GetSubItem(2);

				/*
				* W
				*/
				CMFCPropertyGridProperty * pW = pPositionProperty->GetSubItem(3);

				/*
				* Update the light
				*/
				const vector<COpenGLLight> & vecOGLLights = pOpenGLView->GetOGLLights();

				COpenGLLight newLight(vecOGLLights[pLightData->GetLightIndex()]);
				newLight.setPosition(
					(float)_wtof((LPCTSTR)(CString)pX->GetValue()), 
					(float)_wtof((LPCTSTR)(CString)pY->GetValue()), 
					(float)_wtof((LPCTSTR)(CString)pZ->GetValue()), 
					(float)_wtof((LPCTSTR)(CString)pW->GetValue()));

				pOpenGLView->SetOGLLight(pLightData->GetLightIndex(), newLight);
			} // if ((pData->GetType() == ptLightPosition) || ...
			else
			{
				switch (pData->GetType())
				{
				case ptShowFaces:
				{
					pOpenGLView->ShowFaces(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptCullFaces:
				{
					pOpenGLView->SetCullFacesMode(strValue);
				}
				break;

				case ptShowFacesWireframes:
				{
					pOpenGLView->ShowFacesPolygons(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptShowConceptualFacesWireframes:
				{
					pOpenGLView->ShowConceptualFacesPolygons(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptShowLines:
				{
					pOpenGLView->ShowLines(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptLineWidth:
				{
					int iValue = _wtoi((LPCTSTR)strValue);

					pOpenGLView->SetLineWidth((GLfloat)iValue);
				}
				break;

				case ptShowPoints:
				{
					pOpenGLView->ShowPoints(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptPointSize:
				{
					int iValue = _wtoi((LPCTSTR)strValue);

					pOpenGLView->SetPointSize((GLfloat)iValue);
				}
				break;

				case ptShowNormalVectors:
				{
					pOpenGLView->ShowNormalVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptShowTangenVectors:
				{
					pOpenGLView->ShowTangentVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptShowBiNormalVectors:
				{
					pOpenGLView->ShowBiNormalVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptScaleVectors:
				{
					pOpenGLView->ScaleVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptShowBoundingBoxes:
				{
					pOpenGLView->ShowBoundingBoxes(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				case ptLightModelLocalViewer:
				{
					pOpenGLView->SetLightModelLocalViewer(strValue == TRUE_VALUE_PROPERTY ? true : false);
				}
				break;

				case ptLightModel2Sided:
				{
					pOpenGLView->SetLightModel2Sided(strValue == TRUE_VALUE_PROPERTY ? true : false);
				}
				break;

				case ptLightIsEnabled:
				{
					CLightPropertyData * pLightData = (CLightPropertyData *)pApplicationProperty->GetData();

					/*
					* Update the light
					*/
					const vector<COpenGLLight> & vecOGLLights = pOpenGLView->GetOGLLights();

					COpenGLLight newLight(vecOGLLights[pLightData->GetLightIndex()]);
					newLight.enable(strValue == TRUE_VALUE_PROPERTY ? true : false);

					pOpenGLView->SetOGLLight(pLightData->GetLightIndex(), newLight);
				}
				break;

				case ptVisibleValuesCountLimit:
				{
					int iValue = _wtoi((LPCTSTR)strValue);

					GetController()->SetVisibleValuesCountLimit(iValue);
				}
				break;

				case ptScalelAndCenter:
				{
					GetController()->SetScaleAndCenter(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);
				}
				break;

				default:
					ASSERT(false);
					break;
				} // switch (pData->GetType())
			} // else if ((pData->GetType() == ptLightPosition) || ...			

			return 0;
		} // if (pApplicationProperty != NULL)

		CColorApplicationProperty * pColorApplicationProperty = dynamic_cast<CColorApplicationProperty *>((CMFCPropertyGridProperty *)lparam);
		if (pColorApplicationProperty != NULL)
		{
			float fR = GetRValue(pColorApplicationProperty->GetColor()) / 255.f;
			float fG = GetGValue(pColorApplicationProperty->GetColor()) / 255.f;
			float fB = GetBValue(pColorApplicationProperty->GetColor()) / 255.f;			

			CLightPropertyData * pData = (CLightPropertyData *)pColorApplicationProperty->GetData();
			ASSERT(pData != NULL);

			const vector<COpenGLLight> & vecOGLLights = pOpenGLView->GetOGLLights();

			switch (pData->GetType())
			{
			case ptLightModelAmbient:
			{
				pOpenGLView->SetLightModelAmbient(fR, fG, fB);
			}
			break;

			case ptAmbientLight:
			{
				COpenGLLight newLight(vecOGLLights[pData->GetLightIndex()]);
				newLight.setAmbient(fR, fG, fB);

				pOpenGLView->SetOGLLight(pData->GetLightIndex(), newLight);
			}
			break;

			case ptDiffuseLight:
			{
				COpenGLLight newLight(vecOGLLights[pData->GetLightIndex()]);
				newLight.setDiffuse(fR, fG, fB);

				pOpenGLView->SetOGLLight(pData->GetLightIndex(), newLight);
			}
			break;

			case ptSpecularLight:
			{
				COpenGLLight newLight(vecOGLLights[pData->GetLightIndex()]);
				newLight.setSpecular(fR, fG, fB);

				pOpenGLView->SetOGLLight(pData->GetLightIndex(), newLight);
			}
			break;

			default:
			{
				ASSERT(false); // unexpected!
			}
			break;
			} // switch (pData->GetType())

			return 0;
		} // if (pColorApplicationProperty != NULL)

		ASSERT(false); // unexpected!
	} // if (m_wndObjectCombo.GetCurSel() == 0)

	/*
	* Properties
	*/
	if (m_wndObjectCombo.GetCurSel() == 1)
	{
		/*
		* Object properties
		*/
		CRDFInstanceObjectProperty * pObjectProperty = dynamic_cast<CRDFInstanceObjectProperty *>((CMFCPropertyGridProperty *)lparam);
		if (pObjectProperty != NULL)
		{
			CString strValue = pObjectProperty->GetValue();

			CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)pObjectProperty->GetData();
			ASSERT(pData != NULL);

			int64_t * piInstances = NULL;
			int64_t iCard = 0;
			GetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), &piInstances, &iCard);

			ASSERT(iCard > 0);

			vector<int64_t> vecValues;
			vecValues.assign(piInstances, piInstances + iCard);

			if (strValue == REMOVE_OBJECT_PROPERTY_COMMAND)
			{
				/*
				* Remove the value
				*/
				int64_t	iMinCard = 0;
				int64_t iMaxCard = 0;
				pData->GetProperty()->GetRestrictions(pData->GetInstance()->getInstance(), iMinCard, iMaxCard);				

				ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

				/*
				* Remove a value
				*/
				vecValues.erase(vecValues.begin() + pData->GetCard());

				SetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), vecValues.data(), vecValues.size());

				/*
				* Notify
				*/
				ASSERT(pData->GetController() != NULL);
				pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

				/*
				* Delete this item
				*/
				CMFCPropertyGridProperty * pProperty = pObjectProperty->GetParent();
				ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

				CMFCPropertyGridProperty * pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
				PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
			} // if (strValue == REMOVE_OBJECT_PROPERTY_COMMAND)
			else
			{
				if (strValue == SELECT_OBJECT_PROPERTY_COMMAND)
				{
					CObjectRDFProperty* pObjectRDFProperty = dynamic_cast<CObjectRDFProperty*>(pData->GetProperty());
					ASSERT(pObjectRDFProperty != NULL);

					ASSERT(pData->GetController() != NULL);
					CSelectInstanceDialog dlgSelectInstanceDialog(GetController(), pData->GetInstance(), pObjectRDFProperty, pData->GetCard());
					if (dlgSelectInstanceDialog.DoModal() == IDOK)
					{
						ASSERT(dlgSelectInstanceDialog.m_iInstance != -1);						

						/*
						* Update the value
						*/						
						vecValues[pData->GetCard()] = dlgSelectInstanceDialog.m_iInstance;

						SetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), vecValues.data(), vecValues.size());

						/*
						* Notify
						*/
						ASSERT(pData->GetController() != NULL);
						GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

						/*
						* Value
						*/
						ASSERT(!dlgSelectInstanceDialog.m_strInstanceUniqueName.IsEmpty());

						pObjectProperty->SetValue((_variant_t)dlgSelectInstanceDialog.m_strInstanceUniqueName);
					}
					else
					{
						/*
						* Value
						*/
						ASSERT(!dlgSelectInstanceDialog.m_strOldInstanceUniqueName.IsEmpty());

						pObjectProperty->SetValue((_variant_t)dlgSelectInstanceDialog.m_strOldInstanceUniqueName);
					}
				}
				else
				{
					/*
					* Update the value
					*/
					int64_t iInstance = pObjectProperty->GetInstance((LPCTSTR)strValue);
					vecValues[pData->GetCard()] = iInstance;

					SetObjectProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), vecValues.data(), vecValues.size());

					/*
					* Notify
					*/
					ASSERT(pData->GetController() != NULL);
					GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
				}				
			} // else if (strValue == REMOVE_OBJECT_PROPERTY_COMMAND)

			return 0;
		} // if (pObjectProperty != NULL)

		/*
		* ColorComponent properties
		*/
		CRDFColorSelectorProperty * pColorSelectorProperty = dynamic_cast<CRDFColorSelectorProperty *>((CMFCPropertyGridProperty *)lparam);
		if (pColorSelectorProperty != NULL)
		{	
			CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)pColorSelectorProperty->GetData();
			ASSERT(pData != NULL);

			CRDFModel * pModel = GetController()->GetModel();
			ASSERT(pModel != NULL);

			ASSERT(pData->GetInstance()->getClassInstance() == GetClassByName(pModel->GetModel(), "ColorComponent"));			

			const map<int64_t, CRDFProperty *> & mapRDFProperties = pModel->GetRDFProperties();

			/*
			* R
			*/
			int64_t iRProperty = GetPropertyByName(pModel->GetModel(), "R");

			double dR = GetRValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getInstance(), iRProperty, &dR, 1);

			map<int64_t, CRDFProperty *>::const_iterator itRDFProperty = mapRDFProperties.find(iRProperty);
			ASSERT(itRDFProperty != mapRDFProperties.end());

			/*
			* Notify
			*/
			GetController()->OnInstancePropertyEdited(pData->GetInstance(), itRDFProperty->second);

			/*
			* G
			*/
			int64_t iGProperty = GetPropertyByName(pModel->GetModel(), "G");

			double dG = GetGValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getInstance(), iGProperty, &dG, 1);

			itRDFProperty = mapRDFProperties.find(iGProperty);
			ASSERT(itRDFProperty != mapRDFProperties.end());

			/*
			* Notify
			*/
			GetController()->OnInstancePropertyEdited(pData->GetInstance(), itRDFProperty->second);

			/*
			* B
			*/
			int64_t iBProperty = GetPropertyByName(pModel->GetModel(), "B");

			double dB = GetBValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getInstance(), iBProperty, &dB, 1);

			itRDFProperty = mapRDFProperties.find(iBProperty);
			ASSERT(itRDFProperty != mapRDFProperties.end());

			/*
			* Notify
			*/
			GetController()->OnInstancePropertyEdited(pData->GetInstance(), itRDFProperty->second);

			/*
			* Update UI
			*/
			PostMessage(WM_LOAD_INSTANCE_PROPERTIES, 0, 0);
		} // if (pColorSelectorProperty != NULL)

		/*
		* Data properties
		*/
		CRDFInstanceProperty * pProperty = dynamic_cast<CRDFInstanceProperty *>((CMFCPropertyGridProperty *)lparam);
		if (pProperty == NULL)
		{
			return 0;
		}

		CString strValue = pProperty->GetValue();

		CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)pProperty->GetData();
		ASSERT(pData != NULL);

		switch(pData->GetProperty()->getType())
		{
		case TYPE_BOOL_DATATYPE:
		{
			/*
			* Read the original values
			*/
			int64_t iCard = 0;
			bool* pbValue = NULL;
			GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void**)&pbValue, &iCard);

			ASSERT(iCard > 0);

			vector<bool> vecValues;
			vecValues.assign(pbValue, pbValue + iCard);

			/*
			* Update the modified value
			*/

			bool bValue = strValue != L"0";
			vecValues[pData->GetCard()] = bValue;

			bool* pbNewValues = new bool[vecValues.size()];
			for (size_t iValue = 0; iValue < vecValues.size(); iValue++)
			{
				pbNewValues[iValue] = vecValues[iValue];
			}

			SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void**)pbNewValues, vecValues.size());

			delete[] pbNewValues;
			pbNewValues = NULL;

			ASSERT(GetController() != NULL);

			GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
		} // case TYPE_BOOL_DATATYPE:
		break;

		case TYPE_CHAR_DATATYPE:
		{
			/*
			* Read the original values
			*/
			int64_t iCard = 0;
			char ** szValue = NULL;
			GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&szValue, &iCard);

			ASSERT(iCard > 0);

			char ** szNewValues = (char **)new size_t[iCard];
			for (int iValue = 0; iValue < iCard; iValue++)
			{
				szNewValues[iValue] = new char[strlen(szValue[iValue]) + 1];
				strcpy(szNewValues[iValue], szValue[iValue]);
			}

			/*
			* Update the modified value
			*/
			delete[] szNewValues[pData->GetCard()];
			szNewValues[pData->GetCard()] = new char[strlen(CW2A(strValue)) + 1];
			strcpy(szNewValues[pData->GetCard()], CW2A(strValue));

			SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)szNewValues, iCard);

			for (int iValue = 0; iValue < iCard; iValue++)
			{
				delete [] szNewValues[iValue];
			}
			delete [] szNewValues;

			ASSERT(GetController() != NULL);

			GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
		} // case TYPE_CHAR_DATATYPE:
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			/*
			* Read the original values
			*/
			int64_t iCard = 0;
			double * pdValue = NULL;
			GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&pdValue, &iCard);
			
			ASSERT(iCard > 0);

			vector<double> vecValues;
			vecValues.assign(pdValue, pdValue + iCard);

			/*
			* Update the modified value
			*/

			double dValue = _wtof((LPCTSTR)strValue);
			vecValues[pData->GetCard()] = dValue;

			SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());

			ASSERT(GetController() != NULL);

			GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
		} // case TYPE_DOUBLE_DATATYPE:
		break;

		case TYPE_INT_DATATYPE:
		{
			/*
			* Read the original values
			*/
			int64_t iCard = 0;
			int64_t * piValue = NULL;
			GetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)&piValue, &iCard);

			ASSERT(iCard > 0);

			vector<int64_t> vecValues;
			vecValues.assign(piValue, piValue + iCard);

			/*
			* Update the modified value
			*/
			int64_t iValue = _wtoi64((LPCTSTR)strValue);
			vecValues[pData->GetCard()] = iValue;

			SetDatatypeProperty(pData->GetInstance()->getInstance(), pData->GetProperty()->getInstance(), (void **)vecValues.data(), vecValues.size());

			ASSERT(GetController() != NULL);

			GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
		} // case TYPE_INT_DATATYPE:
		break;

		default:
		{
			ASSERT(false); // unknown type
		}
		break;
		} // switch(pData->GetProperty()->getType())

		return 0;
	} // if (m_wndObjectCombo.GetCurSel() == 1)

	ASSERT(false); // unknown property
	
	return 0;
}

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_CBN_SELENDOK(ID_COMBO_PROPERTIES_VIEW, OnViewModeChanged)
	ON_MESSAGE(WM_LOAD_INSTANCE_PROPERTY_VALUES, OnLoadInstancePropertyValues)
	ON_MESSAGE(WM_LOAD_INSTANCE_PROPERTIES, OnLoadInstanceProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(GetController() != NULL);
	GetController()->RegisterView(this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, ID_COMBO_PROPERTIES_VIEW))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Application"));
	m_wndObjectCombo.AddString(_T("Properties"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	OnViewModeChanged();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::LoadApplicationProperties()
{
	ASSERT(GetController() != NULL);

	COpenGLRDFView * pOpenGLView = GetController()->GetView<COpenGLRDFView>();
	ASSERT(pOpenGLView != NULL);

	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty * pViewGroup = new CMFCPropertyGridProperty(_T("View"));	

	/*
	* Faces
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Faces"), pOpenGLView->AreFacesShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Faces"), (DWORD_PTR)new CApplicationPropertyData(ptShowFaces));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Cull Faces
	*/
	{
		CString strCullFacesMode = pOpenGLView->GetCullFacesMode();

		CApplicationProperty* pProperty = new CApplicationProperty(
			_T("Cull Faces"), 
			strCullFacesMode == CULL_FACES_FRONT ? CULL_FACES_FRONT : strCullFacesMode == CULL_FACES_BACK ? CULL_FACES_BACK : CULL_FACES_NONE,
			_T("Cull Faces"), (DWORD_PTR)new CApplicationPropertyData(ptCullFaces));
		pProperty->AddOption(CULL_FACES_NONE);
		pProperty->AddOption(CULL_FACES_FRONT);
		pProperty->AddOption(CULL_FACES_BACK);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Faces wireframes
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Faces wireframes"), pOpenGLView->AreFacesPolygonsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Faces wireframes"), (DWORD_PTR)new CApplicationPropertyData(ptShowFacesWireframes));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Conceptual faces wireframes
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Conceptual faces wireframes"), pOpenGLView->AreConceptualFacesPolygonsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Conceptual faces wireframes"), (DWORD_PTR)new CApplicationPropertyData(ptShowConceptualFacesWireframes));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Lines
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Lines"), pOpenGLView->AreLinesShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Lines"), (DWORD_PTR)new CApplicationPropertyData(ptShowLines));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Line width
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Line thickness"), (_variant_t)(int)pOpenGLView->GetLineWidth(), _T("Line thickness"), (DWORD_PTR)new CApplicationPropertyData(ptLineWidth));
		pProperty->EnableSpinControl(TRUE, 1, 1000);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Points
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Points"), pOpenGLView->ArePointsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Points"), (DWORD_PTR)new CApplicationPropertyData(ptShowPoints));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Point size
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Point thickness"), (_variant_t)(int)pOpenGLView->GetPointSize(), _T("Point thickness"), (DWORD_PTR)new CApplicationPropertyData(ptPointSize));
		pProperty->EnableSpinControl(TRUE, 1, 1000);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Vectors
	*/
	{
		CMFCPropertyGridProperty * pVectors = new CMFCPropertyGridProperty(_T("Vectors"));
		pViewGroup->AddSubItem(pVectors);

		/*
		* Normal vectors
		*/
		{
			CApplicationProperty * pProperty = new CApplicationProperty(_T("Normal vectors"), pOpenGLView->AreNormalVectorsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Normal vectors"), (DWORD_PTR)new CApplicationPropertyData(ptShowNormalVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		/*
		* Tangent vectors
		*/
		{
			CApplicationProperty * pProperty = new CApplicationProperty(_T("Tangent vectors"), pOpenGLView->AreTangentVectorsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Tangent vectors"), (DWORD_PTR)new CApplicationPropertyData(ptShowTangenVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		/*
		* Bi-normal vectors
		*/
		{
			CApplicationProperty * pProperty = new CApplicationProperty(_T("Bi-normal vectors"), pOpenGLView->AreBiNormalVectorsShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Bi-normal vectors"), (DWORD_PTR)new CApplicationPropertyData(ptShowBiNormalVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		/*
		* Scale
		*/
		{
			CApplicationProperty * pProperty = new CApplicationProperty(_T("Scale"), pOpenGLView->AreVectorsScaled() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Scale"), (DWORD_PTR)new CApplicationPropertyData(ptScaleVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}
	}	

	/*
	* Bounding boxes
	*/
	{
		CApplicationProperty * pProperty = new CApplicationProperty(_T("Bounding boxes"), pOpenGLView->AreBoundingBoxesShown() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Bounding boxes"), 
			(DWORD_PTR)new CApplicationPropertyData(ptShowBoundingBoxes));
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	/*
	* Lights
	*/
	{
		CMFCPropertyGridProperty * pLights = new CMFCPropertyGridProperty(_T("Lights"));
		pViewGroup->AddSubItem(pLights);

		/*
		* Light model
		*/
		{
			CMFCPropertyGridProperty * pLightModel = new CMFCPropertyGridProperty(_T("Light model"));
			pLights->AddSubItem(pLightModel);

			/*
			* Ambient
			*/
			CColorApplicationProperty * pProperty = new CColorApplicationProperty(_T("Ambient"), 
				RGB((BYTE)(pOpenGLView->GetLightModelAmbient()[0] * 255.), (BYTE)(pOpenGLView->GetLightModelAmbient()[1] * 255.), (BYTE)(pOpenGLView->GetLightModelAmbient()[2] * 255.)),
				NULL, _T("Light model - ambient"),
				(DWORD_PTR)new CApplicationPropertyData(ptLightModelAmbient));
			pProperty->EnableOtherButton(_T("Other..."));
			pProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

			pLightModel->AddSubItem(pProperty);

			/*
			* Local viewer
			*/
			{
				CApplicationProperty * pApplicationProperty = new CApplicationProperty(_T("Local viewer"), pOpenGLView->GetLightModelLocalViewer() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
					_T("Light model - local viewer"), (DWORD_PTR)new CApplicationPropertyData(ptLightModelLocalViewer));
				pApplicationProperty->AddOption(TRUE_VALUE_PROPERTY);
				pApplicationProperty->AddOption(FALSE_VALUE_PROPERTY);
				pApplicationProperty->AllowEdit(FALSE);

				pLightModel->AddSubItem(pApplicationProperty);
			}

			/*
			* Two-sided lighting
			*/
			{
				CApplicationProperty * pApplicationProperty = new CApplicationProperty(_T("Two-sided lighting"), pOpenGLView->GetLightModel2Sided() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Light model - two-sided lighting"),
					(DWORD_PTR)new CApplicationPropertyData(ptLightModel2Sided));
				pApplicationProperty->AddOption(TRUE_VALUE_PROPERTY);
				pApplicationProperty->AddOption(FALSE_VALUE_PROPERTY);
				pApplicationProperty->AllowEdit(FALSE);

				pLightModel->AddSubItem(pApplicationProperty);
			}			
		}

		/*
		* Lights
		*/
		const vector<COpenGLLight> & vecOGLLights = pOpenGLView->GetOGLLights();
		for (size_t iLight = 0; iLight < vecOGLLights.size(); iLight++)
		{
			/*
			* Light 0-7
			*/
			{
				wchar_t szLightName[100];
				swprintf(szLightName, 100, L"Light %d", (int)iLight);

				CMFCPropertyGridProperty * pLight = new CMFCPropertyGridProperty(szLightName);
				pLights->AddSubItem(pLight);

				/*
				* Enabled
				*/
				{
					CApplicationProperty * pProperty = new CApplicationProperty(_T("Enabled"), vecOGLLights[iLight].isEnabled() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
						_T("Enabled"), (DWORD_PTR)new CLightPropertyData(ptLightIsEnabled, (int)iLight));
					pProperty->AddOption(TRUE_VALUE_PROPERTY);
					pProperty->AddOption(FALSE_VALUE_PROPERTY);
					pProperty->AllowEdit(FALSE);

					pLight->AddSubItem(pProperty);
				}

				/*
				* Ambient
				*/
				{
					CColorApplicationProperty * pProperty = new CColorApplicationProperty(_T("Ambient"), RGB((BYTE)(vecOGLLights[iLight].getAmbient()[0] * 255.), (BYTE)(vecOGLLights[iLight].getAmbient()[1] * 255.), (BYTE)(vecOGLLights[iLight].getAmbient()[2] * 255.)), NULL, _T("Ambient"),
						(DWORD_PTR)new CLightPropertyData(ptAmbientLight, (int)iLight));
					pProperty->EnableOtherButton(_T("Other..."));
					pProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

					pLight->AddSubItem(pProperty);
				}

				/*
				* Diffuse
				*/
				{
					CColorApplicationProperty * pProperty = new CColorApplicationProperty(_T("Diffuse"), RGB((BYTE)(vecOGLLights[iLight].getDiffuse()[0] * 255.), (BYTE)(vecOGLLights[iLight].getDiffuse()[1] * 255.), (BYTE)(vecOGLLights[iLight].getDiffuse()[2] * 255.)), NULL, _T("Diffuse"),
						(DWORD_PTR)new CLightPropertyData(ptDiffuseLight, (int)iLight));
					pProperty->EnableOtherButton(_T("Other..."));
					pProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

					pLight->AddSubItem(pProperty);
				}

				/*
				* Specular
				*/
				{
					CColorApplicationProperty * pProperty = new CColorApplicationProperty(_T("Specular"), RGB((BYTE)(vecOGLLights[iLight].getSpecular()[0] * 255.), (BYTE)(vecOGLLights[iLight].getSpecular()[1] * 255.), (BYTE)(vecOGLLights[iLight].getSpecular()[2] * 255.)), NULL, _T("Specular"),
						(DWORD_PTR)new CLightPropertyData(ptSpecularLight, (int)iLight));
					pProperty->EnableOtherButton(_T("Other..."));
					pProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

					pLight->AddSubItem(pProperty);
				}

				/*
				* Position
				*/
				{
					CApplicationProperty * pPosition = new CApplicationProperty(_T("Position"), (DWORD_PTR)new CLightPropertyData(ptLightPosition, (int)iLight), TRUE);
					pPosition->AllowEdit(FALSE);

					/*
					* X
					*/
					{
						CApplicationProperty * pProperty = new CApplicationProperty(_T("X"), (_variant_t)vecOGLLights[iLight].getPosition()[0],
							_T("X"), (DWORD_PTR)new CLightPropertyData(ptLightPositionItem, (int)iLight));
						pPosition->AddSubItem(pProperty);
					}				

					/*
					* Y
					*/
					{
						CApplicationProperty * pProperty = new CApplicationProperty(_T("Y"), (_variant_t)vecOGLLights[iLight].getPosition()[1],
							_T("Y"), (DWORD_PTR)new CLightPropertyData(ptLightPositionItem, (int)iLight));
						pPosition->AddSubItem(pProperty);
					}

					/*
					* Z
					*/
					{
						CApplicationProperty * pProperty = new CApplicationProperty(_T("Z"), (_variant_t)vecOGLLights[iLight].getPosition()[2],
							_T("Z"), (DWORD_PTR)new CLightPropertyData(ptLightPositionItem, (int)iLight));
						pPosition->AddSubItem(pProperty);
					}

					/*
					* W
					*/
					{
						CApplicationProperty * pProperty = new CApplicationProperty(_T("W"), (_variant_t)vecOGLLights[iLight].getPosition()[3],
							_T("W"), (DWORD_PTR)new CLightPropertyData(ptLightPositionItem, (int)iLight));
						pPosition->AddSubItem(pProperty);
					}

					pLight->AddSubItem(pPosition);
				}
			}
		} // for (size_t iLight = ...
	}

	/*
	* UI
	*/
	{
		CMFCPropertyGridProperty * pUI = new CMFCPropertyGridProperty(_T("UI"));
		pViewGroup->AddSubItem(pUI);

		/*
		* Visible values count limit
		*/
		{
			CApplicationProperty * pProperty = new CApplicationProperty(_T("Visible values count limit"), (_variant_t)GetController()->GetVisibleValuesCountLimit(), _T("Visible values count limit"),
				(DWORD_PTR)new CApplicationPropertyData(ptVisibleValuesCountLimit));
			pProperty->EnableSpinControl(TRUE, 1, INT_MAX);

			pUI->AddSubItem(pProperty);
		}

		/*
		* Scale and Center
		*/
		{
			CApplicationProperty* pProperty = new CApplicationProperty(_T("Scale and Center all Geometry"), GetController()->GetScaleAndCenter() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Scale and Center all Geometry"), (DWORD_PTR)new CApplicationPropertyData(ptScalelAndCenter));
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pUI->AddSubItem(pProperty);
		}
	}	

	m_wndPropList.AddProperty(pViewGroup);	

	/*
	return;

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
	*/
}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::LoadInstanceProperties()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	ASSERT(GetController() != NULL);

	/*
	* One property
	*/
	pair<CRDFInstance *, CRDFProperty *> prSelectedInstanceProperty = GetController()->GetSelectedInstanceProperty();
	if ((prSelectedInstanceProperty.first != NULL) && (prSelectedInstanceProperty.second != NULL))
	{
		LOG_DEBUG("CPropertiesWnd::LoadInstanceProperties() - 1 property BEGIN");

		CRDFInstance * pRDFInstance = prSelectedInstanceProperty.first;
		CRDFProperty * pRDFProperty = prSelectedInstanceProperty.second;

		CMFCPropertyGridProperty * pInstanceGroup = new CMFCPropertyGridProperty(pRDFInstance->getUniqueName());		

		AddInstanceProperty(pInstanceGroup, pRDFInstance, pRDFProperty);

		m_wndPropList.AddProperty(pInstanceGroup);

		LOG_DEBUG("CPropertiesWnd::LoadInstanceProperties() - 1 property END");

		return;		
	} // if ((prSelectedInstanceProperty.first != NULL) && ...

	/*
	* All properties
	*/
	CRDFInstance * pRDFInstance = GetController()->GetSelectedInstance();
	if (pRDFInstance != NULL)
	{
		CRDFModel * pModel = GetController()->GetModel();
		ASSERT(pModel != NULL);

		const map<int64_t, CRDFProperty *> & mapRDFProperties = pModel->GetRDFProperties();

		LOG_DEBUG("CPropertiesWnd::LoadInstanceProperties() - 1 instance BEGIN");

		CMFCPropertyGridProperty * pInstanceGroup = new CMFCPropertyGridProperty(pRDFInstance->getUniqueName());

		/*
		* ColorComponent
		*/
		if (pRDFInstance->getClassInstance() == GetClassByName(pModel->GetModel(), "ColorComponent"))
		{
			/*
			* R
			*/
			int64_t iRProperty = GetPropertyByName(pModel->GetModel(), "R");

			int64_t iCard = 0;
			double * pdRValue = NULL;
			GetDatatypeProperty(pRDFInstance->getInstance(), iRProperty, (void **)&pdRValue, &iCard);


			/*
			* G
			*/
			int64_t iGProperty = GetPropertyByName(pModel->GetModel(), "G");

			iCard = 0;
			double * pdGValue = NULL;
			GetDatatypeProperty(pRDFInstance->getInstance(), iGProperty, (void **)&pdGValue, &iCard);

			/*
			* B
			*/
			int64_t iBProperty = GetPropertyByName(pModel->GetModel(), "B");

			iCard = 0;
			double * pdBValue = NULL;
			GetDatatypeProperty(pRDFInstance->getInstance(), iBProperty, (void **)&pdBValue, &iCard);

			CRDFColorSelectorProperty * pColorSelectorProperty = new CRDFColorSelectorProperty(_T("Color"), RGB((BYTE)(*pdRValue * 255.), (BYTE)(*pdGValue * 255.), (BYTE)(*pdBValue * 255.)), NULL, _T("Color"),
				(DWORD_PTR)new CRDFInstanceData(GetController(), pRDFInstance));
			pColorSelectorProperty->EnableOtherButton(_T("Other..."));
			pColorSelectorProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
			pInstanceGroup->AddSubItem(pColorSelectorProperty);
		} // if (pRDFInstance->getClassInstance() == ...

		int64_t iPropertyInstance = GetInstancePropertyByIterator(pRDFInstance->getInstance(), 0);
		while (iPropertyInstance != NULL)
		{
			map<int64_t, CRDFProperty *>::const_iterator itRDFProperty = mapRDFProperties.find(iPropertyInstance);
			ASSERT(itRDFProperty != mapRDFProperties.end());

			CRDFProperty * pRDFProperty = itRDFProperty->second;

			AddInstanceProperty(pInstanceGroup, pRDFInstance, pRDFProperty);

			iPropertyInstance = GetInstancePropertyByIterator(pRDFInstance->getInstance(), iPropertyInstance);
		}

		m_wndPropList.AddProperty(pInstanceGroup);

		LOG_DEBUG("CPropertiesWnd::LoadInstanceProperties() - 1 instance END");

		return;
	} // if (pRDFInstance != NULL)
}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::AddInstanceProperty(CMFCPropertyGridProperty * pInstanceGroup, CRDFInstance * pRDFInstance, CRDFProperty * pRDFProperty)
{
	CMFCPropertyGridProperty * pPropertyGroup = new CMFCPropertyGridProperty(pRDFProperty->getName());
	pInstanceGroup->AddSubItem(pPropertyGroup);

	/*
	* range
	*/
	if (pRDFProperty->getType() == TYPE_OBJECTTYPE)
	{
		ASSERT(GetController() != NULL);

		CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pRDFProperty);
		ASSERT(pObjectRDFProperty != NULL);

		wstring strRange;

		const vector<int64_t> & vecRestrictions = pObjectRDFProperty->getRestrictions();
		ASSERT(!vecRestrictions.empty());

		for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
		{
			char * szClassName = NULL;
			GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

			if (!strRange.empty())
			{
				strRange += L"; ";
			}

			strRange += CA2W(szClassName);
		}

		CMFCPropertyGridProperty * pRange = new CMFCPropertyGridProperty(L"rdfs:range", (_variant_t)strRange.c_str(), pRDFProperty->getName());
		pRange->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pRange);
	} // if (pRDFProperty->getType() == TYPE_OBJECTTYPE)
	else
	{
		CMFCPropertyGridProperty * pRange = new CMFCPropertyGridProperty(L"rdfs:range", (_variant_t)pRDFProperty->getRange().c_str(), pRDFProperty->getName());
		pRange->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pRange);
	}	

	/*
	* cardinality
	*/
	AddInstancePropertyCardinality(pPropertyGroup, pRDFInstance, pRDFProperty);

	/*
	* values
	*/
	AddInstancePropertyValues(pPropertyGroup, pRDFInstance, pRDFProperty);	
}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::AddInstancePropertyCardinality(CMFCPropertyGridProperty * pPropertyGroup, CRDFInstance * pRDFInstance, CRDFProperty * pRDFProperty)
{
	switch (pRDFProperty->getType())
	{
	case TYPE_OBJECTTYPE:
	{
		int64_t * piObjectInstances = NULL;
		int64_t iCard = 0;
		GetObjectProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), &piObjectInstances, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pRDFProperty->getName(),
			(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case TYPE_OBJECTTYPE:
	break;

	case TYPE_BOOL_DATATYPE:
	{
		int64_t iCard = 0;
		bool* pbValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void**)&pbValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

		CAddRDFInstanceProperty* pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pRDFProperty->getName(),
			(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case TYPE_BOOL_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&szValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pRDFProperty->getName(),
			(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case TYPE_CHAR_DATATYPE:
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&pdValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pRDFProperty->getName(),
			(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		int64_t iCard = 0;
		int64_t * piValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&piValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pRDFProperty->getName(),
			(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unknown property
	}
	break;
	} // switch (pRDFProperty->getType())
}

// ------------------------------------------------------------------------------------------------
// Moved in CSelectInstanceDialog
//bool IsUsedRecursively(int64_t RDFInstanceI, int64_t RDFInstanceII)
//{
//	int64_t myInstance = GetInstanceInverseReferencesByIterator(RDFInstanceII, 0);
//	while (myInstance) {
//		if ((myInstance == RDFInstanceI) ||
//			IsUsedRecursively(RDFInstanceI, myInstance)) {
//			return true;
//		}
//		myInstance = GetInstanceInverseReferencesByIterator(RDFInstanceII, myInstance);
//	}
//
//	return false;
//}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::AddInstancePropertyValues(CMFCPropertyGridProperty * pPropertyGroup, CRDFInstance * pRDFInstance, CRDFProperty * pRDFProperty)
{
	switch (pRDFProperty->getType())
	{
	case TYPE_OBJECTTYPE:
	{	
		LOG_DEBUG("CPropertiesWnd::AddInstancePropertyValues() - TYPE_OBJECTTYPE BEGIN");

		int64_t * piInstances = NULL;
		int64_t iCard = 0;
		GetObjectProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), &piInstances, &iCard);

		if (iCard > 0)
		{
			int64_t	iMinCard = 0;
			int64_t iMaxCard = 0;
			pRDFProperty->GetRestrictions(pRDFInstance->getInstance(), iMinCard, iMaxCard);

			if ((iMinCard == -1) && (iMaxCard == -1))
			{
				iMinCard = 0;
			}

			ASSERT(GetController() != NULL);

			CRDFModel * pModel = GetController()->GetModel();
			ASSERT(pModel != NULL);

			const map<int64_t, CRDFInstance *> & mapRFDInstances = pModel->GetRDFInstances();

			/*
			* Compatible instances
			*/
			// Moved in CSelectInstanceDialog
			//CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pRDFProperty);
			//ASSERT(pObjectRDFProperty != NULL);

			// Moved in CSelectInstanceDialog
			//vector<int64_t> vecCompatibleInstances;
			//pModel->GetCompatibleInstances(pRDFInstance, pObjectRDFProperty, vecCompatibleInstances);

			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceObjectProperty * pProperty = NULL;

				if (piInstances[iValue] != 0)
				{
					map<int64_t, CRDFInstance *>::const_iterator itInstanceValue = mapRFDInstances.find(piInstances[iValue]);
					ASSERT(itInstanceValue != mapRFDInstances.end());

					pProperty = new CRDFInstanceObjectProperty(L"value", (_variant_t)itInstanceValue->second->getUniqueName(), pRDFProperty->getName(),
						(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));
				}
				else
				{
					pProperty = new CRDFInstanceObjectProperty(L"value", (_variant_t)EMPTY_INSTANCE, pRDFProperty->getName(),
						(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));
				}

				// Moved in CSelectInstanceDialog
				//LOG_DEBUG("Compatible instances: " << (int)vecCompatibleInstances.size());

				//for (size_t iCompatibleInstance = 0; iCompatibleInstance < vecCompatibleInstances.size(); iCompatibleInstance++)
				//{
				//	map<int64_t, CRDFInstance *>::const_iterator itInstanceValue = mapRFDInstances.find(vecCompatibleInstances[iCompatibleInstance]);
				//	ASSERT(itInstanceValue != mapRFDInstances.end());

				//	wstring strInstanceUniqueName = itInstanceValue->second->getUniqueName();
				//	if ((piInstances[iValue] != vecCompatibleInstances[iCompatibleInstance]) && IsUsedRecursively(pRDFInstance->getInstance(), itInstanceValue->second->getInstance()))
				//	{
				//		strInstanceUniqueName += USED_SUFFIX;
				//	}

				//	pProperty->AddOption(strInstanceUniqueName.c_str());
				//	pProperty->AddValue(strInstanceUniqueName.c_str(), vecCompatibleInstances[iCompatibleInstance]);
				//} // for (size_t iCompatibleInstance = ...

				/*
				* Empty command
				*/
				pProperty->AddOption(EMPTY_INSTANCE);
				pProperty->AddValue(EMPTY_INSTANCE, 0);

				/*
				* Remove command
				*/
				if (iCard > iMinCard)
				{
					pProperty->AddOption(REMOVE_OBJECT_PROPERTY_COMMAND);
				}				

				/*
				* Select
				*/
				pProperty->AddOption(SELECT_OBJECT_PROPERTY_COMMAND);

				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pRDFProperty->getName());
				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);
			}
		} // if (iCard > 0)

		LOG_DEBUG("CPropertiesWnd::AddInstancePropertyValues() - TYPE_OBJECTTYPE END");
	} // case TYPE_OBJECTTYPE:
	break;

	case TYPE_BOOL_DATATYPE:
	{
		int64_t iCard = 0;
		bool* pbValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void**)&pbValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty* pProperty = new CRDFInstanceProperty(L"value", (_variant_t)pbValue[iValue], pRDFProperty->getName(),
					(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));

				pPropertyGroup->AddSubItem(pProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pRDFProperty->getName());
				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_BOOL_DATATYPE:
	break;

	case TYPE_CHAR_DATATYPE:
	{
		int64_t iCard = 0;
		char ** szValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&szValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pProperty = new CRDFInstanceProperty(L"value", (_variant_t)szValue[iValue], pRDFProperty->getName(),
					(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));

				pPropertyGroup->AddSubItem(pProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pRDFProperty->getName());
				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_CHAR_DATATYPE:
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		int64_t iCard = 0;
		double * pdValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&pdValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pProperty = new CRDFInstanceProperty(L"value", (_variant_t)pdValue[iValue], pRDFProperty->getName(),
					(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));

				pPropertyGroup->AddSubItem(pProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pRDFProperty->getName());
				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_DOUBLE_DATATYPE:
	break;

	case TYPE_INT_DATATYPE:
	{
		int64_t iCard = 0;
		int64_t * piValue = NULL;
		GetDatatypeProperty(pRDFInstance->getInstance(), pRDFProperty->getInstance(), (void **)&piValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pProperty = new CRDFInstanceProperty(L"value", (_variant_t)piValue[iValue], pRDFProperty->getName(),
					(DWORD_PTR)new CRDFInstancePropertyData(GetController(), pRDFInstance, pRDFProperty, iValue));
				pProperty->EnableSpinControlInt64();

				pPropertyGroup->AddSubItem(pProperty);

				if ((iValue + 1) >= GetController()->GetVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > GetController()->GetVisibleValuesCountLimit())
			{
				CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pRDFProperty->getName());
				pProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_INT_DATATYPE:
	break;

	default:
	{
		ASSERT(false); // unknown property
	}
	break;
	} // switch (pRDFProperty->getType())
}

// ------------------------------------------------------------------------------------------------
void CPropertiesWnd::LoadMetaInformation()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	ASSERT(GetController() != NULL);

	CRDFInstance * pRDFInstance = GetController()->GetSelectedInstance();
	if (pRDFInstance == NULL)
	{
		return;
	}

	CMFCPropertyGridProperty * pMetaInfoGroup = new CMFCPropertyGridProperty(L"Meta information");
	CMFCPropertyGridProperty * pInstanceGroup = new CMFCPropertyGridProperty(pRDFInstance->getName());
	pMetaInfoGroup->AddSubItem(pInstanceGroup);

	/*
	* Centroid, Volume
	*/
	{
		double arCentroid[3];
		double dVolume = GetCentroid(pRDFInstance->getInstance(), NULL, NULL, arCentroid);

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f, %.6f, %.6f", arCentroid[0], arCentroid[1], arCentroid[2]);

		CMFCPropertyGridProperty * pCentroid = new CMFCPropertyGridProperty(L"Centroid", (_variant_t)szBuffer, L"Centroid");
		pCentroid->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pCentroid);

		swprintf(szBuffer, 100, L"%.6f", dVolume);

		CMFCPropertyGridProperty * pVolume = new CMFCPropertyGridProperty(L"Volume", (_variant_t)szBuffer, L"Volume");
		pVolume->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pVolume);
	}	

	/*
	* Area
	*/
	{
		double dArea = GetArea(pRDFInstance->getInstance(), NULL, NULL);

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f", dArea);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Area", (_variant_t)szBuffer, L"Area");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Perimeter
	*/
	{
		double dPerimeter = GetPerimeter(pRDFInstance->getInstance());

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f", dPerimeter);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Perimeter", (_variant_t)szBuffer, L"Perimeter");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry
	*/
	CMFCPropertyGridProperty * pGeometryGroup = new CMFCPropertyGridProperty(L"Geometry");
	pMetaInfoGroup->AddSubItem(pGeometryGroup);

	/*
	* Geometry - Number of conceptual faces
	*/
	{
		int64_t iConceptualFacesCount = pRDFInstance->getConceptualFacesCount();

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iConceptualFacesCount);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Number of conceptual faces", (_variant_t)szBuffer, L"Number of conceptual faces");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}


	/*
	* Geometry - Number of triangles
	*/
	{
		int64_t iTrianglesCount = 0;
		for (size_t iTriangle = 0; iTriangle < pRDFInstance->getTriangles().size(); iTriangle++)
		{
			iTrianglesCount += pRDFInstance->getTriangles()[iTriangle].second / 3;
		}

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iTrianglesCount);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Number of triangles", (_variant_t)szBuffer, L"Number of triangles");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry - Number of lines
	*/
	{
		int64_t iLinesCount = 0;
		for (size_t iLine = 0; iLine < pRDFInstance->getLines().size(); iLine++)
		{
			iLinesCount += pRDFInstance->getLines()[iLine].second / 2;
		}

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iLinesCount);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Number of lines", (_variant_t)szBuffer, L"Number of lines");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry - Number of points
	*/
	{
		int64_t iPointsCount = pRDFInstance->getPoints().size();

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iPointsCount);

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Number of points", (_variant_t)szBuffer, L"Number of points");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 0:   Check Design Tree Consistency
	*   bit 1:   Check Consistency for Triangle Output (through API)
	*   bit 2:   Check Consistency for Line Output (through API)
	*   bit 3:   Check Consistency for Point Output (through API)
	*   bit 4:   Check Consistency for Generated Surfaces (through API)
	*   bit 5:   Check Consistency for Generated Surfaces (internal)
	*   bit 6:   Check Consistency for Generated Solids (through API)
	*   bit 7:   Check Consistency for Generated Solids (internal)
	*   bit 8:   Check Consistency for BoundingBox's
	*   bit 9:   Check Consistency for Triangulation
	*   bit 10:  Check Consistency for Relations (through API)
	*
	*   bit 16:   Contains (Closed) Solid(s)
	*   bit 18:   Contains (Closed) Infinite Solid(s)
	*   bit 20:   Contains Closed Surface(s)
	*   bit 21:   Contains Open Surface(s)
	*   bit 22:   Contains Closed Infinite Surface(s)
	*   bit 23:   Contains Open Infinite Surface(s)
	*   bit 24:   Contains Closed Line(s)
	*   bit 25:   Contains Open Line(s)
	*   bit 26:   Contains Closed Infinite Line(s) [i.e. both ends in infinity]
	*   bit 27:   Contains Open Infinite Line(s) [i.e. one end in infinity]
	*   bit 28:   Contains (Closed) Point(s)
	*/

	/*
	* Consistency
	*/
	CMFCPropertyGridProperty * pConsistencyGroup = new CMFCPropertyGridProperty(L"Consistency");
	pMetaInfoGroup->AddSubItem(pConsistencyGroup);

	/*
	*   bit 0:   Check Design Tree Consistency
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit0) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Design Tree Consistency", (_variant_t)szResult, L"Check Design Tree Consistency");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 1:   Check Consistency for Triangle Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit1) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Triangle Output (through API)", (_variant_t)szResult, L"Check Consistency for Triangle Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 2:   Check Consistency for Line Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit2) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Line Output (through API)", (_variant_t)szResult, L"Check Consistency for Line Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 3:   Check Consistency for Point Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit3) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Point Output (through API)", (_variant_t)szResult, L"Check Consistency for Point Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 4:   Check Consistency for Generated Surfaces (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit4) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Surfaces (through API)", (_variant_t)szResult, L"Check Consistency for Generated Surfaces (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 5:   Check Consistency for Generated Surfaces (internal)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit5) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Surfaces (internal)", (_variant_t)szResult, L"Check Consistency for Generated Surfaces (internal)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 6:   Check Consistency for Generated Solids (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit6) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Solids (through API)", (_variant_t)szResult, L"Check Consistency for Generated Solids (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 7:   Check Consistency for Generated Solids (internal)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit7) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Solids (internal)", (_variant_t)szResult, L"Check Consistency for Generated Solids (internal)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 8:   Check Consistency for BoundingBox's
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit8) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for BoundingBox's", (_variant_t)szResult, L"Check Consistency for BoundingBox's");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 9:   Check Consistency for Triangulation
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit9) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Triangulation", (_variant_t)szResult, L"Check Consistency for Triangulation");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 10:  Check Consistency for Relations (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit10) > 0 ? L"FAILED" : L"OK";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Relations (through API)", (_variant_t)szResult, L"Check Consistency for Relations (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 16:   Contains (Closed) Solid(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit16) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Solid(s)", (_variant_t)szResult, L"Contains (Closed) Solid(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 18:   Contains (Closed) Infinite Solid(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit18) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Infinite Solid(s)", (_variant_t)szResult, L"Contains (Closed) Infinite Solid(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 20:   Contains Closed Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit20) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Closed Surface(s)", (_variant_t)szResult, L"Contains Closed Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 21:   Contains Open Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit21) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Open Surface(s)", (_variant_t)szResult, L"Contains Open Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 22:   Contains Closed Infinite Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit22) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Closed Infinite Surface(s)", (_variant_t)szResult, L"Contains Closed Infinite Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 23:   Contains Open Infinite Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit23) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Open Infinite Surface(s)", (_variant_t)szResult, L"Contains Open Infinite Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 24:   Contains Closed Line(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit24) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Closed Line(s)", (_variant_t)szResult, L"Contains Closed Line(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 25:   Contains Open Line(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit25) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Open Line(s)", (_variant_t)szResult, L"Contains Open Line(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 26:   Contains Closed Infinite Line(s) [i.e. both ends in infinity]
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit26) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Closed Infinite Line(s)", (_variant_t)szResult, L"Contains Closed Infinite Line(s) [i.e. both ends in infinity]");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 27:   Contains Open Infinite Line(s) [i.e. one end in infinity]
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit27) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty* pProperty = new CMFCPropertyGridProperty(L"Contains Open Infinite Line(s)", (_variant_t)szResult, L"Contains Open Infinite Line(s) [i.e. one end in infinity]");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 28:   Contains (Closed) Point(s)
	*/
	{
		wchar_t * szResult = CheckInstanceConsistency(pRDFInstance->getInstance(), flagbit28) > 0 ? L"YES" : L"-";

		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Point(s)", (_variant_t)szResult, L"Contains (Closed) Point(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	m_wndPropList.AddProperty(pMetaInfoGroup);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::OnViewModeChanged()
{
	switch (m_wndObjectCombo.GetCurSel())
	{
	case 0: // Application Properties
	{
		LoadApplicationProperties();
	}
	break;

	case 1: // Instance Properties
	{
		LoadInstanceProperties();
	}
	break;

	default:
	{
		ASSERT(false); // unknown mode
	}
	break;
	} // switch (m_wndObjectCombo.GetCurSel())
}

LRESULT CPropertiesWnd::OnLoadInstancePropertyValues(WPARAM wParam, LPARAM /*lParam*/)
{
	ASSERT(wParam != 0);	

	CMFCPropertyGridProperty * pValueProperty = (CMFCPropertyGridProperty *)wParam;	
	CRDFInstancePropertyData * pValueData = (CRDFInstancePropertyData *)pValueProperty->GetData();
	ASSERT(pValueData != NULL);

	CRDFInstance * pRDFInstance = pValueData->GetInstance();
	CRDFProperty * pRDFProperty = pValueData->GetProperty();

	/*
	* Select the property
	*/
	CMFCPropertyGridProperty * pPropertyGroup = pValueProperty->GetParent();	
	m_wndPropList.SetCurSel(pPropertyGroup);

	/*
	* Update the cardinality
	*/
	CAddRDFInstanceProperty * pCardinalityProperty = (CAddRDFInstanceProperty *)pPropertyGroup->GetSubItem(1);

	wstring strCardinality = pRDFProperty->getCardinality(pRDFInstance->getInstance());

	pCardinalityProperty->SetValue((_variant_t)strCardinality.c_str());
	pCardinalityProperty->SetModified((_variant_t)strCardinality.c_str() != pCardinalityProperty->GetOriginalValue());

	/*
	* Disable the drawing
	*/
	m_wndPropList.SendMessage(WM_SETREDRAW, 0, 0);

	/*
	* Remove all values	
	*/
	while (pPropertyGroup->GetSubItemsCount() > 2/*keep range and cardinality*/)
	{
		CMFCPropertyGridProperty * pValue = (CRDFInstanceProperty *)(pPropertyGroup->GetSubItem(2));

		m_wndPropList.DeleteProperty(pValue);
	}	

	/*
	* Add the values
	*/
	AddInstancePropertyValues(pPropertyGroup, pRDFInstance, pRDFProperty);

	/*
	* Enable the drawing
	*/
	m_wndPropList.SendMessage(WM_SETREDRAW, 1, 0);

	m_wndPropList.AdjustLayout();

	return 0;
}

LRESULT CPropertiesWnd::OnLoadInstanceProperties(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	LoadInstanceProperties();

	return 0;
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}


void CPropertiesWnd::OnDestroy()
{
	ASSERT(GetController() != NULL);
	GetController()->UnRegisterView(this);

	__super::OnDestroy();
}
