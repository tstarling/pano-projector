#include <iostream>

#include "FaceCommand.h"
#include "FaceInfo.h"
#include "extractFace.h"
#include "OutputImage.h"

namespace PanoProjector {

namespace po = boost::program_options;

void FaceCommand::initOptions() {
	m_visible.add_options()
		("help",
		 	"Show help message and exit")
		("size", po::value<int>()->default_value(0),
			"The output image width and height (default: full resolution)")
		("face", po::value<std::string>(),
		 	"Which face to extract")
		;

	m_invisible.add_options()
		("input", po::value<std::string>())
		("output", po::value<std::string>())
		;

	m_pos
		.add("input", 1)
		.add("output", 1)
		;
}

std::string FaceCommand::getSynopsis() {
	return "face --face=<face> [options] <input> <output>";
}

std::string FaceCommand::getName() {
	return "face";
}

std::string FaceCommand::getDescription() {
	return "Extract a cube face from an equirectangular source image.";
}

int FaceCommand::doRun() {
	if (!m_options.count("input") || !m_options.count("output")) {
		std::cerr << "Error: an input filename and an output filename must be specified.\n";
		return 1;
	}
	if (!m_options.count("face")) {
		std::cerr << "Error: the face must be specified.\n";
		return 1;
	}

	const std::string & faceName = m_options["face"].as<std::string>();
	int face = FaceInfo::getFaceFromName(faceName);
	if (face == -1) {
		std::cerr << "Error: invalid face name \"" << faceName << "\", must be one of: ";
		for (int f = 0; f < 6; f++) {
			std::cerr << FaceInfo::getLetter(f) << ", ";
		}
		for (int f = 0; f < 6; f++) {
			std::cerr << FaceInfo::getName(f);
			if (f < 5) {
				std::cerr << ", ";
			}
		}
		std::cerr << "\n";
		return 1;
	}

	const CropRect cropRect = FaceInfo::getCropRect(face);
	InputImage input(
		m_options["input"].as<std::string>(),
		cropRect.left,
		cropRect.right,
		cropRect.top,
		cropRect.bottom
	);

	if (input.getWidth() != input.getHeight() * 2) {
		std::cerr << "Input image has incorrect aspect ratio, must be 2:1.\n";
		return 1;
	}

	int size = m_options["size"].as<int>();
	if (size <= 0) {
		size = 8 * (int)(input.getWidth() / M_PI / 8);
	}

	OutputImage output(m_options["output"].as<std::string>(), size, size);

	extractFace(face, input, output);
	return 0;
}

} // namespace
