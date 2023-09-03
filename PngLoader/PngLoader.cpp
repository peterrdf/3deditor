#include "PngLoader.h"
#include "LodePng/lodepng.h"

PngLoader::PngLoader()
{
	m_pImageInfo = NULL;
}

PngLoader :: ~PngLoader()
{
	Cleanup();
}

const PngLoader::ImageInfo* PngLoader::Load(const char* szFileName)
{
	Cleanup();

	FILE* pFile = fopen(szFileName, "rb");
	if (pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);

		uint32_t nDataSize = ftell(pFile);
		rewind(pFile);

		uint8_t* pData = new uint8_t[nDataSize];
		fread(pData, nDataSize, 1, pFile);

		Load(pData, nDataSize);

		delete[] pData;
	}


	return m_pImageInfo;
}

const PngLoader::ImageInfo* PngLoader::Load(uint8_t* pData, uint32_t nDataSize)
{
	Cleanup();

	std::vector<unsigned char> imageByteVector;
	unsigned nWidth, nHeight;
	if (!lodepng::decode(imageByteVector, nWidth, nHeight, pData, nDataSize))
	{
		m_pImageInfo = new ImageInfo();
		m_pImageInfo->nWidth = nWidth;
		m_pImageInfo->nHeight = nHeight;
		m_pImageInfo->nNumComponent = 4;
		m_pImageInfo->pData = new uint8_t[m_pImageInfo->nWidth * m_pImageInfo->nHeight * m_pImageInfo->nNumComponent];
		memcpy(m_pImageInfo->pData, &imageByteVector[0], imageByteVector.size());
	}

	return m_pImageInfo;
}

void PngLoader::Cleanup()
{
	if (m_pImageInfo)
	{
		delete[] m_pImageInfo->pData;
		delete m_pImageInfo;
		m_pImageInfo = NULL;
	}
}