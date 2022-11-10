#pragma once

#include "_geometry.h"
#include "_openGLUtils.h"

template<class T>
class _buffer
{

private: // Members

	T* m_pData;
	int64_t m_iSize;

public: // Methods

	_buffer()
		: m_pData(nullptr)
		, m_iSize(0)
	{
	}

	virtual ~_buffer()
	{
		delete[] m_pData;
	}

	T*& data()
	{
		return m_pData;
	}

	int64_t& size()
	{
		return m_iSize;
	}
};

template<class V>
class _vertexBuffer : public _buffer<V>
{

private: // Members

	int64_t m_iVertexLength;

public: // Methods

	_vertexBuffer()
		: m_iVertexLength(0)
	{
		static_assert(
			is_same<V, float>::value ||
			is_same<V, double>::value,
			"V must be float or double type.");
	}

	virtual ~_vertexBuffer()
	{
	}

	int64_t& vertexLength()
	{
		return m_iVertexLength;
	}
};

typedef _vertexBuffer<float> _vertices_f;
typedef _vertexBuffer<double> _vertices_d;

template<class I>
class _indexBuffer : public _buffer<I>
{

public: // Methods

	_indexBuffer()
	{
		static_assert(
			is_same<I, int32_t>::value ||
			is_same<I, int64_t>::value,
			"I must be int32_t or int64_t type.");
	}

	virtual ~_indexBuffer()
	{
	}
};

typedef _indexBuffer<int32_t> _indices_i32;
typedef _indexBuffer<int64_t> _indices_i64;

// (255 * 255 * 255)[R] + (255 * 255)[G] + 255[B]
class _i64RGBCoder
{

public: // Methods

	static void encode(int64_t i, float& fR, float& fG, float& fB)
	{
		static const float STEP = 1.f / 255.f;

		fR = 0.f;
		fG = 0.f;
		fB = 0.f;

		// R
		if (i >= (255 * 255))
		{
			int64_t iR = i / (255 * 255);
			fR = iR * STEP;

			i -= iR * (255 * 255);
		}

		// G
		if (i >= 255)
		{
			int64_t iG = i / 255;
			fG = iG * STEP;

			i -= iG * 255;
		}

		// B		
		fB = i * STEP;
	}

	static int64_t decode(unsigned char R, unsigned char G, unsigned char B)
	{
		int64_t i = 0;

		// R
		i += R * (255 * 255);

		// G
		i += G * 255;

		// B
		i += B;

		return i;
	}
};

template <class Instance>
class _openGLBuffers
{

private: // Members

	map<GLuint, vector<Instance*>> m_mapInstancesCohorts;
	map<wstring, GLuint> m_mapVAOs;
	map<wstring, GLuint> m_mapBuffers;

public: // Methods

	_openGLBuffers()
		: m_mapInstancesCohorts()
		, m_mapVAOs()
		, m_mapBuffers()
	{
	}

	virtual ~_openGLBuffers()
	{
	}

	map<GLuint, vector<Instance*>>& instancesCohorts()
	{
		return m_mapInstancesCohorts;
	}

	map<wstring, GLuint>& VAOs()
	{
		return m_mapVAOs;
	}

	map<wstring, GLuint>& buffers()
	{
		return m_mapBuffers;
	}

	GLuint findVAO(Instance* pInstance)
	{
		for (auto itCohort : m_mapInstancesCohorts)
		{
			for (auto itInstance : itCohort.second)
			{
				if (pInstance == itInstance)
				{
					return itCohort.first;
				}
			}
		}

		return 0;
	}	

	GLuint getVAO(const wstring& strName)
	{
		auto itVAO = m_mapVAOs.find(strName);
		if (itVAO != m_mapVAOs.end())
		{
			return itVAO->second;
		}

		return 0;
	}

	GLuint getVAOcreateNew(const wstring& strName, bool& bIsNew)
	{
		bIsNew = false;		

		GLuint iVAO = getVAO(strName);
		if (iVAO == 0)
		{
			glGenVertexArrays(1, &iVAO);
			if (iVAO == 0)
			{
				assert(false);

				return 0;
			}

			_openGLUtils::checkForErrors();

			bIsNew = true;
			m_mapVAOs[strName] = iVAO;
		}

		return iVAO;
	}

	GLuint getBuffer(const wstring& strName)
	{
		auto itBuffer = m_mapBuffers.find(strName);
		if (itBuffer != m_mapBuffers.end())
		{
			return itBuffer->second;
		}

		return 0;
	}

	GLuint getBufferCreateNew(const wstring& strName, bool& bIsNew)
	{
		bIsNew = false;		

		GLuint iBuffer = getBuffer(strName);
		if (iBuffer == 0)
		{
			glGenBuffers(1, &iBuffer);
			if (iBuffer == 0)
			{
				assert(false);

				return 0;
			}

			_openGLUtils::checkForErrors();

			bIsNew = true;
			m_mapBuffers[strName] = iBuffer;
		}

		return iBuffer;
	}	

	int64_t createIBO(const vector<_cohort*>& vecCohorts)
	{
		if (vecCohorts.empty())
		{
			assert(false);

			return 0;
		}

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		if (iIBO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapBuffers[to_wstring(iIBO)] = iIBO;

		int_t iIndicesCount = 0;
		unsigned int* pIndices = _cohort::merge(vecCohorts, iIndicesCount);

		if ((pIndices == nullptr) || (iIndicesCount == 0))
		{
			assert(false);

			return 0;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iIndicesCount, pIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] pIndices;

		_openGLUtils::checkForErrors();

		GLsizei iIBOOffset = 0;
		for (auto pCohort : vecCohorts)
		{
			pCohort->ibo() = iIBO;
			pCohort->iboOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)pCohort->indices().size();
		}

		return iIndicesCount;
	}	

	int64_t createInstancesCohort(const vector<Instance*>& vecInstances, bool bTexture, CBinnPhongGLProgram* pProgram)
	{
		if (vecInstances.empty() || (pProgram == nullptr))
		{
			assert(false);

			return 0;
		}

		int_t iVerticesCount = 0;
		float* pVertices = getVertices(vecInstances, true, iVerticesCount);

		if ((pVertices == nullptr) || (iVerticesCount == 0))
		{
			assert(false);

			return 0;
		}

		GLuint iVAO = 0;
		glGenVertexArrays(1, &iVAO);

		if (iVAO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapVAOs[to_wstring(iVAO)] = iVAO;

		glBindVertexArray(iVAO);

		GLuint iVBO = 0;
		glGenBuffers(1, &iVBO);

		if (iVBO == 0)
		{
			assert(false);

			return 0;
		}

		m_mapBuffers[to_wstring(iVBO)] = iVBO;

		const int64_t _VERTEX_LENGTH = 6 + (bTexture ? 2 : 0);

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * iVerticesCount * _VERTEX_LENGTH, pVertices, GL_STATIC_DRAW);
		delete[] pVertices;

		glVertexAttribPointer(pProgram->getVertexPosition(), 3, GL_FLOAT, false, (GLsizei)(sizeof(GLfloat) * _VERTEX_LENGTH), 0);
		glVertexAttribPointer(pProgram->getVertexNormal(), 3, GL_FLOAT, false, (GLsizei)(sizeof(GLfloat) * _VERTEX_LENGTH), (void*)(sizeof(GLfloat) * 3));
		if (bTexture)
		{
			glVertexAttribPointer(pProgram->getTextureCoord(), 2, GL_FLOAT, false, (GLsizei)(sizeof(GLfloat) * _VERTEX_LENGTH), (void*)(sizeof(GLfloat) * 6));
		}		

		glEnableVertexAttribArray(pProgram->getVertexPosition());
		glEnableVertexAttribArray(pProgram->getVertexNormal());
		if (bTexture)
		{
			glEnableVertexAttribArray(pProgram->getTextureCoord());
		}		
		
		GLsizei iVBOOffset = 0;
		for (auto pInstance : vecInstances)
		{
			pInstance->VBO() = iVBO;
			pInstance->VBOOffset() = iVBOOffset;

			iVBOOffset += (GLsizei)pInstance->getVerticesCount();
		}

		glBindVertexArray(0);

		_openGLUtils::checkForErrors();

		m_mapInstancesCohorts[iVAO] = vecInstances;	

		return iVerticesCount;
	}

	// X, Y, Z, Nx, Ny, Nz, [Tx, Ty]
	static float* getVertices(const vector<Instance*>& vecInstances, bool bTexture, int_t& iVerticesCount)
	{
		const int64_t _VERTEX_LENGTH = 6 + (bTexture ? 2 : 0);

		iVerticesCount = 0;
		for (size_t i = 0; i < vecInstances.size(); i++)
		{
			iVerticesCount += vecInstances[i]->getVerticesCount();
		}

		float* pVertices = new float[iVerticesCount * _VERTEX_LENGTH];

		int_t iOffset = 0;
		for (size_t i = 0; i < vecInstances.size(); i++)
		{
			float* pSrcVertices = getVertices(vecInstances[i], bTexture);

			memcpy((float*)pVertices + iOffset, pSrcVertices,
				vecInstances[i]->getVerticesCount() * _VERTEX_LENGTH * sizeof(float));

			delete[] pSrcVertices;

			iOffset += vecInstances[i]->getVerticesCount() * _VERTEX_LENGTH;
		}

		return pVertices;
	}	

	// X, Y, Z, Nx, Ny, Nz, [Tx, Ty]
	static float* getVertices(Instance* pInstance, bool bTexture)
	{
		const int64_t _SRC_VERTEX_LENGTH = pInstance->getVertexLength();
		const int64_t _DEST_VERTEX_LENGTH = 6 + (bTexture ? 2 : 0);

		float* pVertices = new float[pInstance->getVerticesCount() * _DEST_VERTEX_LENGTH];
		memset(pVertices, 0, pInstance->getVerticesCount() * _DEST_VERTEX_LENGTH * sizeof(float));

		for (int64_t iVertex = 0; iVertex < pInstance->getVerticesCount(); iVertex++)
		{
			// X, Y, Z
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 0] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 0];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 1] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 1];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 2] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 2];

			// Nx, Ny, Nz
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 3] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 3];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 4] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 4];
			pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 5] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 5];

			// Tx, Ty
			if (bTexture)
			{
				pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 6] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 6];
				pVertices[(iVertex * _DEST_VERTEX_LENGTH) + 7] = pInstance->getVertices()[(iVertex * _SRC_VERTEX_LENGTH) + 7];
			}
		}

		return pVertices;
	}

	void clear()
	{
		for (auto itVAO = m_mapVAOs.begin(); itVAO != m_mapVAOs.end(); itVAO++)
		{
			glDeleteVertexArrays(1, &(itVAO->second));
		}
		m_mapVAOs.clear();

		for (auto itBuffer = m_mapBuffers.begin(); itBuffer != m_mapBuffers.end(); itBuffer++)
		{
			glDeleteBuffers(1, &(itBuffer->second));
		}
		m_mapBuffers.clear();
		
		_openGLUtils::checkForErrors();
	}
};