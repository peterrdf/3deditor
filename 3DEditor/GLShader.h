#pragma once

class CGLShader
{
public:
	CGLShader(GLenum shaderType);
	virtual ~CGLShader(void);
	//bool	Load(CString fileName);
	bool	Load(int resource);
	bool	Compile(void);
	void	PrintInfoLog();
	
	CString*		GetInfoLog();
	inline GLuint	GetID(){return m_ID;}

protected:
	char* CGLShader::FileRead(CString fileName) ;

protected:
	GLenum	m_Type; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_ID;
	char*	m_code;
};
