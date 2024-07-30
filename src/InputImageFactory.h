#ifndef PANO_INPUTIMAGEFACTORY_H
#define PANO_INPUTIMAGEFACTORY_H

#include "InputImage.h"

namespace PanoProjector {

class InputImageFactory {
public:
	static InputImage * create(
		const std::string & path,
		const std::string & format,
		const CropRect & cropRect);

	/**
	 * Extract a format from a path and format specification. Convert it to
	 * lowercase, but do not validate it.
	 */
	static std::string normalizeFormat(const std::string & path, const std::string & format);
};

} // namespace

#endif
