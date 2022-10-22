#include "StdAfx.h"
#include "GLProgram.h"
#include "GLShader.h"

CGLProgram::CGLProgram(void)
{
	m_ID = glCreateProgram();
}

CGLProgram::~CGLProgram(void)
{
	glDeleteProgram(m_ID);
}

void CGLProgram::AttachShader(CGLShader* shader)
{
	glAttachShader(m_ID, shader->GetID());
}

void CGLProgram::DetachShader(CGLShader* shader)
{
	glDetachShader(m_ID, shader->GetID());
}

bool CGLProgram::Link()
{
	glLinkProgram(m_ID); 

	int param;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &param);
	return param == GL_TRUE;
}

CString* CGLProgram::GetInfoLog()
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
	CString* retStr = NULL;

	glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = new char [infologLength];
        glGetShaderInfoLog(m_ID, infologLength, &charsWritten, infoLog);
		retStr = new CString(infoLog);
		delete [] infoLog;
    }
	return retStr;
}

GLint CGLProgram::GetUniformLocation(char* name)
{
	return glGetUniformLocation(m_ID, name);
}

void CGLProgram::SetUniform(char* name, int val)
{
	GLint loc = glGetUniformLocation(m_ID, name);
	glUniform1i(loc, val);
}

void CGLProgram::SetUniform(GLint loc, int val)
{
	glUniform1i(loc, val);
}

void CGLProgram::SetUniform(char* name, int* val, int varDim, int count)
{
	GLint loc = glGetUniformLocation(m_ID, name);
	if(loc == -1) return;

	if(varDim == 4)
		glUniform4iv(loc, count, val);
	else if(varDim == 3)
		glUniform3iv(loc, count, val);
	else if(varDim == 2)
		glUniform2iv(loc, count, val);
	else if(varDim == 1)
		glUniform1iv(loc, count, val);
}

void CGLProgram::SetUniform(GLint loc, int* val, int varDim, int count)
{
	if(loc == -1) return;

	if(varDim == 4)
		glUniform4iv(loc, count, val);
	else if(varDim == 3)
		glUniform3iv(loc, count, val);
	else if(varDim == 2)
		glUniform2iv(loc, count, val);
	else if(varDim == 1)
		glUniform1iv(loc, count, val);
}

void CGLProgram::SetUniform(char* name, float val)
{
	GLint loc = glGetUniformLocation(m_ID, name);
	if(loc == -1) return;

	glUniform1f(loc, val);
}

void CGLProgram::SetUniform(GLint loc, float val)
{
	if(loc == -1) return;
	glUniform1f(loc, val);
}

void CGLProgram::SetUniform(char* name, float* val, int varDim, int count)
{
	GLint loc = glGetUniformLocation(m_ID, name);
	if(loc == -1) return;

	if(varDim == 4)
		glUniform4fv(loc, count, val);// count je 1, ako uniform nije polje vektora
	else if(varDim == 3)
		glUniform3fv(loc, count, val);
	else if(varDim == 2)
		glUniform2fv(loc, count, val);
	else if(varDim == 1)
		glUniform1fv(loc, count, val);
}

void CGLProgram::SetUniform(GLint loc, float* val, int varDim, int count)
{
	if(loc == -1) return;

	if(varDim == 4)
		glUniform4fv(loc, count, val);// count je 1, ako uniform nije polje vektora
	else if(varDim == 3)
		glUniform3fv(loc, count, val);
	else if(varDim == 2)
		glUniform2fv(loc, count, val);
	else if(varDim == 1)
		glUniform1fv(loc, count, val);
}

void CGLProgram::SetUniformMatrix(char* name, float* mat, int dimX, int dimY, bool bTranspose)
{
	GLint loc = glGetUniformLocation(m_ID, name);
	if(loc == -1) return;

	if((dimX == 4) && (dimY == 4))
		glUniformMatrix4fv(loc, 1, bTranspose, mat);
}

void CGLProgram::SetUniformMatrix(GLint loc, float* mat, int dimX, int dimY, bool bTranspose)
{
	if(loc == -1) return;

	if((dimX == 4) && (dimY == 4))
		glUniformMatrix4fv(loc, 1, bTranspose, mat);
}

void CGLProgram::SetAttrib(GLint loc, float* val, int varDim)
{
	if(varDim == 4)
		glVertexAttrib4fv(loc, val);
	else if(varDim == 3)
		glVertexAttrib3fv(loc, val);
	else if(varDim == 2)
		glVertexAttrib2fv(loc, val);
	else if(varDim == 1)
		glVertexAttrib1fv(loc, val);
}

GLint CGLProgram::EnableVertexAttribArray(char* name)
{
	GLint loc = glGetAttribLocation(m_ID,name);
	if(loc != -1)
		glEnableVertexAttribArray(loc);
	
	return loc;
}

void CGLProgram::DisableVertexAttribArray(GLint loc)
{
	glDisableVertexAttribArray(loc);
}

void CGLProgram::BindAttribLocation(unsigned int index, char* name)
{
	glBindAttribLocation(m_ID, index, name);
	unsigned int err = glGetError();
	VERIFY(err == 0);
	DWORD werr = GetLastError();
	VERIFY(werr == 0);
}

void CGLProgram::PrintInfoLog()
{
	CString* errStr = GetInfoLog();
	AfxMessageBox(*errStr);
}

void CGLProgram::GetUniformsInfo()
{
	GLint maxUniformLen;
	GLint numUniforms;
	char *uniformName;
	GLint index;
	glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &numUniforms);
	glGetProgramiv(m_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);
	uniformName = new char[maxUniformLen];
	for(index = 0; index < numUniforms; index++)
	{
		GLint size;
		GLenum type;
		GLint location;
		// Get the Uniform Info
		glGetActiveUniform(m_ID, index, maxUniformLen, NULL, &size, &type, uniformName);
		// Get the uniform location
		location = glGetUniformLocation(m_ID, uniformName);
	}
	delete uniformName;
}
