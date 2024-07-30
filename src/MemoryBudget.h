#ifndef PANO_MEMORYBUDGET_H
#define PANO_MEMORYBUDGET_H

namespace PanoProjector {

class MemoryBudget {
public:
	MemoryBudget();

	void setLimit(unsigned long limit);
	void reserve(unsigned long x, unsigned long y = 1, unsigned long z = 1);
	void release(unsigned long x, unsigned long y = 1, unsigned long z = 1) noexcept;

private:
	void throwError(unsigned long x, unsigned long y, unsigned long z) const;

	unsigned long m_limit;
	unsigned long m_usage;
};

extern MemoryBudget g_memBudget;

} // namespace

#endif
