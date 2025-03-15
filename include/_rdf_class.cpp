#include "_host.h"
#include "_rdf_class.h"

// ************************************************************************************************
_rdf_class::_rdf_class(OwlClass owlClass)
	: m_owlClass(owlClass)
	, m_szName(nullptr)
	, m_vecParentClasses()
	, m_vecAncestorClasses()
	, m_vecPropertyRestrictions()
{
	assert(m_owlClass != 0);

	GetNameOfClassW(m_owlClass, &m_szName);

	OwlClass owlParentClass = GetClassParentsByIterator(m_owlClass, 0);
	while (owlParentClass != 0) {
		m_vecParentClasses.push_back(owlParentClass);

		m_vecAncestorClasses.push_back(owlParentClass);
		GetAncestors(owlParentClass, m_vecAncestorClasses);

		char* szParentClassName = nullptr;
		GetNameOfClass(owlParentClass, &szParentClassName);

		owlParentClass = GetClassParentsByIterator(m_owlClass, owlParentClass);
	}
}

_rdf_class::~_rdf_class()
{
	for (size_t iProperty = 0; iProperty < m_vecPropertyRestrictions.size(); iProperty++) {
		delete m_vecPropertyRestrictions[iProperty];
	}
}

void _rdf_class::addPropertyRestriction(_rdf_property_restriction* pPropertyRestriction)
{
	assert(pPropertyRestriction != nullptr);

	char* szPropertyName = nullptr;
	GetNameOfProperty(pPropertyRestriction->getRdfProperty(), &szPropertyName);

	m_vecPropertyRestrictions.push_back(pPropertyRestriction);
}

void _rdf_class::GetAncestors(OwlClass owlClass, vector<OwlClass>& vecAncestorClasses)
{
	OwlClass owlParentClass = GetClassParentsByIterator(owlClass, 0);
	while (owlParentClass != 0) {
		vecAncestorClasses.push_back(owlParentClass);

		GetAncestors(owlParentClass, vecAncestorClasses);

		owlParentClass = GetClassParentsByIterator(owlClass, owlParentClass);
	}
}

/*static*/ wstring _rdf_class::GetAncestors(OwlClass owlClass)
{
	vector<OwlClass> vecAncestors;
	GetAncestors(owlClass, vecAncestors);

	wstring strAncestors;
	for (auto iAncestor : vecAncestors) {
		if (!strAncestors.empty()) {
			strAncestors += L";";
		}

		wchar_t* szClassName = nullptr;
		GetNameOfClassW(iAncestor, &szClassName);

		strAncestors += szClassName;
	}

	return strAncestors;
}


// ************************************************************************************************
_rdf_property_restriction::_rdf_property_restriction(RdfProperty rdfProperty, int64_t iMinCard, int64_t iMaxCard)
	: m_rdfProperty(rdfProperty)
	, m_iMinCard(iMinCard)
	, m_iMaxCard(iMaxCard)
{
	assert(m_rdfProperty != 0);
}

_rdf_property_restriction::~_rdf_property_restriction()
{
}
