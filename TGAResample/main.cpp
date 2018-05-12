
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
		numOfChannels_ = bitsPerPixel_ / 8;
		pitch_ = width_ * numOfChannels_;
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
	
	out.put(0);
	out.put(0);
	out.put(2);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(width_ & 0x00FF);
	out.put((width_ & 0xFF00)/256);
	out.put(height_ & 0x00FF);
	out.put((height_ & 0xFF00) / 256);
	out.put(bitsPerPixel_);
	out.put(0);
	out.write(reinterpret_cast<char*>(imageData_.data()), size_);
	out.close();
	return true;
}

Tga& Tga::resizeTGA()
{
	Tga destTga{};
	destTga.width_ = static_cast<uint32_t>(width_ / 2);
	destTga.height_ = static_cast<uint32_t>(height_ / 2);
	destTga.bitsPerPixel_ = bitsPerPixel_;
	destTga.numOfChannels_ = numOfChannels_;
	destTga.size_ = ((destTga.width_ * destTga.bitsPerPixel_ + 31) / 32) * 4 * destTga.height_;
	destTga.pitch_ = destTga.width_ * destTga.numOfChannels_;
	destTga.imageData_.resize(destTga.size_);

	for (uint32_t j = 0; j < destTga.height_; j++)
	{
		float v = static_cast<float>(j) / static_cast<float>(destTga.height_ - 1);
		int rows = 0;
		for (uint32_t i = 0; i < destTga.width_; i++)
		{
			float u = static_cast<float>(i) / static_cast<float>(destTga.width_ - 1);

			int x = int(u * width_);
			int y = int(v * height_);

			CLAMP(x, 0, width_ - 1);
			CLAMP(y, 0, height_ - 1);
			
			uint8_t* pixel = &destTga.imageData_[(j * destTga.pitch_) + (i + 0) * numOfChannels_];
			uint8_t* sample = &imageData_[(y * pitch_) + (x + 0) * numOfChannels_];

			pixel[0] = sample[0];
			pixel[1] = sample[1];
			pixel[2] = sample[2];
		}
	}

	destTga.saveTGA("Resized.tga");
	return destTga;
}

int main()
{
	Tga tga;
	//tga.loadTGA("xing_b32.tga");
	tga.loadTGA("MARBLES.tga");
	tga.saveTGA("test.tga");
	tga.resizeTGA();
}