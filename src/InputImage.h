#ifndef PANO_INPUT_IMAGE_H
#define PANO_INPUT_IMAGE_H

#include <string>
#include <cstdio>
#include <jpeglib.h>
#include <jerror.h>
#include <cassert>
#include <cmath>
#include "CropRect.h"
#include "IntegerCropRect.h"
#include "Metadata.h"

namespace PanoProjector {

/**
 * A JPEG input image
 */
class InputImage {
public:
	/**
	 * Read a JPEG image from a file to a managed buffer. If a crop rectangle
	 * is given, only the data within that rectangle will be stored.
	 */
	explicit InputImage(const std::string & path, const CropRect & cropRect);

	InputImage(const InputImage & other) = delete;

	~InputImage();

	/** Get the image width */
	int getWidth() const {
		return m_width;
	}

	/** Get the image height */
	int getHeight() const {
		return m_height;
	}

	/** Get metadata from the image file */
	const Metadata & getMetadata() const {
		return m_metadata;
	}

	/**
	 * Get the data of the given image row (scanline).
	 *
	 * The row must be within the crop region. Assertions are disabled by default
	 * so it will probably crash or execute arbitrary code if you get it wrong.
	 */
	uint8_t * row(int row) {
		assert(row >= m_crop.top);
		assert(row < m_crop.bottom);
		return m_data + (row - m_crop.top) * m_crop.width * COMPONENTS;
	}

	/**
	 * Get a pointer to the given image pixel.
	 *
	 * The pixel must be within the crop region. Assertions are disabled by default
	 * so it will probably crash or execute arbitrary code if you get it wrong.
	 */
	uint8_t * pixel(int col, int row) {
		assertBounds(col, row);
		int colOffset = col - m_crop.left;
		if (colOffset < 0) {
			colOffset += m_width;
		}
		return m_data + ((row - m_crop.top) * m_crop.width + colOffset) * COMPONENTS;
	}

	/**
	 * Assert that the coordinates are within bounds.
	 */
	void assertBounds(int x, int y) const {
		if (m_crop.wrap) {
			assert((x >= m_crop.left && x < m_width)
				|| (x >= 0 && x < m_crop.right));
		} else {
			assert(x >= m_crop.left);
			assert(x < m_crop.right);
		}
		assert(y >= m_crop.top);
		assert(y < m_crop.bottom);
	}

	/**
	 * Given image coordinates in units of pixels, perform a bilinear
	 * interpolation between the four nearest neighbours and write an
	 * interpolated value to the given destination buffer.
	 *
	 * The coordinates must be within the crop rectangle, otherwise it will
	 * segfault or do some other undefined but probably undesired thing.
	 */
	inline void interpolate(uint8_t * dest, float x, float y);

private:
	std::string m_path;
	uint8_t * m_data;
	int m_width, m_height;
	IntegerCropRect m_crop;
	struct jpeg_decompress_struct m_cinfo;
	struct jpeg_error_mgr m_jerr;
	Metadata m_metadata;
};

#if 0 && use_float

// Simple and low instruction count, but a litle slower than the fixed-point version

void InputImage::interpolate(uint8_t * dest, float x, float y) {
	int x0 = static_cast<int>(floorf(x));
	int y0 = static_cast<int>(floorf(y));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	float mu = x - x0, nu = y - y0, munu = mu * nu;

	assertBounds(x0, y0);
	assertBounds(x1, y1);

	uint8_t
		*f00 = (*this)(x0, y0),
		*f01 = (*this)(x0, y1),
		*f10 = (*this)(x1, y0),
		*f11 = (*this)(x1, y1);

	for (int c = 0; c < COMPONENTS; c++) {
		float v = f00[c];
		v = fmaf(f10[c] - f00[c], mu, v);
		v = fmaf(f01[c] - f00[c], nu, v);
		v = fmaf(f11[c] - f10[c] - f01[c] + f00[c], munu, v);
		dest[c] = static_cast<uint8_t>(v);
	}
}

#endif

#if 0 && __GNUC__

// Complicated and no faster than plain fixed point version

void InputImage::interpolate(uint8_t * dest, float x, float y) {
	int x0 = static_cast<int>(floorf(x));
	int y0 = static_cast<int>(floorf(y));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int scale = 256;
	int mu = scale * (x - x0);
	int nu = scale * (y - y0);
	int munu = scale * (x - x0) * (y - y0);

	assertBounds(x0, y0);
	assertBounds(x1, y1);

	typedef int32_t v4i4 __attribute ((vector_size(16)));
	typedef uint8_t v4u1 __attribute ((vector_size(4)));

	uint8_t
		*f00 = (*this)(x0, y0),
		*f01 = (*this)(x0, y1),
		*f10 = (*this)(x1, y0),
		*f11 = (*this)(x1, y1);

	v4i4
		v00 = {f00[0], f00[1], f00[2], 0},
		v01 = {f01[0], f01[1], f01[2], 0},
		v10 = {f10[0], f10[1], f10[2], 0},
		v11 = {f11[0], f11[1], f11[2], 0};

	v4i4 res = scale * v00
		+ mu * (v10 - v00)
		+ nu * (v01 - v00)
		+ munu * (v11 - v10 - v01 + v00);

	res /= scale;

	v4u1 res1 = __builtin_convertvector(res, v4u1);

	for (int c = 0; c < COMPONENTS; c++) {
		dest[c] = res1[c];
	}
}

#endif

// Seems fastest despite executing more instructions

void InputImage::interpolate(uint8_t * dest, float x, float y) {
	int x0 = static_cast<int>(floorf(x));
	int y0 = static_cast<int>(floorf(y));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int scale = 256;
	int mu = scale * (x - x0);
	int nu = scale * (y - y0);
	int munu = scale * (x - x0) * (y - y0);

	assertBounds(x0, y0);
	assertBounds(x1, y1);

	uint8_t
		*f00 = pixel(x0, y0),
		*f01 = pixel(x0, y1),
		*f10 = pixel(x1, y0),
		*f11 = pixel(x1, y1);

	for (int c = 0; c < COMPONENTS; c++) {
		int v =
			scale * f00[c]
				+ mu * (f10[c] - f00[c])
				+ nu * (f01[c] - f00[c])
				+ munu * (f11[c] - f10[c] - f01[c] + f00[c]);
		dest[c] = static_cast<uint8_t>(v / scale);
	}
}

} // namespace
#endif
