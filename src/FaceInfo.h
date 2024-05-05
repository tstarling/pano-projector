#ifndef PANO_FACE_INFO_H
#define PANO_FACE_INFO_H
#include <math.h>
#include <string>

namespace PanoProjector {

struct CropRect {
	double left;
	double right;
	double top;
	double bottom;
};

/**
 * Functions giving information about cube faces
 */
namespace FaceInfo {

/**
 * Set the invariant cartesian coordinate for this cube face.
 * Set x, y, or z to ±1.
 */
template <int face>
static inline void setInvariant(float & x, float & y, float & z);

/**
 * Given the vertical cube face image coordinate scaled to |b|≤1,
 * set the relevant 3-d cartesian coordinate.
 */
template <int face>
static inline void setMajor(float & x, float & y, float & z, float b);

/**
 * Given the horizontal cube face image coordinate scaled to |a|≤1,
 * set the relevant 3-d cartesian coordinate.
 */
template <int face>
static inline void setMinor(float & x, float & y, float & z, float a);

/**
 * Adjust the xy Euler angle to account for a reflection about the vertical
 * cube face image axis.
 */
template <int face>
static inline void reflectTheta(float & theta);

/**
 * Get the bounding equirectangular image rectangle scaled to 0≤u<1 for the
 * given cube face. When the cube face is mapped to the equirectangular
 * projection, all points are guaranteed to be within this rectangle.
 */
const CropRect & getCropRect(int face);

/**
 * Get the letter used by Pannellum to identify the specified cube face. The
 * top and bottom cube faces become (u)p and (d)own to avoid conflicting with
 * (b)ack.
 */
const std::string & getLetter(int face);

/**
 * Get the name of the specified cube face
 */
const std::string & getName(int face);

/**
 * Get the index of the cube face for the given user input, which may be an
 * integer, a letter or a name. If there is no match, -1 is returned.
 */
int getFaceFromName(const std::string & name);

// These functions give a warning about being unused when the header is included
// into a file that doesn't use them. It's apparently a GCC bug.
// inline static functions have the unused warning suppressed, but when they are
// template functions, the suppression doesn't happen.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

template<> void setInvariant<0>(float & x, float & y, float & z)      { x = -1.0f; }
template<> void setMajor<0>(float & x, float & y, float & z, float b) { z = -b; }
template<> void setMinor<0>(float & x, float & y, float & z, float a) { y = -a; }
template<> void reflectTheta<0>(float & theta)                        { theta = -theta; }

template<> void setInvariant<1>(float & x, float & y, float & z)      { y = -1.0f; }
template<> void setMajor<1>(float & x, float & y, float & z, float b) { z = -b; }
template<> void setMinor<1>(float & x, float & y, float & z, float a) { x = a; }
template<> void reflectTheta<1>(float & theta)                        { theta = copysignf(M_PI, theta) - theta; }

template<> void setInvariant<2>(float & x, float & y, float & z)      { x = 1.0f; }
template<> void setMajor<2>(float & x, float & y, float & z, float b) { z = -b; }
template<> void setMinor<2>(float & x, float & y, float & z, float a) { y = a; }
template<> void reflectTheta<2>(float & theta)                        { theta = -theta; }

template<> void setInvariant<3>(float & x, float & y, float & z)      { y = 1.0f; }
template<> void setMajor<3>(float & x, float & y, float & z, float b) { z = -b; }
template<> void setMinor<3>(float & x, float & y, float & z, float a) { x = -a; }
template<> void reflectTheta<3>(float & theta)                        { theta = copysignf(M_PI, theta) - theta; }

template<> void setInvariant<4>(float & x, float & y, float & z)      { z = 1.0f; }
template<> void setMajor<4>(float & x, float & y, float & z, float b) { x = b; }
template<> void setMinor<4>(float & x, float & y, float & z, float a) { y = a; }
template<> void reflectTheta<4>(float & theta)                        { theta = -theta; }

template<> void setInvariant<5>(float & x, float & y, float & z)      { z = -1.0f; }
template<> void setMajor<5>(float & x, float & y, float & z, float b) { x = -b; }
template<> void setMinor<5>(float & x, float & y, float & z, float a) { y = a; }
template<> void reflectTheta<5>(float & theta)                        { theta = -theta; }

#pragma GCC diagnostic pop

} // namespace FaceInfo
} // namespace PanoProjector
#endif
