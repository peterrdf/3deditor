#include "stdafx.h"
#include "BinnPhongOGLPipeline.h"
#include "Resource.h"

// ------------------------------------------------------------------------------------------------
CBinnPhongOGLPipeline::CBinnPhongOGLPipeline()
	: COGLPipeline(IDR_TEXTFILE_VERTEX_SHADER2, IDR_TEXTFILE_FRAGMENT_SHADER2)
{
	m_iUseBinnPhongModel = glGetUniformLocation(getVertexProgram(), "uUseBinnPhongModel");
	ASSERT(m_iUseBinnPhongModel >= 0);

	m_iMVMatrix = glGetUniformLocation(getVertexProgram(), "uMVMatrix");
	ASSERT(m_iMVMatrix >= 0);

	m_iPMatrix = glGetUniformLocation(getVertexProgram(), "uPMatrix");
	ASSERT(m_iPMatrix >= 0);

	m_iNMatrix = glGetUniformLocation(getVertexProgram(), "uNMatrix");
	ASSERT(m_iNMatrix >= 0);

	m_iPointLightingLocation = glGetUniformLocation(getFragmentProgram(), "uPointLightingLocation");
	ASSERT(m_iPointLightingLocation >= 0);

	m_iMaterialShininess = glGetUniformLocation(getFragmentProgram(), "uMaterialShininess");
	ASSERT(m_iMaterialShininess >= 0);

	m_iMaterialAmbientColor = glGetUniformLocation(getFragmentProgram(), "uMaterialAmbientColor");
	ASSERT(m_iMaterialAmbientColor >= 0);

	m_iTransparency = glGetUniformLocation(getFragmentProgram(), "uTransparency");
	ASSERT(m_iTransparency >= 0);

	m_iMaterialDiffuseColor = glGetUniformLocation(getFragmentProgram(), "uMaterialDiffuseColor");
	ASSERT(m_iMaterialDiffuseColor >= 0);

	m_iMaterialSpecularColor = glGetUniformLocation(getFragmentProgram(), "uMaterialSpecularColor");
	ASSERT(m_iMaterialSpecularColor >= 0);

	m_iMaterialEmissiveColor = glGetUniformLocation(getFragmentProgram(), "uMaterialEmissiveColor");
	ASSERT(m_iMaterialEmissiveColor >= 0);

	m_iVertexPosition = glGetAttribLocation(getVertexProgram(), "aVertexPosition");
	ASSERT(m_iVertexPosition >= 0);

	m_iVertexNormal = glGetAttribLocation(getVertexProgram(), "aVertexNormal");
	ASSERT(m_iVertexNormal >= 0);
}

// ------------------------------------------------------------------------------------------------
CBinnPhongOGLPipeline::~CBinnPhongOGLPipeline()
{
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::geUseBinnPhongModel() const
{
	return m_iUseBinnPhongModel;
}
// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMVMatrix() const
{
	return m_iMVMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getPMatrix() const
{
	return m_iPMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getNMatrix() const
{
	return m_iNMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getPointLightingLocation() const
{
	return m_iPointLightingLocation;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMaterialShininess() const
{
	return m_iMaterialShininess;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMaterialAmbientColor() const
{
	return m_iMaterialAmbientColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getTransparency() const
{
	return m_iTransparency;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMaterialDiffuseColor() const
{
	return m_iMaterialDiffuseColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMaterialSpecularColor() const
{
	return m_iMaterialSpecularColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getMaterialEmissiveColor() const
{
	return m_iMaterialEmissiveColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getVertexPosition() const
{
	return m_iVertexPosition;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongOGLPipeline::getVertexNormal() const
{
	return m_iVertexNormal;
}