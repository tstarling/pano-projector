#ifndef PANO_MEMORYBUDGET_H
#define PANO_MEMORYBUDGET_H

namespace PanoProjector {

/**
 * Memory accounting
 *
 * Currently we are only accounting for memory which is proportional to the
 * image area. Memory which is proportional to the width is considered to be
 * insignificant.
 */
class MemoryBudget {
public:
	MemoryBudget();

	/**
	 * Set the memory limit
	 */
	void setLimit(unsigned long limit);

	/**
	 * Reserve an amount of memory specified in bytes by multiplying up to
	 * three numbers, with integer overflow protection. If the limit is
	 * exceeded, throw an error.
	 */
	unsigned long reserve(unsigned long x, unsigned long y = 1, unsigned long z = 1);

	/**
	 * Release previously reserved memory.
	 */
	void release(unsigned long x, unsigned long y = 1, unsigned long z = 1) noexcept;

	/**
	 * Get current accounted memory usage
	 */
	unsigned long getUsage() const { return m_usage; }

private:
	void throwError(unsigned long x, unsigned long y, unsigned long z) const;

	unsigned long m_limit;
	unsigned long m_usage;
};

extern MemoryBudget g_memBudget;

} // namespace

#endif
