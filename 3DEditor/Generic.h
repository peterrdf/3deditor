#pragma once

// ------------------------------------------------------------------------------------------------
/*
// X, Y, Z, Nx, Ny, Nz, Tx, Ty, Ambient, Diffuse, Emissive, Specular, Tnx, Tny, Tnz, Bnx, Bny, Bnz
// (Tx, Ty - bit 6; Normal vectors - bit 5, Diffuse, Emissive, Specular - bit 25, 26 & 27, Tangent vectors - bit 28, Binormal vectors - bit 29)
*/
#define VERTEX_LENGTH  18

// ------------------------------------------------------------------------------------------------
// X, Y, Z, Nx, Ny, Nz, Tx, Ty
#define GEOMETRY_VBO_VERTEX_LENGTH  8

// ------------------------------------------------------------------------------------------------
// X, Y, Z
#define VECTORS_VBO_VERTEX_LENGTH  3

// ------------------------------------------------------------------------------------------------
#define DEFAULT_CIRCLE_SEGMENTS 36

// ------------------------------------------------------------------------------------------------
static bool SaveScreenshot(unsigned char* arPixels, unsigned int iWidth, unsigned int iHeight, const wchar_t* szFilePath)
{
	static unsigned char header[54] = {
	0x42, 0x4D, 0x36, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	((unsigned __int16*)header)[9] = (unsigned short)iWidth;
	((unsigned __int16*)header)[11] = (unsigned short)iHeight;

	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	unsigned long lSize = 0;
	::WriteFile(hFile, header, sizeof(header), &lSize, NULL);
	::WriteFile(hFile, arPixels, iWidth * iHeight * 3, &lSize, NULL);

	CloseHandle(hFile);

	return true;
}

// ------------------------------------------------------------------------------------------------
static TCHAR SUPPORTED_FILES[] = _T("RDF Files (*.rdf;*.bin)|*.rdf;*.bin|E57 Files (*.e57)|*.e57|CityGML Files (*.gml;*citygml)|*.gml;*citygml|CityJSON Files (*.city.json)|*.city.json|Autocad DXF Files (*.dxf)|*.dxf|All Files (*.*)|*.*||");


