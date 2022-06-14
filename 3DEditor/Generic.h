#pragma once

#include "stdafx.h"

#include <vector>
#include <map>
#include <limits>

#undef max

using namespace std;

#ifdef _LINUX
#include <wx/wx.h>

typedef int BOOL;
typedef wxPoint CPoint;
typedef wxRect CRect;
typedef unsigned int UINT;
#endif // _LINUX


// ----------------------------------------------------------------------------
/*
// X, Y, Z, Nx, Ny, Nz, Tx, Ty, Ambient, Diffuse, Emissive, Specular, Tnx, Tny, Tnz, Bnx, Bny, Bnz
// (Tx, Ty - bit 6; Normal vectors - bit 5, Diffuse, Emissive, Specular - bit 25, 26 & 27, Tangent vectors - bit 28, Binormal vectors - bit 29)
*/
#define VERTEX_LENGTH  18

// ----------------------------------------------------------------------------
// X, Y, Z, Nx, Ny, Nz, Tx, Ty
#define GEOMETRY_VBO_VERTEX_LENGTH  8

// ----------------------------------------------------------------------------
// X, Y, Z
#define VECTORS_VBO_VERTEX_LENGTH  3

// ----------------------------------------------------------------------------
#define DEFAULT_CIRCLE_SEGMENTS 36

// ----------------------------------------------------------------------------
// OpenGL
class COpenGL
{

private: // Members

public: // Methods

	// ------------------------------------------------------------------------
	// glBufferData, size arg
	static GLsizei GetGeometryVerticesCountLimit()
	{
#ifdef WIN64
		return numeric_limits<GLint>::max() / (GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));
#else
		return 6500000;
#endif
	}

	// ------------------------------------------------------------------------
	// glBufferData, size arg
	static GLsizei GetVectorsVerticesCountLimit()
	{
#ifdef WIN64
		return numeric_limits<GLint>::max() / (VECTORS_VBO_VERTEX_LENGTH * sizeof(float));
#else
		return 6500000;
#endif
	}

	// ------------------------------------------------------------------------
	// glBufferData, size arg
	static GLsizei GetIndicesCountLimit()
	{
		return 65000;
	}

	// ------------------------------------------------------------------------
	// Wrapper for glGetError()/gluErrorStringWIN()
	static void Check4Errors()
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
            MessageBox(NULL, (const wchar_t *)gluErrorStringWIN(errLast), L"OpenGL", MB_ICONERROR | MB_OK);
#endif // _LINUX
		}
	}
};

// ------------------------------------------------------------------------------------------------
static bool SaveScreenshot(unsigned char* arPixels, unsigned int iWidth, unsigned int iHeight, const wchar_t* szFilePath)
{
	static unsigned char header[54] = {
	0x42, 0x4D, 0x36, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	((unsigned __int16*)header)[9] = (unsigned short)iWidth;
	((unsigned __int16*)header)[11] = (unsigned short)iHeight;

	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	unsigned long lSize = 0;
	::WriteFile(hFile, header, sizeof(header), &lSize, NULL);
	::WriteFile(hFile, arPixels, iWidth * iHeight * 3, &lSize, NULL);

	CloseHandle(hFile);

	return true;
}


