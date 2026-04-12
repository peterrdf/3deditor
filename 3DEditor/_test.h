#pragma once

// ************************************************************************************************
// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
static bool SaveScreenshot(unsigned char* arPixels, unsigned int iWidth, unsigned int iHeight, const wchar_t* szFilePath)
{
	// BMP rows must be padded to a multiple of 4 bytes
	const unsigned int iRowStride = iWidth * 3;
	const unsigned int iRowPadding = (4 - (iRowStride % 4)) % 4;
	const unsigned int iPaddedRowSize = iRowStride + iRowPadding;
	const unsigned int iPixelDataSize = iPaddedRowSize * iHeight;
	const unsigned int iFileSize = 54 + iPixelDataSize;

	unsigned char header[54] = {
		// BITMAPFILEHEADER (14 bytes)
		0x42, 0x4D,             // Signature "BM"
		0x00, 0x00, 0x00, 0x00, // File size (filled below)
		0x00, 0x00,             // Reserved1
		0x00, 0x00,             // Reserved2
		0x36, 0x00, 0x00, 0x00, // Pixel data offset = 54

		// BITMAPINFOHEADER (40 bytes)
		0x28, 0x00, 0x00, 0x00, // Header size = 40
		0x00, 0x00, 0x00, 0x00, // Width (filled below)
		0x00, 0x00, 0x00, 0x00, // Height (filled below)
		0x01, 0x00,             // Color planes = 1
		0x18, 0x00,             // Bits per pixel = 24
		0x00, 0x00, 0x00, 0x00, // Compression = BI_RGB
		0x00, 0x00, 0x00, 0x00, // Image size (filled below)
		0xC4, 0x0E, 0x00, 0x00, // X pixels per meter
		0xC4, 0x0E, 0x00, 0x00, // Y pixels per meter
		0x00, 0x00, 0x00, 0x00, // Colors in table
		0x00, 0x00, 0x00, 0x00  // Important colors
	};

	*reinterpret_cast<unsigned int*>(&header[2]) = iFileSize;
	*reinterpret_cast<unsigned int*>(&header[18]) = iWidth;
	*reinterpret_cast<unsigned int*>(&header[22]) = iHeight;
	*reinterpret_cast<unsigned int*>(&header[34]) = iPixelDataSize;

	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	unsigned long lSize = 0;
	::WriteFile(hFile, header, sizeof(header), &lSize, nullptr);

	// Write each row followed by its padding
	const unsigned char padding[3] = { 0, 0, 0 };
	for (unsigned int y = 0; y < iHeight; y++) {
		::WriteFile(hFile, arPixels + y * iRowStride, iRowStride, &lSize, nullptr);
		if (iRowPadding > 0) {
			::WriteFile(hFile, padding, iRowPadding, &lSize, nullptr);
		}
	}

	CloseHandle(hFile);

	return true;
}



