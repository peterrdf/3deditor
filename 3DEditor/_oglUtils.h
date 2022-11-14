#pragma once

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
		assert(m_iID != 0);
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

	GLint getUseBinnPhongModel() const
	{
		return m_iUseBinnPhongModel;
	}

	GLint getUseTexture() const
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

	_oglContext(HDC hDC, int iSamples = 16)
		: m_hDC(hDC)
		, m_hGLContext(NULL)
		, m_iSamples(iSamples)
	{
		assert(m_hDC != NULL);

		create();
	}

	virtual ~_oglContext()
	{
		if (m_hGLContext != NULL)
		{
			BOOL bResult = wglMakeCurrent(m_hDC, NULL);
			assert(bResult);

			bResult = wglDeleteContext(m_hGLContext);
			assert(bResult);
		}
	}

	BOOL makeCurrent()
	{
		assert(m_hDC != NULL);
		assert(m_hGLContext != NULL);

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
		WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClassEx.lpszClassName = L"_OpenGL_Renderer_Window_";

		if (!GetClassInfoEx(::AfxGetInstanceHandle(), WndClassEx.lpszClassName, &WndClassEx))
		{
			if (RegisterClassEx(&WndClassEx) == 0)
			{
				MessageBox(NULL, L"RegisterClassEx() failed.", L"Error", MB_ICONERROR | MB_OK);

				PostQuitMessage(WM_QUIT);
			}
		}

		HWND hWndTemp = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, L"OpenGL", dwStyle, 0, 0, 600, 600, NULL, NULL, ::AfxGetInstanceHandle(), NULL);

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

		BOOL bMSAASupport = FALSE;

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
				strErrorMessage.Format(L"OpenGL version %d.%d or higher is required.", MIN_GL_MAJOR_VERSION, MIN_GL_MINOR_VERSION);

				MessageBox(NULL, strErrorMessage, L"Error", MB_ICONERROR | MB_OK);

				PostQuitMessage(WM_QUIT);
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
			MessageBox(NULL, L"glewInit() failed.", L"Error", MB_ICONERROR | MB_OK);

			PostQuitMessage(WM_QUIT);
		}

		bResult = wglMakeCurrent(NULL, NULL);
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

		glDebugMessageCallbackARB(&_oglContext::debugCallback, NULL);
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