#ifndef IMAGE_CONVERTER_HPP
#define IMAGE_CONVERTER_HPP

#include "../core/common/debug.hpp"
#include "tools.hpp"

namespace luck
{
	namespace tools
	{
		namespace image
		{
			struct file
			{
				file_header header{'L','I','F',1};
				uint32_t width = 0;
				uint32_t height = 0;
				uint8_t* data = nullptr;
				file() = default;
				inline file(const file& other)
					: header(other.header), width(other.width), height(other.height)
				{
					data = new uint8_t[width*height*4];
					std::memcpy(data, other.data, width*height*4);
				}
				inline file& operator=(const file& other)
				{
					header = other.header;
					width = other.width;
					height = other.height;
					data = new uint8_t[width*height*4];
					std::memcpy(data, other.data, width*height*4);
					return *this;
				}
				inline file(file&& other)
					: header(std::move(other.header)), width(std::move(other.width)), height(std::move(other.height)), data(std::move(other.data))
				{
					other.data = nullptr;
				}
				inline file& operator=(file&& other)
				{
					header = std::move(other.header);
					width = std::move(other.width);
					height = std::move(other.height);
					data = other.data;
					other.data = nullptr;
					return *this;
				}
				inline ~file() { delete[] data; }
			};
			
			std::string save(const file& f, std::string path);
			file load(std::string path);
			std::string convert(std::string path, std::string output_path = "");
		}
	}
}

#endif //IMAGE_CONVERTER_HPP