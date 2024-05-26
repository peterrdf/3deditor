#include "stdafx.h"
#include "RDFPropertyRestriction.h"

// ------------------------------------------------------------------------------------------------
CRDFPropertyRestriction::CRDFPropertyRestriction(int64_t iPropertyInstance, int64_t iMinCard, int64_t iMaxCard)
	: m_iPropertyInstance(iPropertyInstance)
	, m_iMinCard(iMinCard)
	, m_iMaxCard(iMaxCard)
{
	assert(m_iPropertyInstance != 0);
}

// ------------------------------------------------------------------------------------------------
CRDFPropertyRestriction::~CRDFPropertyRestriction()
{
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFPropertyRestriction::getPropertyInstance() const
{
	return m_iPropertyInstance;
}

// --------------------------------------------------------------------------------------------
// Getter
int64_t CRDFPropertyRestriction::getMinCard() const
{
	return m_iMinCard;
}

// --------------------------------------------------------------------------------------------
// Getter
int64_t CRDFPropertyRestriction::getMaxCard() const
{
	return m_iMaxCard;
}
