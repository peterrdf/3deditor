#include "_host.h"
#include "_bin2glb.h"

#include "_base64.h"

// ************************************************************************************************
namespace _bin2glb
{
	_exporter::_exporter(_model* pModel, const char* szOutputFile)
		: _bin2gltf::_exporter(pModel, szOutputFile, true)
	{
	}

	/*virtual*/ _exporter::~_exporter()
	{
	}

	void _exporter::execute()
	{
		_bin2gltf::_exporter::execute();
	}

	/*virtual*/ void _exporter::writeBuffersProperty() /*override*/
	{
		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << BUFFERS_PROP;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartArrayTag(false);

		for (size_t iIndex = 0; iIndex < m_vecNodes.size(); iIndex++) {
			auto pNode = m_vecNodes[iIndex];

			const auto VERTEX_LENGTH = pNode->getGeometry()->getVertexLength();

			// buffer: byteLength
			uint32_t iBufferByteLength = 0;

			// vertices/POSITION
			pNode->verticesBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->verticesBufferViewByteLength();

			// vertices/NORMAL
			pNode->normalsBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->normalsBufferViewByteLength();

			// vertices/TEXCOORD_0
			pNode->texturesBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 2 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->texturesBufferViewByteLength();

			// Conceptual faces/indices
			for (auto pCohort : pNode->getGeometry()->concFacesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Conceptual faces polygons/indices
			for (auto pCohort : pNode->getGeometry()->concFacePolygonsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Lines
			for (auto pCohort : pNode->getGeometry()->linesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Points
			for (auto pCohort : pNode->getGeometry()->pointsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			pNode->bufferByteLength() = iBufferByteLength;

			if (iIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			indent()++;
			writeStartObjectTag();

			indent()++;
			writeUIntProperty("byteLength", iBufferByteLength);
			indent()--;

			writeEndObjectTag();
			indent()--;

			m_iBuffersCount++;
		} // for (for (size_t iIndex = ...

		writeEndArrayTag();
	}

	/*virtual*/ void _exporter::writeBufferViewsProperty() /*override*/
	{
		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << BUFFER_VIEWS_PROP;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartArrayTag(false);

		// ARRAY_BUFFER/ELEMENT_ARRAY_BUFFER
		for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
			auto pNode = m_vecNodes[iNodeIndex];

			assert(pNode->indicesBufferViewsByteLength().size() ==
				pNode->getGeometry()->concFacesCohorts().size() +
				pNode->getGeometry()->concFacePolygonsCohorts().size() +
				pNode->getGeometry()->linesCohorts().size() +
				pNode->getGeometry()->pointsCohorts().size());

			if (iNodeIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			uint32_t iByteOffset = 0;

			// vertices/ARRAY_BUFFER/POSITION
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->verticesBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->verticesBufferViewByteLength();
			}

			*m_pOutputStream << COMMA;

			// normals/ARRAY_BUFFER/NORMAL
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->normalsBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->normalsBufferViewByteLength();
			}

			*m_pOutputStream << COMMA;

			// textures/ARRAY_BUFFER/TEXCOORD_0
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->texturesBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->texturesBufferViewByteLength();
			}

			// indices/ELEMENT_ARRAY_BUFFER
			for (size_t iIndicesBufferViewIndex = 0; iIndicesBufferViewIndex < pNode->indicesBufferViewsByteLength().size(); iIndicesBufferViewIndex++) {
				uint32_t iByteLength = pNode->indicesBufferViewsByteLength()[iIndicesBufferViewIndex];

				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", iByteLength);
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34963/*ELEMENT_ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += iByteLength;
			} // for (size_t iIndicesBufferViewIndex = ...

			m_iBufferViewsCount++;
		} // for (size_t iNodeIndex = ...

		writeEndArrayTag();
	}
};
