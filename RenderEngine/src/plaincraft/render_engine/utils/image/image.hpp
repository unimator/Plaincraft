/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G�rka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef PLAINCRAFT_RENDER_ENGINE_IMAGE
#define PLAINCRAFT_RENDER_ENGINE_IMAGE

#include "../../common.hpp"

namespace plaincraft_render_engine {
	struct Image
	{
		uint32_t width = 0;
		uint32_t height = 0;

		std::unique_ptr<uint8_t> data = nullptr;
		uint32_t size = 0;
	};

	struct BmpFile {
#pragma pack(push, 1)
		struct BitmapFileHeader {
			uint8_t magic[2] {'B', 'M'};
			uint32_t size;
			uint16_t reserved1;
			uint16_t reserved2;
			uint32_t data_offset;
		}  bitmap_file_header;

		struct BitmapInfoHeader {
			uint32_t header_size;
			int32_t width;
			int32_t height;
			uint16_t color_planes{ 1 };
			uint16_t bpp;
			uint32_t compression_method;
			uint32_t size;
			uint32_t horizontal_resolution;
			uint32_t vertical_resolution;
			uint32_t palette_size;
			uint32_t important_colors;
		} bitmap_info_header;

		struct Color {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t padding{ 0 };
		};
		std::vector<Color> palette;
#pragma pack(pop)
	};

	Image load_bmp_image_from_file(const std::string& path);
}


#endif // PLAINCRAFT_RENDER_ENGINE_IMAGE