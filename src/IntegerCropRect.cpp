#include "IntegerCropRect.h"

namespace PanoProjector {

static inline int fudgeFloor(double arg) {
	int i = static_cast<int>(floor(arg));
	return i >= 1 ? i - 1 : 0;
}

static inline int fudgeCeil(double arg, int limit) {
	int i = static_cast<int>(ceil(arg));
	return i < limit ? i + 1 : limit;
}

IntegerCropRect::IntegerCropRect(const PanoProjector::CropRect &cropRect, int fullWidth, int fullHeight)
	: left(fudgeFloor(cropRect.left * fullWidth)),
	right(fudgeCeil(cropRect.right * fullWidth, fullWidth)),
	wrap(right < left),
	top(fudgeFloor(cropRect.top * fullHeight)),
	bottom(fudgeCeil(cropRect.bottom * fullHeight, fullHeight)),
	width(wrap ? fullWidth + right - left : right - left),
	height(bottom - top)
{}

} // namespace