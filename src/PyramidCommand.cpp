#include <iostream>
#include <bit>
#include <vector>
#include <filesystem>

#include "PyramidCommand.h"
#include "OutputTiler.h"
#include "OutputPyramid.h"
#include "FaceInfo.h"
#include "extractFace.h"
#include "MemoryBudget.h"

namespace PanoProjector {

namespace fs = std::filesystem;

std::string PyramidCommand::getName() {
	return "pyramid";
}

std::string PyramidCommand::getDescription() {
	return "Extract a cube face or all cube faces from an equirectangular "
		"source image and tile it at multiple resolutions in Pannellum format.";
}

std::string PyramidCommand::getSynopsis() {
	return "pyramid [options] <input> <out-dir>";
}

void PyramidCommand::initOptions() {
	m_visible.add_options()
		("help",
		 	"Show help message and exit")
		("cube-size", po::value<int>(),
			"The output image width and height (default: full resolution)")
		("tile-size", po::value<int>()->default_value(512),
		 	"The tile size in pixels")
		("mem-limit", po::value<unsigned long>(),
			"The approximate maximum memory usage in MiB")
		("face", po::value<std::string>(),
		 	"Which face to extract")
		("levels", po::value<int>(),
			"The number of resolution levels (default: last level has a single tile)")
		("quality", po::value<int>()->default_value(80),
			"The encoder quality, as a percentage")
		("copy-icc", po::bool_switch(),
		 	"Copy the ICC color profile")
		;

	m_invisible.add_options()
		("input", po::value<std::string>())
		("outDir", po::value<std::string>())
		;

	m_pos
		.add("input", 1)
		.add("outDir", 1)
		;
}

static void doFace(
	int face,
	InputImage & input,
	const fs::path & outDir,
	int levels,
	int cubeSize,
	int tileSize,
	const EncoderOptions & options
) {
	OutputPyramid pyramid(levels, cubeSize, cubeSize);

	int levelSize = cubeSize;
	for (int level = 0; level < levels; level++) {
		fs::path levelDir = outDir / std::to_string(levels - level);
		if (!fs::is_directory(levelDir)) {
			fs::create_directory(levelDir);
		}
		pyramid.addLevelOutput(new OutputTiler(
			levelDir / FaceInfo::getLetter(face),
			".jpg",
			levelSize, levelSize,
			tileSize, tileSize,
			input.getMetadata(),
			options));
		levelSize /= 2;
	}

	extractFace(face, input, pyramid);
}

int PyramidCommand::doRun() {
	if (!m_options.count("input") || !m_options.count("outDir")) {
		std::cerr << "Error: an input filename and an output directory must be specified.\n";
		return 1;
	}

	setMemoryLimit();

	CropRect cropRect{};
	int face;
	if (m_options.count("face")) {
		auto & faceName = m_options["face"].as<std::string>();
		face = FaceInfo::getFaceFromName(faceName);
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

		cropRect = FaceInfo::getCropRect(face);
	} else {
		cropRect = {0, 1, 0, 1};
		face = -1;
	}

	InputImage input(m_options["input"].as<std::string>(), cropRect);

	if (input.getWidth() != input.getHeight() * 2) {
		std::cerr << "Input image has incorrect aspect ratio, must be 2:1.\n";
		return 1;
	}

	int cubeSize;
	if (m_options.count("cube-size")) {
		cubeSize = m_options["cube-size"].as<int>();
	} else {
		cubeSize = 8 * (int)(input.getWidth() / M_PI / 8);
	}

	int tileSize = m_options["tile-size"].as<int>();

	int levels;
	if (m_options.count("levels")) {
		levels = m_options["levels"].as<int>();
	} else {
		levels = std::bit_width((unsigned)((cubeSize - 1)/ tileSize)) + 1;
		if (cubeSize / (1 << (levels - 2)) == tileSize) {
			// Due to rounding, we can fit slightly larger source images in a
			// given number of levels than would be expected by just looking at
			// the base-2 logarithm.
			levels -= 1;
		}
	}

	fs::path outDir(m_options["outDir"].as<std::string>());
	if (!fs::is_directory(outDir)) {
		fs::create_directory(outDir);
	}

	EncoderOptions encoderOptions;
	encoderOptions.quality = m_options["quality"].as<int>();

	if (face == -1) {
		for (face = 0; face < 6; face++) {
			doFace(face, input, outDir, levels, cubeSize, tileSize, encoderOptions);
		}
	} else {
		doFace(face, input, outDir, levels, cubeSize, tileSize, encoderOptions);
	}
	return 0;
}

} // namespace
