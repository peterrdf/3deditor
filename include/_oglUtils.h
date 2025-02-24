#pragma once

#include "_mvc.h"
#include "_geometry.h"
#include "_quaterniond.h"

#include "glew.h"
#include "wglew.h"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <chrono>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#undef max
#include <limits>
#include <map>
using namespace std;

// ************************************************************************************************
static const int MIN_VIEW_PORT_LENGTH = 100;

// ************************************************************************************************
class _oglUtils
{

public: // Methods
	
	static GLsizei getVerticesCountLimit(GLint iVertexLengthBytes)
	{
		return numeric_limits<GLint>::max() / iVertexLengthBytes;
	}

	static GLsizei getIndicesCountLimit()
	{
		return numeric_limits<GLint>::max();
	}

#if defined _MFC_VER || defined _AFXDLL
	static void checkForErrors()
	{
		GLenum errLast = GL_NO_ERROR;

		for (;;)
		{
			GLenum err = glGetError();
			if (err == GL_NO_ERROR)
				return;

			// normally the error is reset by the call to glGetError() but if
			// glGetError() itself returns an error, we risk looping forever here
			// so check that we get a different error than the last time
			if (err == errLast)
			{
				::MessageBox(
					::AfxGetMainWnd()->GetSafeHwnd(),
					_T("OpenGL error state couldn't be reset."),
					_T("OpenGL"),
					MB_ICONERROR | MB_OK);

				PostQuitMessage(0);

				return;
			}

			errLast = err;

#ifdef UNICODE
			::MessageBoxW(
				::AfxGetMainWnd()->GetSafeHwnd(),
				gluErrorUnicodeStringEXT(errLast),
				_T("OpenGL"),
				MB_ICONERROR | MB_OK);
#else
			::MessageBoxA(
				::AfxGetMainWnd()->GetSafeHwnd(),
				(LPCSTR)gluErrorString(errLast),
				_T("OpenGL"),
				MB_ICONERROR | MB_OK);
#endif
			PostQuitMessage(0);
		}
	}
#endif // #if defined(_MFC_VER) || defined(_AFXDLL)
};

#if defined _MFC_VER || defined _AFXDLL
// ************************************************************************************************
class _oglShader
{

protected: // Members

	GLenum	m_iType; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_iID;
	char* m_szCode;

public: // Methods

	_oglShader(GLenum iShaderType)
		: m_iType(iShaderType)
		, m_szCode(nullptr)
	{
		m_iID = glCreateShader(m_iType);
		assert(m_iID != 0);
	}

	virtual ~_oglShader(void)
	{
		if (m_szCode)
		{
			delete[] m_szCode;
		}
			
		if (m_iID != 0)
		{
			glDeleteShader(m_iID);
		}		
	}

	GLuint _getID() const 
	{ 
		return m_iID; 
	}

	static char* getResource(int iResource, int iType)
	{
		HMODULE hModule = ::GetModuleHandleW(nullptr);
		HRSRC hResource = ::FindResourceW(hModule, MAKEINTRESOURCEW(iResource), MAKEINTRESOURCEW(iType));
		HGLOBAL rcData = ::LoadResource(hModule, hResource);

		char* szData = static_cast<char*>(::LockResource(rcData));
		DWORD dwSize = ::SizeofResource(hModule, hResource);

		char* szBuffer = new char[dwSize + 1];
		::memcpy(szBuffer, szData, dwSize);
		szBuffer[dwSize] = 0;

		return szBuffer;
	}

	bool load(int iResource, int iType)
	{
		m_szCode = getResource(iResource, iType);
		
		return (m_szCode != nullptr);
	}

	bool compile(void)
	{
		if (m_szCode == nullptr)
		{
			return false;
		}

		glShaderSource(m_iID, 1, &m_szCode, nullptr);
		glCompileShader(m_iID);

		int iParam;
		glGetShaderiv(m_iID, GL_COMPILE_STATUS, &iParam);

		if (iParam == GL_TRUE)
		{
			return true;
		}
			
		printInfoLog();

		return false;
	}	

	void getInfoLog(CString& strInfoLog)
	{
		strInfoLog = _T("NA");

		int iLength = 0;
		glGetShaderiv(m_iID, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 0)
		{
			int iCharsWritten = 0;
			char* szInfoLog = new char[iLength];

			glGetShaderInfoLog(m_iID, iLength, &iCharsWritten, szInfoLog);

			strInfoLog = szInfoLog;
			delete[] szInfoLog;
		}
	}

	void printInfoLog()
	{
		CString strInfoLog;
		getInfoLog(strInfoLog);

		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), strInfoLog, _T("Error"), MB_ICONERROR | MB_OK);
	}
};

// ************************************************************************************************
class _oglProgram
{

private: // Members

	GLuint m_iID;

public: // Methods

	_oglProgram(void)
	{
		m_iID = glCreateProgram();
		assert(m_iID > 0);
	}

	virtual ~_oglProgram()
	{
		if (m_iID != 0)
		{
			glDeleteProgram(m_iID);
		}		
	}

	GLuint _getID() const
	{
		return m_iID;
	}

	void _attachShader(_oglShader* pShader) const
	{
		glAttachShader(m_iID, pShader->_getID());
	}

	void _detachShader(_oglShader* pShader) const
	{
		glDetachShader(m_iID, pShader->_getID());
	}

	virtual bool _link()
	{
		glLinkProgram(m_iID);

		int param;
		glGetProgramiv(m_iID, GL_LINK_STATUS, &param);

		return param == GL_TRUE;
	}

	void _use() const
	{ 
		glUseProgram(m_iID); 
	}

protected: // Methods

	GLint _getUniformLocation(char* szName) const
	{
		return glGetUniformLocation(m_iID, szName);
	}

	void _setUniform(char* szName, int iVal) const
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		glUniform1i(iLocation, iVal);
	}

	void _setUniform(GLint iLocation, int iVal) const
	{
		glUniform1i(iLocation, iVal);
	}

	void _setUniform(char* szName, int* val, int varDim, int count) const
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		if (iLocation == -1)
		{
			assert(false);

			return;
		}			

		if (varDim == 4)
			glUniform4iv(iLocation, count, val);
		else if (varDim == 3)
			glUniform3iv(iLocation, count, val);
		else if (varDim == 2)
			glUniform2iv(iLocation, count, val);
		else if (varDim == 1)
			glUniform1iv(iLocation, count, val);
	}

	void _setUniform(GLint iLocation, int* val, int varDim, int count) const
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		if (varDim == 4)
			glUniform4iv(iLocation, count, val);
		else if (varDim == 3)
			glUniform3iv(iLocation, count, val);
		else if (varDim == 2)
			glUniform2iv(iLocation, count, val);
		else if (varDim == 1)
			glUniform1iv(iLocation, count, val);
	}

	void _setUniform(char* szName, float val) const
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glUniform1f(iLocation, val);
	}

	void _setUniform(GLint iLocation, float val) const
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glUniform1f(iLocation, val);
	}

	void _setUniform(char* szName, float* val, int varDim, int count) const
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		if (iLocation == -1)
		{
			assert(false);

			return;
		}			

		if (varDim == 4)
			glUniform4fv(iLocation, count, val);
		else if (varDim == 3)
			glUniform3fv(iLocation, count, val);
		else if (varDim == 2)
			glUniform2fv(iLocation, count, val);
		else if (varDim == 1)
			glUniform1fv(iLocation, count, val);
	}

	void _setUniformMatrix(char* szName, float* mat, int dimX, int dimY, bool bTranspose) const
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		if ((dimX == 4) && (dimY == 4))
		{
			glUniformMatrix4fv(iLocation, 1, bTranspose, mat);
		}
		else
		{
			assert(false);
		}			
	}

	void _setUniformMatrix(GLint iLocation, float* mat, int dimX, int dimY, bool bTranspose) const
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		if ((dimX == 4) && (dimY == 4))
		{
			glUniformMatrix4fv(iLocation, 1, bTranspose, mat);
		}
		else
		{
			assert(false);
		}
	}

	void _setUniform(GLint iLocation, float* val, int varDim, int count) const
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		if (varDim == 4)
			glUniform4fv(iLocation, count, val);
		else if (varDim == 3)
			glUniform3fv(iLocation, count, val);
		else if (varDim == 2)
			glUniform2fv(iLocation, count, val);
		else if (varDim == 1)
			glUniform1fv(iLocation, count, val);
	}

	GLint _getAttribLocation(char* szName) const
	{ 
		return glGetAttribLocation(m_iID, szName);
	}

	void _setAttrib(GLint iLocation, float val) const
	{ 
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glVertexAttrib1f(iLocation, val);
	}

	void _setAttrib(GLint iLocation, float* val, int varDim) const
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		if (varDim == 4)
			glVertexAttrib4fv(iLocation, val);
		else if (varDim == 3)
			glVertexAttrib3fv(iLocation, val);
		else if (varDim == 2)
			glVertexAttrib2fv(iLocation, val);
		else if (varDim == 1)
			glVertexAttrib1fv(iLocation, val);
	}

	void _bindAttribLocation(unsigned int iIndex, char* szName) const
	{
		glBindAttribLocation(m_iID, iIndex, szName);

		unsigned int iError = glGetError();
		VERIFY(iError == 0);

		DWORD dwError = GetLastError();
		VERIFY(dwError == 0);
	}

	GLint _enableVertexAttribArray(char* szName) const
	{
		GLint iLocation = glGetAttribLocation(m_iID, szName);
		if (iLocation != -1)
		{
			glEnableVertexAttribArray(iLocation);
		}			

		return iLocation;
	}

	void _disableVertexAttribArray(GLint iLocation) const
	{
		glDisableVertexAttribArray(iLocation);
	}

	void _getInfoLog(CString& strInfoLog) const
	{
		strInfoLog = _T("NA");

		int iLength = 0;
		glGetProgramiv(m_iID, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 0)
		{
			int iCharsWritten = 0;
			char* szInfoLog = new char[iLength];

			glGetProgramInfoLog(m_iID, iLength, &iCharsWritten, szInfoLog);

			strInfoLog = szInfoLog;
			delete[] szInfoLog;
		}
	}

	void _printInfoLog() const
	{
		CString strInfoLog;
		_getInfoLog(strInfoLog);

		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), strInfoLog, _T("Error"), MB_ICONERROR | MB_OK);
	}

	glm::vec3 _getUniform3f(GLint iUniform) const
	{
		float arValue[3] = { 0.f, 0.f, 0.f };
		glGetUniformfv(_getID(),
			iUniform,
			arValue);

		return glm::vec3(arValue[0], arValue[1], arValue[2]);
	}	

	void _setUniform3f(GLint iUniform, const glm::vec3& value) const
	{
		_setUniform3f(
			iUniform,
			value.x,
			value.y,
			value.z);
	}

	void _setUniform3f(GLint iUniform, float fX, float fY, float fZ) const
	{
		glProgramUniform3f(
			_getID(),
			iUniform,
			fX,
			fY,
			fZ);
	}

	float _getUniform1f(GLint iUniform) const
	{
		float fValue = 0.f;

		glGetUniformfv(_getID(),
			iUniform,
			&fValue);

		return fValue;
	}

	void _setUniform1f(GLint iUniform, float fValue) const
	{
		glProgramUniform1f(
			_getID(),
			iUniform,
			fValue);
	}
};

// ************************************************************************************************
class _oglBlinnPhongProgram : public _oglProgram
{

#pragma region Members

private: // Members

	// OpenGL

	bool m_bSupportsTexture;

	GLint m_iUseBlinnPhongModel;

	GLint m_iUseTexture;
	GLint m_iSampler;

	GLint m_iMVMatrix;
	GLint m_iPMatrix;
	GLint m_iNMatrix;

	GLint m_iPointLightingLocation;
	GLint m_iAmbientLightWeighting;
	GLint m_iSpecularLightWeighting;
	GLint m_iDiffuseLightWeighting;

	GLint m_iMaterialShininess;

	GLint m_iContrast;
	GLint m_iBrightness;
	GLint m_iGamma;

	GLint m_iMaterialAmbientColor;
	GLint m_iTransparency;
	GLint m_iMaterialDiffuseColor;
	GLint m_iMaterialSpecularColor;
	GLint m_iMaterialEmissiveColor;

	GLint m_iVertexPosition;
	GLint m_iVertexNormal;
	GLint m_iTextureCoord;

#pragma endregion 

public: // Methods

	_oglBlinnPhongProgram(bool bSupportsTexture)
		: _oglProgram()
		, m_bSupportsTexture(bSupportsTexture)
		, m_iUseBlinnPhongModel(-1)
		, m_iUseTexture(-1)
		, m_iSampler(-1)
		, m_iMVMatrix(-1)
		, m_iPMatrix(-1)
		, m_iNMatrix(-1)
		, m_iPointLightingLocation(-1)
		, m_iAmbientLightWeighting(-1)
		, m_iSpecularLightWeighting(-1)
		, m_iDiffuseLightWeighting(-1)
		, m_iMaterialShininess(-1)
		, m_iContrast(-1)
		, m_iBrightness(-1)
		, m_iGamma(-1)
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

	virtual ~_oglBlinnPhongProgram(void)
	{}

	bool _getSupportsTexture() const
	{
		return m_bSupportsTexture;
	}

	void _enableBlinnPhongModel(bool bEnable)
	{
		_setUniform1f(
			m_iUseBlinnPhongModel,
			bEnable ? 1.f : 0.f);
	}

	void _enableTexture(bool bEnable)
	{
		assert(m_bSupportsTexture);

		_setUniform1f(
			m_iUseTexture,
			bEnable ? 1.f : 0.f);
	}

	glm::vec3 _getPointLightingLocation() const 
	{ 
		return _getUniform3f(m_iPointLightingLocation);
	}

	void _setPointLightingLocation(const glm::vec3& value) 
	{ 
		_setUniform3f(
			m_iPointLightingLocation, 
			value);
	}

	glm::vec3 _getAmbientLightWeighting() const
	{
		return _getUniform3f(m_iAmbientLightWeighting);
	}

	void _setAmbientLightWeighting(float fX, float fY, float fZ) const
	{
		assert(m_iAmbientLightWeighting >= 0);

		_setUniform3f(
			m_iAmbientLightWeighting,
			fX,
			fY,
			fZ);
	}

	glm::vec3 _getSpecularLightWeighting() const
	{
		return _getUniform3f(m_iSpecularLightWeighting);
	}

	void _setSpecularLightWeighting(float fX, float fY, float fZ) const
	{
		_setUniform3f(
			m_iSpecularLightWeighting,
			fX,
			fY,
			fZ);
	}

	glm::vec3 _getDiffuseLightWeighting() const
	{
		return _getUniform3f(m_iDiffuseLightWeighting);
	}

	void _setDiffuseLightWeighting(float fX, float fY, float fZ) const
	{
		_setUniform3f(
			m_iDiffuseLightWeighting,
			fX,
			fY,
			fZ);
	}

	float _getMaterialShininess() const
	{
		return _getUniform1f(m_iMaterialShininess);
	}

	void _setMaterialShininess(float fValue) const
	{
		_setUniform1f(
			m_iMaterialShininess,
			fValue);
	}

	float _getContrast() const
	{
		return _getUniform1f(m_iContrast);
	}

	void _setContrast(float fValue) const
	{
		_setUniform1f(
			m_iContrast,
			fValue);
	}

	float _getBrightness() const
	{
		return _getUniform1f(m_iBrightness);
	}

	void _setBrightness(float fValue)
	{
		_setUniform1f(
			m_iBrightness,
			fValue);
	}

	float _getGamma() const
	{
		return _getUniform1f(m_iGamma);
	}

	void _setGamma(float fValue)
	{
		_setUniform1f(
			m_iGamma,
			fValue);
	}

	void _setAmbientColor(float fR, float fG, float fB)
	{
		_setUniform3f(
			m_iMaterialAmbientColor,
			fR,
			fG,
			fB);
	}

	void _setAmbientColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setAmbientColor(
			pMaterial->getAmbientColor().r(),
			pMaterial->getAmbientColor().g(),
			pMaterial->getAmbientColor().b());
	}

	void _setTransparency(float fA)
	{
		_setUniform1f(
			m_iTransparency,
			fA);
	}

	void _setDiffuseColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setUniform3f(
			m_iMaterialDiffuseColor,
#ifdef _BLINN_PHONG_SHADERS
			pMaterial->getDiffuseColor().r() / 2.f,
			pMaterial->getDiffuseColor().g() / 2.f,
			pMaterial->getDiffuseColor().b() / 2.f);
#else
			pMaterial->getDiffuseColor().r(),
			pMaterial->getDiffuseColor().g(),
			pMaterial->getDiffuseColor().b());
#endif
	}

	void _setSpecularColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setUniform3f(
			m_iMaterialSpecularColor,
#ifdef _BLINN_PHONG_SHADERS
			pMaterial->getSpecularColor().r() / 2.f,
			pMaterial->getSpecularColor().g() / 2.f,
			pMaterial->getSpecularColor().b() / 2.f);
#else
			pMaterial->getSpecularColor().r(),
			pMaterial->getSpecularColor().g(),
			pMaterial->getSpecularColor().b());
#endif
	}

	void _setEmissiveColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setUniform3f(
			m_iMaterialEmissiveColor,
			pMaterial->getEmissiveColor().r() / 3.f,
			pMaterial->getEmissiveColor().g() / 3.f,
			pMaterial->getEmissiveColor().b() / 3.f);
	}

	void _setMaterial(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setAmbientColor(pMaterial);
		_setTransparency(pMaterial->getA());
		_setDiffuseColor(pMaterial);
		_setSpecularColor(pMaterial);
		_setEmissiveColor(pMaterial);
	}

	virtual bool _link() override
	{
		if (!_oglProgram::_link())
		{
			assert(false);

			return false;
		}

		m_iUseBlinnPhongModel = glGetUniformLocation(_getID(), "uUseBlinnPhongModel");
		assert(m_iUseBlinnPhongModel >= 0);

		if (m_bSupportsTexture)
		{
			m_iUseTexture = glGetUniformLocation(_getID(), "uUseTexture");
			assert(m_iUseTexture >= 0);

			m_iSampler = glGetUniformLocation(_getID(), "uSampler");
			assert(m_iSampler >= 0);
		}

		m_iMVMatrix = glGetUniformLocation(_getID(), "uMVMatrix");
		assert(m_iMVMatrix >= 0);

		m_iPMatrix = glGetUniformLocation(_getID(), "uPMatrix");
		assert(m_iPMatrix >= 0);

		m_iNMatrix = glGetUniformLocation(_getID(), "uNMatrix");
		assert(m_iNMatrix >= 0);

		m_iPointLightingLocation = glGetUniformLocation(_getID(), "uPointLightingLocation");
		assert(m_iPointLightingLocation >= 0);

		m_iAmbientLightWeighting = glGetUniformLocation(_getID(), "uAmbientLightWeighting");
		assert(m_iAmbientLightWeighting >= 0);

		m_iSpecularLightWeighting = glGetUniformLocation(_getID(), "uSpecularLightWeighting");
		assert(m_iSpecularLightWeighting >= 0);

		m_iDiffuseLightWeighting = glGetUniformLocation(_getID(), "uDiffuseLightWeighting");
		assert(m_iDiffuseLightWeighting >= 0);

		m_iMaterialShininess = glGetUniformLocation(_getID(), "uMaterialShininess");
		assert(m_iMaterialShininess >= 0);

		m_iContrast = glGetUniformLocation(_getID(), "uContrast");
		assert(m_iContrast >= 0);

		m_iBrightness = glGetUniformLocation(_getID(), "uBrightness");
		assert(m_iBrightness >= 0);

		m_iGamma = glGetUniformLocation(_getID(), "uGamma");
		assert(m_iGamma >= 0);

		m_iMaterialAmbientColor = glGetUniformLocation(_getID(), "uMaterialAmbientColor");
		assert(m_iMaterialAmbientColor >= 0);

		m_iTransparency = glGetUniformLocation(_getID(), "uTransparency");
		assert(m_iTransparency >= 0);

		m_iMaterialDiffuseColor = glGetUniformLocation(_getID(), "uMaterialDiffuseColor");
		assert(m_iMaterialDiffuseColor >= 0);

		m_iMaterialSpecularColor = glGetUniformLocation(_getID(), "uMaterialSpecularColor");
		assert(m_iMaterialSpecularColor >= 0);

		m_iMaterialEmissiveColor = glGetUniformLocation(_getID(), "uMaterialEmissiveColor");
		assert(m_iMaterialEmissiveColor >= 0);

		m_iVertexPosition = glGetAttribLocation(_getID(), "aVertexPosition");
		assert(m_iVertexPosition >= 0);

		m_iVertexNormal = glGetAttribLocation(_getID(), "aVertexNormal");
		assert(m_iVertexNormal >= 0);

		if (m_bSupportsTexture)
		{
			m_iTextureCoord = glGetAttribLocation(_getID(), "aTextureCoord");
			assert(m_iTextureCoord >= 0);
		}

		return true;
	}

	GLint _getVertexPosition() const
	{
		return m_iVertexPosition;
	}

	GLint _getVertexNormal() const
	{
		return m_iVertexNormal;
	}

	GLint _getTextureCoord() const
	{
		assert(m_bSupportsTexture);

		return m_iTextureCoord;
	}

	void _setProjectionMatrix(glm::mat4& matProjection) const
	{
		glProgramUniformMatrix4fv(
			_getID(),
			m_iPMatrix,
			1,
			false,
			value_ptr(matProjection));
	}

	void _setModelViewMatrix(glm::mat4& matModelView) const
	{
		glProgramUniformMatrix4fv(
			_getID(),
			m_iMVMatrix,
			1,
			false,
			value_ptr(matModelView));
	}

	void _setNormalMatrix(glm::mat4& matNormal) const
	{
		glProgramUniformMatrix4fv(
			_getID(),
			m_iNMatrix,
			1,
			false,
			value_ptr(matNormal));
	}	

	void _setSampler(int iSampler) const
	{
		assert(m_bSupportsTexture);

		glProgramUniform1i(
			_getID(),
			m_iSampler,
			iSampler);
	}
};

// ************************************************************************************************
class _oglPerPixelProgram : public _oglProgram
{

#pragma region Members

private: // Members

	bool m_bSupportsTexture;

	/* Vertex Shader */

	// Attributes
	GLint m_iVertexPosition;
	GLint m_iVertexNormal;
	GLint m_iUV;

	// Uniforms
	GLint m_iProjectionMatrix;
	GLint m_iModelViewMatrix;
	GLint m_iNormalMatrix;
	GLint m_iDiffuseMaterial;
	GLint m_iEnableLighting;
	GLint m_iEnableTexture;

	/* Fragment Shader */

	// Uniforms
	GLint m_iLightPosition;
	GLint m_iAmbientMaterial;
	GLint m_iSpecularMaterial;
	GLint m_iTransparency;
	GLint m_iShininess;
	GLuint m_iSampler;

#pragma endregion 

public: // Methods

	_oglPerPixelProgram(bool bSupportsTexture)
		: _oglProgram()
		, m_bSupportsTexture(bSupportsTexture)
		, m_iVertexPosition(-1)
		, m_iVertexNormal(-1)
		, m_iUV(-1)
		, m_iProjectionMatrix(-1)
		, m_iModelViewMatrix(-1)
		, m_iNormalMatrix(-1)
		, m_iDiffuseMaterial(-1)
		, m_iEnableLighting(-1)
		, m_iEnableTexture(-1)
		, m_iLightPosition(-1)
		, m_iAmbientMaterial(-1)
		, m_iSpecularMaterial(-1)
		, m_iTransparency(-1)
		, m_iShininess(-1)
	{}

	virtual ~_oglPerPixelProgram(void)
	{}	

	virtual bool _link() override
	{
		if (!_oglProgram::_link())
		{
			assert(false);

			return false;
		}

		/* Vertex Shader */
		m_iVertexPosition = glGetAttribLocation(_getID(), "Position");
		assert(m_iVertexPosition >= 0);

		m_iVertexNormal = glGetAttribLocation(_getID(), "Normal");
		assert(m_iVertexNormal >= 0);

		m_iUV = glGetAttribLocation(_getID(), "UV");
		assert(m_iUV >= 0);

		m_iProjectionMatrix = glGetUniformLocation(_getID(), "ProjectionMatrix");
		assert(m_iProjectionMatrix >= 0);

		m_iModelViewMatrix = glGetUniformLocation(_getID(), "ModelViewMatrix");
		assert(m_iModelViewMatrix >= 0);

		m_iNormalMatrix = glGetUniformLocation(_getID(), "NormalMatrix");
		assert(m_iNormalMatrix >= 0);

		m_iDiffuseMaterial = glGetUniformLocation(_getID(), "DiffuseMaterial");
		assert(m_iDiffuseMaterial >= 0);

		m_iEnableLighting = glGetUniformLocation(_getID(), "EnableLighting");
		assert(m_iEnableLighting >= 0);

		if (m_bSupportsTexture)
		{
			m_iEnableTexture = glGetUniformLocation(_getID(), "EnableTexture");
			assert(m_iEnableTexture >= 0);
		}

		/* Fragment Shader */
		m_iLightPosition = glGetUniformLocation(_getID(), "LightPosition");
		assert(m_iLightPosition >= 0);

		m_iAmbientMaterial = glGetUniformLocation(_getID(), "AmbientMaterial");
		assert(m_iAmbientMaterial >= 0);

		m_iSpecularMaterial = glGetUniformLocation(_getID(), "SpecularMaterial");
		assert(m_iSpecularMaterial >= 0);

		m_iTransparency = glGetUniformLocation(_getID(), "Transparency");
		assert(m_iTransparency >= 0);

		m_iShininess = glGetUniformLocation(_getID(), "Shininess");
		assert(m_iShininess >= 0);

		if (m_bSupportsTexture)
		{
			m_iSampler = glGetUniformLocation(_getID(), "Sampler");
			assert(m_iSampler >= 0);
		}

		return true;
	}

	/* Vertex Shader */
	GLint _getVertexPosition() const { return m_iVertexPosition; }
	GLint _getVertexNormal() const { return m_iVertexNormal; }
	GLint _getUV() const { return m_iUV; }

	void _setProjectionMatrix(glm::mat4& matProjection) const
	{
		glProgramUniformMatrix4fv(
			_getID(),
			m_iProjectionMatrix,
			1,
			false,
			value_ptr(matProjection));
	}

	void _setModelViewMatrix(glm::mat4& matModelView) const
	{
		glProgramUniformMatrix4fv(
			_getID(),
			m_iModelViewMatrix,
			1,
			false,
			value_ptr(matModelView));
	}

	void _setNormalMatrix(glm::mat4& matModelView) const
	{
		glm::mat3 matNormal = matModelView;
		glProgramUniformMatrix3fv(
			_getID(),
			m_iNormalMatrix,
			1,
			false,
			value_ptr(matNormal));
	}

	void _setDiffuseColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setUniform3f(
			m_iDiffuseMaterial,
			pMaterial->getDiffuseColor().r(),
			pMaterial->getDiffuseColor().g(),
			pMaterial->getDiffuseColor().b());
	}

	void _enableLighting(bool bEnable)
	{
		_setUniform1f(
			m_iEnableLighting,
			bEnable ? 1.f : 0.f);
	}

	void _enableTexture(bool bEnable)
	{
		_setUniform1f(
			m_iEnableTexture,
			bEnable ? 1.f : 0.f);
	}

	/* Fragment Shader */
	glm::vec3 _getLightPosition() const { return _getUniform3f(m_iLightPosition); }
	void _setLightPosition(const glm::vec3& value)
	{
		_setUniform3f(
			m_iLightPosition,
			value);
	}

	void _setAmbientColor(float fR, float fG, float fB)
	{
		_setUniform3f(
			m_iAmbientMaterial,
			fR,
			fG,
			fB);
	}

	void _setAmbientColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setAmbientColor(
			pMaterial->getAmbientColor().r(),
			pMaterial->getAmbientColor().g(),
			pMaterial->getAmbientColor().b());
	}

	void _setSpecularColor(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setUniform3f(
			m_iSpecularMaterial,
#ifdef _BLINN_PHONG_SHADERS
			pMaterial->getSpecularColor().r() / 2.f,
			pMaterial->getSpecularColor().g() / 2.f,
			pMaterial->getSpecularColor().b() / 2.f);
#else
			pMaterial->getSpecularColor().r(),
			pMaterial->getSpecularColor().g(),
			pMaterial->getSpecularColor().b());
#endif
	}

	void _setTransparency(float fA)
	{
		_setUniform1f(
			m_iTransparency,
			fA);
	}

	float _getMaterialShininess() const { return _getUniform1f(m_iShininess); }
	void _setMaterialShininess(float fValue) const
	{
		_setUniform1f(
			m_iShininess,
			fValue);
	}

	void _setSampler(int iSampler) const
	{
		assert(m_bSupportsTexture);

		glProgramUniform1i(
			_getID(),
			m_iSampler,
			iSampler);
	}

	bool _getSupportsTexture() const { return m_bSupportsTexture; }

	void _setMaterial(const _material* pMaterial)
	{
		assert(pMaterial != nullptr);

		_setAmbientColor(pMaterial);		
		_setDiffuseColor(pMaterial);
		_setSpecularColor(pMaterial);

		_setTransparency(pMaterial->getA());
	}

	void _setMaterial(const _material* pMaterial, float fTransparency)
	{
		assert(pMaterial != nullptr);

		_setAmbientColor(pMaterial);		
		_setDiffuseColor(pMaterial);
		_setSpecularColor(pMaterial);

		_setTransparency(fTransparency);
	}
};

// ************************************************************************************************
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	return 0;
}

// ************************************************************************************************
class _oglContext
{

public: // Constants

	const int MIN_GL_MAJOR_VERSION = 3;
	const int MIN_GL_MINOR_VERSION = 3;

private: // Members

	HDC m_hDC;
	HGLRC m_hGLContext;

	int m_iSamples;

public: // Methods

	_oglContext(HDC hDC, int iSamples)
		: m_hDC(hDC)
		, m_hGLContext(nullptr)
		, m_iSamples(iSamples)
	{
		assert(m_hDC != nullptr);

		create();
	}

	virtual ~_oglContext()
	{
		if (m_hGLContext != nullptr)
		{
			BOOL bResult = wglMakeCurrent(m_hDC, nullptr);
			assert(bResult);

			bResult = wglDeleteContext(m_hGLContext);
			assert(bResult);
		}
	}

	BOOL makeCurrent()
	{
		assert(m_hDC != nullptr);
		assert(m_hGLContext != nullptr);

		return wglMakeCurrent(m_hDC, m_hGLContext);
	}

	void create()
	{
		// https://www.opengl.org/wiki/Creating_an_OpenGL_Context_(WGL)

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
			1,                     // version number  
			PFD_DRAW_TO_WINDOW |   // support window  
			PFD_SUPPORT_OPENGL |   // support OpenGL  
			PFD_DOUBLEBUFFER,      // double buffered  
			PFD_TYPE_RGBA,         // RGBA type  
			32,                    // 32-bit color depth  
			0, 0, 0, 0, 0, 0,      // color bits ignored  
			0,                     // no alpha buffer  
			0,                     // shift bit ignored  
			0,                     // no accumulation buffer  
			0, 0, 0, 0,            // accum bits ignored  
			24,                    // 24-bit z-buffer
			8,                     // 8-bit stencil buffer
			0,                     // no auxiliary buffer  
			PFD_MAIN_PLANE,        // main layer  
			0,                     // reserved  
			0, 0, 0                // layer masks ignored  
		};

		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		WNDCLASSEX WndClassEx;
		memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

		WndClassEx.cbSize = sizeof(WNDCLASSEX);
		WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		WndClassEx.lpfnWndProc = WndProc;
		WndClassEx.hInstance = ::AfxGetInstanceHandle();
		WndClassEx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		WndClassEx.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		WndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
		WndClassEx.lpszClassName = _T("_OpenGL_Renderer_Window_");

		if (!GetClassInfoEx(::AfxGetInstanceHandle(), WndClassEx.lpszClassName, &WndClassEx))
		{
			if (RegisterClassEx(&WndClassEx) == 0)
			{
				::MessageBox(
					::AfxGetMainWnd()->GetSafeHwnd(), 
					_T("RegisterClassEx() failed."), 
					_T("Error"), 
					MB_ICONERROR | MB_OK);

				PostQuitMessage(0);
			}
		}

		HWND hWndTemp = CreateWindowEx(
			WS_EX_APPWINDOW, 
			WndClassEx.lpszClassName, _T("OpenGL"), 
			dwStyle, 
			0, 0, 600, 600, 
			nullptr, 
			nullptr, 
			::AfxGetInstanceHandle(), 
			nullptr);

		HDC hDCTemp = ::GetDC(hWndTemp);

		// Get the best available match of pixel format for the device context   
		int iPixelFormat = ChoosePixelFormat(hDCTemp, &pfd);
		assert(iPixelFormat > 0);

		// Make that the pixel format of the device context  
		BOOL bResult = SetPixelFormat(hDCTemp, iPixelFormat, &pfd);
		assert(bResult);

		HGLRC hTempGLContext = wglCreateContext(hDCTemp);
		assert(hTempGLContext);

		bResult = wglMakeCurrent(hDCTemp, hTempGLContext);
		assert(bResult);

		_oglUtils::checkForErrors();

		/*
		* MSAA support
		*/

		BOOL bMSAASupport = false;

		int arAttributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 0,
			0, 0,
		};

		glewExperimental = GL_TRUE;
		if (glewInit() == GLEW_OK)
		{
			GLint glMajorVersion = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);

			GLint glMinorVersion = 0;
			glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

			bool bWrongGLVersion = false;
			if (glMajorVersion < MIN_GL_MAJOR_VERSION)
			{
				bWrongGLVersion = true;
			}
			else
			{
				if ((glMajorVersion == MIN_GL_MAJOR_VERSION) && (glMinorVersion < MIN_GL_MINOR_VERSION))
				{
					bWrongGLVersion = true;
				}
			}

			if (bWrongGLVersion)
			{
				CString strErrorMessage;
				strErrorMessage.Format(_T("OpenGL version %d.%d or higher is required."), MIN_GL_MAJOR_VERSION, MIN_GL_MINOR_VERSION);

				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), strErrorMessage, _T("Error"), MB_ICONERROR | MB_OK);

				PostQuitMessage(0);
			}

			/*
			* Pixel format
			*/
			iPixelFormat = 0;
			for (int iSamples = m_iSamples; iSamples >= 0; iSamples--)
			{
				float fAttributes[] = { 0, 0 };
				UINT iNumFormats = 0;

				arAttributes[17/*WGL_SAMPLES_ARB*/] = iSamples;

				if (wglChoosePixelFormatARB(hDCTemp, arAttributes, fAttributes, 1, &iPixelFormat, &iNumFormats) && (iPixelFormat > 0))
				{
					bMSAASupport = TRUE;

					break;
				}
			}
		} // if (glewInit() == GLEW_OK) 
		else
		{
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("glewInit() failed."), _T("Error"), MB_ICONERROR | MB_OK);

			PostQuitMessage(0);
		}

		bResult = wglMakeCurrent(nullptr, nullptr);
		assert(bResult);

		bResult = wglDeleteContext(hTempGLContext);
		assert(bResult);

		if (bMSAASupport)
		{
			bResult = SetPixelFormat(m_hDC, iPixelFormat, &pfd);
			assert(bResult);

			int arContextAttributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, MIN_GL_MAJOR_VERSION,
				WGL_CONTEXT_MINOR_VERSION_ARB, MIN_GL_MINOR_VERSION,
#ifdef _ENABLE_OPENGL_DEBUG
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#endif
				0, 0,
			};

			m_hGLContext = wglCreateContextAttribsARB(m_hDC, 0, arContextAttributes);
			assert(m_hGLContext);
		}
		else
		{
			iPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
			assert(iPixelFormat > 0);

			bResult = SetPixelFormat(m_hDC, iPixelFormat, &pfd);
			assert(bResult);

			m_hGLContext = wglCreateContext(m_hDC);
			assert(m_hGLContext);
		}

		::ReleaseDC(hWndTemp, hDCTemp);
		::DestroyWindow(hWndTemp);

#ifdef _ENABLE_OPENGL_DEBUG
		remove("_OpenGL_Debug_.txt");
#endif
	}

#ifdef _ENABLE_OPENGL_DEBUG
	void enableDebug()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

		glDebugMessageCallbackARB(&_oglContext::debugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
	{
		debugOutputToFile(source, type, id, severity, message);
	}

	static void debugOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message)
	{
		FILE* f = fopen("_OpenGL_Debug_.txt", "a");
		if (f)
		{
			char debSource[50], debType[50], debSev[50];

			// source
			if (source == GL_DEBUG_SOURCE_API_ARB)
				strcpy(debSource, "OpenGL");
			else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
				strcpy(debSource, "Windows");
			else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
				strcpy(debSource, "Shader Compiler");
			else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
				strcpy(debSource, "Third Party");
			else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
				strcpy(debSource, "Application");
			else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
				strcpy(debSource, "Other");

			// type
			if (type == GL_DEBUG_TYPE_ERROR_ARB)
				strcpy(debType, "Error");
			else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
				strcpy(debType, "Deprecated behavior");
			else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
				strcpy(debType, "Undefined behavior");
			else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
				strcpy(debType, "Portability");
			else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
				strcpy(debType, "Performance");
			else if (type == GL_DEBUG_TYPE_OTHER_ARB)
				strcpy(debType, "Other");

			// severity
			if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
				strcpy(debSev, "High");
			else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
				strcpy(debSev, "Medium");
			else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
				strcpy(debSev, "Low");

			fprintf(f, "Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n", debSource, debType, id, debSev, message);

			fclose(f);
		} // if (f)
	}
#endif
};

// ************************************************************************************************
#define BUFFER_SIZE 512

class _oglFramebuffer
{

private: // Members

	GLuint m_iFrameBuffer;
	GLuint m_iTextureBuffer;
	GLuint m_iRenderBuffer;

public: // Methods

	_oglFramebuffer()
		: m_iFrameBuffer(0)
		, m_iTextureBuffer(0)
		, m_iRenderBuffer(0)
	{
	}

	virtual ~_oglFramebuffer()
	{
		if (m_iFrameBuffer != 0)
		{
			glDeleteFramebuffers(1, &m_iFrameBuffer);
		}

		if (m_iTextureBuffer != 0)
		{
			glDeleteTextures(1, &m_iTextureBuffer);
		}

		if (m_iRenderBuffer != 0)
		{
			glDeleteRenderbuffers(1, &m_iRenderBuffer);
		}
	}

	void create(GLsizei iWidth = BUFFER_SIZE, GLsizei iHeight = BUFFER_SIZE)
	{
		if (m_iFrameBuffer == 0)
		{
			assert(m_iTextureBuffer == 0);
			assert(m_iRenderBuffer == 0);

			/*
			* Frame buffer
			*/
			glGenFramebuffers(1, &m_iFrameBuffer);
			assert(m_iFrameBuffer != 0);

			glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);

			/*
			* Texture buffer
			*/
			glGenTextures(1, &m_iTextureBuffer);
			assert(m_iTextureBuffer != 0);

			glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iTextureBuffer, 0);

			/*
			* Depth buffer
			*/
			glGenRenderbuffers(1, &m_iRenderBuffer);
			assert(m_iRenderBuffer != 0);

			glBindRenderbuffer(GL_RENDERBUFFER, m_iRenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, iWidth, iHeight);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iRenderBuffer);

			GLenum arDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, arDrawBuffers);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			_oglUtils::checkForErrors();
		} // if (m_iFrameBuffer == 0)
	}

	virtual bool isInitialized() const
	{
		return m_iFrameBuffer != 0;
	}

	void bind()
	{
		assert(m_iFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);
	}

	void unbind()
	{
		assert(m_iFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

// ************************************************************************************************
class _oglSelectionFramebuffer : public _oglFramebuffer
{

private: // Members

	map<int64_t, _color> m_mapEncoding; // ID : Color

public: // Methods

	_oglSelectionFramebuffer()
		: _oglFramebuffer()
		, m_mapEncoding()
	{
	}

	virtual ~_oglSelectionFramebuffer()
	{
	}

	map<int64_t, _color>& encoding()
	{
		return m_mapEncoding;
	}
};

// ************************************************************************************************
// (255 * 255 * 255)[R] + (255 * 255)[G] + 255[B]
class _i64RGBCoder
{

public: // Methods

	static void encode(int64_t i, float& fR, float& fG, float& fB)
	{
		static const float STEP = 1.f / 255.f;

		fR = 0.f;
		fG = 0.f;
		fB = 0.f;

		// R
		if (i >= (255 * 255))
		{
			int64_t iR = i / (255 * 255);
			fR = iR * STEP;

			i -= iR * (255 * 255);
		}

		// G
		if (i >= 255)
		{
			int64_t iG = i / 255;
			fG = iG * STEP;

			i -= iG * 255;
		}

		// B		
		fB = i * STEP;
	}

	static int64_t decode(unsigned char R, unsigned char G, unsigned char B)
	{
		int64_t i = 0;

		// R
		i += R * (255 * 255);

		// G
		i += G * 255;

		// B
		i += B;

		return i;
	}
};

// ************************************************************************************************
class _oglBuffers
{

private: // Members

	map<GLuint, vector<_geometry*>> m_mapCohorts;
	map<wstring, GLuint> m_mapVAOs;
	map<wstring, GLuint> m_mapBuffers;

public: // Methods

	_oglBuffers()
		: m_mapCohorts()
		, m_mapVAOs()
		, m_mapBuffers()
	{}

	virtual ~_oglBuffers()
	{}

	map<GLuint, vector<_geometry*>>& cohorts() { return m_mapCohorts; }
	map<wstring, GLuint>& VAOs() { return m_mapVAOs; }
	map<wstring, GLuint>& buffers() { return m_mapBuffers; }

	GLuint findVAO(_geometry* pTargetGeometry)
	{
		for (auto itCohort : m_mapCohorts)
		{
			for (auto pGeometry : itCohort.second)
			{
				if (pGeometry == pTargetGeometry)
				{
					return itCohort.first;
				}
			}
		}

		return 0;
	}

	GLuint getVAO(const wstring& strName)
	{
		auto itVAO = m_mapVAOs.find(strName);
		if (itVAO != m_mapVAOs.end())
		{
			return itVAO->second;
		}

		return 0;
	}

	GLuint getVAOcreateNewIfNeeded(const wstring& strName, bool& bIsNew)
	{
		bIsNew = false;

		GLuint iVAO = getVAO(strName);
		if (iVAO == 0)
		{
			glGenVertexArrays(1, &iVAO);
			if (iVAO == 0)
			{
				assert(false);

				return 0;
			}

			_oglUtils::checkForErrors();

			bIsNew = true;
			m_mapVAOs[strName] = iVAO;
		}

		return iVAO;
	}

	GLuint getBuffer(const wstring& strName)
	{
		auto itBuffer = m_mapBuffers.find(strName);
		if (itBuffer != m_mapBuffers.end())
		{
			return itBuffer->second;
		}

		return 0;
	}

	GLuint getBufferCreateNewIfNeeded(const wstring& strName, bool& bIsNew)
	{
		bIsNew = false;

		GLuint iBuffer = getBuffer(strName);
		if (iBuffer == 0)
		{
			glGenBuffers(1, &iBuffer);
			if (iBuffer == 0)
			{
				assert(false);

				return 0;
			}

			_oglUtils::checkForErrors();

			bIsNew = true;
			m_mapBuffers[strName] = iBuffer;
		}

		return iBuffer;
	}

	int64_t createIBO(const vector<_cohort*>& vecCohorts)
	{
		if (vecCohorts.empty())
		{
			assert(false);

			return 0;
		}

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		if (iIBO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapBuffers[to_wstring(iIBO)] = iIBO;

		uint32_t iIndicesCount = 0;
		unsigned int* pIndices = _cohort::merge(vecCohorts, iIndicesCount);

		if ((pIndices == nullptr) || (iIndicesCount == 0))
		{
			assert(false);

			return 0;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iIndicesCount, pIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] pIndices;

		_oglUtils::checkForErrors();

		GLsizei iIBOOffset = 0;
		for (auto pCohort : vecCohorts)
		{
			pCohort->IBO() = iIBO;
			pCohort->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)pCohort->indices().size();
		}

		return iIndicesCount;
	}

#ifdef _BLINN_PHONG_SHADERS
	int64_t createCohort(const vector<_geometry*>& vecGeometries, _oglBlinnPhongProgram* pProgram)
#else
	int64_t createCohort(const vector<_geometry*>& vecGeometries, _oglPerPixelProgram* pProgram)
#endif
	{
		if (vecGeometries.empty() || (pProgram == nullptr))
		{
			assert(false);

			return 0;
		}

		int64_t iVerticesCount = 0;
		float* pVertices = getVertices(vecGeometries, pProgram->_getSupportsTexture(), iVerticesCount);

		if ((pVertices == nullptr) || (iVerticesCount == 0))
		{
			assert(false);

			return 0;
		}

		GLuint iVAO = 0;
		glGenVertexArrays(1, &iVAO);

		if (iVAO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapVAOs[to_wstring(iVAO)] = iVAO;

		glBindVertexArray(iVAO);

		GLuint iVBO = 0;
		glGenBuffers(1, &iVBO);

		if (iVBO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapBuffers[to_wstring(iVBO)] = iVBO;

		const size_t VERTEX_LENGTH = 6 + (pProgram->_getSupportsTexture() ? 2 : 0);

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (size_t)iVerticesCount * VERTEX_LENGTH, pVertices, GL_STATIC_DRAW);
		delete[] pVertices;

		setVBOAttributes(pProgram);

		GLsizei iVBOOffset = 0;
		for (auto pGeometry : vecGeometries)
		{
			pGeometry->VBO() = iVBO;
			pGeometry->VBOOffset() = iVBOOffset;

			iVBOOffset += (GLsizei)pGeometry->getVerticesCount();
		}

		glBindVertexArray(0);

		_oglUtils::checkForErrors();

		m_mapCohorts[iVAO] = vecGeometries;

		return iVerticesCount;
	}

#ifdef _BLINN_PHONG_SHADERS
	void setVBOAttributes(_oglBlinnPhongProgram* pProgram) const
#else
	void setVBOAttributes(_oglPerPixelProgram* pProgram) const
#endif
	{
		const int64_t VERTEX_LENGTH = 6 + (pProgram->_getSupportsTexture() ? 2 : 0);

		glVertexAttribPointer(pProgram->_getVertexPosition(), 3, GL_FLOAT, false, (GLsizei)(sizeof(GLfloat) * VERTEX_LENGTH), 0);
		glVertexAttribPointer(pProgram->_getVertexNormal(), 3, GL_FLOAT, false, (GLsizei)(sizeof(GLfloat) * VERTEX_LENGTH), (void*)(sizeof(GLfloat) * 3));
		if (pProgram->_getSupportsTexture())
		{
			glVertexAttribPointer(
#ifdef _BLINN_PHONG_SHADERS
				pProgram->_getTextureCoord(),
#else
				pProgram->_getUV(),
#endif
				2, 
				GL_FLOAT, 
				false, 
				(GLsizei)(sizeof(GLfloat) * VERTEX_LENGTH), 
				(void*)(sizeof(GLfloat) * 6));
		}

		glEnableVertexAttribArray(pProgram->_getVertexPosition());
		glEnableVertexAttribArray(pProgram->_getVertexNormal());
		if (pProgram->_getSupportsTexture())
		{
#ifdef _BLINN_PHONG_SHADERS
			glEnableVertexAttribArray(pProgram->_getTextureCoord());
#else
			glEnableVertexAttribArray(pProgram->_getUV());
#endif			
		}

		_oglUtils::checkForErrors();
	}

	// X, Y, Z, Nx, Ny, Nz, [Tx, Ty]
	static float* getVertices(const vector<_geometry*>& vecGeometries, bool bSupportsTexture, int64_t& iVerticesCount)
	{
		const size_t VERTEX_LENGTH = 6 + (bSupportsTexture ? 2 : 0);

		iVerticesCount = 0;
		for (size_t i = 0; i < vecGeometries.size(); i++)
		{
			iVerticesCount += vecGeometries[i]->getVerticesCount();
		}

		float* pVertices = new float[(size_t)iVerticesCount * VERTEX_LENGTH];

		int64_t iOffset = 0;
		for (size_t i = 0; i < vecGeometries.size(); i++)
		{
			float* pSrcVertices = getVertices(vecGeometries[i], bSupportsTexture);

			memcpy((float*)pVertices + iOffset, pSrcVertices,
				(size_t)vecGeometries[i]->getVerticesCount() * VERTEX_LENGTH * sizeof(float));

			delete[] pSrcVertices;

			iOffset += vecGeometries[i]->getVerticesCount() * VERTEX_LENGTH;
		}

		return pVertices;
	}

	// X, Y, Z, Nx, Ny, Nz, [Tx, Ty]
	static float* getVertices(_geometry* pGeometry, bool bSupportsTexture)
	{
		const size_t _SRC_VERTEX_LENGTH = (size_t)pGeometry->getVertexLength();
		const size_t _DEST_VERTEX_LENGTH = 6 + (bSupportsTexture ? 2 : 0);

		float* pVertices = new float[(size_t)pGeometry->getVerticesCount() * _DEST_VERTEX_LENGTH];
		memset(pVertices, 0, (size_t)pGeometry->getVerticesCount() * _DEST_VERTEX_LENGTH * sizeof(float));

		for (int64_t iVertex = 0; iVertex < pGeometry->getVerticesCount(); iVertex++)
		{
			// X, Y, Z
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 0] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 0];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 1] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 1];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 2] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 2];

			// Nx, Ny, Nz
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 3] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 3];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 4] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 4];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 5] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 5];

			// Tx, Ty
			if (bSupportsTexture)
			{
				pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 6] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 6];
				pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 7] = pGeometry->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 7];
			}
		}

		return pVertices;
	}

	void clear()
	{
		m_mapCohorts.clear();

		for (auto itVAO = m_mapVAOs.begin(); itVAO != m_mapVAOs.end(); itVAO++)
		{
			glDeleteVertexArrays(1, &(itVAO->second));
		}
		m_mapVAOs.clear();

		for (auto itBuffer = m_mapBuffers.begin(); itBuffer != m_mapBuffers.end(); itBuffer++)
		{
			glDeleteBuffers(1, &(itBuffer->second));
		}
		m_mapBuffers.clear();

		_oglUtils::checkForErrors();
	}
};

// ************************************************************************************************
enum class enumProjection : int
{
	Perspective = 0,
	Orthographic,
};

// ************************************************************************************************
enum class enumView : int64_t
{
	Front = 0,
	Back,
	Top,
	Bottom,
	Left,
	Right,
	FrontTopLeft,
	FrontTopRight,
	FrontBottomLeft,
	FrontBottomRight,
	BackTopLeft,
	BackTopRight,
	BackBottomLeft,
	BackBottomRight,
	Isometric,
};

// ************************************************************************************************
enum class enumRotationMode : int
{
	XY = 0, // Standard
	XYZ,	// Quaternions
};

// ************************************************************************************************
const float DEFAULT_TRANSLATION = -5.f;

const float ZOOM_SPEED_MOUSE = 0.01f;
const float ZOOM_SPEED_MOUSE_WHEEL = 0.005f;
const float ZOOM_SPEED_KEYS = ZOOM_SPEED_MOUSE;
const float PAN_SPEED_KEYS = 0.01f;
const float ROTATION_SPEED = 1.f / 25.f;

const wchar_t NAVIGATION_VIEW_VAO[] = L"NAVIGATION_VIEW_VAO";
const wchar_t NAVIGATION_VIEW_VBO[] = L"NAVIGATION_VIEW_VBO";
const wchar_t NAVIGATION_VIEW_IBO[] = L"NAVIGATION_VIEW_IBO";

const wchar_t COORDINATE_SYSTEM_VAO[] = L"COORDINATE_SYSTEM_VAO";
const wchar_t COORDINATE_SYSTEM_VBO[] = L"COORDINATE_SYSTEM_VBO";
const wchar_t COORDINATE_SYSTEM_IBO[] = L"COORDINATE_SYSTEM_IBO";

// ************************************************************************************************
enum class enumMouseEvent : int
{
	Move = 0,
	LBtnDown,
	LBtnUp,
	MBtnDown,
	MBtnUp,
	RBtnDown,
	RBtnUp,
};

// ************************************************************************************************
#define CULL_FACES_NONE L"<none>"
#define CULL_FACES_FRONT L"Front"
#define CULL_FACES_BACK L"Back"

// ************************************************************************************************
// X, Y, Z, Nx, Ny, Nz
#define GEOMETRY_VBO_VERTEX_LENGTH  6

// ************************************************************************************************
class _oglRendererSettings
{

protected: // Members

	// Mode
	enumProjection m_enProjection;

	// Rotation
	enumRotationMode m_enRotationMode;

	// 2D Rotation
	float m_fXAngle;
	float m_fYAngle;
	float m_fZAngle;

	// 3D Rotation
	_quaterniond m_rotation;

	// View
	BOOL m_bGhostView;
	GLfloat m_fGhostViewTransparency;
	BOOL m_bShowFaces;
	CString m_strCullFaces;
	BOOL m_bShowFacesPolygons;
	BOOL m_bShowConceptualFacesPolygons;
	BOOL m_bShowLines;
	GLfloat m_fLineWidth;
	BOOL m_bShowPoints;
	GLfloat m_fPointSize;
	BOOL m_bShowBoundingBoxes;
	BOOL m_bShowNormalVectors;
	BOOL m_bShowTangenVectors;
	BOOL m_bShowBiNormalVectors;
	BOOL m_bScaleVectors;
	BOOL m_bShowCoordinateSystem;
	BOOL m_bShowNavigator;

	// Selection
	_material* m_pSelectedInstanceMaterial;
	_material* m_pPointedInstanceMaterial;
	bool m_bMultiSelect;

public: // Methods

	_oglRendererSettings();
	virtual ~_oglRendererSettings();

	virtual void _reset();

	void _setView(enumView enView);

	virtual void saveSetting(const string& strName, const string& strValue);
	virtual string loadSetting(const string& strName);
	virtual void loadSettings();

protected: // Properties

	virtual _controller* _getController() const PURE;

public: // Properties

	enumProjection _getProjection() const;
	void _setProjection(enumProjection enProjection);

	enumRotationMode _getRotationMode() const;
	void _setRotationMode(enumRotationMode enRotationMode);

	void setGhostView(BOOL bValue);
	BOOL getGhostView() const;

	void setGhostViewTransparency(GLfloat fTransparency);
	GLfloat getGhostViewTransparency() const;
	
	void setShowFaces(BOOL bValue);
	BOOL getShowFaces();

	void setCullFacesMode(LPCTSTR szMode);
	LPCTSTR getCullFacesMode() const;

	void setShowFacesPolygons(BOOL bValue);
	BOOL getShowFacesPolygons() const;

	void setShowConceptualFacesPolygons(BOOL bValue);
	BOOL getShowConceptualFacesPolygons() const;

	void setShowLines(BOOL bValue);
	BOOL getShowLines() const;

	void setLineWidth(GLfloat fWidth);
	GLfloat getLineWidth() const;

	void setShowPoints(BOOL bValue);
	BOOL getShowPoints() const;

	void setPointSize(GLfloat fSize);
	GLfloat getPointSize() const;

	void setShowBoundingBoxes(BOOL bValue);
	BOOL getShowBoundingBoxes() const;

	void setShowNormalVectors(BOOL bValue);
	BOOL getShowNormalVectors() const;

	void setShowTangentVectors(BOOL bValue);
	BOOL getShowTangentVectors() const;

	void setShowBiNormalVectors(BOOL bValue);
	BOOL getShowBiNormalVectors() const;

	void setScaleVectors(BOOL bValue);
	BOOL getScaleVectors() const;

	void setShowCoordinateSystem(BOOL bValue);
	BOOL getShowCoordinateSystem() const;

	void setShowNavigator(BOOL bValue);
	BOOL getShowNavigator() const;

	void setSelectedInstanceMaterial(float fR, float fG, float fB, float fTransparency);
	_material* getSelectedInstanceMaterial() const { return m_pSelectedInstanceMaterial; }
};

// ************************************************************************************************
class _oglRenderer : public _oglRendererSettings
{

protected: // Members

	// MFC
	CWnd* m_pWnd;
	CMFCToolTipCtrl m_toolTipCtrl;

	// OpenGL
	_oglContext* m_pOGLContext;
#ifdef _BLINN_PHONG_SHADERS
	_oglBlinnPhongProgram* m_pOGLProgram;
#else
	_oglPerPixelProgram* m_pOGLProgram;
#endif
	_oglShader* m_pVertexShader;
	_oglShader* m_pFragmentShader;	
	glm::mat4 m_matModelView;	

	// Cache
	_oglBuffers m_oglBuffers;	

	// World
	float m_fXmin;
	float m_fXmax;
	float m_fYmin;
	float m_fYmax;
	float m_fZmin;
	float m_fZmax;

	// Zoom/Pan
	float m_fZoomMin;
	float m_fZoomMax;
	float m_fZoomInterval;
	float m_fPanXMin;
	float m_fPanXMax;
	float m_fPanXInterval;
	float m_fPanYMin;
	float m_fPanYMax;
	float m_fPanYInterval;

	// Translation
	float m_fXTranslation; // Perspective & Orthographic
	float m_fYTranslation; // Perspective & Orthographic
	float m_fZTranslation; // Perspective

	// Orthographic
	float m_fScaleFactor; 
	float m_fScaleFactorMin;
	float m_fScaleFactorMax;
	float m_fScaleFactorInterval;

public: // Methods

	_oglRenderer();
	virtual ~_oglRenderer();

	// _oglRendererSettings
	virtual void _reset() override;

	void _initialize(CWnd* pWnd,
		int iSamples,
		int iVertexShader,
		int iFragmentShader,
		int iResourceType,
		bool bSupportsTexture);
	void _destroy();

	void _prepare(
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight,
		float fXmin, float fXmax,
		float fYmin, float fYmax,
		float fZmin, float fZmax,
		bool bClearScene,
		bool bApplyTranslations);

	void _redraw() { m_pWnd->RedrawWindow(); }

	void _rotateMouseLButton(float fXAngle, float fYAngle);
	void _zoomMouseMButton(LONG lDelta);
	void _panMouseRButton(float fX, float fY);

private: //  Methods

	void _rotate(float fXAngle, float fYAngle);
	void _zoom(float fZTranslation);
	void _pan(float fX, float fY);

public: // Methods

	virtual void _onMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/);
	virtual void _onKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/);	

	void _showTooltip(LPCTSTR szTitle, LPCTSTR szText);
	void _hideTooltip();

public: // Properties

	_oglProgram* _getOGLProgram() const { return m_pOGLProgram; }
	template<class Program>
	Program* _getOGLProgramAs() const { return dynamic_cast<Program*>(_getOGLProgram()); }
};

// ************************************************************************************************
class _model;
class _view;
class _instance;

// ************************************************************************************************
class _oglView 
	: public _oglRenderer
	, public _view
{

protected: // Members

	// Override the material
	map<_instance*, _material*> m_mapUserDefinedMaterials;

	// Mouse
	CPoint m_ptStartMousePosition;
	CPoint m_ptPrevMousePosition;	

	// Selection
	_oglSelectionFramebuffer* m_pSelectInstanceFrameBuffer;
	_instance* m_pPointedInstance;	

	// Tooltip
	clock_t m_tmShowTooltip;

public: // Methods

	_oglView();
	virtual ~_oglView();

protected: // Properties

	// _oglRendererSettings
	virtual _controller* _getController() const override { return getController(); }

public: // Methods

	// _view
	virtual void onWorldDimensionsChanged() override;
	virtual void onInstanceSelected(_view* pSender) override;
	virtual void onInstanceEnabledStateChanged(_view* pSender, _instance* pInstance, int iFlag) override;
	virtual void onInstancesEnabledStateChanged(_view* pSender) override;
	virtual void onInstancesShowStateChanged(_view* pSender) override;
	virtual void onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty) override;
	virtual void onControllerChanged() override;

	virtual void _load();

	virtual void _draw(CDC* pDC);

protected: // Methods

	virtual bool _prepareScene();
	virtual void _preDraw() {}
	virtual void _postDraw() {}
	virtual void _drawBuffers();
	virtual void _onPointInstance(const CPoint& /*point*/) {}

	void _drawFaces();
	void _drawConceptualFacesPolygons();
	void _drawLines();
	void _drawPoints();
	void _drawInstancesFrameBuffer();

	void _drawFaces(_model* pModel, bool bTransparent);	
	void _drawConceptualFacesPolygons(_model* pModel);
	void _drawLines(_model* pModel);
	void _drawPoints(_model* pModel);
	void _drawInstancesFrameBuffer(_model* pModel);	

	// http://nehe.gamedev.net/article/using_gluunproject/16013/
	bool getOGLPos(_model* pModel, int iX, int iY, float fDepth, GLdouble& dX, GLdouble& dY, GLdouble& dZ);

public: // Methods

	void _onMouseMoveEvent(UINT nFlags, CPoint point);
	void _onMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point);

	void addUserDefinedMaterial(const vector<_instance*>& vecInstances, float fR, float fG, float fB);
	void removeUserDefinedMaterials();

public: // Properties

	const map<_instance*, _material*>& getUserDefinedMaterials() const { return m_mapUserDefinedMaterials; }
};
#endif // #if defined _MFC_VER || defined _AFXDLL