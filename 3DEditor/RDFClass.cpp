#include "stdafx.h"
#include "RDFClass.h"

#ifdef _LINUX
#include <wx/string.h>
#endif // _LINUX

// ------------------------------------------------------------------------------------------------
CRDFClass::CRDFClass(int64_t iInstance)
	: m_iInstance(iInstance)
	, m_strName(L"")
	, m_vecParentClasses()
	, m_vecAncestorClasses()
	, m_vecPropertyRestrictions()
{
	ASSERT(m_iInstance != 0);

	/*
	* Name
	*/
	wchar_t* szClassName = nullptr;
	GetNameOfClassW(m_iInstance, &szClassName);

#ifndef _LINUX
    m_strName = szClassName;
#else
	m_strName = wxString(szClassName).wchar_str();
#endif // _LINUX

	/*
	* Parents
	*/
	int64_t iParentClassInstance = GetClassParentsByIterator(m_iInstance, 0);
	while (iParentClassInstance != 0)
	{
		m_vecParentClasses.push_back(iParentClassInstance);

		m_vecAncestorClasses.push_back(iParentClassInstance);
		GetAncestors(iParentClassInstance, m_vecAncestorClasses);

		char * szParentClassName = nullptr;
		GetNameOfClass(iParentClassInstance, &szParentClassName);

		iParentClassInstance = GetClassParentsByIterator(m_iInstance, iParentClassInstance);
	}
}

// ------------------------------------------------------------------------------------------------
CRDFClass::~CRDFClass()
{
	for (size_t iProperty = 0; iProperty < m_vecPropertyRestrictions.size(); iProperty++)
	{
		delete m_vecPropertyRestrictions[iProperty];
	}
}

// ------------------------------------------------------------------------------------------------
// Getter
int64_t CRDFClass::GetInstance() const
{
	return m_iInstance;
}

// ------------------------------------------------------------------------------------------------
// Getter
const wchar_t * CRDFClass::GetName() const
{
	return m_strName.c_str();
}

// ------------------------------------------------------------------------------------------------
const vector<int64_t> & CRDFClass::getParentClasses()
{
	return m_vecParentClasses;
}

// ------------------------------------------------------------------------------------------------
const vector<int64_t> & CRDFClass::getAncestorClasses()
{
	return m_vecAncestorClasses;
}

// ------------------------------------------------------------------------------------------------
void CRDFClass::AddPropertyRestriction(CRDFPropertyRestriction * pPropertyRestriction)
{
	ASSERT(pPropertyRestriction != nullptr);

	char * szPropertyName = nullptr;
	GetNameOfProperty(pPropertyRestriction->getPropertyInstance(), &szPropertyName);

	m_vecPropertyRestrictions.push_back(pPropertyRestriction);
}

// ------------------------------------------------------------------------------------------------
const vector<CRDFPropertyRestriction *> & CRDFClass::getPropertyRestrictions()
{
	return m_vecPropertyRestrictions;
}

// ------------------------------------------------------------------------------------------------
void CRDFClass::GetAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestorClasses)
{
	int64_t iParentClassInstance = GetClassParentsByIterator(iClassInstance, 0);
	while (iParentClassInstance != 0)
	{
		vecAncestorClasses.push_back(iParentClassInstance);

		GetAncestors(iParentClassInstance, vecAncestorClasses);

		iParentClassInstance = GetClassParentsByIterator(iClassInstance, iParentClassInstance);
	}
}

/*static*/ wstring CRDFClass::GetAncestors(int64_t iClassInstance)
{
	vector<int64_t> vecAncestors;
	GetAncestors(iClassInstance, vecAncestors);

	wstring strAncestors;
	for (auto iAncestor : vecAncestors)
	{
		if (!strAncestors.empty())
		{
			strAncestors += L";";
		}

		wchar_t* szClassName = nullptr;
		GetNameOfClassW(iAncestor, &szClassName);

		strAncestors += szClassName;
	}

	return strAncestors;
}
