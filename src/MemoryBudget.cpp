#include <stdexcept>
#include <sstream>
#include <climits>
#include "MemoryBudget.h"

namespace PanoProjector {

MemoryBudget g_memBudget;

MemoryBudget::MemoryBudget()
	: m_limit(ULONG_MAX), m_usage(0)
{}

void MemoryBudget::setLimit(unsigned long limit) {
	m_limit = limit;
}

unsigned long MemoryBudget::reserve(unsigned long x, unsigned long y, unsigned long z) {
	unsigned long increment, newUsage;
	if (__builtin_umull_overflow(x, y, &increment)) {
		// Integer overflow
		throwError(x, y, z);
	}
	if (__builtin_umull_overflow(increment, z, &increment)) {
		// Integer overflow
		throwError(x, y, z);
	}
	if (__builtin_uaddl_overflow(increment, m_usage, &newUsage)) {
		// Integer overflow
		throwError(x, y, z);
	}
	if (newUsage > m_limit) {
		// Limit exceeded
		throwError(x, y, z);
	}
	m_usage = newUsage;
	return increment;
}

void MemoryBudget::release(unsigned long x, unsigned long y, unsigned long z) noexcept {
	m_usage -= x * y * z;
}

void MemoryBudget::throwError(unsigned long x, unsigned long y, unsigned long z) const {
	std::stringstream ss;
	ss << "Out of memory: tried to allocate " << x;
	if (y > 1) {
		ss << " x " << y;
	}
	if (z > 1) {
		ss << " x " << z;
	}
	ss << " bytes with usage " << m_usage << " and limit " << m_limit;

	throw std::runtime_error(ss.str());
}

} // PanoProjector