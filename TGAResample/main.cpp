
#include "TGA.h"

bool Tga::loadTGA(const char* FilePath)
{
	std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
	if (!hFile.is_open())
		return false;

	hFile.read(reinterpret_cast<char*>(&header_), sizeof(header_));

	if (std::memcmp(compressed_, &header_, sizeof(compressed_)))
	{
		bitsPerPixel_ = header_[16];
		width_ = header_[13] * 256 + header_[12];
		height_ = header_[15] * 256 + header_[14];
		size_ = ((width_ * bitsPerPixel_ + 31) / 32) * 4 * height_ ;

		if (width_ <= 0 || height_ <=0 || (bitsPerPixel_ != 24) && (bitsPerPixel_ != 32))
		{
			hFile.close();
			return false;
		}

		imageData_.resize(size_);
		bCompressed_ = false;
		hFile.read(reinterpret_cast<char*>(imageData_.data()), size_);
	}
	else
	{
		return false;
	}

	hFile.close();
	return true;
}

int main()
{
	Tga tga;
	tga.loadTGA("xing_b32.tga");

}