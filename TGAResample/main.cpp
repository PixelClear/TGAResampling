#include "TGA.h"

bool LoadTGA(string& fileName)
{
	ifstream in{ fileName,std::ifstream::in | std::ifstream::binary };
	if (!in.is_open())
		return false;

	//Read first 12 bytes header
	in.read(reinterpret_cast<char*>(&tgaHeader), sizeof(tgaHeader));
	if (in.gcount() <= 0)
		return false;

	//Assuming only compressed TGA
	if (std::memcmp(uTgaHeader, &tgaHeader, sizeof(tgaHeader)) == 0)
	{
		//Read next 6 bytes of header
		in.read(reinterpret_cast<char*>(&tga.header), sizeof(tga.header));
		if (in.gcount() <= 0)
			return false;

		//Fill tga structure
		tga.width = tga.header[1] * 256 + tga.header[0];
		tga.height = tga.header[3] * 256 + tga.header[2];
		tga.bpp = tga.header[4];

		if (tga.width <= 0 || tga.height <= 0 || (tga.bpp != 24 && tga.bpp != 32))
			return false;

		(tga.bpp == 24) ? tga.type = RGB : tga.type = RGBA;
		tga.bytesPerPixel = tga.bpp / 8;
		tga.imageSize = tga.bytesPerPixel * tga.height * tga.width;

		//Read Image Data
		tga.imageData = new byte[tga.imageSize];
		if (!tga.imageData)
			return false;

		in.read(reinterpret_cast<char*>(&tga.imageData), tga.imageSize);
		if (in.gcount() <= 0)
			return false;

		//TGA reads data in reverse order
		for (uint i = 0; i < tga.imageSize; i++)
		{
			//Xor 1st and 3rd byte twice to swap it
			tga.imageData[i] ^= tga.imageData[i + 2] ^=
			tga.imageData[i] ^= tga.imageData[i + 2];
		}
	}
	else if (std::memcmp(cTgaHeader, &tgaHeader, sizeof(tgaHeader)) == 0)
	{
		//Not Yet Implemented
	}
	else
	{
		return false;
	}

	in.close();
	return true;
}