#include "InputImage.h"

#include <stdexcept>
#include <cstring>

namespace PanoProjector {

InputImage::InputImage(const std::string & path, const CropRect & cropRect)
	: m_path(path), m_data(nullptr), m_width(0), m_height(0),
	m_cinfo(), m_jerr()
{
	FILE * f = fopen(path.c_str(), "rb");
	if (!f) {
		throw std::runtime_error("Unable to open input image");
	}

	m_cinfo.err = jpeg_std_error(&m_jerr);
	jpeg_create_decompress(&m_cinfo);
	jpeg_stdio_src(&m_cinfo, f);

	(void)jpeg_read_header(&m_cinfo, TRUE);
	(void)jpeg_start_decompress(&m_cinfo);

	if (m_cinfo.output_components != 3) {
		throw std::runtime_error("Invalid input image: wrong number of components");
	}
	if (m_cinfo.data_precision != 8) {
		throw std::runtime_error("Invalid input image: wrong data precision");
	}
	m_width = m_cinfo.output_width;
	m_height = m_cinfo.output_height;

	// Determine cropping region
	m_crop = IntegerCropRect(cropRect, m_width, m_height);

	JDIMENSION sourceCropLeft, sourceCropWidth;

	if (m_crop.height < 0 || m_crop.height >= 65536) {
		throw std::runtime_error("Invalid cropped height");
	}

	if (m_crop.wrap) {
		sourceCropLeft = 0;
		sourceCropWidth = m_width;
	} else {
		sourceCropLeft = m_crop.left;
		sourceCropWidth = m_crop.right - m_crop.left;
	}

	if (m_crop.width < 0 || m_crop.width >= 65536) {
		throw std::runtime_error("Invalid cropped width");
	}

	m_data = new uint8_t[3 * m_crop.width * m_crop.height];

	if ((int)sourceCropWidth < m_width) {
		jpeg_crop_scanline(&m_cinfo, &sourceCropLeft, &sourceCropWidth);
	}
	if (m_crop.top > 0) {
		jpeg_skip_scanlines(&m_cinfo, m_crop.top);
	}

	if (m_crop.wrap) {
		uint8_t buffer[m_width * 3];
		uint8_t *bufPtr = buffer;
		int leftOffset = 3 * m_crop.left;
		int leftSize = 3 * (m_width - m_crop.left);
		int rightSize = 3 * m_crop.right;
		while ((int)m_cinfo.output_scanline < m_crop.bottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(pixel(m_crop.left, j), buffer + leftOffset, leftSize);
			memcpy(pixel(0, j), buffer, rightSize);
		}
	} else if (m_crop.width < (int)sourceCropWidth) {
		uint8_t buffer[sourceCropWidth * 3];
		uint8_t *bufPtr = buffer;
		while ((int)m_cinfo.output_scanline < m_crop.bottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(row(j), buffer + 3 * (m_crop.left - sourceCropLeft), 3 * m_crop.width);
		}
	} else {
		while ((int)m_cinfo.output_scanline < m_crop.bottom) {
			uint8_t * rowptr = row(m_cinfo.output_scanline);
			(void)jpeg_read_scanlines(&m_cinfo, &rowptr, 1);
		}
	}
	jpeg_skip_scanlines(&m_cinfo, m_height - m_cinfo.output_scanline);
	(void)jpeg_finish_decompress(&m_cinfo);
	jpeg_destroy_decompress(&m_cinfo);
	fclose(f);
}

InputImage::~InputImage()
{
	delete[] m_data;
}

} // namespace
