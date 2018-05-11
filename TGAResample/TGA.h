#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Tga
{
private:
	std::vector<std::uint8_t> pixels_;
	bool bCompressed_;
	std::uint32_t width_;
	std::uint32_t height_;
	std::uint32_t size_;
	std::uint32_t bitsPerPixel_;

	std::uint8_t header_[18] = { 0 };
	std::vector<std::uint8_t> imageData_;
	const std::uint8_t compressed_[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

public:
	Tga() = default;
	~Tga() {};

	bool loadTGA(const char* FilePath);
	bool saveTGA(const char* fileName);
	std::vector<std::uint8_t> getPixels() const { return pixels_; }
	std::uint32_t getWidth() const { return this->width_; }
	std::uint32_t getHeight() const { return this->height_; }
	bool hasAlphaChannel() const { return bitsPerPixel_ == 32; }
};
