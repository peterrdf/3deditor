#include "JpegLoader.h"
#include "StbImageWrite/stb_image_write.h"


// A demo using libJpeg to load an image.
// We save the image with stb_image_write just to prove that we actually loaded it.

// Image by Dave Catchpole
// http://www.flickr.com/photos/yaketyyakyak/sets/72157629976688365/
// http://creativecommons.org/licenses/by/2.0/deed.en

int main(int argc, char* argv[])
{
	JpegLoader jpegLoader;
	const JpegLoader::ImageInfo* pImageInfo = jpegLoader.Load("In.jpg");
	if (pImageInfo)
	{
		stbi_write_png("Out.png", pImageInfo->nWidth, pImageInfo->nHeight, pImageInfo->nNumComponent, pImageInfo->pData, 0);
	}

	return 0;
}