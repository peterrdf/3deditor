#include "StdAfx.h"
#include "BinnPhongGLProgram.h"

// ------------------------------------------------------------------------------------------------
CBinnPhongGLProgram::CBinnPhongGLProgram(void)
	: CGLProgram()
	, m_iUseBinnPhongModel(-1)
	, m_iUseTexture(-1)
	, m_iMVMatrix(-1)
	, m_iPMatrix(-1)
	, m_iNMatrix(-1)
	, m_iPointLightingLocation(-1)
	, m_iMaterialShininess(-1)
	, m_iMaterialAmbientColor(-1)
	, m_iTransparency(-1)
	, m_iMaterialDiffuseColor(-1)
	, m_iMaterialSpecularColor(-1)
	, m_iMaterialEmissiveColor(-1)
	, m_iVertexPosition(-1)
	, m_iVertexNormal(-1)
	, m_iTextureCoord(-1)
{
}

// ------------------------------------------------------------------------------------------------
CBinnPhongGLProgram::~CBinnPhongGLProgram(void)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ bool CBinnPhongGLProgram::Link()
{
	if (CGLProgram::Link())
	{
		m_iUseBinnPhongModel = glGetUniformLocation(m_ID, "uUseBinnPhongModel");
		ASSERT(m_iUseBinnPhongModel >= 0);

		m_iUseTexture = glGetUniformLocation(m_ID, "uUseTexture");
		ASSERT(m_iUseTexture >= 0);

		m_iMVMatrix = glGetUniformLocation(m_ID, "uMVMatrix");
		ASSERT(m_iMVMatrix >= 0);

		m_iPMatrix = glGetUniformLocation(m_ID, "uPMatrix");
		ASSERT(m_iPMatrix >= 0);

		m_iNMatrix = glGetUniformLocation(m_ID, "uNMatrix");
		ASSERT(m_iNMatrix >= 0);

		m_iPointLightingLocation = glGetUniformLocation(m_ID, "uPointLightingLocation");
		ASSERT(m_iPointLightingLocation >= 0);

		m_iMaterialShininess = glGetUniformLocation(m_ID, "uMaterialShininess");
		ASSERT(m_iMaterialShininess >= 0);

		m_iMaterialAmbientColor = glGetUniformLocation(m_ID, "uMaterialAmbientColor");
		ASSERT(m_iMaterialAmbientColor >= 0);

		m_iTransparency = glGetUniformLocation(m_ID, "uTransparency");
		ASSERT(m_iTransparency >= 0);

		m_iMaterialDiffuseColor = glGetUniformLocation(m_ID, "uMaterialDiffuseColor");
		ASSERT(m_iMaterialDiffuseColor >= 0);

		m_iMaterialSpecularColor = glGetUniformLocation(m_ID, "uMaterialSpecularColor");
		ASSERT(m_iMaterialSpecularColor >= 0);

		m_iMaterialEmissiveColor = glGetUniformLocation(m_ID, "uMaterialEmissiveColor");
		ASSERT(m_iMaterialEmissiveColor >= 0);

		m_iVertexPosition = glGetAttribLocation(m_ID, "aVertexPosition");
		ASSERT(m_iVertexPosition >= 0);

		m_iVertexNormal = glGetAttribLocation(m_ID, "aVertexNormal");
		ASSERT(m_iVertexNormal >= 0);

		m_iTextureCoord = glGetAttribLocation(m_ID, "aTextureCoord");
		ASSERT(m_iTextureCoord >= 0);

		return true;
	}
	
	return false; 
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::geUseBinnPhongModel() const
{
	return m_iUseBinnPhongModel;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::geUseTexture() const
{
	return m_iUseTexture;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMVMatrix() const
{
	return m_iMVMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getPMatrix() const
{
	return m_iPMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getNMatrix() const
{
	return m_iNMatrix;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getPointLightingLocation() const
{
	return m_iPointLightingLocation;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMaterialShininess() const
{
	return m_iMaterialShininess;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMaterialAmbientColor() const
{
	return m_iMaterialAmbientColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getTransparency() const
{
	return m_iTransparency;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMaterialDiffuseColor() const
{
	return m_iMaterialDiffuseColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMaterialSpecularColor() const
{
	return m_iMaterialSpecularColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getMaterialEmissiveColor() const
{
	return m_iMaterialEmissiveColor;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getVertexPosition() const
{
	return m_iVertexPosition;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getVertexNormal() const
{
	return m_iVertexNormal;
}

// ------------------------------------------------------------------------------------------------
GLint CBinnPhongGLProgram::getTextureCoord() const
{
	return m_iTextureCoord;
}