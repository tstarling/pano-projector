#include "InputImageFactory.h"
#include "InputJpeg.h"
#include <filesystem>
#include <locale>

namespace PanoProjector {

namespace fs = std::filesystem;

InputImage * InputImageFactory::create(
	const std::string &path,
	const std::string &format,
	const PanoProjector::CropRect &cropRect)
{
	std::string normalFormat = normalizeFormat(path, format);

	if (normalFormat.empty()) {
		throw std::runtime_error("No file extension: use --input-format to specify the input file format");
	}

	if (normalFormat == "jpg" || normalFormat == "jpeg") {
		return new InputJpeg(path, cropRect);
	} else {
		throw std::runtime_error("Unknown input image format \"" + normalFormat + "\"");
	}
}

std::string InputImageFactory::normalizeFormat(const std::string &path, const std::string &format) {
	std::string normalFormat;
	if (format.empty()) {
		// Get the format from the file extension
		fs::path extension = fs::path(path).extension();
		if (extension.empty()) {
			return "";
		}
		// Cut off dot
		normalFormat = std::string(extension.string(), 1);
	} else {
		normalFormat = format;
	}

	// Convert the string to lowercase by this very convenient method /s
	std::use_facet<std::ctype<char>>(std::locale::classic())
		.tolower(&normalFormat.front(), &normalFormat.back() + 1);

	if (normalFormat == "jpg") {
		return "jpeg";
	} else {
		return normalFormat;
	}
}

} // namespace