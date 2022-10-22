#include "stdafx.h"
#include "SimpleOGLPipeline.h"
#include "Resource.h"

// ------------------------------------------------------------------------------------------------
CSimpleOGLPipeline::CSimpleOGLPipeline()
	: COGLPipeline(IDR_TEXTFILE_VERTEX_SHADER2, IDR_TEXTFILE_FRAGMENT_SHADER2)
{
	m_iMVMatrix = glGetUniformLocation(getVertexProgram(), "uMVMatrix");
	ASSERT(m_iMVMatrix >= 0);

	m_iPMatrix = glGetUniformLocation(getVertexProgram(), "uPMatrix");
	ASSERT(m_iPMatrix >= 0);

	m_iMaterialAmbientColor = glGetUniformLocation(getFragmentProgram(), "uMaterialAmbientColor");
	ASSERT(m_iMaterialAmbientColor >= 0);

	m_iTransparency = glGetUniformLocation(getFragmentProgram(), "uTransparency");
	ASSERT(m_iTransparency >= 0);

	m_iVertexPosition = glGetAttribLocation(getVertexProgram(), "aVertexPosition");
	ASSERT(m_iVertexPosition >= 0);
}

// ------------------------------------------------------------------------------------------------
CSimpleOGLPipeline::~CSimpleOGLPipeline()
{
}

// ------------------------------------------------------------------------------------------------
GLint CSimpleOGLPipeline::getMVMatrix() const
{
	return m_iMVMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CSimpleOGLPipeline::getPMatrix() const
{
	return m_iPMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CSimpleOGLPipeline::getMaterialAmbientColor() const
{
	return m_iMaterialAmbientColor;
}

// ------------------------------------------------------------------------------------------------
GLint CSimpleOGLPipeline::getTransparency() const
{
	return m_iTransparency;
}

// ------------------------------------------------------------------------------------------------
GLint CSimpleOGLPipeline::getVertexPosition() const
{
	return m_iVertexPosition;
}