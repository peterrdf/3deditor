#pragma once

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "stb/stb_image_write.h"

// ************************************************************************************************
// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
static bool SaveScreenshot(unsigned char* arPixels, unsigned int iWidth, unsigned int iHeight, const wchar_t* szFilePath)
{
	fs::path pathFilePath = szFilePath;

	string strExtension = pathFilePath.extension().string();
	std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);

	if ((strExtension == ".jpg") || (strExtension == ".jpeg")) {
		// RGB (no swap needed)
		stbi_flip_vertically_on_write(1);
		return stbi_write_jpg(pathFilePath.string().c_str(), (int)iWidth, (int)iHeight, 3, arPixels, 90) != 0;
	}
	else if (strExtension == ".png") {
		// RGB (no swap needed)
		stbi_flip_vertically_on_write(1);
		return stbi_write_png(pathFilePath.string().c_str(), (int)iWidth, (int)iHeight, 3, arPixels, (int)(iWidth * 3)) != 0;
	}
	else if (strExtension == ".bmp")  {
		// Swap R and B channels for BMP (BGR)
		for (unsigned int i = 0; i < iWidth * iHeight * 3; i += 3) {
			unsigned char temp = arPixels[i];
			arPixels[i] = arPixels[i + 2];
			arPixels[i + 2] = temp;
		}

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
			ASSERT(FALSE);
			return false;
		}

		unsigned long lSize = 0;
		::WriteFile(hFile, header, sizeof(header), &lSize, nullptr);

		const unsigned char padding[3] = { 0, 0, 0 };
		for (unsigned int y = 0; y < iHeight; y++) {
			::WriteFile(hFile, arPixels + y * iRowStride, iRowStride, &lSize, nullptr);
			if (iRowPadding > 0) {
				::WriteFile(hFile, padding, iRowPadding, &lSize, nullptr);
			}
		}

		::CloseHandle(hFile);
		return true;
	} 
	else {
		ASSERT(FALSE);
		return false;
	}
}


