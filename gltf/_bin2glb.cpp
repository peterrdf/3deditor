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

		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << BUFFERS_PROP;
		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << COLON;
		*getOutputStream() << SPACE;

		writeStartArrayTag(false);

		for (size_t iIndex = 0; iIndex < m_vecNodes.size(); iIndex++) {
			auto pNode = m_vecNodes[iIndex];

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
				*getOutputStream() << COMMA;
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

		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << BUFFER_VIEWS_PROP;
		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << COLON;
		*getOutputStream() << SPACE;

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
				*getOutputStream() << COMMA;
			}

			uint32_t iByteOffset = 0;

			// vertices/ARRAY_BUFFER/POSITION
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*getOutputStream() << COMMA;
				writeUIntProperty("byteLength", pNode->verticesBufferViewByteLength());
				*getOutputStream() << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*getOutputStream() << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->verticesBufferViewByteLength();
			}

			*getOutputStream() << COMMA;

			// normals/ARRAY_BUFFER/NORMAL
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*getOutputStream() << COMMA;
				writeUIntProperty("byteLength", pNode->normalsBufferViewByteLength());
				*getOutputStream() << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*getOutputStream() << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->normalsBufferViewByteLength();
			}

			*getOutputStream() << COMMA;

			// textures/ARRAY_BUFFER/TEXCOORD_0
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*getOutputStream() << COMMA;
				writeUIntProperty("byteLength", pNode->texturesBufferViewByteLength());
				*getOutputStream() << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*getOutputStream() << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->texturesBufferViewByteLength();
			}

			// indices/ELEMENT_ARRAY_BUFFER
			for (size_t iIndicesBufferViewIndex = 0; iIndicesBufferViewIndex < pNode->indicesBufferViewsByteLength().size(); iIndicesBufferViewIndex++) {
				uint32_t iByteLength = pNode->indicesBufferViewsByteLength()[iIndicesBufferViewIndex];

				*getOutputStream() << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*getOutputStream() << COMMA;
				writeUIntProperty("byteLength", iByteLength);
				*getOutputStream() << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*getOutputStream() << COMMA;
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
