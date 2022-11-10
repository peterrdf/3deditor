#pragma once

// ------------------------------------------------------------------------------------------------
#include "_oglUtils.h"

// ------------------------------------------------------------------------------------------------
class COpenGLContext
{

private: // Members

	// --------------------------------------------------------------------------------------------
	HDC m_hDC;

	// --------------------------------------------------------------------------------------------
	HGLRC m_hGLContext;

public: // Methods

	// --------------------------------------------------------------------------------------------
	COpenGLContext(HDC hDC);

	// --------------------------------------------------------------------------------------------
	~COpenGLContext();

	// --------------------------------------------------------------------------------------------
	BOOL MakeCurrent();	
	
#ifdef _ENABLE_OPENGL_DEBUG
	// --------------------------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	void EnableDebug();
#endif // _ENABLE_OPENGL_DEBUG

private: // Methods

	// --------------------------------------------------------------------------------------------
	void Create();	

#ifdef _ENABLE_OPENGL_DEBUG
	// --------------------------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	// --------------------------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	static void DebugOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message);
#endif // _ENABLE_OPENGL_DEBUG
};

