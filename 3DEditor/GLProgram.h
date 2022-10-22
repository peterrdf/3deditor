#pragma once

class CGLShader;
class CGLProgram
{
public:
	CGLProgram(void);
	virtual ~CGLProgram(void);
	void	AttachShader(CGLShader* shader);
	void	DetachShader(CGLShader* shader);
	virtual bool Link();
	void	PrintInfoLog();
	void	GetUniformsInfo();

	inline void Use() { glUseProgram(m_ID); }

	CString*		GetInfoLog();
	GLuint			GetID(){return m_ID;}

	GLint	GetUniformLocation(char* name);
	void	SetUniform(char* name, int val);				// prima jednu int prom
	void	SetUniform(GLint loc, int val);
	// prima vektor duzine count, a varDim je dimenzionalnost promenljive (npr vec4 za varDim=4)
	void	SetUniform(char* name, int* val, int varDim, int count);	
	void	SetUniform(char* name, float val);
	void	SetUniform(char* name, float* val, int varDim, int count);	// prima vektor duzine count
	void	SetUniformMatrix(char* name, float* mat, int dimX, int dimY, bool bTranspose);
	void	SetUniform(GLint loc, int* val, int varDim, int count);	
	void	SetUniform(GLint loc, float val);
	void	SetUniform(GLint loc, float* val, int varDim, int count);	// prima vektor duzine count
	void	SetUniformMatrix(GLint loc, float* mat, int dimX, int dimY, bool bTranspose);

	GLint	GetAttribLocation(char* name) {return glGetAttribLocation(m_ID, name);}
	void	SetAttrib(GLint loc, float val) {glVertexAttrib1f(loc, val);}
	void	SetAttrib(GLint loc, float* val, int varDim);

	void	BindAttribLocation(unsigned int index, char* name);

	GLint	EnableVertexAttribArray(char* name);
	void	DisableVertexAttribArray(GLint loc);

protected:
	GLuint m_ID;
};
