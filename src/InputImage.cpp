#include "InputImage.h"

#include <stdexcept>
#include <cstring>

namespace PanoProjector {

static int ifloor(double arg) {
	return static_cast<int>(floor(arg));
}

static int iceil(double arg) {
	return static_cast<int>(ceil(arg));
}

InputImage::InputImage(const std::string & path,
		double relCropLeft, double relCropRight,
		double relCropTop, double relCropBottom)
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
	m_cropLeft = ifloor(relCropLeft * m_width);
	m_cropRight = iceil(relCropRight * m_width);
	m_cropTop = ifloor(relCropTop * m_height);
	m_cropBottom = iceil(relCropBottom * m_height);

	// Add fudge factor
	if (m_cropLeft > 0) m_cropLeft--;
	if (m_cropRight < m_width) m_cropRight++;
	if (m_cropTop > 0) m_cropTop--;
	if (m_cropBottom < m_height) m_cropBottom++;

	m_cropWidth = m_cropRight - m_cropLeft;

	JDIMENSION sourceCropLeft, sourceCropWidth;
	m_cropHeight = m_cropBottom - m_cropTop;

	if (m_cropHeight < 0 || m_cropHeight >= 65536) {
		throw std::runtime_error("Invalid cropped height");
	}

	if (m_cropRight < m_cropLeft) {
		m_cropWidth += m_width;
		sourceCropLeft = 0;
		sourceCropWidth = m_width;
		m_wrap = true;
	} else {
		sourceCropLeft = m_cropLeft;
		sourceCropWidth = m_cropRight - m_cropLeft;
		m_wrap = false;
	}

	if (m_cropWidth < 0 || m_cropWidth >= 65536) {
		throw std::runtime_error("Invalid cropped width");
	}

	m_data = new uint8_t[3 * m_cropWidth * m_cropHeight];

	if ((int)sourceCropWidth < m_width) {
		jpeg_crop_scanline(&m_cinfo, &sourceCropLeft, &sourceCropWidth);
	}
	if (m_cropTop > 0) {
		jpeg_skip_scanlines(&m_cinfo, m_cropTop);
	}

	if (m_wrap) {
		uint8_t buffer[m_width * 3];
		uint8_t *bufPtr = buffer;
		int leftOffset = 3 * m_cropLeft;
		int leftSize = 3 * (m_width - m_cropLeft);
		int rightSize = 3 * m_cropRight;
		while ((int)m_cinfo.output_scanline < m_cropBottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(pixel(m_cropLeft, j), buffer + leftOffset, leftSize);
			memcpy(pixel(0, j), buffer, rightSize);
		}
	} else if (m_cropWidth < (int)sourceCropWidth) {
		uint8_t buffer[sourceCropWidth * 3];
		uint8_t *bufPtr = buffer;
		while ((int)m_cinfo.output_scanline < m_cropBottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(row(j), buffer + 3 * (m_cropLeft - sourceCropLeft), 3 * m_cropWidth);
		}
	} else {
		while ((int)m_cinfo.output_scanline < m_cropBottom) {
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
