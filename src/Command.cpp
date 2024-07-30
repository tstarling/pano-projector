#include <iostream>
#include "Command.h"
#include "MemoryBudget.h"

namespace PanoProjector {

Command::Command()
	: m_optionsDone(false)
{}

void Command::usage() {
	if (!m_optionsDone) {
		m_optionsDone = true;
		initOptions();
	}
	std::cerr << "pano-projector " << getSynopsis()
		<< "\nAccepted options are:\n"
		<< m_visible;
}

int Command::run(int argc, char **argv) {
	po::options_description all;

	if (!m_optionsDone) {
		m_optionsDone = true;
		initOptions();
	}
	all.add(m_visible).add(m_invisible);

	try {
		po::store(po::command_line_parser(argc, argv)
			.options(all)
			.positional(m_pos)
			.run(), m_options);
		po::notify(m_options);
	} catch (po::error & e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	if (m_options.count("help")) {
		std::cerr << "Usage: ";
		usage();
		return 1;
	}

	try {
		return doRun();
	} catch (std::exception & e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
}

void Command::setMemoryLimit() {
	if (m_options.count("mem-limit")) {
		auto limit = m_options["mem-limit"].as<unsigned long>();
		if (limit < ULONG_MAX / 1048576) {
			g_memBudget.setLimit(limit * 1048576);
		}
	}
}

} // namespace
