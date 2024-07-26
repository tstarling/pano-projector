#include "OutputTiler.h"

#include <string>

namespace PanoProjector {

using std::to_string;

OutputTiler::OutputTiler(const std::string & prefix, const std::string & suffix,
	int width, int height, int tileWidth, int tileHeight,
	const Metadata & metadata
)
	: m_width(width), m_height(height),
	  m_tileWidth(tileWidth), m_tileHeight(tileHeight),
	  m_metadata(metadata),
	  m_numTilesWide(width / tileWidth + (width % tileWidth ? 1 : 0)),
	  m_numTilesHigh(height / tileHeight + (height % tileHeight ? 1 : 0)),
	  m_rowIndex(0),
	  m_prefix(prefix), m_suffix(suffix)
{}

OutputTiler::OutputTiler(OutputTiler && other) noexcept
	: m_width(other.m_width),
	  m_height(other.m_height),
	  m_tileWidth(other.m_tileWidth),
	  m_tileHeight(other.m_tileHeight),
	  m_metadata(other.m_metadata),
	  m_numTilesWide(other.m_numTilesWide),
	  m_numTilesHigh(other.m_numTilesHigh),
	  m_rowIndex(other.m_rowIndex)
{
	m_prefix.swap(other.m_prefix);
	m_suffix.swap(other.m_suffix);
	m_outputs.swap(other.m_outputs);
}

OutputTiler::~OutputTiler()
{}

void OutputTiler::writeRow(uint8_t * data) {
	openStrip();
	for (int t = 0; t < m_numTilesWide; t++) {
		m_outputs[t].writeRow(data + 3 * t * m_tileWidth);
	}
	if (m_rowIndex % m_tileHeight == m_tileHeight - 1) {
		closeStrip();
	}
	m_rowIndex++;
}

void OutputTiler::finish() {
	if (isStripOpen()) {
		closeStrip();
	}
}

void OutputTiler::openStrip() {
	if (isStripOpen()) {
		return;
	}
	for (int t = 0; t < m_numTilesWide; t++) {
		std::string path = m_prefix
			+ to_string(m_rowIndex / m_tileHeight)
			+ "_" + to_string(t) + m_suffix;
		int tileWidth, tileHeight;
		if (t == m_numTilesWide - 1) {
			tileWidth = m_width - m_tileWidth * (m_numTilesWide -1);
		} else {
			tileWidth = m_tileWidth;
		}
		if (m_rowIndex / m_tileHeight == m_numTilesHigh - 1) {
			tileHeight = m_height - m_tileHeight * (m_numTilesHigh - 1);
		} else {
			tileHeight = m_tileHeight;
		}
		m_outputs.emplace_back(path, tileWidth, tileHeight, m_metadata);
	}
}

void OutputTiler::closeStrip() {
	for (auto & output : m_outputs) {
		output.finish();
	}
	m_outputs.clear();
}

int OutputTiler::getWidth() const {
	return m_width;
}

int OutputTiler::getHeight() const {
	return m_height;
}

}
