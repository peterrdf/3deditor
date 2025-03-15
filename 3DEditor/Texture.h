#pragma once

#include <fstream>

#include "_geometry.h"
#include "./../stb/stb_image.h"

// ************************************************************************************************
class CTexture
{

private: // Fields

	GLuint m_iOGLName;
	
public: // Methods

	CTexture();
	virtual ~CTexture();

	bool LoadFile(LPCTSTR lpszPathName);

	GLuint getOGLName() const { return m_iOGLName; }
};