#ifndef PANO_EXTRACT_FACE_H
#define PANO_EXTRACT_FACE_H

#include "InputImage.h"
#include "OutputBase.h"

namespace PanoProjector {

	/**
	 * Given an equirectangular 360°x180° source image, which must have a 2:1
	 * aspect ratio, extract the given cube face and write it to the given
	 * destination image, which must be square.
	 */
	void extractFace(int face, InputImage & input, OutputBase & output);

	/**
	 * Equivalent of extractFace() but with the face as a template parameter.
	 * The code runs much faster if the face index is a compile-time constant.
	 */
	template <int face>
	void extractFaceTpl(InputImage & input, OutputBase & output);

}
#endif
