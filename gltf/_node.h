#pragma once

#include "_geometry.h"
#include "_oglUtils.h"

#include <vector>
using namespace std;

// ************************************************************************************************
class _node
{

private: // Fields

	_geometry* m_pGeometry;

	uint32_t m_iBufferByteLength; // buffer: byteLength
	string m_strBufferBinFile; // buffer: uri (*.bin)	
	uint32_t m_iVerticesBufferViewByteLength; // vertices/ARRAY_BUFFER/POSITION
	uint32_t m_iNormalsBufferViewByteLength; // normals/ARRAY_BUFFER/NORMAL
	uint32_t m_iTexturesBufferViewByteLength; // normals/ARRAY_BUFFER/TEXTURE
	vector<uint32_t> m_vecIndicesBufferViewsByteLength; // indices/ELEMENT_ARRAY_BUFFER: conceptual faces, ..., conceptual face polygons, ..., lines..., points...
	vector<uint32_t> m_vecAccessors; // accessors: bufferView vertices, bufferView indices conceptual faces, ...
	vector<uint32_t> m_vecMeshes; // meshes: conceptual face 1, 2, ...

public: // Methods

	_node(_geometry* pGeometry);
	virtual ~_node();

	// glTF
	uint32_t& bufferByteLength() { return m_iBufferByteLength; }
	string& bufferBinFile() { return m_strBufferBinFile; }
	uint32_t& verticesBufferViewByteLength() { return m_iVerticesBufferViewByteLength; }
	uint32_t& normalsBufferViewByteLength() { return m_iNormalsBufferViewByteLength; }
	uint32_t& texturesBufferViewByteLength() { return m_iTexturesBufferViewByteLength; }
	vector<uint32_t>& indicesBufferViewsByteLength() { return m_vecIndicesBufferViewsByteLength; }
	vector<uint32_t>& accessors() { return m_vecAccessors; }
	vector<uint32_t>& meshes() { return m_vecMeshes; }

protected: // Methods

public: // Properties

	_geometry* getGeometry() const { return m_pGeometry; }
};
