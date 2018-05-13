#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>

class Tga
{

public:
	Tga() = default;
	~Tga() {};

	bool loadTGA(const char* FilePath);
	bool saveTGA(const char* fileName);
	void resizeTGA(Tga& destTga);

public:
	std::vector<std::uint8_t> pixels_;
	bool bCompressed_;
	std::uint32_t width_;
	std::uint32_t height_;
	std::uint32_t size_;
	std::uint32_t bitsPerPixel_;
	std::uint32_t numOfChannels_;
	std::uint32_t pitch_;
	std::uint8_t header_[18] = { 0 };
	std::vector<std::uint8_t> imageData_;
	const std::uint8_t compressed_[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

};


class Resample
{
public:
	Resample() = default;
	~Resample() {}

public:
	virtual void resize(Tga& in, Tga& out) = 0;
};

class NearestNeighbour : public Resample
{
public:
	NearestNeighbour() = default;
	virtual ~NearestNeighbour() {}
public:

	void resize(Tga& in, Tga& out) override;
};

class Bilinear : public Resample
{
public:
	Bilinear() = default;
	virtual ~Bilinear() {}
public:

	void resize(Tga& in, Tga& out) override;
};

namespace Helper
{
	template<class T>
	inline void clamp(T& v, const T min, const T max) {
		if (v < min) { v = min; }
		else if (v > max) { v = max; }
	}

	inline float lerp(const float a, const float b, const float t)
	{
		return a * (1.0f - t) + b * t;
	}

	inline uint8_t* getClampedSample(Tga& in, int x, int y)
	{
		clamp<int>(x, 0, in.width_ - 1);
		clamp<int>(y, 0, in.height_ - 1);

		return &in.imageData_[(y * in.pitch_) + (x + 0) * in.numOfChannels_];
	}
};
