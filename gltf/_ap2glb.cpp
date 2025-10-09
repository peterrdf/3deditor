#include "_host.h"
#include "_ap2glb.h"

#include "_base64.h"
#include "_ifc_geometry.h"
#include "_ap_mvc.h"

// ************************************************************************************************
namespace _ap2glb
{
	_exporter::_exporter(_model* pModel, const char* szOutputFile)
		: _ap2gltf::_exporter(pModel, szOutputFile, true)
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
		std::locale loc(std::locale::classic());
		getOutputStream()->imbue(loc);

		return getOutputStream()->good();
	}

	/*virtual*/ void _exporter::postExecute() /*override*/
	{
		std::ofstream outputStream(m_strOutputFile, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!outputStream.is_open()) {
			getLog()->logWrite(enumLogEvent::error, "Failed to create output file.");
			return;
		}

		float fLengthConversionFactor = (float)getProjectUnitConversionFactor(
			_ptr<_ap_model>(m_pModel)->getSdaiModel(), "LENGTHUNIT", nullptr, nullptr, nullptr);

		_vector3d vecVertexBufferOffset;
		GetVertexBufferOffset(m_pModel->getOwlModel(), (double*)&vecVertexBufferOffset);

		float fScaleFactor = (float)m_pModel->getOriginalBoundingSphereDiameter() / 2.f;
		fScaleFactor *= fLengthConversionFactor;
		vecVertexBufferOffset.x *= fLengthConversionFactor;
		vecVertexBufferOffset.y *= fLengthConversionFactor;
		vecVertexBufferOffset.z *= fLengthConversionFactor;

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
			auto pGeometry = pNode->getGeometry();

			const auto VERTEX_LENGTH = pGeometry->getVertexLength();

			_ptr<_ifc_geometry> ifcGeometry(pGeometry, false);
			bool bIsMappeditem = ifcGeometry && ifcGeometry->getIsMappedItem();

			// Vertices/POSITION
			for (int64_t iVertex = 0; iVertex < pGeometry->getVerticesCount(); iVertex++) {
				float fValue = pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 0] * fScaleFactor;
				fValue -= bIsMappeditem ? 0.f : (float)vecVertexBufferOffset.x;
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 1] * fScaleFactor;
				fValue -= bIsMappeditem ? 0.f : (float)vecVertexBufferOffset.y;
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 2] * fScaleFactor;
				fValue -= bIsMappeditem ? 0.f : (float)vecVertexBufferOffset.z;
				binaryStream.write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			// Vertices/NORMAL
			float arNormalizedVector[3] = { 0.f, 0.f, 0.f };
			for (int64_t iVertex = 0; iVertex < pGeometry->getVerticesCount(); iVertex++) {
				_normalizeVector(
					pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 3],
					pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 4],
					pGeometry->getVertices()[(iVertex * VERTEX_LENGTH) + 5],
					arNormalizedVector);
				binaryStream.write(reinterpret_cast<const char*>(&arNormalizedVector[0]), sizeof(float));
				binaryStream.write(reinterpret_cast<const char*>(&arNormalizedVector[1]), sizeof(float));
				binaryStream.write(reinterpret_cast<const char*>(&arNormalizedVector[2]), sizeof(float));
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

			writeIndices(pGeometry->concFacesCohorts());
			writeIndices(pGeometry->concFacePolygonsCohorts());
			writeIndices(pGeometry->linesCohorts());
			writeIndices(pGeometry->pointsCohorts());
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

		_ap2gltf::_exporter::postExecute();
	}

	/*virtual*/ void _exporter::writeBuffersProperty() /*override*/
	{
		*getOutputStream() << getNewLine();
		writeIndent();

		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << BUFFERS_PROP;
		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << COLON;
		*getOutputStream() << SPACE;

		uint32_t iGlobalBufferByteLength = 0;

		for (size_t iIndex = 0; iIndex < m_vecNodes.size(); iIndex++) {
			auto pNode = m_vecNodes[iIndex];
			auto pGeometry = pNode->getGeometry();

			if (pGeometry->isPlaceholder()) {
				continue;
			}

			// buffer: byteLength
			uint32_t iBufferByteLength = 0;

			// vertices/POSITION
			pNode->verticesBufferViewByteLength() = (uint32_t)pGeometry->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->verticesBufferViewByteLength();

			// vertices/NORMAL
			pNode->normalsBufferViewByteLength() = (uint32_t)pGeometry->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->normalsBufferViewByteLength();

			// Conceptual faces/indices
			for (auto pCohort : pGeometry->concFacesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Conceptual faces polygons/indices
			for (auto pCohort : pGeometry->concFacePolygonsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Lines
			for (auto pCohort : pGeometry->linesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			// Points
			for (auto pCohort : pGeometry->pointsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;
			}

			pNode->bufferByteLength() = iBufferByteLength;

			iGlobalBufferByteLength += iBufferByteLength;
		} // for (for (size_t iIndex = ...

		m_iBuffersCount = 1;

		writeStartArrayTag(false);

		indent()++;
		writeStartObjectTag();

		indent()++;
		writeUIntProperty("byteLength", iGlobalBufferByteLength);
		indent()--;

		writeEndObjectTag();
		indent()--;

		writeEndArrayTag();
	}

	/*virtual*/ void _exporter::writeBufferViewsProperty() /*override*/
	{
		*getOutputStream() << getNewLine();
		writeIndent();

		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << BUFFER_VIEWS_PROP;
		*getOutputStream() << DOULE_QUOT_MARK;
		*getOutputStream() << COLON;
		*getOutputStream() << SPACE;

		writeStartArrayTag(false);

		uint32_t iGlobalByteOffset = 0;

		// ARRAY_BUFFER/ELEMENT_ARRAY_BUFFER
		size_t iBufferIndex = 0;
		for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
			auto pNode = m_vecNodes[iNodeIndex];
			auto pGeometry = pNode->getGeometry();

			if (pGeometry->isPlaceholder()) {
				continue;
			}

			assert(pNode->indicesBufferViewsByteLength().size() ==
				pNode->getGeometry()->concFacesCohorts().size() +
				pNode->getGeometry()->concFacePolygonsCohorts().size() +
				pNode->getGeometry()->linesCohorts().size() +
				pNode->getGeometry()->pointsCohorts().size());

			if (iBufferIndex > 0) {
				*getOutputStream() << COMMA;
			}

			uint32_t iByteOffset = iGlobalByteOffset;

			// vertices/ARRAY_BUFFER/POSITION
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", 0);
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
				writeUIntProperty("buffer", 0);
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

			// indices/ELEMENT_ARRAY_BUFFER
			for (size_t iIndicesBufferViewIndex = 0; iIndicesBufferViewIndex < pNode->indicesBufferViewsByteLength().size(); iIndicesBufferViewIndex++) {
				uint32_t iByteLength = pNode->indicesBufferViewsByteLength()[iIndicesBufferViewIndex];

				*getOutputStream() << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", 0);
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

			iGlobalByteOffset += pNode->bufferByteLength();

			iBufferIndex++;
			m_iBufferViewsCount++;
		} // for (size_t iNodeIndex = ...

		writeEndArrayTag();
	}
};
