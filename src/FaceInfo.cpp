#include "FaceInfo.h"

namespace PanoProjector::FaceInfo {

static CropRect g_cropRects[6] = {
	{ 7./8, 1./8, 1./4, 3./4 },
	{ 1./8, 3./8, 1./4, 3./4 },
	{ 3./8, 5./8, 1./4, 3./4 },
	{ 5./8, 7./8, 1./4, 3./4 },
	{ 0, 1, 0, 0.3040867239847 /* atan(sqrt(2)) / pi */ },
	{ 0, 1, 0.6959132760153 /* 1 - atan(sqrt(2)) / pi */, 1 }
};

static std::string g_letters[6] = {
	"b", "l", "f", "r", "u", "d"
};

static std::string g_names[6] = {
	"back", "left", "front", "right", "top", "bottom"
};

int getFaceFromName(const std::string & name) {
	for (int face = 0; face < 6; face++) {
		if (name == getLetter(face)
			|| name == getName(face)
			|| name == std::to_string(face)
		) {
			return face;
		}
	}
	return -1;
}

const CropRect & getCropRect(int face) {
	return g_cropRects[face];
}

const std::string & getLetter(int face) {
	return g_letters[face];
}

const std::string & getName(int face) {
	return g_names[face];
}

} // namespace
