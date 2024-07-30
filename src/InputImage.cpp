#include "InputImage.h"
#include "MemoryBudget.h"

#include <stdexcept>
#include <cstring>

namespace PanoProjector {

InputImage::InputImage(const std::string & path, const CropRect & cropRect)
	: m_path(path), m_data(nullptr), m_width(0), m_height(0),
	m_cinfo(), m_jerr()
{
	FILE * f = fopen(path.c_str(), "rb");
	if (!f) {
		throw std::runtime_error(
			std::string("Unable to open input image: ") +
			std::string(strerror(errno)));
	}

	m_cinfo.err = jpeg_std_error(&m_jerr);
	jpeg_create_decompress(&m_cinfo);
	jpeg_stdio_src(&m_cinfo, f);

	// Save the ICC profile while reading the header
	jpeg_save_markers(&m_cinfo, JPEG_APP0 + 2, 0xFFFF);
	(void)jpeg_read_header(&m_cinfo, TRUE);

	// Read the ICC profile
	JOCTET * iccData;
	unsigned int iccDataLength;
	if (jpeg_read_icc_profile(&m_cinfo, &iccData, &iccDataLength)) {
		m_metadata.icc = std::string(
			reinterpret_cast<char*>(iccData),
			iccDataLength);
		free(iccData);
	}

	// The YCbCr -> RGB converter is actually faster than the "null" converter,
	// so it doesn't help to use the input color space here
	m_cinfo.out_color_space = JCS_RGB;

	if (m_cinfo.num_components != COMPONENTS) {
		throw std::runtime_error("Invalid input image: wrong number of components");
	}

	(void)jpeg_start_decompress(&m_cinfo);

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

	g_memBudget.reserve(COMPONENTS, m_crop.width, m_crop.height);
	m_data = new uint8_t[COMPONENTS * m_crop.width * m_crop.height];

	if ((int)sourceCropWidth < m_width) {
		jpeg_crop_scanline(&m_cinfo, &sourceCropLeft, &sourceCropWidth);
	}
	if (m_crop.top > 0) {
		jpeg_skip_scanlines(&m_cinfo, m_crop.top);
	}

	if (m_crop.wrap) {
		uint8_t buffer[m_width * COMPONENTS];
		uint8_t *bufPtr = buffer;
		int leftOffset = COMPONENTS * m_crop.left;
		int leftSize = COMPONENTS * (m_width - m_crop.left);
		int rightSize = COMPONENTS * m_crop.right;
		while ((int)m_cinfo.output_scanline < m_crop.bottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(pixel(m_crop.left, j), buffer + leftOffset, leftSize);
			memcpy(pixel(0, j), buffer, rightSize);
		}
	} else if (m_crop.width < (int)sourceCropWidth) {
		uint8_t buffer[sourceCropWidth * COMPONENTS];
		uint8_t *bufPtr = buffer;
		while ((int)m_cinfo.output_scanline < m_crop.bottom) {
			int j = m_cinfo.output_scanline;
			(void)jpeg_read_scanlines(&m_cinfo, &bufPtr, 1);
			memcpy(row(j), buffer + COMPONENTS * (m_crop.left - sourceCropLeft), COMPONENTS * m_crop.width);
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
	g_memBudget.release(COMPONENTS, m_crop.width, m_crop.height);
	delete[] m_data;
}

} // namespace
