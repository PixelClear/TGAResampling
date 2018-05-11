
#include "TGA.h"

bool Tga::loadTGA(const char* FilePath)
{
	std::fstream in{ FilePath, std::ios::in | std::ios::binary };
	if (!in.is_open())
		return false;

	in.read(reinterpret_cast<char*>(&header_), sizeof(header_));

	if (std::memcmp(compressed_, &header_, sizeof(compressed_)))
	{
		bitsPerPixel_ = header_[16];
		width_ = header_[13] * 256 + header_[12];
		height_ = header_[15] * 256 + header_[14];
		size_ = ((width_ * bitsPerPixel_ + 31) / 32) * 4 * height_ ;

		if (width_ <= 0 || height_ <=0 || (bitsPerPixel_ != 24) && (bitsPerPixel_ != 32))
		{
			in.close();
			return false;
		}

		imageData_.resize(size_);
		bCompressed_ = false;
		in.read(reinterpret_cast<char*>(imageData_.data()), size_);
	}
	else
	{
		return false;
	}

	in.close();
	return true;
}

bool Tga::saveTGA(const char* fileName)
{
	std::fstream out{fileName, std::fstream::out | std::fstream::binary};
	if (!out.is_open())
		return false;

	out.write(reinterpret_cast<char*>(header_), sizeof(header_));
	out.write(reinterpret_cast<char*>(imageData_.data()), size_);
	out.close();
}

int main()
{
	Tga tga;
	tga.loadTGA("flag_b24.tga");
	tga.saveTGA("test.tga");
}