#ifndef PANO_OUTPUT_PYRAMID_H
#define PANO_OUTPUT_PYRAMID_H
#include <cstdint>
#include <cstddef>
#include <vector>

#include "OutputBase.h"

namespace PanoProjector {

/**
 * An image output class that writes to a set of resolution levels, each level
 * being scaled down by a factor of 2 compared to the previous level.
 */
class OutputPyramid : public OutputBase {
public:
	/**
	 * Constructor. After calling this, addLevelOutput() must be called once for
	 * each level in order to define the next pipeline stage.
	 */
	OutputPyramid(int levels, int width, int height);

	/** Not copyable due to custom memory management */
	OutputPyramid(const OutputPyramid & other) = delete;

	virtual ~OutputPyramid() override;

	virtual void writeRow(uint8_t * data) override;
	virtual void finish() override;
	virtual int getWidth() const override;
	virtual int getHeight() const override;

	/**
	 * Add an output object which will be used for output at a resolution level.
	 * The first time this is called will define level 0 (unscaled), the next
	 * is level 1 (2x downscaled), etc.
	 *
	 * The object should be allocated with "new" and becomes owned by
	 * OutputPyramid. It will be deleted when OutputPyramid is destroyed.
	 */
	void addLevelOutput(OutputBase * output);
private:
	/** Get the size of a destination row, in bytes */
	size_t getLevelRowSize(int level) const {
		return (size_t)(m_width >> level) * 3;
	}

	/** Get the width of a destination row, in pixels */
	int getLevelWidth(int level) const {
		return m_width >> level;
	}

	/** Write data to the destination */
	void flushRow(int level, uint8_t * data);

	/**
	 * Mix the saved row data from level-1 (the previous 2x scale level)
	 * with the specified level-1 data (also 2x scale), and produce a
	 * destination row for the specified level, by simple block averaging.
	 * The result will be stored to a temporary buffer and the address of
	 * that buffer will be returned.
	 */
	uint8_t * mixRow(int level, uint8_t * data);

	/**
	 * Save a row of data so that it can be mixed with a subsequent row by
	 * mixRow().
	 */
	void saveRow(int level, uint8_t * data);

	int m_levels, m_width, m_height, m_nextRowIndex;
	std::vector<OutputBase*> m_outputs;
	uint8_t ** m_savedRows;
	uint8_t ** m_mixResults;
};

} // namespace
#endif
