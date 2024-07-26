#ifndef PANO_METADATA_H
#define PANO_METADATA_H

#include <jpeglib.h>
#include <string>

namespace PanoProjector {

/**
 * The number of channels or components in buffers passed between classes.
 *
 * I tried passing the number of components from the input image down through
 * the pipeline, but I didn't find a way to recover the performance of having
 * the number of components fixed to 3. It would be possible to support
 * greyscale by converting it in InputImage while writing it to the buffer,
 * but probably nobody needs that.
 *
 * PanoProjector is fast because it's focused on use cases that we actually
 * need. If it did everything, it would turn into Hugin or libpano, i.e. 10x
 * slower.
 */
enum { COMPONENTS = 3 };

struct Metadata {
	std::string icc;
};

} // namespace

#endif
