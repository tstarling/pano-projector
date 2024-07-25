#include <iostream>
#include "FaceCommand.h"
#include "FaceDiagramCommand.h"
#include "PyramidCommand.h"

using namespace PanoProjector;

void usage();

int main(int argc, char** argv) {
	if (argc < 2) {
		usage();
		return 1;
	}
	int cmdArgc = argc - 1;
	char ** cmdArgv = argv + 1;
	std::string cmd(argv[1]);
	if (cmd == "face") {
		return FaceCommand().run(cmdArgc, cmdArgv);
	} else if (cmd == "face-diagram") {
		return FaceDiagramCommand().run(cmdArgc, cmdArgv);
	} else if (cmd == "pyramid") {
		return PyramidCommand().run(cmdArgc, cmdArgv);
	} else {
		usage();
		return 1;
	}
}

void showCommandUsage(const Command & cmd) {
	auto & mcmd = const_cast<Command&>(cmd);
	std::cerr << "pano-projector " << mcmd.getName() << " -- "
		<< mcmd.getDescription() << "\n";
	mcmd.usage();
}

void usage() {
	std::cerr << "Usage: pano-projector <face|face-diagram|pyramid> ...\n\n";
	showCommandUsage(FaceCommand());
	std::cerr << "\n";
	showCommandUsage(FaceDiagramCommand());
	std::cerr << "\n";
	showCommandUsage(PyramidCommand());
}

