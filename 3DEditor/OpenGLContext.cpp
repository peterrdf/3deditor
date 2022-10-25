#include "stdafx.h"
#include "OpenGLContext.h"

// ------------------------------------------------------------------------------------------------
const int MIN_GL_MAJOR_VERSION = 3;
const int MIN_GL_MINOR_VERSION = 3;

// ------------------------------------------------------------------------------------------------
extern BOOL TEST_MODE;

// ------------------------------------------------------------------------------------------------
extern wchar_t OPENGL_RENDERER_WINDOW[];

// ------------------------------------------------------------------------------------------------
COpenGLContext::COpenGLContext(HDC hDC)
	: m_hDC(hDC)
	, m_hGLContext(NULL)
{
	ASSERT(m_hDC != NULL);

	Create();
}

// ------------------------------------------------------------------------------------------------
COpenGLContext::~COpenGLContext()
{
	// PATCH: AMD 6700 XT - Access violation
	if (TEST_MODE)
	{
		return;
	}

	if (m_hGLContext != NULL)
	{
		BOOL bResult = wglMakeCurrent(m_hDC, NULL);
		ASSERT(bResult);

		bResult = wglDeleteContext(m_hGLContext);
		ASSERT(bResult);
	}
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLContext::MakeCurrent()
{
	ASSERT(m_hDC != NULL);
	ASSERT(m_hGLContext != NULL);

	return wglMakeCurrent(m_hDC, m_hGLContext);
}

// ------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
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

// ------------------------------------------------------------------------------------------------
void COpenGLContext::Create()
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

	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	WNDCLASSEX WndClassEx;
	memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.hInstance = AfxGetInstanceHandle();
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.lpszClassName = OPENGL_RENDERER_WINDOW;

	if (!GetClassInfoEx(AfxGetInstanceHandle(), WndClassEx.lpszClassName, &WndClassEx))
	{
		if (RegisterClassEx(&WndClassEx) == 0)
		{
			MessageBox(NULL, L"RegisterClassEx() failed.", L"Error", MB_ICONERROR | MB_OK);

			PostQuitMessage(WM_QUIT);
		}
	}

	HWND hWndTemp = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, L"OpenGL", Style, 0, 0, 600, 600, NULL, NULL, AfxGetInstanceHandle(), NULL);

	HDC hDCTemp = ::GetDC(hWndTemp);

	// Get the best available match of pixel format for the device context   
	int iPixelFormat = ChoosePixelFormat(hDCTemp, &pfd);
	ASSERT(iPixelFormat > 0);

	// Make that the pixel format of the device context  
	BOOL bResult = SetPixelFormat(hDCTemp, iPixelFormat, &pfd);
	ASSERT(bResult);

	HGLRC hTempGLContext = wglCreateContext(hDCTemp);
	ASSERT(hTempGLContext);

	bResult = wglMakeCurrent(hDCTemp, hTempGLContext);
	ASSERT(bResult);

	COpenGLUtils::Check4Errors();

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
			strErrorMessage.Format(L"IFCViewerAx component requires OpenGL version %d.%d or higher.", MIN_GL_MAJOR_VERSION, MIN_GL_MINOR_VERSION);

			MessageBox(NULL, strErrorMessage, L"Error", MB_ICONERROR | MB_OK);

			PostQuitMessage(WM_QUIT);
		}

		/*
		* Pixel format
		*/
		iPixelFormat = 0;
		for (int iSamples = TEST_MODE ? 1 : 16; iSamples >= 0; iSamples--)
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
	ASSERT(bResult);

	bResult = wglDeleteContext(hTempGLContext);
	ASSERT(bResult);

	if (bMSAASupport)
	{
		bResult = SetPixelFormat(m_hDC, iPixelFormat, &pfd);
		ASSERT(bResult);

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
		ASSERT(m_hGLContext);		
	}
	else
	{
		iPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
		ASSERT(iPixelFormat > 0);

		bResult = SetPixelFormat(m_hDC, iPixelFormat, &pfd);
		ASSERT(bResult);

		m_hGLContext = wglCreateContext(m_hDC);
		ASSERT(m_hGLContext);
	}

	::ReleaseDC(hWndTemp, hDCTemp);
	::DestroyWindow(hWndTemp);

#ifdef _ENABLE_OPENGL_DEBUG
	remove("_OpenGL_Debug_.txt");
#endif
}

// ------------------------------------------------------------------------------------------------
#ifdef _ENABLE_OPENGL_DEBUG
void COpenGLContext::EnableDebug()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

	glDebugMessageCallbackARB(&COpenGLContext::DebugCallback, NULL);	
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}

// ------------------------------------------------------------------------------------------------
void COpenGLContext::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
	DebugOutputToFile(source, type, id, severity, message);
}

// ------------------------------------------------------------------------------------------------
void COpenGLContext::DebugOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message)
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
