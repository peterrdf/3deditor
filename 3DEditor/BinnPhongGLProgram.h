#pragma once

#include "GLProgram.h"

class CBinnPhongGLProgram : public CGLProgram
{

private:

	bool m_bTextureSupport;
	GLint m_iUseBinnPhongModel;
	GLint m_iUseTexture;
	GLint m_iSampler;
	GLint m_iMVMatrix;
	GLint m_iPMatrix;
	GLint m_iNMatrix;
	GLint m_iPointLightingLocation;
	GLint m_iMaterialShininess;
	GLint m_iMaterialAmbientColor;
	GLint m_iTransparency;
	GLint m_iMaterialDiffuseColor;
	GLint m_iMaterialSpecularColor;
	GLint m_iMaterialEmissiveColor;
	GLint m_iVertexPosition;
	GLint m_iVertexNormal;
	GLint m_iTextureCoord;

public:
	
	CBinnPhongGLProgram(bool bTextureSupport);
	virtual ~CBinnPhongGLProgram(void);

	bool getTextureSupport() const;
	
	virtual bool Link();

	GLint geUseBinnPhongModel() const;

	GLint geUseTexture() const;
	GLint getSampler() const;

	GLint getMVMatrix() const;
	GLint getPMatrix() const;
	GLint getNMatrix() const;

	GLint getPointLightingLocation() const;

	GLint getMaterialShininess() const;
	GLint getMaterialAmbientColor() const;
	GLint getTransparency() const;
	GLint getMaterialDiffuseColor() const;
	GLint getMaterialSpecularColor() const;
	GLint getMaterialEmissiveColor() const;

	GLint getVertexPosition() const;
	GLint getVertexNormal() const;
	GLint getTextureCoord() const;
};
