
#include "TGA.h"

void NearestNeighbour::resize(Tga& in, Tga& out)
{
	for (uint32_t j = 0; j < out.height_; j++){
		const float v = static_cast<float>(j) / static_cast<float>(out.height_ - 1);
		for (uint32_t i = 0; i < out.width_; i++){
			const float u = static_cast<float>(i) / static_cast<float>(out.width_ - 1);

			const int x = int(u * in.width_);
			const int y = int(v * in.height_);
			
			assert(in.numOfChannels_ == out.numOfChannels_);

		    auto pixel = &out.imageData_[(j * out.pitch_) + (i + 0) * in.numOfChannels_];
			const auto sample = Helper::getClampedSample(in, x, y);

			pixel[0] = sample[0];
			pixel[1] = sample[1];
			pixel[2] = sample[2];
			if (in.numOfChannels_ == 4){ pixel[3] = sample[3];	}
		}
	}
}

void Bilinear::resize(Tga& in, Tga& out)
{
	for (uint32_t j = 0; j < out.height_; j++){
		const float v = static_cast<float>(j) / static_cast<float>(out.height_ - 1);
		for (uint32_t i = 0; i < out.width_; i++){
			const float u = static_cast<float>(i) / static_cast<float>(out.width_ - 1);

			const float x = (u * in.width_) - 0.5f;
			const int xint = int(x);
			const float tx = x - floor(x);

			const float y = (v * in.height_) - 0.5f;
			const int yint = int(y);
			const float ty = y - floor(y);

			const auto sample00 = Helper::getClampedSample(in, xint, yint);
			const auto sample10 = Helper::getClampedSample(in, xint + 1, yint);
			const auto sample01 = Helper::getClampedSample(in, xint, yint + 1);
			const auto sample11 = Helper::getClampedSample(in, xint + 1, yint + 1);

			std::vector<uint8_t>finaleSample;
			finaleSample.resize(in.numOfChannels_);
			for (uint32_t i = 0 ; i < in.numOfChannels_; i++){
				//Interpolation in X direction
				const float col0 = Helper::lerp(sample00[i], sample01[i], tx);
				const float col1 = Helper::lerp(sample10[i], sample11[i], tx);
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
			if (in.numOfChannels_ == 4) { pixel[3] = finaleSample[3]; }
		}
	}
}

void CubicSpline::resize(Tga& in, Tga& out)
{
	for (uint32_t j = 0; j < out.height_; j++){
		float v = static_cast<float>(j) / static_cast<float>(out.height_ - 1);
		for (uint32_t i = 0; i < out.width_; i++){
			const float u = static_cast<float>(i) / static_cast<float>(out.width_ - 1);

			const float x = (u * in.width_) - 0.5f;
			const int xint = int(x);
			const float tx = x - floor(x);

			const float y = (v * in.height_) - 0.5f;
			const int yint = int(y);
			const float ty = y - floor(y);

			//Get 4 X 4 sample to interpolate
			const auto sample00 = Helper::getClampedSample(in, xint - 1, yint - 1);
			const auto sample10 = Helper::getClampedSample(in, xint + 0, yint - 1);
			const auto sample20 = Helper::getClampedSample(in, xint + 1, yint - 1);
			const auto sample30 = Helper::getClampedSample(in, xint + 2, yint - 1);

			const auto sample01 = Helper::getClampedSample(in, xint - 1, yint + 0);
			const auto sample11 = Helper::getClampedSample(in, xint + 0, yint + 0);
			const auto sample21 = Helper::getClampedSample(in, xint + 1, yint + 0);
			const auto sample31 = Helper::getClampedSample(in, xint + 2, yint + 0);

			const auto sample02 = Helper::getClampedSample(in, xint - 1, yint + 1);
			const auto sample12 = Helper::getClampedSample(in, xint + 0, yint + 1);
			const auto sample22 = Helper::getClampedSample(in, xint + 1, yint + 1);
			const auto sample32 = Helper::getClampedSample(in, xint + 2, yint + 1);

			const auto sample03 = Helper::getClampedSample(in, xint - 1, yint + 2);
			const auto sample13 = Helper::getClampedSample(in, xint + 0, yint + 2);
			const auto sample23 = Helper::getClampedSample(in, xint + 1, yint + 2);
			const auto sample33 = Helper::getClampedSample(in, xint + 2, yint + 2);

			std::vector<uint8_t>finaleSample;
			finaleSample.resize(in.numOfChannels_);
			for (uint32_t i = 0; i < in.numOfChannels_; i++){
				//Interpolation in X direction
				const float col0 = Helper::cubicHermite(sample00[i], sample10[i], sample20[i], sample30[i], tx);
				const float col1 = Helper::cubicHermite(sample01[i], sample11[i], sample21[i], sample31[i], tx);
				const float col2 = Helper::cubicHermite(sample02[i], sample12[i], sample22[i], sample32[i], tx);
				const float col3 = Helper::cubicHermite(sample03[i], sample13[i], sample23[i], sample33[i], tx);

				//Interpolation in Y direction
				float col4 = Helper::cubicHermite(col0, col1,col2, col3 ,ty);
				Helper::clamp<float>(col4, 0.0f, 255.0f);
				finaleSample[i] = static_cast<uint8_t>(col4);
			}

			//Get Current pixel
			auto pixel = &out.imageData_[(j * out.pitch_) + (i + 0) * in.numOfChannels_];

			pixel[0] = finaleSample[0];
			pixel[1] = finaleSample[1];
			pixel[2] = finaleSample[2];
			if (in.numOfChannels_ == 4) { pixel[3] = finaleSample[3]; }
		}
	}
}

bool Tga::loadTGA(const char* FilePath)
{
	std::cout << "Reading \"" << FilePath << "\".... \n";
	std::fstream in{ FilePath, std::ios::in | std::ios::binary };
	if (!in.is_open())
		return false;

	in.read(reinterpret_cast<char*>(&header_), sizeof(header_));

	if (std::memcmp(compressed_, &header_, sizeof(compressed_))){
		bitsPerPixel_ = header_[16];
		width_ = header_[13] * 256 + header_[12];
		height_ = header_[15] * 256 + header_[14];
		size_ = ((width_ * bitsPerPixel_ + 31) / 32) * 4 * height_ ;
		numOfChannels_ = bitsPerPixel_ / 8;
		pitch_ = width_ * numOfChannels_;
		if (width_ <= 0 || height_ <= 0 || (bitsPerPixel_ != 24) && (bitsPerPixel_ != 32)){
			in.close();
			throw std::exception("Image width, height, bits per pixel is not correct!!!");
		}

		imageData_.resize(size_);
		bCompressed_ = false;
		in.read(reinterpret_cast<char*>(imageData_.data()), size_);
	}
	else{
		//Compressed TGA not supported
		in.close();
		throw std::exception("Compressed TGA is not supported!!!");
	}

	in.close();
	std::cout << "Done ....\n";
	return true;
}

bool Tga::saveTGA(const char* fileName)
{
	std::cout << "Saving \"" << fileName << "\".... \n";

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
	std::cout << "Done ....\n";

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

#ifdef NEAREST_
	std::unique_ptr<Resample> sampler = std::make_unique<NearestNeighbour>();
	sampler->resize(*this, destTga);
#endif
#ifdef LINEAR_
	std::unique_ptr<Resample> sampler = std::make_unique<Bilinear>();
	sampler->resize(*this, destTga);
#endif
#ifdef CUBIC_
	std::unique_ptr<Resample> sampler = std::make_unique<CubicSpline>();
	sampler->resize(*this, destTga);
#endif

	sampler.reset(nullptr);
}

int main(int argc, char** argv)
{
	Tga in;
	Tga out;
	
	if (argc != 3) { 
		std::cerr << "Please provide command in format : TGAResample.exe in.tga nameOfOutFile.tga !!!\n"; 
		return -1;
	}
	try	{

		if (!in.loadTGA(argv[1])) {
			std::cerr << "Error in reading" << argv[1] << "!!!\n";
			return -1;
		}

		std::cout << "Original Size:" << in.width_ << "X" << in.height_ << "\n";
		in.resizeTGA(out);
		std::cout << "Resizing to:" << out.width_ << "X" << out.height_ << "\n";

		if (!out.saveTGA(argv[2])) {
			std::cerr << "Error in saving" << argv[1] << "!!!\n";
			return -1;
		}
	}
	catch (std::exception e){
		std::cerr << e.what();
		return -1;
	}

	return 0;
}