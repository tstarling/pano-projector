#include "OutputPyramid.h"

#include <cstdlib>
#include <cstring>

namespace PanoProjector {

OutputPyramid::OutputPyramid(int levels, int width, int height)
	: m_levels(levels), m_width(width), m_height(height),
	m_nextRowIndex(0)
{
	m_savedRows = new uint8_t*[levels];
	m_mixResults = new uint8_t*[levels];
	for (int level = 0; level < m_levels; level++) {
		m_savedRows[level] = new uint8_t[getLevelRowSize(level)];
		m_mixResults[level] = new uint8_t[getLevelRowSize(level)];
	}
	m_outputs.reserve(levels);
}

OutputPyramid::~OutputPyramid() {
	for (int level = 0; level < m_levels; level++) {
		delete[] m_savedRows[level];
		delete[] m_mixResults[level];
	}
	delete[] m_savedRows;
	delete[] m_mixResults;

	for (size_t i = 0; i < m_outputs.size(); i++) {
		delete m_outputs[i];
	}
}

void OutputPyramid::writeRow(uint8_t * data) {
	int j = m_nextRowIndex++;
	flushRow(0, data);
	for (int level = 1; level < m_levels; level++) {
		if (j & (1 << (level - 1))) {
			data = mixRow(level, data);
			flushRow(level, data);
		} else {
			saveRow(level - 1, data);
			break;
		}
	}
}

void OutputPyramid::flushRow(int level, uint8_t * data) {
	m_outputs[level]->writeRow(data);
}

uint8_t * OutputPyramid::mixRow(int level, uint8_t * data) {
	uint8_t * oldData = m_savedRows[level - 1];
	uint8_t * result = m_mixResults[level];
	int n = getLevelWidth(level);
	for (int si = 0, di = 0; di < n; si += 2, di++) {
		for (int c = 0; c < 3; c++) {
			if (si < 2 * n - 1) {
				result[3 * di + c] = (uint8_t)(
					(
						(int)oldData[3 * si + c]
						+ oldData[3 * (si + 1) + c]
						+ data[3 * si + c]
						+ data[3 * (si + 1) + c]
					) / 4);
			} else {
				result[3 * di + c] = (uint8_t)(
					(
						(int)oldData[3 * si + c]
						+ data[3 * si + c]
					) / 2);
			}
		}
	}
	return result;
}

void OutputPyramid::saveRow(int level, uint8_t * data) {
	memcpy(m_savedRows[level], data, getLevelRowSize(level));
}

void OutputPyramid::finish() {
	for (int level = 0; level < m_levels; level++) {
		m_outputs[level]->finish();
	}
}

int OutputPyramid::getWidth() const {
	return m_width;
}

int OutputPyramid::getHeight() const {
	return m_height;
}

void OutputPyramid::addLevelOutput(OutputBase * output) {
	m_outputs.push_back(output);
}

} // namespace

