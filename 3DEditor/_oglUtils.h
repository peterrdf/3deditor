#pragma once

#include "BinnPhongGLProgram.h"

#include "glew.h"
#include "wglew.h"

#include <limits>
#undef max
using namespace std;

class _oglUtils
{

public: // Methods
	
	static GLsizei getVerticesCountLimit(GLint iVertexLengthBytes)
	{
#ifdef WIN64
		return numeric_limits<GLint>::max() / iVertexLengthBytes;
#else
		return 6500000;
#endif
	}

	static GLsizei getIndicesCountLimit()
	{
		return 64800;
	}

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
#ifdef _LINUX
				wxLogError(wxT("OpenGL error state couldn't be reset."));
#else
				MessageBox(NULL, L"OpenGL error state couldn't be reset.", L"OpenGL", MB_ICONERROR | MB_OK);
#endif // _LINUX

				return;
			}

			errLast = err;

#ifdef _LINUX
			wxLogError(wxT("OpenGL error %d"), err);
#else
			MessageBox(NULL, (const wchar_t*)gluErrorStringWIN(errLast), L"OpenGL", MB_ICONERROR | MB_OK);
#endif // _LINUX
		}
	}
};

class _oglShader
{

protected: // Members

	GLenum	m_iType; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_iID;
	char* m_szCode;

public: // Methods

	_oglShader(GLenum iShaderType)
		: m_iType(iShaderType)
		, m_iID(0)
		, m_szCode(nullptr)
	{
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

	GLuint getID() const 
	{ 
		return m_iID; 
	}

	static char* getResource(int iResource, int iType)
	{
		HMODULE hModule = ::GetModuleHandleW(NULL);
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

	void getInfoLog(CString& stInfoLog)
	{
		stInfoLog = L"NA";

		int iLength = 0;
		glGetShaderiv(m_iID, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 0)
		{
			int iCharsWritten = 0;
			char* szInfoLog = new char[iLength];

			glGetShaderInfoLog(m_iID, iLength, &iCharsWritten, szInfoLog);

			stInfoLog = szInfoLog;
			delete[] szInfoLog;
		}
	}

	void printInfoLog()
	{
		CString stInfoLog;
		getInfoLog(stInfoLog);

		AfxMessageBox(stInfoLog);
	}
};

class _oglProgram
{

protected: // Members

	GLuint m_iID;

public: // Methods

	_oglProgram(void)
	{
		m_iID = glCreateProgram();
	}

	virtual ~_oglProgram()
	{
		if (m_iID != 0)
		{
			glDeleteProgram(m_iID);
		}		
	}

	void attachShader(_oglShader* pShader)
	{
		glAttachShader(m_iID, pShader->getID());
	}

	void detachShader(_oglShader* pShader)
	{
		glDetachShader(m_iID, pShader->getID());
	}

	virtual bool link()
	{
		glLinkProgram(m_iID);

		int param;
		glGetProgramiv(m_iID, GL_LINK_STATUS, &param);

		return param == GL_TRUE;
	}

	void use() 
	{ 
		glUseProgram(m_iID); 
	}
	
	GLuint getID() 
	{ 
		return m_iID; 
	}

	GLint getUniformLocation(char* szName)
	{
		return glGetUniformLocation(m_iID, szName);
	}

	void setUniform(char* szName, int iVal)
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		glUniform1i(iLocation, iVal);
	}

	void setUniform(GLint iLocation, int iVal)
	{
		glUniform1i(iLocation, iVal);
	}

	void setUniform(char* szName, int* val, int varDim, int count)
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

	void setUniform(GLint iLocation, int* val, int varDim, int count)
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

	void setUniform(char* szName, float val)
	{
		GLint iLocation = glGetUniformLocation(m_iID, szName);
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glUniform1f(iLocation, val);
	}

	void setUniform(GLint iLocation, float val)
	{
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glUniform1f(iLocation, val);
	}

	void setUniform(char* szName, float* val, int varDim, int count)
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

	void setUniformMatrix(char* szName, float* mat, int dimX, int dimY, bool bTranspose)
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

	void setUniformMatrix(GLint iLocation, float* mat, int dimX, int dimY, bool bTranspose)
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

	void setUniform(GLint iLocation, float* val, int varDim, int count)
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

	GLint getAttribLocation(char* szName)
	{ 
		return glGetAttribLocation(m_iID, szName);
	}

	void setAttrib(GLint iLocation, float val)
	{ 
		if (iLocation == -1)
		{
			assert(false);

			return;
		}

		glVertexAttrib1f(iLocation, val);
	}

	void setAttrib(GLint iLocation, float* val, int varDim)
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

	void bindAttribLocation(unsigned int iIndex, char* szName)
	{
		glBindAttribLocation(m_iID, iIndex, szName);

		unsigned int iError = glGetError();
		VERIFY(iError == 0);

		DWORD dwError = GetLastError();
		VERIFY(dwError == 0);
	}

	GLint enableVertexAttribArray(char* szName)
	{
		GLint iLocation = glGetAttribLocation(m_iID, szName);
		if (iLocation != -1)
		{
			glEnableVertexAttribArray(iLocation);
		}			

		return iLocation;
	}

	void disableVertexAttribArray(GLint iLocation)
	{
		glDisableVertexAttribArray(iLocation);
	}

	void getInfoLog(CString& stInfoLog)
	{
		stInfoLog = L"NA";

		int iLength = 0;
		glGetProgramiv(m_iID, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 0)
		{
			int iCharsWritten = 0;
			char* szInfoLog = new char[iLength];

			glGetProgramInfoLog(m_iID, iLength, &iCharsWritten, szInfoLog);

			stInfoLog = szInfoLog;
			delete[] szInfoLog;
		}
	}

	void printInfoLog()
	{
		CString stInfoLog;
		getInfoLog(stInfoLog);

		AfxMessageBox(stInfoLog);
	}
};

class _oglBinnPhongProgram : public _oglProgram
{

private: // Members

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

public: // Methods

	_oglBinnPhongProgram(bool bTextureSupport)
		: _oglProgram()
		, m_bTextureSupport(bTextureSupport)
		, m_iUseBinnPhongModel(-1)
		, m_iUseTexture(-1)
		, m_iSampler(-1)
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

	virtual ~_oglBinnPhongProgram(void)
	{
	}

	virtual bool link()
	{
		if (_oglProgram::link())
		{
			m_iUseBinnPhongModel = glGetUniformLocation(m_iID, "uUseBinnPhongModel");
			ASSERT(m_iUseBinnPhongModel >= 0);

			if (m_bTextureSupport)
			{
				m_iUseTexture = glGetUniformLocation(m_iID, "uUseTexture");
				ASSERT(m_iUseTexture >= 0);

				m_iSampler = glGetUniformLocation(m_iID, "uSampler");
				ASSERT(m_iSampler >= 0);
			}

			m_iMVMatrix = glGetUniformLocation(m_iID, "uMVMatrix");
			ASSERT(m_iMVMatrix >= 0);

			m_iPMatrix = glGetUniformLocation(m_iID, "uPMatrix");
			ASSERT(m_iPMatrix >= 0);

			m_iNMatrix = glGetUniformLocation(m_iID, "uNMatrix");
			ASSERT(m_iNMatrix >= 0);

			m_iPointLightingLocation = glGetUniformLocation(m_iID, "uPointLightingLocation");
			ASSERT(m_iPointLightingLocation >= 0);

			m_iMaterialShininess = glGetUniformLocation(m_iID, "uMaterialShininess");
			ASSERT(m_iMaterialShininess >= 0);

			m_iMaterialAmbientColor = glGetUniformLocation(m_iID, "uMaterialAmbientColor");
			ASSERT(m_iMaterialAmbientColor >= 0);

			m_iTransparency = glGetUniformLocation(m_iID, "uTransparency");
			ASSERT(m_iTransparency >= 0);

			m_iMaterialDiffuseColor = glGetUniformLocation(m_iID, "uMaterialDiffuseColor");
			ASSERT(m_iMaterialDiffuseColor >= 0);

			m_iMaterialSpecularColor = glGetUniformLocation(m_iID, "uMaterialSpecularColor");
			ASSERT(m_iMaterialSpecularColor >= 0);

			m_iMaterialEmissiveColor = glGetUniformLocation(m_iID, "uMaterialEmissiveColor");
			ASSERT(m_iMaterialEmissiveColor >= 0);

			m_iVertexPosition = glGetAttribLocation(m_iID, "aVertexPosition");
			ASSERT(m_iVertexPosition >= 0);

			m_iVertexNormal = glGetAttribLocation(m_iID, "aVertexNormal");
			ASSERT(m_iVertexNormal >= 0);

			if (m_bTextureSupport)
			{
				m_iTextureCoord = glGetAttribLocation(m_iID, "aTextureCoord");
				ASSERT(m_iTextureCoord >= 0);
			}

			return true;
		}

		return false;
	}

	bool getTextureSupport() const
	{
		return m_bTextureSupport;
	}

	GLint geUseBinnPhongModel() const
	{
		return m_iUseBinnPhongModel;
	}

	GLint geUseTexture() const
	{
		ASSERT(m_bTextureSupport);

		return m_iUseTexture;
	}

	GLint getSampler() const
	{
		ASSERT(m_bTextureSupport);

		return m_iSampler;
	}

	GLint getMVMatrix() const
	{
		return m_iMVMatrix;
	}

	GLint getPMatrix() const
	{
		return m_iPMatrix;
	}

	GLint getNMatrix() const
	{
		return m_iNMatrix;
	}

	GLint getPointLightingLocation() const
	{
		return m_iPointLightingLocation;
	}

	GLint getMaterialShininess() const
	{
		return m_iMaterialShininess;
	}

	GLint getMaterialAmbientColor() const
	{
		return m_iMaterialAmbientColor;
	}

	GLint getTransparency() const
	{
		return m_iTransparency;
	}


	GLint getMaterialDiffuseColor() const
	{
		return m_iMaterialDiffuseColor;
	}

	GLint getMaterialSpecularColor() const
	{
		return m_iMaterialSpecularColor;
	}

	GLint getMaterialEmissiveColor() const
	{
		return m_iMaterialEmissiveColor;
	}

	GLint getVertexPosition() const
	{
		return m_iVertexPosition;
	}

	GLint getVertexNormal() const
	{
		return m_iVertexNormal;
	}

	GLint getTextureCoord() const
	{
		ASSERT(m_bTextureSupport);

		return m_iTextureCoord;
	}
};