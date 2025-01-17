#ifndef PANO_OUTPUT_IMAGE_H
#define PANO_OUTPUT_IMAGE_H

#include <string>
#include <cstdio>
#include <jpeglib.h>
#include <jerror.h>

#include "OutputBase.h"
#include "Metadata.h"
#include "EncoderOptions.h"

namespace PanoProjector {

/**
 * An output class that writes to a JPEG file.
 */
class OutputImage : public OutputBase {
public:
	/**
	 * Open the file and begin the JPEG compression
	 */
	OutputImage(const std::string & name, int width, int height,
		const Metadata & metadata, const EncoderOptions & options);

	/** Not copyable due to resource and memory ownership */
	OutputImage(const OutputImage & other) = delete;

	/** Can be moved -- allows it to go in a std::vector<> */
	OutputImage(OutputImage && other) noexcept;

	~OutputImage() override;
	void writeRow(uint8_t * data) override;
	void finish() override;
	int getWidth() const override;
	int getHeight() const override;

private:
	FILE * m_file;
	struct jpeg_compress_struct * m_cinfo;
	struct jpeg_error_mgr * m_jerr;
};

} // namespace
#endif
