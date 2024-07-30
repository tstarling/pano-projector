#ifndef PANO_INPUTJPEG_H
#define PANO_INPUTJPEG_H

#include "InputImage.h"

namespace PanoProjector {

class InputJpeg : public InputImage {
public:
	/**
	 * Read a JPEG image from a file to a managed buffer. If a crop rectangle
	 * is given, only the data within that rectangle will be stored.
	 */
	InputJpeg(const std::string & path, const CropRect & cropRect);

	~InputJpeg() override;

private:
	struct jpeg_decompress_struct m_cinfo;
	struct jpeg_error_mgr m_jerr;
	unsigned long m_extraMem;
};

} // namespace

#endif
