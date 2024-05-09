#pragma once

// ************************************************************************************************
static bool SaveScreenshot(unsigned char* arPixels, unsigned int iWidth, unsigned int iHeight, const wchar_t* szFilePath)
{
	static unsigned char header[54] = {
	0x42, 0x4D, 0x36, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	((unsigned __int16*)header)[9] = (unsigned short)iWidth;
	((unsigned __int16*)header)[11] = (unsigned short)iHeight;

	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	unsigned long lSize = 0;
	::WriteFile(hFile, header, sizeof(header), &lSize, nullptr);
	::WriteFile(hFile, arPixels, iWidth * iHeight * 3, &lSize, nullptr);

	CloseHandle(hFile);

	return true;
}




