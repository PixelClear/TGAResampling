
#include "TGA.h"

void NearestNeighbour::resize(Tga& in, Tga& out)
{
	for (uint32_t j = 0; j < out.height_; j++)
	{
		float v = static_cast<float>(j) / static_cast<float>(out.height_ - 1);
		for (uint32_t i = 0; i < out.width_; i++)
		{
			float u = static_cast<float>(i) / static_cast<float>(out.width_ - 1);

			int x = int(u * in.width_);
			int y = int(v * in.height_);
			
			assert(in.numOfChannels_ == out.numOfChannels_);

		    auto pixel = &out.imageData_[(j * out.pitch_) + (i + 0) * in.numOfChannels_];
			auto sample = Helper::getClampedSample(in, x, y);

			pixel[0] = sample[0];
			pixel[1] = sample[1];
			pixel[2] = sample[2];
		}
	}
}

void Bilinear::resize(Tga& in, Tga& out)
{
	for (uint32_t j = 0; j < out.height_; j++)
	{
		float v = static_cast<float>(j) / static_cast<float>(out.height_ - 1);
		for (uint32_t i = 0; i < out.width_; i++)
		{
			float u = static_cast<float>(i) / static_cast<float>(out.width_ - 1);

			float x = (u * in.width_) - 0.5f;
			int xint = int(x);
			float tx = x - floor(x);

			float y = (v * in.height_) - 0.5f;
			int yint = int(y);
			float ty = y - floor(y);

			auto sample00 = Helper::getClampedSample(in, xint, yint);
			auto sample10 = Helper::getClampedSample(in, xint + 1, yint);
			auto sample01 = Helper::getClampedSample(in, xint, yint + 1);
			auto sample11 = Helper::getClampedSample(in, xint + 1, yint + 1);

			uint8_t finaleSample[3];
			for (uint32_t i = 0 ; i < 3; i++)
			{
				//Interpolation in X direction
				float col0 = Helper::lerp(sample00[i], sample01[i], tx);
				float col1 = Helper::lerp(sample10[i], sample11[i], tx);
				//Interpolation in Y direction
				float col2 = Helper::lerp(col0, col1, ty);
				Helper::clamp<float>(col2, 0.0f, 255.0f);
				finaleSample[i] = static_cast<uint8_t>(col2);
			}

			//Get Current pixel
			auto pixel = &out.imageData_[(j * out.pitch_) + (i + 0) * in.numOfChannels_];

			pixel[0] = finaleSample[0];
			pixel[1] = finaleSample[1];
			pixel[2] = finaleSample[2];
		}
	}
}

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
		if (width_ <= 0 || height_ <= 0 || (bitsPerPixel_ != 24) && (bitsPerPixel_ != 32))
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

void Tga::resizeTGA(Tga& destTga) //New name is assumed with .tga extension
{
	destTga.width_ = static_cast<uint32_t>(width_ / 2);
	destTga.height_ = static_cast<uint32_t>(height_ / 2);
	destTga.bitsPerPixel_ = bitsPerPixel_;
	destTga.numOfChannels_ = numOfChannels_;
	destTga.size_ = ((destTga.width_ * destTga.bitsPerPixel_ + 31) / 32) * 4 * destTga.height_;
	destTga.pitch_ = destTga.width_ * destTga.numOfChannels_;
	destTga.imageData_.resize(destTga.size_);

	std::unique_ptr<Resample> sampler = std::make_unique<Bilinear>();
	sampler->resize(*this, destTga);
	
	sampler.reset(nullptr);
}

int main()
{
	Tga tga;
	Tga out;
	tga.loadTGA("MARBLES.tga");
	tga.resizeTGA(out);
	out.saveTGA("Resize.tga");
}