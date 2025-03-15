#include "stdafx.h"
#include "RDFView.h"
#include "RDFController.h"

// ------------------------------------------------------------------------------------------------
CRDFView::CRDFView()
	: _rdf_view()
	, m_pController(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CRDFView::~CRDFView()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstancePropertyEdited(CRDFInstance * /*pInstance*/, _rdf_property * /*pProperty*/)
{
}

// ------------------------------------------------------------------------------------------------
CRDFController * CRDFView::GetController() const
{
	return m_pController;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnControllerChanged()
{
}

