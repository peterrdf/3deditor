#include "stdafx.h"
#include "RDFClass.h"

// ************************************************************************************************
CRDFClass::CRDFClass(OwlClass iInstance)
	: m_iInstance(iInstance)
	, m_szName(nullptr)
	, m_vecParentClasses()
	, m_vecAncestorClasses()
	, m_vecPropertyRestrictions()
{
	assert(m_iInstance != 0);

	GetNameOfClassW(m_iInstance, &m_szName);

	OwlClass iParentClassInstance = GetClassParentsByIterator(m_iInstance, 0);
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

CRDFClass::~CRDFClass()
{
	for (size_t iProperty = 0; iProperty < m_vecPropertyRestrictions.size(); iProperty++)
	{
		delete m_vecPropertyRestrictions[iProperty];
	}
}

void CRDFClass::AddPropertyRestriction(CRDFPropertyRestriction * pPropertyRestriction)
{
	assert(pPropertyRestriction != nullptr);

	char* szPropertyName = nullptr;
	GetNameOfProperty(pPropertyRestriction->getPropertyInstance(), &szPropertyName);

	m_vecPropertyRestrictions.push_back(pPropertyRestriction);
}

void CRDFClass::GetAncestors(OwlClass iClassInstance, vector<OwlClass> & vecAncestorClasses)
{
	OwlClass iParentClassInstance = GetClassParentsByIterator(iClassInstance, 0);
	while (iParentClassInstance != 0)
	{
		vecAncestorClasses.push_back(iParentClassInstance);

		GetAncestors(iParentClassInstance, vecAncestorClasses);

		iParentClassInstance = GetClassParentsByIterator(iClassInstance, iParentClassInstance);
	}
}

/*static*/ wstring CRDFClass::GetAncestors(OwlClass iClassInstance)
{
	vector<OwlClass> vecAncestors;
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
