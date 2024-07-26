#ifndef PANO_OUTPUT_TILER_H
#define PANO_OUTPUT_TILER_H

#include <string>
#include <vector>
#include "OutputImage.h"

namespace PanoProjector {

/**
 * An output class that splits the image up into separate JPEG tiles of a
 * specified maximum width and height. If the overall width and height is
 * not an exact multiple of the tile width and height, the tiles on the right
 * and bottom edge will be truncated.
 */
class OutputTiler : public OutputBase {
public:
	/**
	 * Create an OutputTiler. Images will be saved at the path
	 *
	 *   {prefix}{row}_{col}{suffix}
	 *
	 * Where row and col are zero-based tile indexes. This is compatible with
	 * Pannellum.
	 */
	OutputTiler(const std::string & prefix, const std::string & suffix,
		int width, int height, int tileWidth, int tileHeight,
		const Metadata & metadata);

	/** Not copyable because OutputImage is not copyable */
	OutputTiler(const OutputTiler & other) = delete;

	/** Can be moved -- so that it can go in a std::vector */
	OutputTiler(OutputTiler && other) noexcept;

	~OutputTiler() override;
	void writeRow(uint8_t * data) override;
	void finish() override;
	int getWidth() const override;
	int getHeight() const override;

private:
	/**
	 * Determine if the row of tiles containing the source pixel coordinate
	 * m_rowIndex is present in m_outputs.
	 */
	bool isStripOpen() const {
		return !m_outputs.empty();
	}

	/**
	 * Populate m_outputs with a set of images for the current row.
	 */
	void openStrip();

	/**
	 * Finish all currently open images. libjpeg will raise an error if this is
	 * called too early.
	 */
	void closeStrip();

	int m_width, m_height, m_tileWidth, m_tileHeight;

	const Metadata m_metadata;

	/** The number of tiles in the horizontal direction */
	int m_numTilesWide;

	/** The number of tiles in the vertical direction */
	int m_numTilesHigh;

	/** The current vertical pixel coordinate, to be used for the next call to writeRow() */
	int m_rowIndex;

	std::string m_prefix, m_suffix;
	std::vector<OutputImage> m_outputs;
};

} // namespace
#endif
