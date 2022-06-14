// https://www.cse.msu.edu/~cse872/tutorial4.html

//
// Name :         Texture.h
// Description :  Header for CTexture, texture image class for OpenGL.
// Version :      See Texture.cpp
//

#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <GL/gl.h>

#ifdef _LINUX
typedef int BOOLEAN;
typedef const char * LPCTSTR;
typedef unsigned char BYTE;
#endif // _LINUX


class CTexture
{
public:
	GLuint MipTexName();
	CTexture();
	CTexture(const CTexture &p_img);
	virtual ~CTexture();

	GLuint TexName();

	void Fill(int r, int g, int b);
	void Set(int x, int y, int r, int g, int b);
	void SetSize(int p_x, int p_y);
	void SameSize(const CTexture &p_img);
	void Copy(const CTexture &p_img);
	bool LoadFile(LPCTSTR lpszPathName);
   BOOLEAN Empty() const {return m_width <= 0 || m_height <= 0;}
   CTexture &operator=(const CTexture &p_img);

   BYTE *operator[](int i) {return m_image[i];}
   const BYTE *operator[](int i) const {return m_image[i];}
	BYTE *Row(int i) {return m_image[i];}
	const BYTE *Row(int i) const {return m_image[i];}

   int Width() const {return m_width;}
   int Height() const {return m_height;}
   BYTE *ImageBits() const {return m_image[0];}

private:
	bool m_mipinitialized;
	bool m_initialized;
	GLuint m_texname;
   GLuint m_miptexname;
   bool ReadDIBFile(std::istream &file);
   bool ReadPPMFile(std::istream &file);
	int m_height;
	int m_width;
	BYTE ** m_image;
};

#endif
