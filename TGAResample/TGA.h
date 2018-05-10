#pragma once
#include <iostream>
#include <fstream>
#include <string>

using std::ofstream;
using std::ifstream;
using std::string;
using byte = unsigned char;
using uint = uint16_t;

enum ImageType
{
	RGB,
	RGBA
};

struct TGAHeader {
	byte header[12];
};

struct TGA {
	byte header[6];
	byte* imageData;
	uint bytesPerPixel;
	uint imageSize; //Total memory needed for image
	ImageType type; //RGB or RGBA
	uint height;
	uint width;
	uint bpp; //Bits per pixel (24 or 32)
};

TGAHeader tgaHeader;
TGA tga;
byte uTgaHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte cTgaHeader[12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

