#include "stdafx.h"
#include "_node.h"

#include "_errors.h"

#include <memory>

// ************************************************************************************************
_node::_node(_geometry* pGeometry)
	: m_pGeometry(pGeometry)
	, m_iBufferByteLength(0)
	, m_strBufferBinFile("")
	, m_iVerticesBufferViewByteLength(0)
	, m_iNormalsBufferViewByteLength(0)
	, m_iTexturesBufferViewByteLength(0)
	, m_vecIndicesBufferViewsByteLength()
	, m_vecAccessors()
	, m_vecMeshes()
{
	VERIFY_POINTER(m_pGeometry);
}

/*virtual*/ _node::~_node()
{
}
