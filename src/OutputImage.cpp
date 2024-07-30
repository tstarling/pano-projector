#include "OutputImage.h"
#include <stdexcept>
#include <cstring>

namespace PanoProjector {

OutputImage::OutputImage(const std::string & name, int width, int height,
	const Metadata & metadata, const EncoderOptions & options)
{
	m_file = fopen(name.c_str(), "wb");
	if (m_file == nullptr) {
		throw std::runtime_error(
			"Unable to open output file \"" + name + "\": " +
			std::string(strerror(errno))
		);
	}
	m_cinfo = new struct jpeg_compress_struct();
	m_jerr = new struct jpeg_error_mgr();
	m_cinfo->err = jpeg_std_error(m_jerr);
	jpeg_create_compress(m_cinfo);
	jpeg_stdio_dest(m_cinfo, m_file);
	m_cinfo->in_color_space = JCS_RGB;
	jpeg_set_defaults(m_cinfo);
	jpeg_set_quality(m_cinfo, options.quality, FALSE);
	m_cinfo->image_width = width;
	m_cinfo->image_height = height;
	m_cinfo->input_components = COMPONENTS;
	m_cinfo->data_precision = 8;
	jpeg_start_compress(m_cinfo, TRUE);

	if (!metadata.icc.empty()) {
		jpeg_write_icc_profile(
			m_cinfo,
			reinterpret_cast<const JOCTET*>(metadata.icc.data()),
			static_cast<unsigned int>(metadata.icc.length()));
	}
}

OutputImage::OutputImage(OutputImage && other) noexcept
	: m_file(other.m_file),
	m_cinfo(other.m_cinfo),
	m_jerr(other.m_jerr)
{
	other.m_file = nullptr;
	other.m_cinfo = nullptr;
	other.m_jerr = nullptr;
}

OutputImage::~OutputImage() {
	if (m_file) {
		fclose(m_file);
		m_file = nullptr;
	}
	if (m_cinfo) {
		jpeg_destroy_compress(m_cinfo);
		delete m_cinfo;
		delete m_jerr;
	}
}

void OutputImage::writeRow(uint8_t * data)
{
	jpeg_write_scanlines(m_cinfo, &data, 1);
}

void OutputImage::finish() {
	jpeg_finish_compress(m_cinfo);
	fclose(m_file);
	m_file = nullptr;
}

int OutputImage::getWidth() const {
	return m_cinfo->image_width;
}

int OutputImage::getHeight() const {
	return m_cinfo->image_height;
}


} // namespace
