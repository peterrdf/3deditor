#pragma once

#include <fstream>

#include "../glew-2.2.0/include/GL/glew.h"
#include <GL/glu.h>

#include "stb/stb_image.h"

#ifdef _WINDOWS
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

// ************************************************************************************************
class _texture
{

private: // Fields

	GLuint m_iName;
	
public: // Methods

	_texture()
		: m_iName(0)
	{
	}

	virtual ~_texture()
	{
		if (m_iName != 0)
		{
			glDeleteTextures(1, &m_iName);
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

	bool load(const wchar_t* lpszPathName)
	{
#ifdef _WINDOWS
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
				glGenTextures(1, &m_iName);
				glBindTexture(GL_TEXTURE_2D, m_iName);

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
#endif // _WINDOWS

		return false;
	}

	GLuint getName() const { return m_iName; }
};