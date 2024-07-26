#ifndef PANO_INTEGERCROPRECT_H
#define PANO_INTEGERCROPRECT_H

#include <cmath>
#include "CropRect.h"

namespace PanoProjector {

/**
 * A crop rectangle with integer coordinates
 */
struct IntegerCropRect {
	/**
	 * The left coordinate of the crop rectangle.
	 *
	 * If this is greater than the right coordinate, the crop region is assumed
	 * to wrap around, being split between the far left and far right of the image.
	 */
	int left;

	/**
	 * The past-the-end right coordinate of the crop rectangle, that is, the
	 * maximum x coordinate plus one.
	 */
	int right;

	/**
	 * Whether the crop rectangle is wrapped, with the right less than the left
	 */
	bool wrap;

	/**
	 * The top coordinate of the crop rectangle.
	 */
	int top;

	/**
	 * The past-the-end bottom coordinate of the crop rectangle, that is,
	 * the maximum y coordinate plus one.
	 */
	int bottom;

	/**
	 * The width of the crop rectangle
	 */
	int width;

	/**
	 * The height of the crop rectangle
	 */
	int height;

	IntegerCropRect()
		: left(0), right(0), wrap(false), top(0), bottom(0), width(0), height(0)
	{}

	IntegerCropRect(const CropRect & cropRect, int fullWidth, int fullHeight);
};

} // PanoProjector

#endif
