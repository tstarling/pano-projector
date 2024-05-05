#include <cmath>
#include "FaceDiagramCommand.h"
#include "OutputImage.h"

namespace PanoProjector {

int FaceDiagramCommand::doRun() {
	const std::string & output = m_options["output"].as<std::string>();
	int width = m_options["width"].as<int>();
	width += width % 2;

	if (m_options["numerical"].as<bool>()) {
		makeFaceDiagramNumerical(output, width);
	} else {
		makeFaceDiagramAnalytic(output, width);
	}
	return 0;
}

std::string FaceDiagramCommand::getSynopsis() {
	return "face-diagram [options] <output>";
}

std::string FaceDiagramCommand::getName() {
	return "face-diagram";
}

std::string FaceDiagramCommand::getDescription() {
	return "Generate an equirectangular image with colours indicating cube face "
		"mappings. This helps to explain the cropping regions in FaceInfo::getCropRect()";
}

void FaceDiagramCommand::initOptions() {
	m_visible.add_options()
		("help",
		 	"Show help message and exit")
		("width", po::value<int>()->default_value(800),
			"The output image width (rounded to nearest even number)")
		("numerical", po::bool_switch(),
			"invert the transformation numerically, not analytically")
		;

	m_invisible.add_options()
		("output", po::value<std::string>())
		;

	m_pos
		.add("output", 1)
		;
}

void FaceDiagramCommand::makeFaceDiagramNumerical(std::string outputPath, int width) {
	int height = width / 2;
	uint8_t * buffer = new uint8_t[width * height * 3];

	for (int face = 0; face < 6; face++) {
		for (int j = 0; j < 5000; j++) {
			for (int i = 0; i < 5000; i++) {
				float a = (2.0f*i)/5000 - 1.0f;
				float b = (2.0f*j)/5000 - 1.0f;
				float x, y, z;
				uint32_t color;
				if (face==0) {
					x = -1.0f;
					y = -a;
					z = -b;
					color = 0xff0000; // red
				} else if (face==1) {
					x = a;
					y = -1.0f;
					z = -b;
					color = 0x00ff00; // green
				} else if (face==2) {
					x = 1.0f;
					y = a;
					z = -b;
					color = 0x0000ff; // blue
				} else if (face==3) {
					x = -a;
					y = 1.0f;
					z = -b;
					color = 0x00ffff; // cyan
				} else if (face==4) {
					x = b;
					y = a;
					z = 1.0f;
					color = 0xff00ff; // magenta
				} else if (face==5) {
					x = -b;
					y = a;
					z = -1.0f;
					color = 0xffff00; // yellow
				}

				float theta = atan2f(y, x);
				float r = hypotf(x, y);
				float phi = atan2f(z, r);

				// source img coords
				int u = (theta + M_PI) / M_PI * (height - 1);
				int v = (M_PI_2 - phi) / M_PI * (height - 1);

				buffer[0 + 3 * (u + width * v)] = color >> 16;
				buffer[1 + 3 * (u + width * v)] = (color >> 8) & 0xff;
				buffer[2 + 3 * (u + width * v)] = color & 0xff;
			}
		}
	}
	OutputImage output(outputPath, width, height);
	for (int j = 0; j < height; j++) {
		output.writeRow(buffer + 3 * j * width);
	}
	output.finish();
}

void FaceDiagramCommand::makeFaceDiagramAnalytic(std::string outputPath, int width) {
	int height = width / 2;
	float pi4 = M_PI / 4;
	uint8_t buffer[width * 3];
	uint32_t color;
	OutputImage output(outputPath, width, height);

	for (int v = 0; v < height; v++) {
		for (int u = 0; u < width; u++) {
			float theta = u * M_PI / (height - 1) - M_PI;
			float phi = M_PI_2 - v * M_PI / (height - 1);
			float evenThreshold = 1 / sqrt(pow(tan(theta), 2) + 1);
			float oddThreshold = 1 / sqrt(pow(tan(theta), -2) + 1);
			float tanPhi = tan(phi);
			int face;

			if (fabs(theta) < pi4 || fabs(theta) >= 3 * pi4) {
				if (tanPhi >= evenThreshold) {
					face = 4;
				} else if (tanPhi < -evenThreshold) {
					face = 5;
				} else if (fabs(theta) < pi4) {
					face = 2;
				} else {
					face = 0;
				}
			} else {
				if (tanPhi >= oddThreshold) {
					face = 4;
				} else if (tanPhi < -oddThreshold) {
					face = 5;
				} else if (theta < 0) {
					face = 1;
				} else {
					face = 3;
				}
			}
			if (face==0) {
				// Red
				color = 0xff0000;
			} else if (face==1) {
				// Green
				color = 0x00ff00;
			} else if (face==2) {
				// Blue
				color = 0x0000ff;
			} else if (face==3) {
				// Cyan
				color = 0x00ffff;
			} else if (face==4) {
				// Magenta
				color = 0xff00ff;
			} else if (face==5) {
				// Yellow
				color = 0xffff00;
			} else {
				color = 0;
			}
			buffer[0 + 3 * u] = color >> 16;
			buffer[1 + 3 * u] = (color >> 8) & 0xff;
			buffer[2 + 3 * u] = color & 0xff;
		}
		output.writeRow(buffer);
	}
	output.finish();
}

}
