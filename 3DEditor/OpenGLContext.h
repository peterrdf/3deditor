#pragma once

#include "Generic.h"

// ----------------------------------------------------------------------------
// Describes an OpenGL context
class COpenGLContext
{

private: // Members

	// ------------------------------------------------------------------------
	// DC
	HDC m_hDC;

	// ------------------------------------------------------------------------
	// OpenGL Context
	HGLRC m_hGLContext;

public: // Methods

	// ------------------------------------------------------------------------
	// ctor
	COpenGLContext(HDC hDC);

	// ------------------------------------------------------------------------
	// dtor
	~COpenGLContext();	

	// ------------------------------------------------------------------------
	// Wrapper method for wglMakeCurrent()
	BOOL MakeCurrent();	

	// ------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	void EnableDebug();

private: // Methods

	// ------------------------------------------------------------------------
	// Creates new context (MSAA)
	void Create();	

	// ------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	// ------------------------------------------------------------------------
	// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
	static void DebugOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message);
};

