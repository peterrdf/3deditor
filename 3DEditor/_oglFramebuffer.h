#pragma once

#include "_openGLUtils.h"

#include <assert.h>

#include <map>
using namespace std;

#include "_material.h"

#define BUFFER_SIZE 512

class _oglFramebuffer
{

private: // Members

	GLuint m_iFrameBuffer;
	GLuint m_iTextureBuffer;
	GLuint m_iRenderBuffer;

public: // Methods

	_oglFramebuffer()
		: m_iFrameBuffer(0)
		, m_iTextureBuffer(0)
		, m_iRenderBuffer(0)
	{
	}

	virtual ~_oglFramebuffer()
	{
		if (m_iFrameBuffer != 0)
		{
			glDeleteFramebuffers(1, &m_iFrameBuffer);
		}

		if (m_iTextureBuffer != 0)
		{
			glDeleteTextures(1, &m_iTextureBuffer);
		}

		if (m_iRenderBuffer != 0)
		{
			glDeleteRenderbuffers(1, &m_iRenderBuffer);
		}
	}

	void Create(GLsizei iWidth = BUFFER_SIZE, GLsizei iHeight = BUFFER_SIZE)
	{
		if (m_iFrameBuffer == 0)
		{
			assert(m_iTextureBuffer == 0);
			assert(m_iRenderBuffer == 0);

			/*
			* Frame buffer
			*/
			glGenFramebuffers(1, &m_iFrameBuffer);
			assert(m_iFrameBuffer != 0);

			glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);

			/*
			* Texture buffer
			*/
			glGenTextures(1, &m_iTextureBuffer);
			assert(m_iTextureBuffer != 0);

			glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iTextureBuffer, 0);

			/*
			* Depth buffer
			*/
			glGenRenderbuffers(1, &m_iRenderBuffer);
			assert(m_iRenderBuffer != 0);

			glBindRenderbuffer(GL_RENDERBUFFER, m_iRenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, iWidth, iHeight);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iRenderBuffer);

			GLenum arDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, arDrawBuffers);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			COpenGLUtils::Check4Errors();
		} // if (m_iFrameBuffer == 0)
	}

	virtual bool IsInitialized() const
	{
		return m_iFrameBuffer != 0;
	}

	void Bind()
	{
		assert(m_iFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);
	}

	void Unbind()
	{
		assert(m_iFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

class _oglSelectionFramebuffer : public _oglFramebuffer
{

private: // Members

	map<int64_t, _material> m_mapSelectionColors;

public: // Methods

	_oglSelectionFramebuffer()
		: _oglFramebuffer()
		, m_mapSelectionColors()
	{
	}

	virtual ~_oglSelectionFramebuffer()
	{
	}

	void Initialize()
	{

	}
	
	virtual bool IsInitialized() const override
	{
		return _oglFramebuffer::IsInitialized() && !m_mapSelectionColors.empty();
	}
};