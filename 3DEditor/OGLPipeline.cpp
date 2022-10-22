#include "stdafx.h"
#include "OGLPipeline.h"
#include "Resource.h"
#include "glew.h"
#include "generic.h"

// ------------------------------------------------------------------------------------------------
COGLPipeline::COGLPipeline(int iVertexShader, int iFragmentShader)
	: m_iVertexProgram(0)
	, m_iFragmentProgram(0)
	, m_iPipeline(0)
{
	char* szVertexProgram = LoadTextFileResource(iVertexShader);
	m_iVertexProgram = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &szVertexProgram);
	delete szVertexProgram;	

	if (m_iVertexProgram == 0)
	{
		ASSERT(FALSE);

		return;
	}

	char* szFragmentShader = LoadTextFileResource(iFragmentShader);
	m_iFragmentProgram = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &szFragmentShader);
	delete szFragmentShader;

	if (m_iFragmentProgram == 0)
	{
		ASSERT(FALSE);

		return;
	}

	glGenProgramPipelines(1, &m_iPipeline);
	if (m_iPipeline == 0)
	{
		ASSERT(FALSE);

		return;
	}

	glBindProgramPipeline(m_iPipeline);
	glUseProgramStages(m_iPipeline, GL_VERTEX_SHADER_BIT, m_iVertexProgram);
	glUseProgramStages(m_iPipeline, GL_FRAGMENT_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, m_iFragmentProgram);
}

// ------------------------------------------------------------------------------------------------
// dtor
COGLPipeline::~COGLPipeline()
{
	if (m_iPipeline != 0)
	{
		glDeleteProgramPipelines(1, &m_iPipeline);
	}	

	if (m_iVertexProgram != 0)
	{
		glDeleteProgram(m_iVertexProgram);
	}

	if (m_iFragmentProgram != 0)
	{
		glDeleteProgram(m_iFragmentProgram);
	}
}

// ------------------------------------------------------------------------------------------------
/*static*/ char* COGLPipeline::LoadTextFileResource(int name)
{
	HMODULE hModule = ::GetModuleHandleW(NULL);
	HRSRC hResource = ::FindResourceW(hModule, MAKEINTRESOURCEW(name), MAKEINTRESOURCEW(TEXTFILE));
	HGLOBAL rcData = ::LoadResource(hModule, hResource);

	char* data = static_cast<char*>(::LockResource(rcData));
	DWORD size = ::SizeofResource(hModule, hResource);

	char* buffer = new char[size + 1];
	::memcpy(buffer, data, size);
	buffer[size] = 0;

	return  buffer;
}

// ------------------------------------------------------------------------------------------------
GLuint COGLPipeline::getPipeline() const
{
	return m_iPipeline;
}

// ------------------------------------------------------------------------------------------------
GLint COGLPipeline::getVertexProgram() const
{
	return m_iVertexProgram;
}

// ------------------------------------------------------------------------------------------------
GLint COGLPipeline::getFragmentProgram() const
{
	return m_iFragmentProgram;
}