#include "stdafx.h"
#include "Texture.h"

#include <GL/glu.h>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CTexture::CTexture()
	: m_iOGLName(0)
{}

/*virtual*/ CTexture::~CTexture()
{
	if (m_iOGLName != 0)
	{
		glDeleteTextures(1, &m_iOGLName);
	}
}


static bool isPowerOf2(int iNumber)
{
	if (iNumber > 0)
	{
		return (iNumber & (iNumber - 1)) == 0;
	}

	return false;
}

bool CTexture::LoadFile(LPCTSTR lpszPathName)
{
	fs::path pthFile = lpszPathName;
	if ((pthFile.extension() == ".jpg") ||
		(pthFile.extension() == ".jpeg") ||
		(pthFile.extension() == ".png") ||
		(pthFile.extension() == ".bmp"))
	{
		stbi_set_flip_vertically_on_load(true);

		int iWidth = 0;
		int iHeight = 0;
		int iChannels = 0;
		unsigned char* pData = stbi_load(CW2A(lpszPathName), &iWidth, &iHeight, &iChannels, 0);
		if (pData != nullptr)
		{
			glGenTextures(1, &m_iOGLName);
			glBindTexture(GL_TEXTURE_2D, m_iOGLName);

			// TODO:
			// https://learnopengl.com/Getting-started/Textures
			// https://www.cityjson.org/specs/1.1.3/#texture-object
			// wrapMode, borderColor
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			if (!isPowerOf2(iWidth) || !isPowerOf2(iHeight))
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			}

			glTexImage2D(GL_TEXTURE_2D, 0,
				iChannels == 3 ? GL_RGB : GL_RGBA,
				iWidth,
				iHeight,
				0,
				iChannels == 3 ? GL_RGB : GL_RGBA,
				GL_UNSIGNED_BYTE, 
				pData);
		}

		stbi_image_free(pData);

		return true;
	} // if (pData != nullptr)

	assert(false); // Unknown file type

	return false;
}




