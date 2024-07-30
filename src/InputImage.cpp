#include "InputImage.h"
#include "MemoryBudget.h"

#include <stdexcept>
#include <cstring>

namespace PanoProjector {

InputImage::InputImage()
	: m_data(nullptr), m_width(0), m_height(0)
{}

InputImage::~InputImage() {}

} // namespace
