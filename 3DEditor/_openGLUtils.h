#pragma once

#include "glew.h"
#include "wglew.h"

#include <limits>
#undef max
using namespace std;

struct _vector3d
{
	double x;
	double y;
	double z;
};

struct _matrix
{
	double _11, _12, _13;
	double _21, _22, _23;
	double _31, _32, _33;
	double _41, _42, _43;
};

struct _oglMatrix
{
	double _11, _12, _13, _14;
	double _21, _22, _23, _24;
	double _31, _32, _33, _34;
	double _41, _42, _43, _44;
};

class _openGLUtils
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

	static void matrixIdentity(_matrix* pInOut)
	{
		memset(pInOut, 0, sizeof(_matrix));

		pInOut->_11 = pInOut->_22 = pInOut->_33 = 1.;
	}

	static void oglMatrixIdentity(_oglMatrix* pInOut)
	{
		memset(pInOut, 0, sizeof(_oglMatrix));

		pInOut->_11 = pInOut->_22 = pInOut->_33 = pInOut->_44 = 1.;
	}

	static void	_transform(const _vector3d* pV, const _matrix* pM, _vector3d* pOut)
	{
		_vector3d pTmp;
		pTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
		pTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
		pTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

		pOut->x = pTmp.x;
		pOut->y = pTmp.y;
		pOut->z = pTmp.z;
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

