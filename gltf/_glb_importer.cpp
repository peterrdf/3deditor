#include "_host.h"

#ifdef _WINDOWS
#include "../include/engine.h"
#else
#include "../include/engine.h"
#endif

#include "_string.h"
#include "_glb_importer.h"

#include <chrono>

// ************************************************************************************************
_glb_importer::_glb_importer(OwlModel iModel, const string& strRootFolder, const string& strOutputFolder)
	: _gltf_importer(iModel, strRootFolder, strOutputFolder)
	, m_strInputFile("")
	, m_nBINDataOffset(0)
{
}

/*virtual*/ _glb_importer::~_glb_importer()
{
}

/*virtual*/ string _glb_importer::getURI(const _json::_object* /*pBufferObject*/) const /*override*/
{
	return m_strInputFile;
}

/*virtual*/ streampos _glb_importer::getBINDataOffset() const /*override*/
{
	return m_nBINDataOffset;
}

void _glb_importer::load(const char* szFile)
{
	std::ifstream file(szFile, std::ios::binary);
	if (!file.is_open()) {
		LOG_THROW_ERROR_F("Failed to open GLB file: '%s'", szFile);
	}

	fs::path pathInputFile = szFile;
	m_strInputFile = pathInputFile.filename().string();

	// Read GLB header (12 bytes)
	uint32_t magic, version, length;
	file.read(reinterpret_cast<char*>(&magic), 4);
	file.read(reinterpret_cast<char*>(&version), 4);
	file.read(reinterpret_cast<char*>(&length), 4);

	// Verify magic number ("glTF")
	if (magic != 0x46546C67) { // "glTF" in little-endian
		LOG_THROW_ERROR_F("Invalid GLB file: incorrect magic number (expected 0x46546C67, got 0x%08X)", magic);
	}

	// Read first chunk (JSON content)
	uint32_t chunkLength, chunkType;
	file.read(reinterpret_cast<char*>(&chunkLength), 4);
	file.read(reinterpret_cast<char*>(&chunkType), 4);

	// Verify chunk type (should be "JSON")
	if (chunkType != 0x4E4F534A) { // "JSON" in little-endian
		LOG_THROW_ERROR_F("Invalid GLB file: first chunk type is not JSON (expected 0x4E4F534A, got 0x%08X)", chunkType);
	}

	// Read JSON content
	std::vector<char> jsonData(chunkLength);
	file.read(jsonData.data(), chunkLength);

	// Read BIN chunk header
	chunkLength = 0, chunkType = 0;
	file.read(reinterpret_cast<char*>(&chunkLength), 4);
	file.read(reinterpret_cast<char*>(&chunkType), 4);

	// Verify BIN chunk type
	if (chunkType != 0x004E4942) { // "BIN"
		LOG_THROW_ERROR_F("Invalid GLB file: second chunk type is not BIN (expected 0x004E4942, got 0x%08X)", chunkType);
	}

	// Store BIN chunk start position
	m_nBINDataOffset = file.tellg();

	loadArray((const unsigned char*)jsonData.data(), jsonData.size());
}
