#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include "LodePNG/lodepng.h"

#include <stdio.h>
#include <stdint.h>
#include <vector>

class PngLoader
{
public:
	struct ImageInfo
	{
		uint16_t nWidth;
		uint16_t nHeight;
		uint8_t nNumComponent;
		uint8_t* pData;
	};

	PngLoader();
	~PngLoader();

	const ImageInfo* Load(const char* szFileName);
	const ImageInfo* Load(uint8_t* pData, uint32_t nDataSize);

	ImageInfo* getImageInfo() const { return m_pImageInfo; }

private:
	ImageInfo* m_pImageInfo;
	void Cleanup();
};
#endif