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

	/*virtual*/ bool _exporter::createOuputStream() /*override*/
	{
		if (m_pOutputStream != nullptr) {
			delete m_pOutputStream;
		}

		m_pOutputStream = new std::ostringstream();

		return getOutputStream()->good();
	}

	/*virtual*/ void _exporter::postExecute() /*override*/
	{
		std::ofstream outputStream(m_strOutputFile, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!outputStream.is_open()) {
			getLog()->logWrite(enumLogEvent::error, "Failed to create output file.");
			return;
		}

		// Get JSON content as string
		std::string jsonContent = ((std::ostringstream*)m_pOutputStream)->str();

		// GLB requires JSON chunk to be 4-byte aligned
		uint32_t jsonPadding = (4 - (jsonContent.length() % 4)) % 4;
		for (uint32_t i = 0; i < jsonPadding; i++) {
			jsonContent += ' ';
		}

		// First, collect all binary data in a memory buffer to get its size
		std::ostringstream binaryStream;
		for (size_t iIndex = 0; iIndex < m_vecNodes.size(); iIndex++) {
			auto pNode = m_vecNodes[iIndex];
			const auto VERTEX_LENGTH = pNode->getGeometry()->getVertexLength();

			// Vertices/POSITION
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 0];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 1];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 2];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			// Vertices/NORMAL
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 3];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 4];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 5];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			// Vertices/TEXCOORD_0
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 6];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 7];
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			// Write all indices
			auto writeIndices = [&binaryStream](const auto& cohorts) {
				for (auto pCohort : cohorts) {
					for (size_t i = 0; i < pCohort->indices().size(); i++) {
						GLuint iIndexValue = pCohort->indices()[i];
						binaryStream.write(reinterpret_cast<const char*>(&iIndexValue), sizeof(GLuint));
					}
				}
			};

			writeIndices(pNode->getGeometry()->concFacesCohorts());
			writeIndices(pNode->getGeometry()->concFacePolygonsCohorts());
			writeIndices(pNode->getGeometry()->linesCohorts());
			writeIndices(pNode->getGeometry()->pointsCohorts());
		}

		// Get binary content and apply padding
		std::string binaryContent = binaryStream.str();
		uint32_t binaryPadding = (4 - (binaryContent.length() % 4)) % 4;
		for (uint32_t i = 0; i < binaryPadding; i++) {
			binaryContent += '\0';
		}

		// Calculate sizes
		uint32_t jsonChunkLength = static_cast<uint32_t>(jsonContent.length());
		uint32_t binaryChunkLength = static_cast<uint32_t>(binaryContent.length());

		// Total length = 12 (header) + 8 (JSON chunk header) + jsonChunkLength + 8 (BIN chunk header) + binaryChunkLength
		uint32_t totalLength = 12 + 8 + jsonChunkLength + 8 + binaryChunkLength;

		// Write GLB header (12 bytes)
		uint32_t magic = 0x46546C67;  // 'glTF' in ASCII (little-endian)
		outputStream.write(reinterpret_cast<const char*>(&magic), 4);

		uint32_t version = 2;
		outputStream.write(reinterpret_cast<const char*>(&version), 4);

		outputStream.write(reinterpret_cast<const char*>(&totalLength), 4);

		// Write JSON chunk header (8 bytes)
		outputStream.write(reinterpret_cast<const char*>(&jsonChunkLength), 4);

		uint32_t jsonChunkType = 0x4E4F534A;  // 'JSON' in ASCII (little-endian)
		outputStream.write(reinterpret_cast<const char*>(&jsonChunkType), 4);

		// Write JSON content
		outputStream.write(jsonContent.c_str(), jsonChunkLength);

		// Write BIN chunk header (8 bytes)
		outputStream.write(reinterpret_cast<const char*>(&binaryChunkLength), 4);

		uint32_t binaryChunkType = 0x004E4942;  // 'BIN\0' in ASCII (little-endian)
		outputStream.write(reinterpret_cast<const char*>(&binaryChunkType), 4);

		// Write binary content
		outputStream.write(binaryContent.c_str(), binaryChunkLength);

		if (!outputStream.good()) {
			getLog()->logWrite(enumLogEvent::error, "Error while writing GLB file.");
		}
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
};
