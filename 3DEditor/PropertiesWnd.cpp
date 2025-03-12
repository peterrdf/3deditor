
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "3DEditor.h"
#include "RDFOpenGLView.h"
#include "RDFClass.h"
#include "RDFModel.h"
#include "EditObjectPropertyDialog.h"
#include "SelectInstanceDialog.h"

#include "_ptr.h"

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
#define ROTATION_MODE_XY L"2D"
#define ROTATION_MODE_XYZ L"3D"
#define REMOVE_OBJECT_PROPERTY_COMMAND L"---<REMOVE>---"
#define SELECT_OBJECT_PROPERTY_COMMAND L"---<SELECT>---"

// Moved in CSelectInstanceDialog
//#define USED_SUFFIX L" [used]"

CApplicationPropertyData::CApplicationPropertyData(enumApplicationProperty enApplicationProperty)
{
	m_enPropertyType = enApplicationProperty;
}

enumApplicationProperty CApplicationPropertyData::GetType() const
{
	return m_enPropertyType;
}

CLightPropertyData::CLightPropertyData(enumApplicationProperty enApplicationProperty, int iLightIndex)
	: CApplicationPropertyData(enApplicationProperty)
	, m_iLightIndex(iLightIndex)
{
}

int CLightPropertyData::GetLightIndex() const
{
	return m_iLightIndex;
}

CApplicationProperty::CApplicationProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

CApplicationProperty::CApplicationProperty(const CString & strGroupName, DWORD_PTR dwData, BOOL bIsValueList)
	: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
}

/*virtual*/ CApplicationProperty::~CApplicationProperty()
{
	delete (CApplicationPropertyData *)GetData();
}

CColorApplicationProperty::CColorApplicationProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridColorProperty(strName, color, pPalette, szDescription, dwData)
{
}

/*virtual*/ CColorApplicationProperty::~CColorApplicationProperty()
{
	delete (CApplicationPropertyData *)GetData();
}

CRDFInstanceData::CRDFInstanceData(_rdf_controller * pController, _rdf_instance * pInstance)
	: m_pController(pController)
	, m_pInstance(pInstance)
{
	ASSERT(m_pController != nullptr);
	ASSERT(m_pInstance != nullptr);
}

_rdf_controller * CRDFInstanceData::GetController() const
{
	return m_pController;
}

_rdf_instance * CRDFInstanceData::GetInstance() const
{
	return m_pInstance;
}

CRDFInstancePropertyData::CRDFInstancePropertyData(_rdf_controller * pController, _rdf_instance * pInstance, CRDFProperty * pProperty, int64_t iCard)
	: CRDFInstanceData(pController, pInstance)
	, m_pProperty(pProperty)
	, m_iCard(iCard)
{
	ASSERT(m_pProperty != nullptr);
}

CRDFProperty * CRDFInstancePropertyData::GetProperty() const
{
	return m_pProperty;
}

int64_t CRDFInstancePropertyData::GetCard() const
{
	return m_iCard;
}

void CRDFInstancePropertyData::SetCard(int64_t iCard)
{
	ASSERT(iCard >= 0);

	m_iCard = iCard;
}

CRDFInstanceProperty::CRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

/*virtual*/ CRDFInstanceProperty::~CRDFInstanceProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

/*virtual*/ BOOL CRDFInstanceProperty::HasButton() const
{
	ASSERT(GetData() != NULL);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != nullptr);

	BOOL bHasButton = FALSE;

	/*
	* Read the restrictions
	*/
	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getOwlInstance(), iMinCard, iMaxCard);

	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		iMinCard = 0;
	}

	switch (pData->GetProperty()->GetType())
	{
	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		bHasButton = FALSE;
	} // case DATATYPEPROPERTY_TYPE_BOOLEAN:
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		int64_t iCard = 0;
		wchar_t ** szValue = nullptr;
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&szValue, &iCard);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		int64_t iCard = 0;
		char** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		/*
		* Read the values
		*/
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&pdValue, &iCard);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		/*
		* Read the values
		*/
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

		ASSERT(iCard > 0);

		bHasButton = iCard > iMinCard ? TRUE : FALSE;
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())

	return bHasButton;
}

/*virtual*/ void CRDFInstanceProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT(GetData() != NULL);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != nullptr);

	/*
	* Read the restrictions
	*/
	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getOwlInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->GetType())
	{
	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		// NA
	} // case DATATYPEPROPERTY_TYPE_BOOLEAN:
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&szValue, &iCard);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		wchar_t** szNewValues = (wchar_t**)new size_t[iCard - 1];

		int iCurrentValue = 0;
		for (int iValue = 0; iValue < iCard; iValue++)
		{
			if (iValue == pData->GetCard())
			{
				continue;
			}

			szNewValues[iCurrentValue] = new wchar_t[wcslen(szValue[iCurrentValue]) + 1];
			wcscpy(szNewValues[iCurrentValue], szValue[iCurrentValue]);

			iCurrentValue++;
		}

		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)szNewValues, iCard - 1);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		for (int iValue = 0; iValue < iCard - 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete [] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo

		/*
		* Update the values
		*/
		auto pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		char** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		char** szNewValues = (char**)new size_t[iCard - 1];

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

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard - 1);

		for (int iValue = 0; iValue < iCard - 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete[] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo

		/*
		* Update the values
		*/
		auto pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		wchar_t ** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		wchar_t** szNewValues = (wchar_t**)new size_t[iCard - 1];

		int iCurrentValue = 0;
		for (int iValue = 0; iValue < iCard; iValue++)
		{
			if (iValue == pData->GetCard())
			{
				continue;
			}

			szNewValues[iCurrentValue] = new wchar_t[wcslen(szValue[iCurrentValue]) + 1];
			wcscpy(szNewValues[iCurrentValue], szValue[iCurrentValue]);

			iCurrentValue++;
		}

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard - 1);

		for (int iValue = 0; iValue < iCard - 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete[] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo

		/*
		* Update the values
		*/
		auto pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&pdValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		vector<double> vecValues;
		vecValues.assign(pdValue, pdValue + iCard);

		vecValues.erase(vecValues.begin() + pData->GetCard());

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());		//#todo

		/*
		* Update the values
		*/
		auto pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

		ASSERT(iCard > 0);
		ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

		/*
		* Remove a value
		*/
		vector<int64_t> vecValues;
		vecValues.assign(piValue, piValue + iCard);

		vecValues.erase(vecValues.begin() + pData->GetCard());

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		auto pProperty = GetParent();
		ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

		auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())
}

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

/*virtual*/ void CRDFInstanceProperty::EnableSpinControlInt64()
{
	ASSERT(m_varValue.vt == VT_I8);

	// Cheat the base class
	m_varValue.vt = VT_I4;

	EnableSpinControl(TRUE, 0, INT_MAX);

	m_varValue.vt = VT_I8;
}

CRDFInstanceObjectProperty::CRDFInstanceObjectProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
	, m_mapValues()
{
}

/*virtual*/ CRDFInstanceObjectProperty::~CRDFInstanceObjectProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

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

CRDFColorSelectorProperty::CRDFColorSelectorProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridColorProperty(strName, color, pPalette, szDescription, dwData)
{

}

/*virtual*/ CRDFColorSelectorProperty::~CRDFColorSelectorProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

CAddRDFInstanceProperty::CAddRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, vtValue, szDescription, dwData)
{
}

/*virtual*/ CAddRDFInstanceProperty::~CAddRDFInstanceProperty()
{
	delete (CRDFInstancePropertyData *)GetData();
}

void CAddRDFInstanceProperty::SetModified(BOOL bModified)
{
	m_bIsModified = bModified;
}

/*virtual*/ BOOL CAddRDFInstanceProperty::HasButton() const
{
	ASSERT(GetData() != NULL);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != nullptr);

	BOOL bHasButton = FALSE;

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getOwlInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		/*
		* Read the card
		*/
		int64_t * piObjectInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), &piObjectInstances, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case OBJECTPROPERTY_TYPE:
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		bool* pbValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&pbValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_BOOLEAN
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		int64_t iCard = 0;
		char** szValue = nullptr;
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_STRING
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		int64_t iCard = 0;
		char** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_CHAR_ARRAY
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&pdValue, &iCard);
		
		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		/*
		* Read the card
		*/
		int64_t iCard = 0;
		double * piValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

		if (iMaxCard == -1)
		{
			bHasButton = TRUE;
		}
		else
		{
			bHasButton = iCard < iMaxCard ? TRUE : FALSE;
		}
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())

	return bHasButton;
}

/*virtual*/ void CAddRDFInstanceProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT(GetData() != NULL);

	/*
	* Select the property
	*/
	auto pPropertyGroup = GetParent();
	m_pWndList->SetCurSel(pPropertyGroup);

	CRDFInstancePropertyData * pData = (CRDFInstancePropertyData *)GetData();
	ASSERT(pData != nullptr);

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	pData->GetProperty()->GetRestrictions(pData->GetInstance()->getOwlInstance(), iMinCard, iMaxCard);

	switch (pData->GetProperty()->GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		ASSERT(FALSE);//#todo
		//CEditObjectPropertyDialog dlgEditObjectProperty(pData->GetController(), pData->GetInstance(), pData->GetProperty(), ::AfxGetMainWnd());
		//if (dlgEditObjectProperty.DoModal() != IDOK)
		//{
		//	return;
		//}

		/*
		* Read the original values
		*/
		int64_t * piInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), &piInstances, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));

		/*
		* Add a value
		*/
		vector<int64_t> vecValues;
		if (iCard > 0)
		{
			vecValues.assign(piInstances, piInstances + iCard);
		}

		ASSERT(FALSE);//#todo
		//switch (dlgEditObjectProperty.m_iMode)
		//{
		//case 0: // Existing instance
		//{
		//	ASSERT(dlgEditObjectProperty.m_pExisitngRDFInstance != nullptr);

		//	vecValues.push_back(dlgEditObjectProperty.m_pExisitngRDFInstance->getOwlInstance());
		//}
		//break;

		//case 1: // New instance
		//{
		//	ASSERT(dlgEditObjectProperty.m_iNewInstanceRDFClass != 0);

		//	ASSERT(FALSE);//#todo
		//	//_rdf_instance * pNewRDFInstance = pData->GetController()->CreateNewInstance((CPropertiesWnd *)m_pWndList->GetParent(), dlgEditObjectProperty.m_iNewInstanceRDFClass);
		//	//ASSERT(pNewRDFInstance != nullptr);

		//	//vecValues.push_back(pNewRDFInstance->getOwlInstance());
		//}
		//break;

		//default:
		//{
		//	ASSERT(false);
		//}
		//break;
		//} // switch (dlgEditObjectProperty.m_iMode)

		SetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr);
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update UI
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTIES, 0, 0);
	} // case OBJECTPROPERTY_TYPE:
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		bool* pbValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&pbValue, &iCard);

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

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)pbNewValues, vecValues.size());

		delete[] pbNewValues;
		pbNewValues = nullptr;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		wchar_t ** szValue = nullptr;
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetInstance()->getOwlInstance(), (void **)&szValue, &iCard);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));
		ASSERT(iMaxCard == 1);

		wchar_t** szNewValues = (wchar_t**)new size_t[iCard + 1];
		if (iCard > 0)
		{
			for (int iValue = 0; iValue < iCard; iValue++)
			{
				szNewValues[iValue] = new wchar_t[wcslen(szValue[iValue]) + 1];
				wcscpy(szNewValues[iValue], szValue[iValue]);
			}
		}

		szNewValues[iCard] = new wchar_t[1];
		szNewValues[iCard][0] = '\0';

		/*
		* Add a value
		*/
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)szNewValues, iCard + 1);
		SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

		for (int iValue = 0; iValue < iCard + 1; iValue++)
		{
			delete [] szNewValues[iValue];
		}
		delete [] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_CHAR_DATATYPE
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		char** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetInstance()->getOwlInstance(), (void**)&szValue, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));
		ASSERT(iMaxCard == 1);

		char** szNewValues = (char**)new size_t[iCard + 1];
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
		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard + 1);

		for (int iValue = 0; iValue < iCard + 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete[] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_CHAR_DATATYPE
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetInstance()->getOwlInstance(), (void**)&szValue, &iCard);

		ASSERT((iMaxCard == -1) || (iCard < iMaxCard));
		ASSERT(iMaxCard == 1);

		wchar_t** szNewValues = (wchar_t**)new size_t[iCard + 1];
		if (iCard > 0)
		{
			for (int iValue = 0; iValue < iCard; iValue++)
			{
				szNewValues[iValue] = new wchar_t[wcslen(szValue[iValue]) + 1];
				wcscpy(szNewValues[iValue], szValue[iValue]);
			}
		}

		szNewValues[iCard] = new wchar_t[1];
		szNewValues[iCard][0] = '\0';

		/*
		* Add a value
		*/
		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard + 1);

		for (int iValue = 0; iValue < iCard + 1; iValue++)
		{
			delete[] szNewValues[iValue];
		}
		delete[] szNewValues;

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case TYPE_CHAR_DATATYPE
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&pdValue, &iCard);

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

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		/*
		* Read the original values
		*/
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

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

		SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

		/*
		* Notify
		*/
		ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
		//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

		/*
		* Update the values
		*/
		m_pWndList->GetParent()->PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)this, 0);
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unexpected
	}
	break;
	} // switch (pData->GetProperty()->getType())
}

/*virtual*/ void CPropertiesWnd::OnInstanceSelected(CRDFView * /*pSender*/)
{
	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadInstanceProperties();
}

/*virtual*/ void CPropertiesWnd::OnInstancePropertySelected()
{
	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadInstanceProperties();
}

/*virtual*/ void CPropertiesWnd::OnInstanceNameEdited(CRDFView* pSender, _rdf_instance* /*pInstance*/)
{
	if (pSender == this)
	{
		return;
	}

	m_wndObjectCombo.SetCurSel(1 /*Properties*/);

	LoadInstanceProperties();
}

/*virtual*/ void CPropertiesWnd::OnNewInstanceCreated(CRDFView * pSender, _rdf_instance * /*pInstance*/)
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

/*virtual*/ void CPropertiesWnd::postModelLoaded() /*override*/
{
	/*if (GetController()->IsTestMode())#todo
	{
		return;
	}*/

	m_wndObjectCombo.SetCurSel(0 /*Application*/);

	LoadApplicationProperties();
}

/*virtual*/ void CPropertiesWnd::onApplicationPropertyChanged(_view* pSender, enumApplicationProperty /*enApplicationProperty*/) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		LoadApplicationProperties();
	}
}

/*virtual*/ void CPropertiesWnd::onShowBaseInformation(_view* pSender, _rdf_instance* pInstance) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	m_wndObjectCombo.SetCurSel(1/*Properties*/);

	LoadBaseInformation(pInstance);
}

/*virtual*/ void CPropertiesWnd::onShowMetaInformation(_view* pSender, _rdf_instance* pInstance) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	m_wndObjectCombo.SetCurSel(1/*Properties*/);

	LoadMetaInformation(pInstance);
}

/*afx_msg*/ LRESULT CPropertiesWnd::OnPropertyChanged(__in WPARAM /*wparam*/, __in LPARAM lparam)
{
	auto pController = getRDFController();
	if (pController == nullptr)
	{
		ASSERT(false);

		return 0;
	}

#pragma region Application
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		auto pOGLRenderer = getRDFController()->getViewAs<_oglRenderer>();
		if (pOGLRenderer == nullptr)
		{
			ASSERT(false);

			return 0;
		}		

		auto pApplicationProperty = dynamic_cast<CApplicationProperty*>((CMFCPropertyGridProperty*)lparam);
		if (pApplicationProperty != nullptr)
		{
			CString strValue = pApplicationProperty->GetValue();

			auto pData = (CApplicationPropertyData*)pApplicationProperty->GetData();
			ASSERT(pData != nullptr);

			switch (pData->GetType())
			{
				case enumApplicationProperty::ShowFaces:
				{
					pOGLRenderer->setShowFaces(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowFaces);
				}
				break;

				case enumApplicationProperty::CullFaces:
				{
					pOGLRenderer->setCullFacesMode(strValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::CullFaces);
				}
				break;

				case enumApplicationProperty::ShowFacesWireframes:
				{
					pOGLRenderer->setShowFacesPolygons(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowFacesWireframes);
				}
				break;

				case enumApplicationProperty::ShowConceptualFacesWireframes:
				{
					pOGLRenderer->setShowConceptualFacesPolygons(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowConceptualFacesWireframes);
				}
				break;

				case enumApplicationProperty::ShowLines:
				{
					pOGLRenderer->setShowLines(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowLines);
				}
				break;

				case enumApplicationProperty::ShowPoints:
				{
					pOGLRenderer->setShowPoints(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowPoints);
				}
				break;

				case enumApplicationProperty::ShowNormalVectors:
				{
					pOGLRenderer->setShowNormalVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowNormalVectors);
				}
				break;

				case enumApplicationProperty::ShowTangenVectors:
				{
					pOGLRenderer->setShowTangentVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowTangenVectors);
				}
				break;

				case enumApplicationProperty::ShowBiNormalVectors:
				{
					pOGLRenderer->setShowBiNormalVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowBiNormalVectors);
				}
				break;

				case enumApplicationProperty::ScaleVectors:
				{
					pOGLRenderer->setScaleVectors(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ScaleVectors);
				}
				break;

				case enumApplicationProperty::ShowBoundingBoxes:
				{
					pOGLRenderer->setShowBoundingBoxes(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowBoundingBoxes);
				}
				break;

				case enumApplicationProperty::VisibleValuesCountLimit:
				{
					int iValue = _wtoi((LPCTSTR)strValue);

					getRDFController()->setVisibleValuesCountLimit(iValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::VisibleValuesCountLimit);
				}
				break;

				case enumApplicationProperty::ScalelAndCenter:
				{
					getRDFController()->setScaleAndCenter(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ScalelAndCenter);
				}
				break;

				case enumApplicationProperty::RotationMode:
				{
					pOGLRenderer->_setRotationMode(strValue == ROTATION_MODE_XY ? enumRotationMode::XY : enumRotationMode::XYZ);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::RotationMode);
				}
				break;
				
				case enumApplicationProperty::ShowCoordinateSystem:
				{
					pOGLRenderer->setShowCoordinateSystem(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowCoordinateSystem);
				}
				break;

				case enumApplicationProperty::CoordinateSystemType:
				{
					getRDFController()->setModelCoordinateSystem(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::CoordinateSystemType);
				}
				break;

				case enumApplicationProperty::ShowNavigator:
				{
					pOGLRenderer->setShowNavigator(strValue == TRUE_VALUE_PROPERTY ? TRUE : FALSE);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::ShowNavigator);
				}
				break;				

				case enumApplicationProperty::PointLightingLocation:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					auto pProperty = pApplicationProperty->GetParent();
					ASSERT(pProperty != nullptr);
					ASSERT(dynamic_cast<CApplicationProperty*>(pProperty) != nullptr);
					ASSERT(((CApplicationPropertyData*)dynamic_cast<CApplicationProperty*>(pProperty)->
						GetData())->GetType() == enumApplicationProperty::PointLightingLocation);
					ASSERT(pProperty->GetSubItemsCount() == 3);

					auto pX = pProperty->GetSubItem(0);
					auto pY = pProperty->GetSubItem(1);
					auto pZ = pProperty->GetSubItem(2);

					pBlinnPhongProgram->_setPointLightingLocation(glm::vec3(
						(float)_wtof((LPCTSTR)(CString)pX->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pY->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pZ->GetValue()))
					);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::PointLightingLocation);
				}
				break;

				case enumApplicationProperty::AmbientLightWeighting:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					auto pProperty = pApplicationProperty->GetParent();
					ASSERT(pProperty != nullptr);
					ASSERT(dynamic_cast<CApplicationProperty*>(pProperty) != nullptr);
					ASSERT(((CApplicationPropertyData*)dynamic_cast<CApplicationProperty*>(pProperty)->
						GetData())->GetType() == enumApplicationProperty::AmbientLightWeighting);
					ASSERT(pProperty->GetSubItemsCount() == 3);

					auto pX = pProperty->GetSubItem(0);
					auto pY = pProperty->GetSubItem(1);
					auto pZ = pProperty->GetSubItem(2);

					pBlinnPhongProgram->_setAmbientLightWeighting(
						(float)_wtof((LPCTSTR)(CString)pX->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pY->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pZ->GetValue())
					);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::AmbientLightWeighting);
				}
				break;

				case enumApplicationProperty::DiffuseLightWeighting:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					auto pProperty = pApplicationProperty->GetParent();
					ASSERT(pProperty != nullptr);
					ASSERT(dynamic_cast<CApplicationProperty*>(pProperty) != nullptr);
					ASSERT(((CApplicationPropertyData*)dynamic_cast<CApplicationProperty*>(pProperty)->
						GetData())->GetType() == enumApplicationProperty::DiffuseLightWeighting);
					ASSERT(pProperty->GetSubItemsCount() == 3);

					auto pX = pProperty->GetSubItem(0);
					auto pY = pProperty->GetSubItem(1);
					auto pZ = pProperty->GetSubItem(2);

					pBlinnPhongProgram->_setDiffuseLightWeighting(
						(float)_wtof((LPCTSTR)(CString)pX->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pY->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pZ->GetValue())
					);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::DiffuseLightWeighting);
				}
				break;

				case enumApplicationProperty::SpecularLightWeighting:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					auto pProperty = pApplicationProperty->GetParent();
					ASSERT(pProperty != nullptr);
					ASSERT(dynamic_cast<CApplicationProperty*>(pProperty) != nullptr);
					ASSERT(((CApplicationPropertyData*)dynamic_cast<CApplicationProperty*>(pProperty)->
						GetData())->GetType() == enumApplicationProperty::SpecularLightWeighting);
					ASSERT(pProperty->GetSubItemsCount() == 3);

					auto pX = pProperty->GetSubItem(0);
					auto pY = pProperty->GetSubItem(1);
					auto pZ = pProperty->GetSubItem(2);

					pBlinnPhongProgram->_setSpecularLightWeighting(
						(float)_wtof((LPCTSTR)(CString)pX->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pY->GetValue()),
						(float)_wtof((LPCTSTR)(CString)pZ->GetValue())
					);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::SpecularLightWeighting);
				}
				break;

				case enumApplicationProperty::MaterialShininess:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					float fValue = (float)_wtof((LPCTSTR)strValue);

					pBlinnPhongProgram->_setMaterialShininess(fValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::MaterialShininess);
				}
				break;

				case enumApplicationProperty::Contrast:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					float fValue = (float)_wtof((LPCTSTR)strValue);

					pBlinnPhongProgram->_setContrast(fValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::Contrast);
				}
				break;

				case enumApplicationProperty::Brightness:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					float fValue = (float)_wtof((LPCTSTR)strValue);

					pBlinnPhongProgram->_setBrightness(fValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::Brightness);
				}
				break;

				case enumApplicationProperty::Gamma:
				{
					auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
					ASSERT(pBlinnPhongProgram != nullptr);

					float fValue = (float)_wtof((LPCTSTR)strValue);

					pBlinnPhongProgram->_setGamma(fValue);

					getRDFController()->onApplicationPropertyChanged(this, enumApplicationProperty::Gamma);
				}
				break;

				default:
					ASSERT(false);
					break;
			} // switch (pData->GetType())

			return 0;
		} // if (pApplicationProperty != nullptr)

		auto pColorApplicationProperty = dynamic_cast<CColorApplicationProperty*>((CMFCPropertyGridProperty*)lparam);
		if (pColorApplicationProperty != nullptr)
		{
			ASSERT(false); // DISABLED

			return 0;
		} // if (pColorApplicationProperty != nullptr)

		ASSERT(false); // unexpected!

		return 0;
	} // if (m_wndObjectCombo.GetCurSel() == 0)
#pragma endregion

	/*
	* Properties
	*/
	if (m_wndObjectCombo.GetCurSel() == 1)
	{
		/*
		* Object properties
		*/
		auto pObjectProperty = dynamic_cast<CRDFInstanceObjectProperty*>((CMFCPropertyGridProperty*)lparam);
		if (pObjectProperty != nullptr)
		{
			CString strValue = pObjectProperty->GetValue();

			auto pData = (CRDFInstancePropertyData *)pObjectProperty->GetData();
			ASSERT(pData != nullptr);

			int64_t* piInstances = nullptr;
			int64_t iCard = 0;
			GetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), &piInstances, &iCard);

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
				pData->GetProperty()->GetRestrictions(pData->GetInstance()->getOwlInstance(), iMinCard, iMaxCard);

				ASSERT((iCard - 1) >= (((iMinCard == -1) && (iMaxCard == -1)) ? 0 : iMinCard));

				/*
				* Remove a value
				*/
				vecValues.erase(vecValues.begin() + pData->GetCard());

				SetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), vecValues.data(), vecValues.size());

				/*
				* Notify
				*/
				ASSERT(pData->GetController() != nullptr); ASSERT(FALSE);//#todo
				//pData->GetController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());

				/*
				* Delete this item
				*/
				auto pProperty = pObjectProperty->GetParent();
				ASSERT(pProperty->GetSubItemsCount() >= 3/*range, cardinality and at least 1 value*/);

				auto pValue = pProperty->GetSubItem((int)pData->GetCard() + 2/*range and cardinality*/);
				PostMessage(WM_LOAD_INSTANCE_PROPERTY_VALUES, (WPARAM)pValue, 0);
			} // if (strValue == REMOVE_OBJECT_PROPERTY_COMMAND)
			else
			{
				ASSERT(FALSE);//#todo
				//if (strValue == SELECT_OBJECT_PROPERTY_COMMAND)
				//{
				//	auto pObjectRDFProperty = dynamic_cast<CObjectRDFProperty*>(pData->GetProperty());
				//	ASSERT(pObjectRDFProperty != nullptr);

				//	ASSERT(pData->GetController() != nullptr);
				//	ASSERT(FALSE);//#todo
				//	CSelectInstanceDialog dlgSelectInstanceDialog(getRDFController(), pData->GetInstance(), pObjectRDFProperty, pData->GetCard());
				//	if (dlgSelectInstanceDialog.DoModal() == IDOK)
				//	{
				//		ASSERT(dlgSelectInstanceDialog.m_iInstance != -1);						

				//		/*
				//		* Update the value
				//		*/						
				//		vecValues[pData->GetCard()] = dlgSelectInstanceDialog.m_iInstance;

				//		SetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), vecValues.data(), vecValues.size());

				//		/*
				//		* Notify
				//		*/
				//		ASSERT(pData->GetController() != nullptr);
				//		//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo

				//		/*
				//		* Value
				//		*/
				//		ASSERT(!dlgSelectInstanceDialog.m_strInstanceUniqueName.IsEmpty());

				//		pObjectProperty->SetValue((_variant_t)dlgSelectInstanceDialog.m_strInstanceUniqueName);
				//	}
				//	else
				//	{
				//		/*
				//		* Value
				//		*/
				//		ASSERT(!dlgSelectInstanceDialog.m_strOldInstanceUniqueName.IsEmpty());

				//		pObjectProperty->SetValue((_variant_t)dlgSelectInstanceDialog.m_strOldInstanceUniqueName);
				//	}
				//}
				//else
				//{
				//	/*
				//	* Update the value
				//	*/
				//	int64_t iInstance = pObjectProperty->GetInstance((LPCTSTR)strValue);
				//	vecValues[pData->GetCard()] = iInstance;

				//	SetObjectProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), vecValues.data(), vecValues.size());

				//	/*
				//	* Notify
				//	*/
				//	ASSERT(pData->GetController() != nullptr);
				//	//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo
				//}				
			} // else if (strValue == REMOVE_OBJECT_PROPERTY_COMMAND)

			return 0;
		} // if (pObjectProperty != nullptr)

		/*
		* ColorComponent properties
		*/
		auto pColorSelectorProperty = dynamic_cast<CRDFColorSelectorProperty *>((CMFCPropertyGridProperty*)lparam);
		if (pColorSelectorProperty != nullptr)
		{	
			auto pData = (CRDFInstancePropertyData *)pColorSelectorProperty->GetData();
			ASSERT(pData != nullptr);

			auto pModel = getRDFController()->getModel();
			ASSERT(pModel != nullptr);

			ASSERT(pData->GetInstance()->getGeometry()->getClassInstance() == GetClassByName(pModel->getOwlModel(), "ColorComponent"));

			auto& mapProperties = _ptr<CRDFModel>(pModel)->GetProperties();

			/*
			* R
			*/
			int64_t iRProperty = GetPropertyByName(pModel->getOwlModel(), "R");

			double dR = GetRValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), iRProperty, &dR, 1);

			map<int64_t, CRDFProperty*>::const_iterator itProperty = mapProperties.find(iRProperty);
			ASSERT(itProperty != mapProperties.end());

			/*
			* Notify
			*/
			//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), itProperty->second);//#todo

			/*
			* G
			*/
			int64_t iGProperty = GetPropertyByName(pModel->getOwlModel(), "G");

			double dG = GetGValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), iGProperty, &dG, 1);

			itProperty = mapProperties.find(iGProperty);
			ASSERT(itProperty != mapProperties.end());

			/*
			* Notify
			*/
			//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), itProperty->second);//#todo

			/*
			* B
			*/
			int64_t iBProperty = GetPropertyByName(pModel->getOwlModel(), "B");

			double dB = GetBValue(pColorSelectorProperty->GetColor()) / 255.;
			SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), iBProperty, &dB, 1);

			itProperty = mapProperties.find(iBProperty);
			ASSERT(itProperty != mapProperties.end());

			/*
			* Notify
			*/
			//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), itProperty->second);//#todo

			/*
			* Update UI
			*/
			PostMessage(WM_LOAD_INSTANCE_PROPERTIES, 0, 0);
		} // if (pColorSelectorProperty != nullptr)

		/*
		* Data properties
		*/
		auto pProperty = dynamic_cast<CRDFInstanceProperty*>((CMFCPropertyGridProperty*)lparam);
		if (pProperty == nullptr)
		{
			return 0;
		}

		CString strValue = pProperty->GetValue();

		auto pData = (CRDFInstancePropertyData *)pProperty->GetData();
		ASSERT(pData != nullptr);

		switch(pData->GetProperty()->GetType())
		{
			case DATATYPEPROPERTY_TYPE_BOOLEAN:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				bool* pbValue = nullptr;
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&pbValue, &iCard);

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

				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)pbNewValues, vecValues.size());

				delete[] pbNewValues;
				pbNewValues = nullptr;

				ASSERT(getRDFController() != nullptr);

				ASSERT(FALSE);//#todo
				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
			} // case DATATYPEPROPERTY_TYPE_BOOLEAN:
			break;

			case DATATYPEPROPERTY_TYPE_STRING:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				wchar_t ** szValue = nullptr;
				SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&szValue, &iCard);
				SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

				ASSERT(iCard > 0);

				wchar_t** szNewValues = (wchar_t**)new size_t[iCard];
				for (int iValue = 0; iValue < iCard; iValue++)
				{
					szNewValues[iValue] = new wchar_t[wcslen(szValue[iValue]) + 1];
					wcscpy(szNewValues[iValue], szValue[iValue]);
				}

				/*
				* Update the modified value
				*/
				delete[] szNewValues[pData->GetCard()];
				szNewValues[pData->GetCard()] = new wchar_t[wcslen(strValue) + 1];
				wcscpy(szNewValues[pData->GetCard()], strValue);

				SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, false);
				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)szNewValues, iCard);
				SetCharacterSerialization(pData->GetInstance()->getOwlModel(), 0, 0, true);

				for (int iValue = 0; iValue < iCard; iValue++)
				{
					delete [] szNewValues[iValue];
				}
				delete [] szNewValues;

				ASSERT(getRDFController() != nullptr);

				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo
			} // case TYPE_CHAR_DATATYPE:
			break;

			case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				char** szValue = nullptr;
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

				ASSERT(iCard > 0);

				char** szNewValues = (char**)new size_t[iCard];
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

				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard);

				for (int iValue = 0; iValue < iCard; iValue++)
				{
					delete[] szNewValues[iValue];
				}
				delete[] szNewValues;

				ASSERT(getRDFController() != nullptr);

				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo
			} // case TYPE_CHAR_DATATYPE:
			break;

			case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				wchar_t** szValue = nullptr;
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)&szValue, &iCard);

				ASSERT(iCard > 0);

				wchar_t** szNewValues = (wchar_t**)new size_t[iCard];
				for (int iValue = 0; iValue < iCard; iValue++)
				{
					szNewValues[iValue] = new wchar_t[wcslen(szValue[iValue]) + 1];
					wcscpy(szNewValues[iValue], szValue[iValue]);
				}

				/*
				* Update the modified value
				*/
				delete[] szNewValues[pData->GetCard()];
				szNewValues[pData->GetCard()] = new wchar_t[wcslen(strValue) + 1];
				wcscpy(szNewValues[pData->GetCard()], strValue);

				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void**)szNewValues, iCard);

				for (int iValue = 0; iValue < iCard; iValue++)
				{
					delete[] szNewValues[iValue];
				}
				delete[] szNewValues;

				ASSERT(getRDFController() != nullptr);

				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());//#todo
			} // case TYPE_CHAR_DATATYPE:
			break;

			case DATATYPEPROPERTY_TYPE_DOUBLE:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				double* pdValue = nullptr;
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&pdValue, &iCard);
			
				ASSERT(iCard > 0);

				vector<double> vecValues;
				vecValues.assign(pdValue, pdValue + iCard);

				/*
				* Update the modified value
				*/

				double dValue = _wtof((LPCTSTR)strValue);
				vecValues[pData->GetCard()] = dValue;

				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

				ASSERT(getRDFController() != nullptr);
				ASSERT(FALSE);//#todo
				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
			} // case DATATYPEPROPERTY_TYPE_DOUBLE:
			break;

			case DATATYPEPROPERTY_TYPE_INTEGER:
			{
				/*
				* Read the original values
				*/
				int64_t iCard = 0;
				int64_t * piValue = nullptr;
				GetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)&piValue, &iCard);

				ASSERT(iCard > 0);

				vector<int64_t> vecValues;
				vecValues.assign(piValue, piValue + iCard);

				/*
				* Update the modified value
				*/
				int64_t iValue = _wtoi64((LPCTSTR)strValue);
				vecValues[pData->GetCard()] = iValue;

				SetDatatypeProperty(pData->GetInstance()->getOwlInstance(), pData->GetProperty()->GetInstance(), (void **)vecValues.data(), vecValues.size());

				ASSERT(getRDFController() != nullptr);
				ASSERT(FALSE);//#todo
				//getRDFController()->OnInstancePropertyEdited(pData->GetInstance(), pData->GetProperty());
			} // case DATATYPEPROPERTY_TYPE_INTEGER:
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
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_toolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_toolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	getRDFController()->registerView(this);

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

	m_toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_toolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_toolBar.CleanUpLockedImages();
	m_toolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_toolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_toolBar.SetRouteCommandsViaFrame(FALSE);

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

void CPropertiesWnd::LoadApplicationProperties()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	auto pController = getRDFController();
	if (pController == nullptr)
	{
		ASSERT(false);

		return;
	}

	auto pOGLRenderer = getRDFController()->getViewAs<_oglRenderer>();
	if (pOGLRenderer == nullptr)
	{
		return;
	}

#pragma region View
	auto pViewGroup = new CMFCPropertyGridProperty(_T("View"));	
	
	{
		auto pProperty = new CApplicationProperty(_T("Faces"), 
			pOGLRenderer->getShowFaces() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Faces"),
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowFaces));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}
	
	{
		CString strCullFacesMode = pOGLRenderer->getCullFacesMode();

		auto pProperty = new CApplicationProperty(
			_T("Cull Faces"), 
			strCullFacesMode == CULL_FACES_FRONT ? CULL_FACES_FRONT : strCullFacesMode == CULL_FACES_BACK ? CULL_FACES_BACK : CULL_FACES_NONE,
			_T("Cull Faces"), (DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::CullFaces));
		pProperty->AddOption(CULL_FACES_NONE);
		pProperty->AddOption(CULL_FACES_FRONT);
		pProperty->AddOption(CULL_FACES_BACK);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Faces wireframes"), 
			pOGLRenderer->getShowFacesPolygons() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
			_T("Faces wireframes"), (DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowFacesWireframes));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Conceptual faces wireframes"), 
			pOGLRenderer->getShowConceptualFacesPolygons() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
			_T("Conceptual faces wireframes"), 
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowConceptualFacesWireframes));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Lines"), pOGLRenderer->getShowLines() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
			_T("Lines"), 
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowLines));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	// ********************************************************************************************
	// DSIABLED
	///*
	//* Line width
	//*/
	//{
	//	CApplicationProperty * pProperty = new CApplicationProperty(_T("Line thickness"), (_variant_t)(int)pOGLRenderer->GetLineWidth(), _T("Line thickness"), (DWORD_PTR)new CApplicationPropertyData(ptLineWidth));
	//	pProperty->EnableSpinControl(TRUE, 1, 1000);

	//	pViewGroup->AddSubItem(pProperty);
	//}

	{
		auto pProperty = new CApplicationProperty(_T("Points"), pOGLRenderer->getShowPoints() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
			_T("Points"), 
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowPoints));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	// ********************************************************************************************
	// DSIABLED
	///*
	//* Point size
	//*/
	//{
	//	CApplicationProperty * pProperty = new CApplicationProperty(_T("Point thickness"), (_variant_t)(int)pOGLRenderer->GetPointSize(), _T("Point thickness"), (DWORD_PTR)new CApplicationPropertyData(ptPointSize));
	//	pProperty->EnableSpinControl(TRUE, 1, 1000);

	//	pViewGroup->AddSubItem(pProperty);
	//}

	{
		auto pVectors = new CMFCPropertyGridProperty(_T("Vectors"));
		pViewGroup->AddSubItem(pVectors);

		/*
		* Normal vectors
		*/
		{
			auto pProperty = new CApplicationProperty(_T("Normal vectors"), pOGLRenderer->getShowNormalVectors() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Normal vectors"), 
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowNormalVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		{
			auto pProperty = new CApplicationProperty(_T("Tangent vectors"), pOGLRenderer->getShowTangentVectors() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Tangent vectors"), 
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowTangenVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		{
			auto pProperty = new CApplicationProperty(_T("Bi-normal vectors"), pOGLRenderer->getShowBiNormalVectors() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Bi-normal vectors"), 
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowBiNormalVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}

		{
			auto pProperty = new CApplicationProperty(_T("Scale"), pOGLRenderer->getScaleVectors() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Scale"), 
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ScaleVectors));
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pVectors->AddSubItem(pProperty);
		}
	}

	{
		auto pProperty = new CApplicationProperty(_T("Bounding boxes"), 
			pOGLRenderer->getShowBoundingBoxes() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
			_T("Bounding boxes"), 
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowBoundingBoxes));
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(
			_T("Rotation mode"),
			pOGLRenderer->_getRotationMode() == enumRotationMode::XY ? ROTATION_MODE_XY : ROTATION_MODE_XYZ,
			_T("XY/XYZ"),
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::RotationMode));
		pProperty->AddOption(ROTATION_MODE_XY);
		pProperty->AddOption(ROTATION_MODE_XYZ);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Show Coordinate System"),
			pOGLRenderer->getShowCoordinateSystem() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Show Coordinate System"),
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowCoordinateSystem));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Model Coordinate System"),
			getRDFController()->getModelCoordinateSystem() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Model Coordinate System"),
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::CoordinateSystemType));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}

	{
		auto pProperty = new CApplicationProperty(_T("Navigator"),
			pOGLRenderer->getShowNavigator() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY, _T("Navigator"),
			(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ShowNavigator));
		pProperty->AddOption(TRUE_VALUE_PROPERTY);
		pProperty->AddOption(FALSE_VALUE_PROPERTY);
		pProperty->AllowEdit(FALSE);

		pViewGroup->AddSubItem(pProperty);
	}	
#pragma endregion

#pragma region UI
	{
		auto pUI = new CMFCPropertyGridProperty(_T("UI"));
		pViewGroup->AddSubItem(pUI);

		// Visible values count limit
		{
			auto pProperty = new CApplicationProperty(_T("Visible values count limit"), 
				(_variant_t)getRDFController()->getVisibleValuesCountLimit(), 
				_T("Visible values count limit"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::VisibleValuesCountLimit));
			pProperty->EnableSpinControl(TRUE, 1, INT_MAX);

			pUI->AddSubItem(pProperty);
		}

		// Scale and Center
		{
			auto pProperty = new CApplicationProperty(_T("Scale and Center all Visible Geometry"), 
				getRDFController()->getScaleAndCenter() ? TRUE_VALUE_PROPERTY : FALSE_VALUE_PROPERTY,
				_T("Scale and Center all Visible Geometry"), 
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::ScalelAndCenter));
			pProperty->AddOption(TRUE_VALUE_PROPERTY);
			pProperty->AddOption(FALSE_VALUE_PROPERTY);
			pProperty->AllowEdit(FALSE);

			pUI->AddSubItem(pProperty);
		}
	}	
#pragma endregion

#pragma region OpenGL
	auto pBlinnPhongProgram = pOGLRenderer->_getOGLProgramAs<_oglBlinnPhongProgram>();
	if (pBlinnPhongProgram != nullptr)
	{
		auto pOpenGL = new CMFCPropertyGridProperty(_T("OpenGL"));
		pViewGroup->AddSubItem(pOpenGL);

#pragma region Point light position
		{
			auto pPointLightingLocation = new CApplicationProperty(_T("Point lighting location"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::PointLightingLocation), TRUE);
			pPointLightingLocation->AllowEdit(FALSE);

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("X"),
					(_variant_t)pBlinnPhongProgram->_getPointLightingLocation().x,
					_T("X"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::PointLightingLocation));
				pPointLightingLocation->AddSubItem(pProperty);
			}

			// Y
			{
				auto pProperty = new CApplicationProperty(
					_T("Y"),
					(_variant_t)pBlinnPhongProgram->_getPointLightingLocation().y,
					_T("Y"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::PointLightingLocation));
				pPointLightingLocation->AddSubItem(pProperty);
			}

			// Z
			{
				auto pProperty = new CApplicationProperty(
					_T("Z"),
					(_variant_t)pBlinnPhongProgram->_getPointLightingLocation().z,
					_T("Z"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::PointLightingLocation));
				pPointLightingLocation->AddSubItem(pProperty);
			}

			pOpenGL->AddSubItem(pPointLightingLocation);
		}
#pragma endregion

#pragma region Ambient light weighting
		{
			auto pAmbientLightWeighting = new CApplicationProperty(_T("Ambient light weighting"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::AmbientLightWeighting), TRUE);
			pAmbientLightWeighting->AllowEdit(FALSE);

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("X"),
					(_variant_t)pBlinnPhongProgram->_getAmbientLightWeighting().x,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::AmbientLightWeighting));
				pAmbientLightWeighting->AddSubItem(pProperty);
			}

			// Y
			{
				auto pProperty = new CApplicationProperty(
					_T("Y"),
					(_variant_t)pBlinnPhongProgram->_getAmbientLightWeighting().y,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::AmbientLightWeighting));
				pAmbientLightWeighting->AddSubItem(pProperty);
			}

			// Z
			{
				auto pProperty = new CApplicationProperty(
					_T("Z"),
					(_variant_t)pBlinnPhongProgram->_getAmbientLightWeighting().z,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::AmbientLightWeighting));
				pAmbientLightWeighting->AddSubItem(pProperty);
			}

			pOpenGL->AddSubItem(pAmbientLightWeighting);
		}
#pragma endregion

#pragma region Diffuse light weighting
		{
			auto pDiffuseLightWeighting = new CApplicationProperty(_T("Diffuse light weighting"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::DiffuseLightWeighting), TRUE);
			pDiffuseLightWeighting->AllowEdit(FALSE);

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("X"),
					(_variant_t)pBlinnPhongProgram->_getDiffuseLightWeighting().x,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::DiffuseLightWeighting));
				pDiffuseLightWeighting->AddSubItem(pProperty);
			}

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("Y"),
					(_variant_t)pBlinnPhongProgram->_getDiffuseLightWeighting().y,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::DiffuseLightWeighting));
				pDiffuseLightWeighting->AddSubItem(pProperty);
			}

			// Z
			{
				auto pProperty = new CApplicationProperty(
					_T("Z"),
					(_variant_t)pBlinnPhongProgram->_getDiffuseLightWeighting().z,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::DiffuseLightWeighting));
				pDiffuseLightWeighting->AddSubItem(pProperty);
			}

			pOpenGL->AddSubItem(pDiffuseLightWeighting);
		}
#pragma endregion

#pragma region Specular light weighting
		{
			auto pSpecularLightWeighting = new CApplicationProperty(_T("Specular light weighting"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::SpecularLightWeighting), TRUE);
			pSpecularLightWeighting->AllowEdit(FALSE);

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("X"),
					(_variant_t)pBlinnPhongProgram->_getSpecularLightWeighting().x,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::SpecularLightWeighting));
				pSpecularLightWeighting->AddSubItem(pProperty);
			}

			// X
			{
				auto pProperty = new CApplicationProperty(
					_T("Y"),
					(_variant_t)pBlinnPhongProgram->_getSpecularLightWeighting().y,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::SpecularLightWeighting));
				pSpecularLightWeighting->AddSubItem(pProperty);
			}

			// Z
			{
				auto pProperty = new CApplicationProperty(
					_T("Z"),
					(_variant_t)pBlinnPhongProgram->_getSpecularLightWeighting().z,
					_T("[0.0 - 1.0]"),
					(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::SpecularLightWeighting));
				pSpecularLightWeighting->AddSubItem(pProperty);
			}

			pOpenGL->AddSubItem(pSpecularLightWeighting);
		}
#pragma endregion

#pragma region Material shininess
		{
			auto pMaterialShininess = new CApplicationProperty(
				_T("Material shininess"),
				(_variant_t)pBlinnPhongProgram->_getMaterialShininess(),
				_T("[0.0 - 1.0]"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::MaterialShininess));

			pOpenGL->AddSubItem(pMaterialShininess);
		}
#pragma endregion

#pragma region Contrast
		{
			auto pContrast = new CApplicationProperty(
				_T("Contrast"),
				(_variant_t)pBlinnPhongProgram->_getContrast(),
				_T("Contrast"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::Contrast));

			pOpenGL->AddSubItem(pContrast);
		}
#pragma endregion

#pragma region Brightness
		{
			auto pBrightness = new CApplicationProperty(
				_T("Brightness"),
				(_variant_t)pBlinnPhongProgram->_getBrightness(),
				_T("Brightness"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::Brightness));

			pOpenGL->AddSubItem(pBrightness);
		}
#pragma endregion

#pragma region Gamma
		{
			auto pGamma = new CApplicationProperty(
				_T("Gamma"),
				(_variant_t)pBlinnPhongProgram->_getGamma(),
				_T("Gamma"),
				(DWORD_PTR)new CApplicationPropertyData(enumApplicationProperty::Gamma));

			pOpenGL->AddSubItem(pGamma);
		}
#pragma endregion
	} // if (pBlinnPhongProgram != nullptr)
#pragma endregion

	m_wndPropList.AddProperty(pViewGroup);
}

void CPropertiesWnd::LoadInstanceProperties()
{
	return;//#todo
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	if (getRDFController() == nullptr)
	{
		ASSERT(false);

		return;
	}

	/*
	* One property
	*/
	//#todo
	//auto prSelectedInstanceProperty = getRDFController()->GetSelectedInstanceProperty();
	//if ((prSelectedInstanceProperty.first != nullptr) && (prSelectedInstanceProperty.second != nullptr))
	//{
	//	auto pInstance = prSelectedInstanceProperty.first;
	//	auto pProperty = prSelectedInstanceProperty.second;

	//	auto pInstanceGroup = new CMFCPropertyGridProperty(pInstance->getUniqueName());
	//	
	//	wstring strAncestors = CRDFClass::GetAncestors(pInstance->getGeometry()->getClassInstance());
	//	pInstanceGroup->SetDescription(strAncestors.c_str());

	//	AddInstanceProperty(pInstanceGroup, pInstance, pProperty);

	//	m_wndPropList.AddProperty(pInstanceGroup);

	//	return;		
	//} // if ((prSelectedInstanceProperty.first != nullptr) && ...

	/*
	* All properties
	*/
	auto pInstance = getRDFController()->getSelectedInstance();
	if (pInstance != nullptr)
	{
		auto pModel = getRDFController()->getModel();
		if (pModel == nullptr)
		{
			ASSERT(false);

			return;
		}

		_ptr<_rdf_instance> rdfInstance(pInstance);

		auto& mapProperties = _ptr<CRDFModel>(pModel)->GetProperties();

		auto pInstanceGroup = new CMFCPropertyGridProperty(pInstance->getUniqueName());

		wstring strAncestors = CRDFClass::GetAncestors(pInstance->getGeometry()->getClassInstance());
		pInstanceGroup->SetDescription(strAncestors.c_str());

		/*
		* ColorComponent
		*/
		if (pInstance->getGeometry()->getClassInstance() == GetClassByName(pModel->getOwlModel(), "ColorComponent"))
		{
			/*
			* R
			*/
			int64_t iRProperty = GetPropertyByName(pModel->getOwlModel(), "R");

			int64_t iCard = 0;
			double * pdRValue = nullptr;
			GetDatatypeProperty(pInstance->getOwlInstance(), iRProperty, (void **)&pdRValue, &iCard);


			/*
			* G
			*/
			int64_t iGProperty = GetPropertyByName(pModel->getOwlModel(), "G");

			iCard = 0;
			double * pdGValue = nullptr;
			GetDatatypeProperty(pInstance->getOwlInstance(), iGProperty, (void **)&pdGValue, &iCard);

			/*
			* B
			*/
			int64_t iBProperty = GetPropertyByName(pModel->getOwlModel(), "B");

			iCard = 0;
			double * pdBValue = nullptr;
			GetDatatypeProperty(pInstance->getOwlInstance(), iBProperty, (void **)&pdBValue, &iCard);

			CRDFColorSelectorProperty * pColorSelectorProperty = new CRDFColorSelectorProperty(_T("Color"), RGB((BYTE)(*pdRValue * 255.), (BYTE)(*pdGValue * 255.), (BYTE)(*pdBValue * 255.)), nullptr, _T("Color"),
				(DWORD_PTR)new CRDFInstanceData(getRDFController(), rdfInstance));
			pColorSelectorProperty->EnableOtherButton(_T("Other..."));
			pColorSelectorProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
			pInstanceGroup->AddSubItem(pColorSelectorProperty);
		} // if (pInstance->GetClassInstance() == ...

		int64_t iPropertyInstance = GetInstancePropertyByIterator(pInstance->getOwlInstance(), 0);
		while (iPropertyInstance != 0)
		{
			map<int64_t, CRDFProperty *>::const_iterator itProperty = mapProperties.find(iPropertyInstance);
			ASSERT(itProperty != mapProperties.end());

			CRDFProperty * pProperty = itProperty->second;

			AddInstanceProperty(pInstanceGroup, rdfInstance, pProperty);

			iPropertyInstance = GetInstancePropertyByIterator(pInstance->getOwlInstance(), iPropertyInstance);
		}

		m_wndPropList.AddProperty(pInstanceGroup);

		return;
	} // if (pInstance != nullptr)
}

void CPropertiesWnd::AddInstanceProperty(CMFCPropertyGridProperty* pInstanceGroup, _rdf_instance* pInstance, CRDFProperty* pProperty)
{
	auto pPropertyGroup = new CMFCPropertyGridProperty(pProperty->GetName());
	pInstanceGroup->AddSubItem(pPropertyGroup);

	/*
	* range
	*/
	if (pProperty->GetType() == OBJECTPROPERTY_TYPE)
	{
		ASSERT(getRDFController() != nullptr);

		CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
		ASSERT(pObjectRDFProperty != nullptr);

		wstring strRange;

		auto vecRestrictions = pObjectRDFProperty->GetRestrictions();
		for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
		{
			char * szClassName = nullptr;
			GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

			if (!strRange.empty())
			{
				strRange += L"; ";
			}

			strRange += CA2W(szClassName);
		}

		auto pRange = new CMFCPropertyGridProperty(L"rdfs:range", (_variant_t)strRange.c_str(), pProperty->GetName());
		pRange->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pRange);
	} // if (pProperty->getType() == OBJECTPROPERTY_TYPE)
	else
	{
		auto pRange = new CMFCPropertyGridProperty(L"rdfs:range", (_variant_t)pProperty->GetRange().c_str(), pProperty->GetName());
		pRange->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pRange);
	}	

	/*
	* cardinality
	*/
	AddInstancePropertyCardinality(pPropertyGroup, pInstance, pProperty);

	/*
	* values
	*/
	AddInstancePropertyValues(pPropertyGroup, pInstance, pProperty);	
}

void CPropertiesWnd::AddInstancePropertyCardinality(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, CRDFProperty* pProperty)
{
	switch (pProperty->GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		int64_t * piObjectInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), &piObjectInstances, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case OBJECTPROPERTY_TYPE:
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		int64_t iCard = 0;
		bool* pbValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&pbValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty* pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_BOOLEAN:
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		int64_t iCard = 0;
		char ** szValue = nullptr;
		SetCharacterSerialization(pInstance->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&szValue, &iCard);
		SetCharacterSerialization(pInstance->getOwlModel(), 0, 0, true);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_STRING:
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		int64_t iCard = 0;
		char** szValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty* pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty* pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&pdValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&piValue, &iCard);

		/*
		* owl:cardinality
		*/
		wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

		CAddRDFInstanceProperty * pCardinality = new CAddRDFInstanceProperty(L"owl:cardinality", (_variant_t)strCardinality.c_str(), pProperty->GetName(),
			(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iCard));
		pCardinality->AllowEdit(FALSE);

		pPropertyGroup->AddSubItem(pCardinality);
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unknown property
	}
	break;
	} // switch (pProperty->getType())
}

void CPropertiesWnd::AddInstancePropertyValues(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, CRDFProperty* pProperty)
{
	switch (pProperty->GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{	
		int64_t * piInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), &piInstances, &iCard);

		//#todo
		//if (iCard > 0)
		//{
		//	int64_t	iMinCard = 0;
		//	int64_t iMaxCard = 0;
		//	pProperty->GetRestrictions(pInstance->getOwlInstance(), iMinCard, iMaxCard);

		//	if ((iMinCard == -1) && (iMaxCard == -1))
		//	{
		//		iMinCard = 0;
		//	}

		//	ASSERT(getRDFController() != nullptr);

		//	CRDFModel * pModel = getRDFController()->GetModel();
		//	ASSERT(pModel != nullptr);

		//	auto& mapInstances = pModel->GetInstances();

		//	/*
		//	* Compatible instances
		//	*/
		//	// Moved in CSelectInstanceDialog
		//	//CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
		//	//ASSERT(pObjectRDFProperty != nullptr);

		//	// Moved in CSelectInstanceDialog
		//	//vector<int64_t> vecCompatibleInstances;
		//	//pModel->GetCompatibleInstances(pInstance, pObjectRDFProperty, vecCompatibleInstances);

		//	int64_t iValuesCount = iCard;
		//	for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
		//	{
		//		CRDFInstanceObjectProperty * pInstanceObjectProperty = nullptr;

		//		if (piInstances[iValue] != 0)
		//		{
		//			map<int64_t, _rdf_instance *>::const_iterator itInstanceValue = mapInstances.find(piInstances[iValue]);
		//			ASSERT(itInstanceValue != mapInstances.end());

		//			pInstanceObjectProperty = new CRDFInstanceObjectProperty(L"value", (_variant_t)itInstanceValue->second->getUniqueName(), pProperty->GetName(),
		//				(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));
		//		}
		//		else
		//		{
		//			pInstanceObjectProperty = new CRDFInstanceObjectProperty(L"value", (_variant_t)EMPTY_INSTANCE, pProperty->GetName(),
		//				(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));
		//		}

		//		/*
		//		* Empty command
		//		*/
		//		pInstanceObjectProperty->AddOption(EMPTY_INSTANCE);
		//		pInstanceObjectProperty->AddValue(EMPTY_INSTANCE, 0);

		//		/*
		//		* Remove command
		//		*/
		//		if (iCard > iMinCard)
		//		{
		//			pInstanceObjectProperty->AddOption(REMOVE_OBJECT_PROPERTY_COMMAND);
		//		}				

		//		/*
		//		* Select
		//		*/
		//		pInstanceObjectProperty->AddOption(SELECT_OBJECT_PROPERTY_COMMAND);

		//		pInstanceObjectProperty->AllowEdit(FALSE);

		//		pPropertyGroup->AddSubItem(pInstanceObjectProperty);

		//		if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
		//		{
		//			break;
		//		}
		//	} // for (int64_t iValue = ...

		//	if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
		//	{
		//		auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
		//		pGridProperty->AllowEdit(FALSE);

		//		pPropertyGroup->AddSubItem(pGridProperty);
		//	}
		//} // if (iCard > 0)
	} // case OBJECTPROPERTY_TYPE:
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		int64_t iCard = 0;
		bool* pbValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&pbValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty* pInstanceProperty = new CRDFInstanceProperty(L"value", (_variant_t)pbValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));

				pPropertyGroup->AddSubItem(pInstanceProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case DATATYPEPROPERTY_TYPE_BOOLEAN:
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		int64_t iCard = 0;
		wchar_t ** szValue = nullptr;
		SetCharacterSerialization(pInstance->getOwlModel(), 0, 0, false);
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&szValue, &iCard);
		SetCharacterSerialization(pInstance->getOwlModel(), 0, 0, true);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pInstancProperty = new CRDFInstanceProperty(L"value", (_variant_t)szValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));

				pPropertyGroup->AddSubItem(pInstancProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		int64_t iCard = 0;
		char ** szValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty* pInstancProperty = new CRDFInstanceProperty(L"value", (_variant_t)szValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));

				pPropertyGroup->AddSubItem(pInstancProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		int64_t iCard = 0;
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty* pInstancProperty = new CRDFInstanceProperty(L"value", (_variant_t)szValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));

				pPropertyGroup->AddSubItem(pInstancProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case TYPE_CHAR_DATATYPE:
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		int64_t iCard = 0;
		double * pdValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&pdValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pInstanceProperty = new CRDFInstanceProperty(L"value", (_variant_t)pdValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));

				pPropertyGroup->AddSubItem(pInstanceProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case DATATYPEPROPERTY_TYPE_DOUBLE:
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		int64_t iCard = 0;
		int64_t * piValue = nullptr;
		GetDatatypeProperty(pInstance->getOwlInstance(), pProperty->GetInstance(), (void **)&piValue, &iCard);

		if (iCard > 0)
		{
			int64_t iValuesCount = iCard;
			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				CRDFInstanceProperty * pInstanceProperty = new CRDFInstanceProperty(L"value", (_variant_t)piValue[iValue], pProperty->GetName(),
					(DWORD_PTR)new CRDFInstancePropertyData(getRDFController(), pInstance, pProperty, iValue));
				pInstanceProperty->EnableSpinControlInt64();

				pPropertyGroup->AddSubItem(pInstanceProperty);

				if ((iValue + 1) >= getRDFController()->getVisibleValuesCountLimit())
				{
					break;
				}
			} // for (int64_t iValue = ...

			if (iValuesCount > getRDFController()->getVisibleValuesCountLimit())
			{
				auto pGridProperty = new CMFCPropertyGridProperty(L"...", (_variant_t)L"...", pProperty->GetName());
				pGridProperty->AllowEdit(FALSE);

				pPropertyGroup->AddSubItem(pGridProperty);
			}
		} // if (iCard > 0)
	} // case DATATYPEPROPERTY_TYPE_INTEGER:
	break;

	default:
	{
		ASSERT(false); // unknown property
	}
	break;
	} // switch (pProperty->getType())
}

void CPropertiesWnd::LoadBaseInformation(_rdf_instance* pInstance)
{
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	if (pInstance == nullptr)
	{
		return;
	}

	wchar_t szBuffer[200];

	auto pBaseInfoGroup = new CMFCPropertyGridProperty(L"Base information");
	auto pInstanceGroup = new CMFCPropertyGridProperty(pInstance->getName());
	pBaseInfoGroup->AddSubItem(pInstanceGroup);

	/*
	* Bounding box min
	*/
	{
		auto pBBMin = pInstance->getGeometry()->getOriginalBBMin();

		swprintf(szBuffer, 100, 
			L"%.6f, %.6f, %.6f", 
			pBBMin->x, pBBMin->y, pBBMin->z);

		auto pItem = new CMFCPropertyGridProperty(L"Bounding box min", (_variant_t)szBuffer, L"Bounding box min");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}

	/*
	* Bounding box max
	*/
	{	
		auto pBBMax = pInstance->getGeometry()->getOriginalBBMax();

		swprintf(szBuffer, 100,
			L"%.6f, %.6f, %.6f",
			pBBMax->x, pBBMax->y, pBBMax->z);

		auto pItem = new CMFCPropertyGridProperty(L"Bounding box max", (_variant_t)szBuffer, L"Bounding box max");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}

	/*
	* AABB Bounding box min
	*/
	{
		auto pAABBBBMin = pInstance->getGeometry()->getAABBMin();

		swprintf(szBuffer, 100,
			L"%.6f, %.6f, %.6f",
			pAABBBBMin->x, pAABBBBMin->y, pAABBBBMin->z);

		auto pItem = new CMFCPropertyGridProperty(L"AABB Bounding box min", (_variant_t)szBuffer, L"AABB Bounding box min");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}

	/*
	* AABB Bounding box max
	*/
	{
		auto pAABBBBMax = pInstance->getGeometry()->getAABBMax();

		swprintf(szBuffer, 100,
			L"%.6f, %.6f, %.6f",
			pAABBBBMax->x, pAABBBBMax->y, pAABBBBMax->z);

		auto pItem = new CMFCPropertyGridProperty(L"AABB Bounding box max", (_variant_t)szBuffer, L"AABB Bounding box max");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}

	/*
	* Vertices
	*/
	{	
		swprintf(szBuffer, 100, L"%lld", pInstance->getGeometry()->getVerticesCount());

		auto pItem = new CMFCPropertyGridProperty(L"Number of vertices", (_variant_t)szBuffer, L"Number of vertices");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}	

	/*
	* Indices
	*/
	{	
		swprintf(szBuffer, 100, L"%lld", pInstance->getGeometry()->getIndicesCount());

		auto pItem = new CMFCPropertyGridProperty(L"Number of indices", (_variant_t)szBuffer, L"Number of indices");
		pItem->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pItem);
	}

	/*
	* Conceptual faces
	*/
	{
		swprintf(szBuffer, 100, L"%lld", pInstance->getGeometry()->getConceptualFacesCount());

		auto pProperty = new CMFCPropertyGridProperty(L"Number of conceptual faces", (_variant_t)szBuffer, L"Number of conceptual faces");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Triangles
	*/
	{
		int64_t iCount = 0;

		auto& vecTriangles = pInstance->getGeometry()->getTriangles();
		for (auto pTriangle : vecTriangles)
		{
			iCount += const_cast<_primitives*>(&pTriangle)->indicesCount();
		}

		swprintf(szBuffer, 100, L"%lld", iCount / 3);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of triangles", (_variant_t)szBuffer, L"Number of triangles");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Lines
	*/
	{
		int64_t iCount = 0;

		auto& vecLines = pInstance->getGeometry()->getLines();
		for (auto pLine : vecLines)
		{
			iCount += const_cast<_primitives*>(&pLine)->indicesCount();
		}

		swprintf(szBuffer, 100, L"%lld", iCount / 2);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of lines", (_variant_t)szBuffer, L"Number of lines");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Points
	*/
	{
		int64_t iCount = 0;

		auto& vecPoints = pInstance->getGeometry()->getPoints();
		for (auto pPoint : vecPoints)
		{
			iCount += const_cast<_primitives*>(&pPoint)->indicesCount();
		}

		swprintf(szBuffer, 100, L"%lld", iCount);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of points", (_variant_t)szBuffer, L"Number of points");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Area
	*/
	{
		swprintf(szBuffer, 100, L"%.6f", GetArea(pInstance->getOwlInstance(), nullptr, nullptr));

		auto pProperty = new CMFCPropertyGridProperty(L"Area", (_variant_t)szBuffer, L"Area");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Perimeter
	*/
	{
		swprintf(szBuffer, 100, L"%.6f", GetPerimeter(pInstance->getOwlInstance()));

		auto pProperty = new CMFCPropertyGridProperty(L"Perimeter", (_variant_t)szBuffer, L"Perimeter");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Centroid, Volume
	*/
	{
		double arCentroid[3];
		double dVolume = GetCentroid(pInstance->getOwlInstance(), nullptr, nullptr, arCentroid);

		swprintf(szBuffer, 100, L"%.6f, %.6f, %.6f", arCentroid[0], arCentroid[1], arCentroid[2]);

		auto pCentroid = new CMFCPropertyGridProperty(L"Centroid", (_variant_t)szBuffer, L"Centroid");
		pCentroid->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pCentroid);

		swprintf(szBuffer, 100, L"%.6f", dVolume);

		auto pVolume = new CMFCPropertyGridProperty(L"Volume", (_variant_t)szBuffer, L"Volume");
		pVolume->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pVolume);
	}

	m_wndPropList.AddProperty(pBaseInfoGroup);
}

void CPropertiesWnd::LoadMetaInformation(_rdf_instance* pInstance)
{
	m_wndPropList.RemoveAll();
	m_wndPropList.AdjustLayout();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	if (pInstance == nullptr)
	{
		return;
	}

	auto pMetaInfoGroup = new CMFCPropertyGridProperty(L"Meta information");
	auto pInstanceGroup = new CMFCPropertyGridProperty(pInstance->getName());
	pMetaInfoGroup->AddSubItem(pInstanceGroup);

	/*
	* Centroid, Volume
	*/
	{
		double arCentroid[3];
		double dVolume = GetCentroid(pInstance->getOwlInstance(), nullptr, nullptr, arCentroid);

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f, %.6f, %.6f", arCentroid[0], arCentroid[1], arCentroid[2]);

		auto pCentroid = new CMFCPropertyGridProperty(L"Centroid", (_variant_t)szBuffer, L"Centroid");
		pCentroid->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pCentroid);

		swprintf(szBuffer, 100, L"%.6f", dVolume);

		auto pVolume = new CMFCPropertyGridProperty(L"Volume", (_variant_t)szBuffer, L"Volume");
		pVolume->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pVolume);
	}	

	/*
	* Area
	*/
	{
		double dArea = GetArea(pInstance->getOwlInstance(), nullptr, nullptr);

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f", dArea);

		auto pProperty = new CMFCPropertyGridProperty(L"Area", (_variant_t)szBuffer, L"Area");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Perimeter
	*/
	{
		double dPerimeter = GetPerimeter(pInstance->getOwlInstance());

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%.6f", dPerimeter);

		auto pProperty = new CMFCPropertyGridProperty(L"Perimeter", (_variant_t)szBuffer, L"Perimeter");
		pProperty->AllowEdit(FALSE);

		pInstanceGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry
	*/
	auto pGeometryGroup = new CMFCPropertyGridProperty(L"Geometry");
	pMetaInfoGroup->AddSubItem(pGeometryGroup);

	/*
	* Geometry - Number of conceptual faces
	*/
	{
		int64_t iConceptualFacesCount = pInstance->getGeometry()->getConceptualFacesCount();

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iConceptualFacesCount);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of conceptual faces", (_variant_t)szBuffer, L"Number of conceptual faces");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}


	/*
	* Geometry - Number of triangles
	*/
	{
		int64_t iTrianglesCount = 0;
		for (size_t iTriangle = 0; iTriangle < pInstance->getGeometry()->getTriangles().size(); iTriangle++)
		{
			iTrianglesCount += const_cast<_primitives*>(&(pInstance->getGeometry()->getTriangles()[iTriangle]))->indicesCount() / 3;
		}

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iTrianglesCount);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of triangles", (_variant_t)szBuffer, L"Number of triangles");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry - Number of lines
	*/
	{
		int64_t iLinesCount = 0;
		for (size_t iLine = 0; iLine < pInstance->getGeometry()->getLines().size(); iLine++)
		{
			iLinesCount += const_cast<_primitives*>(&(pInstance->getGeometry()->getLines()[iLine]))->indicesCount() / 2;
		}

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iLinesCount);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of lines", (_variant_t)szBuffer, L"Number of lines");
		pProperty->AllowEdit(FALSE);

		pGeometryGroup->AddSubItem(pProperty);
	}

	/*
	* Geometry - Number of points
	*/
	{
		int64_t iPointsCount = pInstance->getGeometry()->getPoints().size();

		wchar_t szBuffer[100];
		swprintf(szBuffer, 100, L"%lld", iPointsCount);

		auto pProperty = new CMFCPropertyGridProperty(L"Number of points", (_variant_t)szBuffer, L"Number of points");
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
	auto pConsistencyGroup = new CMFCPropertyGridProperty(L"Consistency");
	pMetaInfoGroup->AddSubItem(pConsistencyGroup);

	/*
	*   bit 0:   Check Design Tree Consistency
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(0)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Design Tree Consistency", (_variant_t)szResult, L"Check Design Tree Consistency");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 1:   Check Consistency for Triangle Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(1)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Triangle Output (through API)", (_variant_t)szResult, L"Check Consistency for Triangle Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 2:   Check Consistency for Line Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(2)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Line Output (through API)", (_variant_t)szResult, L"Check Consistency for Line Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 3:   Check Consistency for Point Output (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(3)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Point Output (through API)", (_variant_t)szResult, L"Check Consistency for Point Output (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 4:   Check Consistency for Generated Surfaces (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(4)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Surfaces (through API)", (_variant_t)szResult, L"Check Consistency for Generated Surfaces (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 5:   Check Consistency for Generated Surfaces (internal)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(5)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Surfaces (internal)", (_variant_t)szResult, L"Check Consistency for Generated Surfaces (internal)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 6:   Check Consistency for Generated Solids (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(6)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Solids (through API)", (_variant_t)szResult, L"Check Consistency for Generated Solids (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 7:   Check Consistency for Generated Solids (internal)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(7)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Generated Solids (internal)", (_variant_t)szResult, L"Check Consistency for Generated Solids (internal)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 8:   Check Consistency for BoundingBox's
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(8)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for BoundingBox's", (_variant_t)szResult, L"Check Consistency for BoundingBox's");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 9:   Check Consistency for Triangulation
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(9)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Triangulation", (_variant_t)szResult, L"Check Consistency for Triangulation");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 10:  Check Consistency for Relations (through API)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(10)) > 0 ? L"FAILED" : L"OK";

		auto pProperty = new CMFCPropertyGridProperty(L"Check Consistency for Relations (through API)", (_variant_t)szResult, L"Check Consistency for Relations (through API)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 16:   Contains (Closed) Solid(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(16)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Solid(s)", (_variant_t)szResult, L"Contains (Closed) Solid(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 18:   Contains (Closed) Infinite Solid(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(18)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Infinite Solid(s)", (_variant_t)szResult, L"Contains (Closed) Infinite Solid(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 20:   Contains Closed Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(20)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Closed Surface(s)", (_variant_t)szResult, L"Contains Closed Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 21:   Contains Open Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(21)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Open Surface(s)", (_variant_t)szResult, L"Contains Open Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 22:   Contains Closed Infinite Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(22)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Closed Infinite Surface(s)", (_variant_t)szResult, L"Contains Closed Infinite Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 23:   Contains Open Infinite Surface(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(23)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Open Infinite Surface(s)", (_variant_t)szResult, L"Contains Open Infinite Surface(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 24:   Contains Closed Line(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(24)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Closed Line(s)", (_variant_t)szResult, L"Contains Closed Line(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 25:   Contains Open Line(s)
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(25)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Open Line(s)", (_variant_t)szResult, L"Contains Open Line(s)");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 26:   Contains Closed Infinite Line(s) [i.e. both ends in infinity]
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(26)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Closed Infinite Line(s)", (_variant_t)szResult, L"Contains Closed Infinite Line(s) [i.e. both ends in infinity]");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 27:   Contains Open Infinite Line(s) [i.e. one end in infinity]
	*/
	{
		wchar_t* szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(27)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains Open Infinite Line(s)", (_variant_t)szResult, L"Contains Open Infinite Line(s) [i.e. one end in infinity]");
		pProperty->AllowEdit(FALSE);

		pConsistencyGroup->AddSubItem(pProperty);
	}

	/*
	*   bit 28:   Contains (Closed) Point(s)
	*/
	{
		wchar_t * szResult = CheckInstanceConsistency(pInstance->getOwlInstance(), FLAGBIT(28)) > 0 ? L"YES" : L"-";

		auto pProperty = new CMFCPropertyGridProperty(L"Contains (Closed) Point(s)", (_variant_t)szResult, L"Contains (Closed) Point(s)");
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
	}
}

LRESULT CPropertiesWnd::OnLoadInstancePropertyValues(WPARAM wParam, LPARAM /*lParam*/)
{
	ASSERT(wParam != 0);	

	auto pValueProperty = (CMFCPropertyGridProperty*)wParam;
	auto pValueData = (CRDFInstancePropertyData*)pValueProperty->GetData();
	ASSERT(pValueData != nullptr);

	auto pInstance = pValueData->GetInstance();
	auto pProperty = pValueData->GetProperty();

	/*
	* Select the property
	*/
	auto pPropertyGroup = pValueProperty->GetParent();	
	m_wndPropList.SetCurSel(pPropertyGroup);

	/*
	* Update the cardinality
	*/
	auto pCardinalityProperty = (CAddRDFInstanceProperty*)pPropertyGroup->GetSubItem(1);

	wstring strCardinality = pProperty->GetCardinality(pInstance->getOwlInstance());

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
		auto pValue = pPropertyGroup->GetSubItem(2);

		m_wndPropList.DeleteProperty(pValue);
	}	

	/*
	* Add the values
	*/
	AddInstancePropertyValues(pPropertyGroup, pInstance, pProperty);

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
	getRDFController()->unRegisterView(this);

	__super::OnDestroy();
}
