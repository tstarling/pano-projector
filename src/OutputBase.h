#ifndef PANO_OUTPUT_BASE_H
#define PANO_OUTPUT_BASE_H

#include <cstdint>

namespace PanoProjector {

/**
 * The abstract base for classes that can write image data.
 */
class OutputBase {
public:
	/** Virtual destructor so that OutputPyramid can own polymorphic instances */
	virtual ~OutputBase() = 0;

	/**
	 * Write a row (scanline) of image data. The size of the data array must be
	 * the width multiplied by the number of channels.
	 */
	virtual void writeRow(uint8_t * data) = 0;

	/**
	 * Notify the subclass that writing of rows is done. If writeRow() has not
	 * been called the correct number of times, bad things will happen.
	 */
	virtual void finish() = 0;

	/**
	 * Get the image width.
	 *
	 * Making it be virtual avoids the need for a base class constructor, and
	 * avoids duplication of the width/height in OutputImage where they are
	 * stored in the JPEG struct.
	 *
	 * It's a judgement call, I could go either way. Non-virtual would be inline
	 * and fast, and would simplify the subclasses.
	 */
	virtual int getWidth() const = 0;

	/** Get the image height */
	virtual int getHeight() const = 0;
};

} // namespace
#endif
