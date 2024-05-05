#include <math.h>
#include <stdexcept>
#include <iostream>
#include <cassert>

#include "extractFace.h"
#include "FaceInfo.h"

namespace PanoProjector {

// Based on https://gist.github.com/bitonic/d0f5a0a44e37d4f0be03d34d47acb6cf
static inline float atan_approx(float x) {
	float a1  =  0.99997726f;
	float a3  = -0.33262347f;
	float a5  =  0.19354346f;
	float a7  = -0.11643287f;
	float a9  =  0.05265332f;
	float a11 = -0.01172120f;
	float x_sq = x*x;
	return
		x * fmaf(x_sq, fmaf(x_sq, fmaf(x_sq, fmaf(x_sq, fmaf(x_sq, a11, a9), a7), a5), a3), a1);
}

// Based on https://gist.github.com/bitonic/d0f5a0a44e37d4f0be03d34d47acb6cf
static float atan2f_approx(float y, float x) {
	const float pi = M_PI;
	const float pi_2 = M_PI_2;
	bool swap = fabs(x) < fabs(y);
	float atan_input = (swap ? x : y) / (swap ? y : x);
	float res = atan_approx(atan_input);
	res = swap ? copysignf(pi_2, atan_input) - res : res;
	if (x < 0.0f) {
		res = copysignf(pi, y) + res;
	}
	return res;
}

void extractFace(int face, InputImage & input, OutputBase & output) {
	// The code runs much faster if the face index is a compile-time constant.
	if (face == 0) extractFaceTpl<0>(input, output);
	if (face == 1) extractFaceTpl<1>(input, output);
	if (face == 2) extractFaceTpl<2>(input, output);
	if (face == 3) extractFaceTpl<3>(input, output);
	if (face == 4) extractFaceTpl<4>(input, output);
	if (face == 5) extractFaceTpl<5>(input, output);
}

template <int face>
void extractFaceTpl(InputImage & input, OutputBase & output) {
	const int destWidth = output.getWidth();
	const int destHeight = output.getHeight();
	const int srcWidth = input.getWidth();
	const int srcHeight = input.getHeight();
	const float pi = M_PI;
	const float pi_2 = M_PI_2;
	const float pi_x2 = M_PI * 2;

	uint8_t buffer[destWidth * 3];

	// Cartesian coords on the 2x2x2 cube |x|<=1, |y|<=1, |z|<=1
	float x = 0, y = 0, z = 0;

	FaceInfo::setInvariant<face>(x, y, z);

	for (int j = 0; j < destHeight; j++) {
		FaceInfo::setMajor<face>(x, y, z, (2.0f * j) / destHeight - 1.0f);

		for (int i = 0; i < destWidth / 2; i++) {
			FaceInfo::setMinor<face>(x, y, z, (2.0f * i) / destWidth - 1.0f);

			float theta = atan2f_approx(y, x);
			float phi = atan2f_approx(z, hypotf(x, y));

			// Source image coords
			float uf = (theta + pi) / pi_x2 * (srcWidth - 1);
			float vf = (pi_2 - phi) / pi * (srcHeight - 1);
			input.interpolate(&buffer[3 * i], uf, vf);

			// Reflect the horizontal destination coordinate and repeat
			int ii = destWidth - i - 1;
			// Reflecting i does not change phi, we only have to reflect theta
			FaceInfo::reflectTheta<face>(theta);
			uf = (theta + pi) / pi_x2 * (srcWidth - 1);
			input.interpolate(&buffer[3 * ii], uf, vf);
		}
		output.writeRow(buffer);
	}
	output.finish();
}

} // namespace
